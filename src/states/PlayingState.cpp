#include "states/PlayingState.hpp"
#include "states/GameStateManager.hpp"
#include "collision/handlers/BulletTerrainHandler.hpp"
#include "collision/handlers/BulletTankHandler.hpp"
#include "collision/handlers/TankTerrainHandler.hpp"
#include "collision/handlers/TankTankHandler.hpp"
#include "collision/handlers/BulletBulletHandler.hpp"
#include "entities/effects/Effect.hpp"
#include "entities/terrain/Water.hpp"
#include "input/IInput.hpp"
#include "input/PlayerInput.hpp"
#include "level/EnemyWaveGenerator.hpp"
#include "graphics/SpriteSheet.hpp"
#include "utils/DamageCalculator.hpp"
#include "ai/AIBehavior.hpp"
#include "core/ServiceLocator.hpp"
#include <array>
#include <algorithm>
#include <random>
#include <unordered_map>
#include <unordered_set>

namespace tank {

// Convert ms to seconds for spawn interval
constexpr float SPAWN_INTERVAL_SECONDS = Constants::ENEMY_SPAWN_INTERVAL / 1000.0f;

namespace {

void playSfx(SoundId id) {
    if (ServiceLocator::hasAudio()) {
        ServiceLocator::getAudio().playSound(id);
    }
}

void playMusicTrack(const std::string& path) {
    if (ServiceLocator::hasAudio()) {
        ServiceLocator::getAudio().playMusic(path, true);
    }
}

PlayerInput readPlayer1Input(const IInput& input) {
    PlayerInput playerInput;
    playerInput.up = input.isKeyDown(SDL_SCANCODE_W);
    playerInput.down = input.isKeyDown(SDL_SCANCODE_S);
    playerInput.left = input.isKeyDown(SDL_SCANCODE_A);
    playerInput.right = input.isKeyDown(SDL_SCANCODE_D);
    playerInput.fire = input.isKeyDown(SDL_SCANCODE_SPACE);
    return playerInput;
}

PlayerInput readPlayer2Input(const IInput& input) {
    PlayerInput playerInput;
    playerInput.up = input.isKeyDown(SDL_SCANCODE_UP);
    playerInput.down = input.isKeyDown(SDL_SCANCODE_DOWN);
    playerInput.left = input.isKeyDown(SDL_SCANCODE_LEFT);
    playerInput.right = input.isKeyDown(SDL_SCANCODE_RIGHT);
    playerInput.fire = input.isKeyDown(SDL_SCANCODE_RETURN) || input.isKeyDown(SDL_SCANCODE_KP_ENTER) ||
                       input.isKeyDown(SDL_SCANCODE_RCTRL);
    return playerInput;
}

Vector2 centeredEffectTopLeft(const Rectangle& bounds, float effectSize) {
    const Vector2 center = bounds.center();
    return Vector2(center.x - effectSize / 2.0f, center.y - effectSize / 2.0f);
}

std::mt19937& powerUpRandom() {
    static std::mt19937 generator(std::random_device{}());
    return generator;
}
} // namespace

PlayingState::PlayingState(GameStateManager& manager, int levelNumber, bool twoPlayer, bool useWaveGenerator)
    : stateManager_(manager)
    , currentLevel_(levelNumber)
    , twoPlayerMode_(twoPlayer)
    , useWaveGenerator_(useWaveGenerator)
    , levelFilePath_()
    , paused_(false)
    , gameOver_(false)
    , levelComplete_(false)
    , enemySpawnTimer_(0.0f)
    , enemiesSpawned_(0)
    , enemiesAlive_(0)
    , maxEnemiesOnScreen_(4)
    , currentSpawnPoint_(0)
    , player1Lives_(3)
    , player2Lives_(3)
    , hud_()
    , gameOverOverlay_()
    , pauseOverlay_()
{
    // Initialize HUD
    hud_.setTwoPlayerMode(twoPlayerMode_);
    hud_.setCurrentLevel(currentLevel_);
}

PlayingState::PlayingState(GameStateManager& manager, int levelNumber, bool twoPlayer, const std::string& levelFilePath, bool useWaveGenerator)
    : PlayingState(manager, levelNumber, twoPlayer, useWaveGenerator)
{
    levelFilePath_ = levelFilePath;
}

void PlayingState::enter() {
    setupCollisionHandlers();
    loadLevel();
    playMusicTrack("assets/audio/music/battle_theme.wav");
}

void PlayingState::exit() {
    detachAllBulletOwners();
    bullets_.clear();
    enemies_.clear();
    terrains_.clear();
    effects_.clear();
    powerUpManager_.clear();
    player1_.reset();
    player2_.reset();
    base_.reset();
}

void PlayingState::loadLevel() {
    if (!levelFilePath_.empty()) {
        level_ = levelLoader_.loadFromFile(levelFilePath_, currentLevel_);
    } else {
        level_ = levelLoader_.loadLevel(currentLevel_);
    }
    if (!level_) {
        // Create default level if load fails
        level_ = std::make_unique<Level>(currentLevel_);
    }

    if (useWaveGenerator_) {
        EnemyWaveGenerator::applyToLevel(*level_, currentLevel_, stateManager_.getDifficulty());
    }

    enemiesSpawned_ = 0;
    enemiesAlive_ = 0;
    levelComplete_ = false;
    effects_.clear();
    powerUpManager_.clear();
    freezeTimer_ = 0.0f;
    baseFortifyTimer_ = 0.0f;
    fortifiedCells_.clear();
    enemyDefeats_.clear();

    createTerrain();
    createPlayers();

    // Spawn initial enemies
    for (int i = 0; i < maxEnemiesOnScreen_ && i < static_cast<int>(level_->getEnemySpawnList().size()); ++i) {
        if (!spawnEnemy()) {
            break;
        }
    }
}

void PlayingState::createTerrain() {
    terrains_.clear();

    const auto& terrainMap = level_->getTerrainMap();

    const int width = level_->getWidth();
    const int height = level_->getHeight();

    // Brick walls are rendered as 2x2 half-cells (34x34) with 4-corner state (classic Battle City).
    for (int y = 0; y + 1 < height; y += 2) {
        for (int x = 0; x + 1 < width; x += 2) {
            const std::array<bool, 4> corners = {
                terrainMap[y][x] == TerrainType::Brick,
                terrainMap[y][x + 1] == TerrainType::Brick,
                terrainMap[y + 1][x] == TerrainType::Brick,
                terrainMap[y + 1][x + 1] == TerrainType::Brick
            };

            if (corners[0] || corners[1] || corners[2] || corners[3]) {
                const int posX = x * Constants::CELL_SIZE;
                const int posY = y * Constants::CELL_SIZE;
                terrains_.push_back(std::make_unique<BrickWall>(
                    Vector2(static_cast<float>(posX), static_cast<float>(posY)),
                    corners));
            }
        }
    }

    // Other terrain remains half-cell based (17x17) for now.
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const TerrainType type = terrainMap[y][x];
            if (type == TerrainType::Brick) {
                continue;
            }

            const int posX = x * Constants::CELL_SIZE;
            const int posY = y * Constants::CELL_SIZE;

            switch (type) {
                case TerrainType::Steel:
                    terrains_.push_back(std::make_unique<SteelWall>(
                        Vector2(static_cast<float>(posX), static_cast<float>(posY))));
                    break;
                case TerrainType::Water:
                    terrains_.push_back(std::make_unique<Water>(
                        Vector2(static_cast<float>(posX), static_cast<float>(posY))));
                    break;
                case TerrainType::Grass:
                    terrains_.push_back(std::make_unique<Grass>(
                        Vector2(static_cast<float>(posX), static_cast<float>(posY))));
                    break;
                default:
                    break;
            }
        }
    }

    // Create base
    Vector2 basePos = level_->getBasePosition();
    base_ = std::make_unique<Base>(static_cast<int>(basePos.x), static_cast<int>(basePos.y));
}

void PlayingState::createPlayers() {
    Vector2 spawn1 = level_->getPlayer1Spawn();
    player1_ = std::make_unique<PlayerTank>(1, spawn1);
    // Restore saved level
    player1_->setLevel(stateManager_.getPlayer1Level());
    player1_->addScore(stateManager_.getPlayerScore(1));

    if (twoPlayerMode_) {
        Vector2 spawn2 = level_->getPlayer2Spawn();
        player2_ = std::make_unique<PlayerTank>(2, spawn2);
        // Restore saved level
        player2_->setLevel(stateManager_.getPlayer2Level());
        player2_->addScore(stateManager_.getPlayerScore(2));
    }
}

void PlayingState::setupCollisionHandlers() {
    collisionManager_.addHandler(std::make_unique<BulletBulletHandler>());
    collisionManager_.addHandler(std::make_unique<BulletTerrainHandler>());
    collisionManager_.addHandler(std::make_unique<BulletTankHandler>());
    collisionManager_.addHandler(std::make_unique<TankTerrainHandler>());
    collisionManager_.addHandler(std::make_unique<TankTankHandler>());
}

void PlayingState::update(float deltaTime) {
    // Update game over animation even when game is over
    if (gameOver_) {
        gameOverOverlay_.update(deltaTime);
        updateEffects(deltaTime);
        powerUpManager_.update(deltaTime);
        return;
    }

    if (paused_) return;

    updateTimedPowerUps(deltaTime);

    // Enemy spawn timer
    if (freezeTimer_ <= 0.0f) {
        enemySpawnTimer_ = std::min(enemySpawnTimer_ + deltaTime, SPAWN_INTERVAL_SECONDS);
    }
    if (freezeTimer_ <= 0.0f && enemySpawnTimer_ >= SPAWN_INTERVAL_SECONDS &&
        enemiesAlive_ < maxEnemiesOnScreen_ &&
        enemiesSpawned_ < static_cast<int>(level_->getEnemySpawnList().size())) {
        if (spawnEnemy()) {
            enemySpawnTimer_ = 0.0f;
        }
    }

    updateEntities(deltaTime);
    checkCollisions();
    removeDeadEntities();
    checkGameState(deltaTime);
}

void PlayingState::updateEntities(float deltaTime) {
    // Update players
    if (player1_ && player1_->isAlive()) {
        player1_->update(deltaTime);
        handleTankShooting(*player1_);
    }
    if (player2_ && player2_->isAlive()) {
        player2_->update(deltaTime);
        handleTankShooting(*player2_);
    }

    // Update enemies
    if (freezeTimer_ <= 0.0f) {
        for (auto& enemy : enemies_) {
            if (enemy->isAlive()) {
                enemy->update(deltaTime);
                handleTankShooting(*enemy);
            }
        }
    }

    // Update bullets
    for (auto& bullet : bullets_) {
        if (bullet->isAlive()) {
            bullet->update(deltaTime);
        }
    }

    // Update terrain (ITerrain interface provides update())
    for (auto& terrain : terrains_) {
        if (terrain->isActive()) {
            terrain->update(deltaTime);
        }
    }

    if (base_) {
        base_->update(deltaTime);
    }

    updateEffects(deltaTime);
    powerUpManager_.update(deltaTime);
}

void PlayingState::updateEffects(float deltaTime) {
    for (auto& effect : effects_) {
        if (effect->isActive()) {
            effect->update(deltaTime);
        }
    }

    effects_.erase(
        std::remove_if(effects_.begin(), effects_.end(),
            [](const std::unique_ptr<Effect>& e) { return !e->isActive(); }),
        effects_.end()
    );
}

void PlayingState::checkCollisions() {
    std::vector<Bullet*> bulletsAliveAtStart;
    bulletsAliveAtStart.reserve(bullets_.size());
    for (auto& bullet : bullets_) {
        if (bullet->isAlive()) {
            bulletsAliveAtStart.push_back(bullet.get());
        }
    }

    std::vector<ITank*> tanksAliveAtStart;
    tanksAliveAtStart.reserve(enemies_.size() + 2);
    if (player1_ && player1_->isAlive()) {
        tanksAliveAtStart.push_back(player1_.get());
    }
    if (player2_ && player2_->isAlive()) {
        tanksAliveAtStart.push_back(player2_.get());
    }
    for (auto& enemy : enemies_) {
        if (enemy->isAlive()) {
            tanksAliveAtStart.push_back(enemy.get());
        }
    }

    // Collect tanks and save positions before terrain collision
    std::vector<Tank*> allTanks;
    std::unordered_map<Tank*, Vector2> positionsBeforeTerrain;
    if (player1_ && player1_->isAlive() && !player1_->isSpawning()) {
        allTanks.push_back(player1_.get());
        positionsBeforeTerrain[player1_.get()] = player1_->getPreviousPosition();
    }
    if (player2_ && player2_->isAlive() && !player2_->isSpawning()) {
        allTanks.push_back(player2_.get());
        positionsBeforeTerrain[player2_.get()] = player2_->getPreviousPosition();
    }
    for (auto& enemy : enemies_) {
        if (enemy->isAlive() && !enemy->isSpawning()) {
            allTanks.push_back(enemy.get());
            positionsBeforeTerrain[enemy.get()] = enemy->getPreviousPosition();
        }
    }

    // Tank vs Terrain collisions (must happen before any other checks)
    checkTankTerrainCollisions();

    // Check tank-to-tank collisions
    for (size_t i = 0; i < allTanks.size(); ++i) {
        Tank* tankA = allTanks[i];
        if (!tankA->isAlive()) continue;

        for (size_t j = i + 1; j < allTanks.size(); ++j) {
            Tank* tankB = allTanks[j];
            if (!tankB->isAlive()) continue;

            // Check collision after terrain handling
            if (CollisionManager::checkAABB(tankA->getBounds(), tankB->getBounds())) {
                // Check if tanks are moving towards each other
                Vector2 posA = tankA->getPosition();
                Vector2 posB = tankB->getPosition();
                Vector2 prevA = positionsBeforeTerrain[tankA];
                Vector2 prevB = positionsBeforeTerrain[tankB];

                // Tank A moved towards Tank B (movement direction points towards B's previous position)
                bool aMovesToB = (posA.x != prevA.x && (posA.x - prevA.x) * (posB.x - prevA.x) > 0) ||
                                  (posA.y != prevA.y && (posA.y - prevA.y) * (posB.y - prevA.y) > 0);
                // Tank B moved towards Tank A (movement direction points towards A's previous position)
                bool bMovesToA = (posB.x != prevB.x && (posB.x - prevB.x) * (posA.x - prevB.x) > 0) ||
                                  (posB.y != prevB.y && (posB.y - prevB.y) * (posA.y - prevB.y) > 0);

                // Only restore tanks that moved towards the other
                if (aMovesToB) {
                    tankA->setPosition(prevA);
                }
                if (bMovesToA) {
                    tankB->setPosition(prevB);
                }
            }
        }
    }

    // Bullets vs Base
    if (base_ && base_->isAlive()) {
        for (auto& bullet : bullets_) {
            if (bullet->isAlive() && CollisionManager::checkAABB(bullet->getBounds(), base_->getBounds())) {
                base_->takeDamage(bullet->getAttack(), bullet->getBounds());
                bullet->hit();
                bullet->die();
            }
        }
    }

    // Bullets vs Terrain (using ITerrain interface directly)
    for (auto& bullet : bullets_) {
        if (!bullet->isAlive()) continue;

        const Rectangle bulletBounds = bullet->getBounds();
        for (auto& terrain : terrains_) {
            if (terrain->isBulletPassable()) continue;
            if (terrain->isDestroyed()) continue;

            // ITerrain provides getBounds() directly
            if (auto* brick = dynamic_cast<BrickWall*>(terrain.get())) {
                if (!brick->intersectsSolid(bulletBounds)) {
                    continue;
                }
                brick->takeDamage(bullet->getAttack(), bulletBounds);
                playSfx(SoundId::BrickBreak);
                bullet->hit();
                bullet->die();
                break;
            }
            if (auto* steel = dynamic_cast<SteelWall*>(terrain.get())) {
                steel->setDestructible(bullet->getLevel() >= 3);
            }
            if (CollisionManager::checkAABB(bulletBounds, terrain->getBounds())) {
                terrain->takeDamage(bullet->getAttack(), bulletBounds);
                bullet->hit();
                bullet->die();
                break;
            }
        }
    }

    // Bullets vs Tanks (reuse allTanks from tank-tank collision check above)
    for (auto& bullet : bullets_) {
        if (!bullet->isAlive()) continue;

        for (auto* tank : allTanks) {
            if (!tank->isAlive()) continue;
            if (bullet->getOwner() == tank) continue;

            if (CollisionManager::checkAABB(bullet->getBounds(), tank->getBounds())) {
                // Check friendly fire
                bool bulletFromPlayer = dynamic_cast<PlayerTank*>(bullet->getOwner()) != nullptr;
                bool targetIsPlayer = dynamic_cast<PlayerTank*>(tank) != nullptr;

                if (bulletFromPlayer != targetIsPlayer) {
                    // Check invincibility
                    if (auto* player = dynamic_cast<PlayerTank*>(tank)) {
                        if (player->isInvincible()) {
                            bullet->die();
                            continue;
                        }
                    }

                    // Apply damage using DamageCalculator
                    int damage = DamageCalculator::calculateDamage(
                        bullet->getAttack(), tank->getDefense(), tank->getMaxHealth());
                    tank->takeDamage(damage);
                    playSfx(dynamic_cast<PlayerTank*>(tank) ? SoundId::PlayerDamage : SoundId::TankHit);
                    if (!tank->isAlive()) {
                        if (auto* enemy = dynamic_cast<EnemyTank*>(tank)) {
                            registerEnemyDefeat(*enemy, dynamic_cast<PlayerTank*>(bullet->getOwner()), bullet.get());
                        }
                    }
                    bullet->die();
                }
            }
        }
    }

    // Bullet vs Bullet
    collisionManager_.checkCollisionsInternal(bullets_);

    // Spawn explosions for bullets/tanks destroyed during the collision phase.
    for (Bullet* bullet : bulletsAliveAtStart) {
        if (bullet && !bullet->isAlive()) {
            const Vector2 pos = centeredEffectTopLeft(bullet->getBounds(), static_cast<float>(Constants::ELEMENT_SIZE));
            effects_.push_back(std::make_unique<BulletExplosion>(static_cast<int>(pos.x), static_cast<int>(pos.y)));
        }
    }

    for (ITank* tank : tanksAliveAtStart) {
        if (tank && !tank->isAlive()) {
            const Vector2 pos = centeredEffectTopLeft(tank->getBounds(), static_cast<float>(Constants::ELEMENT_SIZE * 2));
            effects_.push_back(std::make_unique<TankExplosion>(static_cast<int>(pos.x), static_cast<int>(pos.y)));
            playSfx(SoundId::Explosion);
        }
    }

    if (player1_) {
        if (const auto collected = powerUpManager_.tryCollect(*player1_)) {
            applyPowerUp(*player1_, *collected);
        }
    }
    if (player2_) {
        if (const auto collected = powerUpManager_.tryCollect(*player2_)) {
            applyPowerUp(*player2_, *collected);
        }
    }
}

void PlayingState::removeDeadEntities() {
    // Remove dead bullets
    bullets_.erase(
        std::remove_if(bullets_.begin(), bullets_.end(),
            [](const std::unique_ptr<Bullet>& b) { return !b->isAlive(); }),
        bullets_.end()
    );

    // Remove dead enemies
    std::unordered_map<const void*, int> killsByDamageSource;
    for (const auto& enemy : enemies_) {
        if (enemy->isAlive()) {
            continue;
        }
        const auto defeat = enemyDefeats_.find(enemy.get());
        if (defeat != enemyDefeats_.end() && defeat->second.owner &&
            defeat->second.damageSource && !defeat->second.preventsMultiplier) {
            ++killsByDamageSource[defeat->second.damageSource];
        }
    }

    int deadEnemies = 0;
    enemies_.erase(
        std::remove_if(enemies_.begin(), enemies_.end(),
            [this, &deadEnemies, &killsByDamageSource](const std::unique_ptr<EnemyTank>& e) {
                if (!e->isAlive()) {
                    stateManager_.recordEnemyKill(e->getEnemyType());
                    const auto defeat = enemyDefeats_.find(e.get());
                    const bool wasBombed = defeat != enemyDefeats_.end() && defeat->second.preventsMultiplier;
                    if (e->carriesPowerUp() && !wasBombed) {
                        powerUpManager_.spawn(e->getPosition(), chooseRandomPowerUp());
                    }

                    if (defeat != enemyDefeats_.end() && defeat->second.owner) {
                        int multiplier = 1;
                        if (!defeat->second.preventsMultiplier && defeat->second.damageSource) {
                            multiplier = std::min(3, killsByDamageSource[defeat->second.damageSource]);
                        }
                        const int points = e->getReward() * multiplier;
                        defeat->second.owner->addScore(points);
                        stateManager_.addPlayerScore(defeat->second.owner->getPlayerId(), points);
                        effects_.push_back(std::make_unique<ScorePopup>(
                            static_cast<int>(e->getPosition().x),
                            static_cast<int>(e->getPosition().y), e->getReward(), multiplier));
                    }

                    enemyDefeats_.erase(e.get());
                    detachBulletsFromTank(e.get());
                    ++deadEnemies;
                    return true;
                }
                return false;
            }),
        enemies_.end()
    );
    enemiesAlive_ -= deadEnemies;
    enemiesAlive_ = std::max(0, enemiesAlive_);

    // Remove destroyed terrain
    terrains_.erase(
        std::remove_if(terrains_.begin(), terrains_.end(),
            [](const std::unique_ptr<ITerrain>& t) { return t->isDestroyed(); }),
        terrains_.end()
    );
}

void PlayingState::updateTimedPowerUps(float deltaTime) {
    if (freezeTimer_ > 0.0f) {
        freezeTimer_ = std::max(0.0f, freezeTimer_ - deltaTime);
    }

    if (baseFortifyTimer_ > 0.0f) {
        baseFortifyTimer_ = std::max(0.0f, baseFortifyTimer_ - deltaTime);
        if (baseFortifyTimer_ <= 0.0f) {
            restoreFortifiedBase();
        }
    }
}

void PlayingState::applyPowerUp(PlayerTank& player, PowerUpType type) {
    playSfx(SoundId::GetBonus);
    switch (type) {
        case PowerUpType::Star:
            player.upgrade();
            break;
        case PowerUpType::Gun:
            player.setLevel(3);
            break;
        case PowerUpType::IronCap:
            player.makeInvincible(Constants::POWERUP_INVINCIBILITY_DURATION);
            break;
        case PowerUpType::StopWatch:
            freezeTimer_ = Constants::POWERUP_FREEZE_DURATION;
            break;
        case PowerUpType::Bomb:
            for (auto& enemy : enemies_) {
                if (!enemy->isAlive()) {
                    continue;
                }
                enemy->setCarriesPowerUp(false);
                registerEnemyDefeat(*enemy, &player, nullptr, true);
                const Vector2 pos = centeredEffectTopLeft(
                    enemy->getBounds(), static_cast<float>(Constants::ELEMENT_SIZE * 2));
                effects_.push_back(std::make_unique<TankExplosion>(static_cast<int>(pos.x), static_cast<int>(pos.y)));
                enemy->die();
            }
            break;
        case PowerUpType::Tank:
            if (player.getPlayerId() == 1) {
                player1Lives_ = std::min(player1Lives_ + 1, Constants::MAX_PLAYER_LIVES);
            } else {
                player2Lives_ = std::min(player2Lives_ + 1, Constants::MAX_PLAYER_LIVES);
            }
            break;
        case PowerUpType::Spade:
            fortifyBase();
            break;
    }
}

void PlayingState::fortifyBase() {
    if (!level_) {
        return;
    }

    syncDestructibleTerrainToMap();

    fortifiedCells_.clear();
    const Vector2 basePosition = level_->getBasePosition();
    const int baseX = static_cast<int>(basePosition.x) / Constants::CELL_SIZE;
    const int baseY = static_cast<int>(basePosition.y) / Constants::CELL_SIZE;

    // The 2x2 base occupies cells (baseX..baseX+1, baseY..baseY+1). Four cells
    // directly above it and two cells down each side form the classic
    // eight-cell protective wall.
    for (int x = baseX - 1; x <= baseX + 2; ++x) {
        fortifiedCells_.push_back({x, baseY - 1});
    }
    for (int y = baseY; y <= baseY + 1; ++y) {
        fortifiedCells_.push_back({baseX - 1, y});
        fortifiedCells_.push_back({baseX + 2, y});
    }

    for (const FortifiedCell& cell : fortifiedCells_) {
        level_->setTerrainAt(cell.x, cell.y, TerrainType::Steel);
    }
    createTerrain();
    baseFortifyTimer_ = Constants::POWERUP_BASE_FORTIFY_DURATION;
}

void PlayingState::restoreFortifiedBase() {
    if (!level_) {
        return;
    }

    syncDestructibleTerrainToMap();

    for (const FortifiedCell& cell : fortifiedCells_) {
        // Match the original game's behaviour: after the shield expires the
        // perimeter is rebuilt as brick, even if a protected cell was hit.
        level_->setTerrainAt(cell.x, cell.y, TerrainType::Brick);
    }
    fortifiedCells_.clear();
    createTerrain();
}

void PlayingState::syncDestructibleTerrainToMap() {
    if (!level_) {
        return;
    }

    const int width = level_->getWidth();
    const int height = level_->getHeight();

    // Live destructible coverage derived from entities (the source of truth
    // mid-game; destroyed entities are erased, so stale map marks must be
    // cleared as well as live ones written back).
    std::vector<std::vector<bool>> liveBrick(height, std::vector<bool>(width, false));
    std::vector<std::vector<bool>> liveSteel(height, std::vector<bool>(width, false));

    for (const auto& terrain : terrains_) {
        if (terrain->isDestroyed()) {
            continue;
        }
        const Rectangle bounds = terrain->getBounds();
        const int cellX = static_cast<int>(bounds.x) / Constants::CELL_SIZE;
        const int cellY = static_cast<int>(bounds.y) / Constants::CELL_SIZE;
        if (auto* brick = dynamic_cast<BrickWall*>(terrain.get())) {
            for (int i = 0; i < 4; ++i) {
                if (brick->isCornerAlive(i)) {
                    liveBrick[cellY + i / 2][cellX + i % 2] = true;
                }
            }
        } else if (dynamic_cast<SteelWall*>(terrain.get())) {
            liveSteel[cellY][cellX] = true;
        }
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const TerrainType type = level_->getTerrainAt(x, y);
            if (type == TerrainType::Brick && !liveBrick[y][x]) {
                level_->setTerrainAt(x, y, TerrainType::Empty);
            } else if (type == TerrainType::Empty && liveBrick[y][x]) {
                level_->setTerrainAt(x, y, TerrainType::Brick);
            } else if (type == TerrainType::Steel && !liveSteel[y][x]) {
                level_->setTerrainAt(x, y, TerrainType::Empty);
            }
        }
    }
}

PowerUpType PlayingState::chooseRandomPowerUp() {
    std::discrete_distribution<int> distribution(
        std::begin(Constants::POWERUP_DROP_WEIGHTS), std::end(Constants::POWERUP_DROP_WEIGHTS));
    return static_cast<PowerUpType>(distribution(powerUpRandom()));
}

void PlayingState::registerEnemyDefeat(EnemyTank& enemy, PlayerTank* owner,
                                       const void* damageSource, bool fromBomb) {
    enemyDefeats_.emplace(&enemy, EnemyDefeat{owner, damageSource, fromBomb});
}

void PlayingState::checkGameState(float deltaTime) {
    // Check base destruction
    if (base_ && !base_->isAlive()) {
        gameOver_ = true;
        gameOverOverlay_.start();
        playSfx(SoundId::GameOver);
        return;
    }

    // Check player deaths
    bool player1Dead = !player1_ || (!player1_->isAlive() && player1Lives_ <= 0);
    bool player2Dead = !twoPlayerMode_ || !player2_ || (!player2_->isAlive() && player2Lives_ <= 0);

    if (player1Dead && player2Dead) {
        gameOver_ = true;
        gameOverOverlay_.start();
        playSfx(SoundId::GameOver);
        return;
    }

    // Handle player respawn - wait for spawn area to be clear
    if (player1_ && !player1_->isAlive() && player1Lives_ > 0) {
        player1RespawnTimer_ += deltaTime;
        if (player1RespawnTimer_ >= RESPAWN_CHECK_INTERVAL) {
            player1RespawnTimer_ = 0.0f;
            // Check if spawn area is free (using spawn position from player tank)
            if (isTankSpawnAreaFree(player1_->getSpawnPosition())) {
                --player1Lives_;
                player1_->respawn();
            }
        }
    }

    if (twoPlayerMode_ && player2_ && !player2_->isAlive() && player2Lives_ > 0) {
        player2RespawnTimer_ += deltaTime;
        if (player2RespawnTimer_ >= RESPAWN_CHECK_INTERVAL) {
            player2RespawnTimer_ = 0.0f;
            // Check if spawn area is free (using spawn position from player tank)
            if (isTankSpawnAreaFree(player2_->getSpawnPosition())) {
                --player2Lives_;
                player2_->respawn();
            }
        }
    }

    // Check level complete
    if (!levelComplete_ &&
        enemiesAlive_ == 0 &&
        enemiesSpawned_ >= static_cast<int>(level_->getEnemySpawnList().size())) {
        levelComplete_ = true;

        // Save player levels before transitioning
        int p1Level = player1_ ? player1_->getLevel() : 0;
        int p2Level = player2_ ? player2_->getLevel() : 0;
        stateManager_.setPlayerLevels(p1Level, p2Level);

        if (!useWaveGenerator_) {
            stateManager_.unlockCampaignLevel(currentLevel_);
        }

        // Show the per-stage score tally; ScoreState carries the run forward.
        stateManager_.changeToScore(currentLevel_, /*victory=*/true, twoPlayerMode_, useWaveGenerator_);
    }
}

void PlayingState::handleInput(const IInput& input) {
    if (!levelFilePath_.empty() && input.isKeyPressed(SDL_SCANCODE_F6)) {
        stateManager_.popState();
        return;
    }

    // Toggle debug mode with F1 key
    if (input.isKeyPressed(SDL_SCANCODE_F1)) {
        debugMode_ = !debugMode_;
    }

    if (gameOver_) {
        handleGameOverMenuInput(input);
        return;
    }

    if (paused_) {
        handlePauseMenuInput(input);
        return;
    }

    if (input.isKeyPressed(SDL_SCANCODE_ESCAPE)) {
        openPauseMenu();
        return;
    }

    handlePlayer1Input(input);
    if (twoPlayerMode_) {
        handlePlayer2Input(input);
    }
}

void PlayingState::openPauseMenu() {
    paused_ = true;
    pauseOverlay_.setActive(true);
    pauseOverlay_.resetSelection();
    playSfx(SoundId::Pause);
}

void PlayingState::resumeFromPause() {
    paused_ = false;
    pauseOverlay_.setActive(false);
}

void PlayingState::restartLevel() {
    if (!levelFilePath_.empty()) {
        stateManager_.changeState(
            std::make_unique<PlayingState>(stateManager_, currentLevel_, twoPlayerMode_, levelFilePath_, useWaveGenerator_));
        return;
    }

    stateManager_.changeState(std::make_unique<PlayingState>(stateManager_, currentLevel_, twoPlayerMode_, useWaveGenerator_));
}

void PlayingState::handlePauseMenuInput(const IInput& input) {
    if (input.isKeyPressed(SDL_SCANCODE_ESCAPE)) {
        resumeFromPause();
        return;
    }

    if (input.isKeyPressed(SDL_SCANCODE_UP) || input.isKeyPressed(SDL_SCANCODE_W)) {
        pauseOverlay_.selectPreviousItem();
    } else if (input.isKeyPressed(SDL_SCANCODE_DOWN) || input.isKeyPressed(SDL_SCANCODE_S)) {
        pauseOverlay_.selectNextItem();
    }

    if (input.isKeyPressed(SDL_SCANCODE_R)) {
        restartLevel();
        return;
    }
    if (input.isKeyPressed(SDL_SCANCODE_M)) {
        stateManager_.changeToMenu();
        return;
    }

    if (input.isKeyPressed(SDL_SCANCODE_RETURN) || input.isKeyPressed(SDL_SCANCODE_SPACE)) {
        switch (pauseOverlay_.getSelectedItem()) {
            case PauseOverlay::MenuItem::Continue:
                resumeFromPause();
                break;
            case PauseOverlay::MenuItem::Restart:
                restartLevel();
                break;
            case PauseOverlay::MenuItem::MainMenu:
                stateManager_.changeToMenu();
                break;
        }
    }
}

void PlayingState::handleGameOverMenuInput(const IInput& input) {
    if (input.isKeyPressed(SDL_SCANCODE_ESCAPE) || input.isKeyPressed(SDL_SCANCODE_M)) {
        gameOverOverlay_.setSelectedItem(GameOverOverlay::MenuItem::MainMenu);
        stateManager_.changeToMenu();
        return;
    }

    if (input.isKeyPressed(SDL_SCANCODE_UP) || input.isKeyPressed(SDL_SCANCODE_W)) {
        gameOverOverlay_.selectPreviousItem();
    } else if (input.isKeyPressed(SDL_SCANCODE_DOWN) || input.isKeyPressed(SDL_SCANCODE_S)) {
        gameOverOverlay_.selectNextItem();
    }

    if (input.isKeyPressed(SDL_SCANCODE_R)) {
        gameOverOverlay_.setSelectedItem(GameOverOverlay::MenuItem::Restart);
        restartLevel();
        return;
    }

    if (input.isKeyPressed(SDL_SCANCODE_RETURN) || input.isKeyPressed(SDL_SCANCODE_SPACE)) {
        switch (gameOverOverlay_.getSelectedItem()) {
            case GameOverOverlay::MenuItem::Restart:
                restartLevel();
                break;
            case GameOverOverlay::MenuItem::MainMenu:
                stateManager_.changeToMenu();
                break;
        }
    }
}

void PlayingState::handlePlayer1Input(const IInput& input) {
    if (!player1_) {
        return;
    }
    if (!player1_->isAlive()) {
        return;
    }

    player1_->handleInput(readPlayer1Input(input));
}

void PlayingState::handlePlayer2Input(const IInput& input) {
    if (!player2_ || !player2_->isAlive()) return;

    player2_->handleInput(readPlayer2Input(input));
}

void PlayingState::render(IRenderer& renderer) {
    // Clear with black
    renderer.clear(0, 0, 0, 255);

    renderTerrain(renderer);
    renderEntities(renderer);
    renderUI(renderer);

    // Debug mode: render collision bounds
    if (debugMode_) {
        renderDebugBounds(renderer);
    }

    if (paused_) {
        pauseOverlay_.render(renderer);
    }

    if (gameOver_) {
        gameOverOverlay_.render(renderer);
    }
}

void PlayingState::renderTerrain(IRenderer& renderer) {
    // Use ITerrain interface's getRenderLayer() instead of dynamic_cast
    // Render water first (under everything)
    for (const auto& terrain : terrains_) {
        if (terrain->getRenderLayer() == RenderLayer::Water && terrain->isActive()) {
            terrain->render(renderer);
        }
    }

    // Render base
    if (base_) {
        base_->render(renderer);
    }

    // Render walls (non-water, non-grass terrain)
    for (const auto& terrain : terrains_) {
        RenderLayer layer = terrain->getRenderLayer();
        if (layer != RenderLayer::Water && layer != RenderLayer::Grass && terrain->isActive()) {
            terrain->render(renderer);
        }
    }
}

void PlayingState::renderEntities(IRenderer& renderer) {
    // Render players
    if (player1_ && player1_->isAlive()) {
        player1_->render(renderer);
    }
    if (player2_ && player2_->isAlive()) {
        player2_->render(renderer);
    }

    // Render enemies
    for (const auto& enemy : enemies_) {
        if (enemy->isAlive()) {
            enemy->render(renderer);
        }
    }

    // Render bullets
    for (const auto& bullet : bullets_) {
        if (bullet->isAlive()) {
            bullet->render(renderer);
        }
    }

    // Render grass last (on top of tanks) using ITerrain interface
    for (const auto& terrain : terrains_) {
        if (terrain->getRenderLayer() == RenderLayer::Grass && terrain->isActive()) {
            terrain->render(renderer);
        }
    }

    powerUpManager_.render(renderer);

    // Render effects on top (explosions, etc.)
    for (const auto& effect : effects_) {
        if (effect->isActive()) {
            effect->render(renderer);
        }
    }
}

void PlayingState::renderUI(IRenderer& renderer) {
    // Update HUD with current game state
    int remainingEnemies = static_cast<int>(level_->getEnemySpawnList().size()) - enemiesSpawned_ + enemiesAlive_;
    hud_.setRemainingEnemies(remainingEnemies);
    hud_.setPlayer1Lives(player1Lives_);
    hud_.setPlayer2Lives(player2Lives_);
    hud_.setScore(stateManager_.getPlayerScore(1) + stateManager_.getPlayerScore(2));

    // Render sidebar with remaining enemies, lives, etc.
    hud_.render(renderer);
}

void PlayingState::renderDebugBounds(IRenderer& renderer) {
    // Define colors for different entity types
    const Constants::Color colorPlayer1{255, 215, 0, 180};     // Gold
    const Constants::Color colorPlayer2{0, 191, 255, 180};     // Deep sky blue
    const Constants::Color colorEnemy{255, 99, 71, 180};       // Tomato red
    const Constants::Color colorBullet{255, 105, 180, 180};    // Hot pink
    const Constants::Color colorBrick{139, 69, 19, 180};       // Saddle brown
    const Constants::Color colorSteel{192, 192, 192, 180};     // Silver
    const Constants::Color colorBase{255, 0, 255, 180};        // Magenta
    const Constants::Color colorWater{0, 191, 255, 120};       // Transparent blue
    const Constants::Color labelColor{255, 255, 255, 255};

    // Helper lambda to render a labeled rectangle
    auto renderLabeledRect = [&renderer, &labelColor](const Rectangle& bounds, const Constants::Color& color, const char* label) {
        renderer.drawRectangle(bounds, color, false);  // Wireframe
        if (label && label[0]) {
            Vector2 labelPos(bounds.x, bounds.y - 12);
            renderer.drawText(label, labelPos, labelColor, 10);
        }
    };

    // Render tank bounds
    if (player1_ && player1_->isAlive()) {
        renderLabeledRect(player1_->getBounds(), colorPlayer1, "P1");
    }
    if (player2_ && player2_->isAlive()) {
        renderLabeledRect(player2_->getBounds(), colorPlayer2, "P2");
    }
    for (const auto& enemy : enemies_) {
        if (enemy->isAlive()) {
            renderLabeledRect(enemy->getBounds(), colorEnemy, "E");
        }
    }

    // Render bullet bounds
    for (const auto& bullet : bullets_) {
        if (bullet->isAlive()) {
            renderLabeledRect(bullet->getBounds(), colorBullet, "");
        }
    }

    // Render terrain bounds
    for (const auto& terrain : terrains_) {
        if (!terrain->isActive() || terrain->isDestroyed()) continue;

        const Constants::Color* color = nullptr;
        const char* label = "";

        if (auto* brick = dynamic_cast<BrickWall*>(terrain.get())) {
            color = &colorBrick;
            label = "B";
            // Render each corner for brick walls
            for (int i = 0; i < 4; ++i) {
                if (brick->isCornerAlive(i)) {
                    Rectangle corner = brick->getCornerBounds(i);
                    renderLabeledRect(corner, *color, "");
                }
            }
            continue;
        } else if (dynamic_cast<SteelWall*>(terrain.get())) {
            color = &colorSteel;
            label = "S";
        } else if (dynamic_cast<Water*>(terrain.get())) {
            color = &colorWater;
            label = "W";
        }

        if (color) {
            renderLabeledRect(terrain->getBounds(), *color, label);
        }
    }

    // Render base bounds
    if (base_ && base_->isAlive()) {
        renderLabeledRect(base_->getBounds(), colorBase, "BASE");
    }

    // Render debug info text
    Vector2 infoPos(5, 5);
    renderer.drawText("DEBUG MODE - Collision Bounds", infoPos, labelColor, 12);
    renderer.drawText("P1=Player P2=P2 E=Enemy B=Brick S=Steel W=Water", Vector2(5, 20), Constants::Color(200, 200, 200, 200), 10);
}

void PlayingState::handleTankShooting(Tank& tank) {
    if (!tank.consumeShotRequest()) {
        return;
    }

    Vector2 spawnPos = calculateBulletSpawnPosition(tank);
    auto bullet = std::make_unique<Bullet>(spawnPos, tank.getDirection(), &tank, tank.getLevel());
    addBullet(std::move(bullet));

    if (dynamic_cast<PlayerTank*>(&tank)) {
        playSfx(SoundId::BulletShot);
    }
}

Vector2 PlayingState::calculateBulletSpawnPosition(const Tank& tank) const {
    Rectangle bounds = tank.getBounds();
    float bulletSize = static_cast<float>(Sprites::Bullet::SIZE);
    float spawnX = bounds.x + (bounds.width / 2.0f) - (bulletSize / 2.0f);
    float spawnY = bounds.y + (bounds.height / 2.0f) - (bulletSize / 2.0f);

    switch (tank.getDirection()) {
        case Direction::Up:
            spawnY = bounds.y - bulletSize;
            break;
        case Direction::Down:
            spawnY = bounds.y + bounds.height;
            break;
        case Direction::Left:
            spawnX = bounds.x - bulletSize;
            break;
        case Direction::Right:
            spawnX = bounds.x + bounds.width;
            break;
    }

    return Vector2(spawnX, spawnY);
}

void PlayingState::detachBulletsFromTank(ITank* tank) {
    if (!tank) {
        return;
    }

    for (auto& bullet : bullets_) {
        if (bullet->getOwner() == tank) {
            bullet->clearOwner();
        }
    }
}

void PlayingState::detachAllBulletOwners() {
    for (auto& bullet : bullets_) {
        bullet->clearOwner();
    }
}

void PlayingState::addBullet(std::unique_ptr<Bullet> bullet) {
    bullets_.push_back(std::move(bullet));
}

bool PlayingState::spawnEnemy() {
    const auto& spawnList = level_->getEnemySpawnList();
    if (enemiesSpawned_ >= static_cast<int>(spawnList.size())) return false;

    const auto& spawnPoints = level_->getEnemySpawnPoints();
    if (spawnPoints.empty()) return false;

    const int spawnPointCount = static_cast<int>(spawnPoints.size());
    int chosenIndex = -1;
    Vector2 chosenPoint;
    for (int attempt = 0; attempt < spawnPointCount; ++attempt) {
        const int index = (currentSpawnPoint_ + attempt) % spawnPointCount;
        const Vector2& candidate = spawnPoints[index];
        if (!isTankSpawnAreaFree(candidate)) {
            continue;
        }
        chosenIndex = index;
        chosenPoint = candidate;
        break;
    }

    if (chosenIndex < 0) {
        return false;
    }

    const EnemySpawnInfo& info = spawnList[enemiesSpawned_];
    auto enemy = std::make_unique<EnemyTank>(chosenPoint, info.type);

    configureEnemyAI(*enemy);

    // Set power-up carrying based on spawn info
    if (info.hasPowerUp) {
        enemy->setCarriesPowerUp(true);
    }

    // Initialize spawn animation
    enemy->spawn(chosenPoint);
    enemy->applyDifficulty(stateManager_.getDifficulty());

    enemies_.push_back(std::move(enemy));
    ++enemiesSpawned_;
    ++enemiesAlive_;

    // Rotate spawn points
    currentSpawnPoint_ = (chosenIndex + 1) % spawnPointCount;
    return true;
}

void PlayingState::configureEnemyAI(EnemyTank& enemy) {
    const Vector2 target = level_ ? level_->getBasePosition() : Vector2{};
    switch (enemy.getEnemyType()) {
        case EnemyType::Basic:
            enemy.setAIBehavior(std::make_unique<SimpleAI>());
            break;
        case EnemyType::Fast: {
            auto behavior = std::make_unique<PathfindingAI>();
            behavior->setTarget(target);
            behavior->setLevel(level_.get());
            enemy.setAIBehavior(std::move(behavior));
            break;
        }
        case EnemyType::Power: {
            auto behavior = std::make_unique<RangedAI>();
            behavior->setTarget(target);
            enemy.setAIBehavior(std::move(behavior));
            break;
        }
        case EnemyType::Heavy: {
            auto behavior = std::make_unique<DirectAI>();
            behavior->setTarget(target);
            enemy.setAIBehavior(std::move(behavior));
            break;
        }
    }
}

void PlayingState::nextLevel() {
    ++currentLevel_;
    if (currentLevel_ > LevelLoader::getTotalLevels()) {
        // Victory!
        currentLevel_ = 1;  // Or go to victory state
    }
    loadLevel();
}

bool PlayingState::isTankSpawnAreaFree(const Vector2& position) const {
    // position is the top-left of the tank's actual collision box (see Tank::getBounds)
    constexpr float TANK_SIZE = static_cast<float>(Constants::TANK_COLLISION_SIZE);
    Rectangle spawnArea(position.x, position.y, TANK_SIZE, TANK_SIZE);

    if (base_ && base_->isAlive() && CollisionManager::checkAABB(spawnArea, base_->getBounds())) {
        return false;
    }

    for (const auto& terrain : terrains_) {
        if (!terrain->isActive()) continue;
        if (terrain->isDestroyed()) continue;
        if (terrain->isTankPassable()) continue;

        if (auto* brick = dynamic_cast<BrickWall*>(terrain.get())) {
            if (brick->intersectsSolid(spawnArea)) {
                return false;
            }
            continue;
        }

        if (CollisionManager::checkAABB(spawnArea, terrain->getBounds())) {
            return false;
        }
    }

    if (player1_ && player1_->isAlive() && CollisionManager::checkAABB(spawnArea, player1_->getBounds())) {
        return false;
    }
    if (player2_ && player2_->isAlive() && CollisionManager::checkAABB(spawnArea, player2_->getBounds())) {
        return false;
    }

    for (const auto& enemy : enemies_) {
        if (!enemy->isAlive()) continue;
        if (CollisionManager::checkAABB(spawnArea, enemy->getBounds())) {
            return false;
        }
    }

    return true;
}

void PlayingState::checkTankTerrainCollisions() {
    // Collect all tanks. Spawning tanks are included: they are allowed to move
    // during the spawn animation, so they must still collide with terrain -
    // otherwise they can drive into walls before the animation ends and get
    // stuck. (Bullets still ignore spawning tanks - spawn protection.)
    std::vector<Tank*> allTanks;
    if (player1_ && player1_->isAlive()) {
        allTanks.push_back(player1_.get());
    }
    if (player2_ && player2_->isAlive()) {
        allTanks.push_back(player2_.get());
    }
    for (auto& enemy : enemies_) {
        if (enemy->isAlive()) {
            allTanks.push_back(enemy.get());
        }
    }

    // Check each tank against terrain and base with sliding collision
    for (Tank* tank : allTanks) {
        Vector2 previousPos = tank->getPreviousPosition();
        Vector2 currentPos = tank->getPosition();

        // Calculate movement delta
        float deltaX = currentPos.x - previousPos.x;
        float deltaY = currentPos.y - previousPos.y;

        // If no movement, skip collision check
        if (deltaX == 0.0f && deltaY == 0.0f) continue;

        // Reset to previous position for sliding collision
        tank->setPosition(previousPos);

        // Helper lambda to check if tank collides with terrain/base at current position
        auto checkCollision = [this, tank]() -> bool {
            Rectangle tankBounds = tank->getBounds();

            // Check against base
            if (base_ && base_->isAlive()) {
                if (CollisionManager::checkAABB(tankBounds, base_->getBounds())) {
                    return true;
                }
            }

            // Check against terrain
            for (auto& terrain : terrains_) {
                if (terrain->isTankPassable()) continue;
                if (terrain->isDestroyed()) continue;

                if (auto* brick = dynamic_cast<BrickWall*>(terrain.get())) {
                    if (!brick->intersectsSolid(tankBounds)) {
                        continue;
                    }
                    return true;
                }
                if (CollisionManager::checkAABB(tankBounds, terrain->getBounds())) {
                    return true;
                }
            }
            return false;
        };

        // Check if the tank's previousPosition is already in collision
        // If so, try to find a safe position first
        bool initialCollision = checkCollision();
        if (initialCollision) {
            // Tank spawned in collision - try to find nearby safe position
            constexpr float SEARCH_STEP = 2.0f;
            constexpr float MAX_SEARCH = 20.0f;

            // Try moving in all 4 directions to find a safe spot
            bool foundSafe = false;
            for (float offset = SEARCH_STEP; offset <= MAX_SEARCH && !foundSafe; offset += SEARCH_STEP) {
                // Try 4 directions
                Vector2 testPositions[] = {
                    {previousPos.x - offset, previousPos.y},
                    {previousPos.x + offset, previousPos.y},
                    {previousPos.x, previousPos.y - offset},
                    {previousPos.x, previousPos.y + offset}
                };

                for (const auto& testPos : testPositions) {
                    tank->setPosition(testPos);
                    if (!checkCollision()) {
                        // Found safe position
                        tank->updatePreviousPosition();
                        previousPos = testPos;
                        foundSafe = true;
                        break;
                    }
                }
            }

            if (!foundSafe) {
                // Couldn't find safe position, stay where we are
                tank->setPosition(previousPos);
                continue;
            }

            // Recalculate movement delta from safe position
            currentPos = tank->getPosition();
            deltaX = currentPos.x - previousPos.x;
            deltaY = currentPos.y - previousPos.y;

            if (deltaX == 0.0f && deltaY == 0.0f) continue;
        }

        // Helper lambda to move tank and update previousPosition only on success
        auto safeMove = [this, tank, &checkCollision](float dx, float dy) -> bool {
            if (dx == 0.0f && dy == 0.0f) return false;

            Vector2 oldPos = tank->getPosition();

            // Try movement
            if (dx != 0.0f) {
                tank->moveXInternal(dx);
            }
            if (dy != 0.0f) {
                tank->moveYInternal(dy);
            }

            // Check collision
            if (checkCollision()) {
                // Collision detected - revert to old position
                tank->setPosition(oldPos);
                return false;
            }

            // Movement successful - update previousPosition
            tank->updatePreviousPosition();
            return true;
        };

        // Try X axis movement first
        safeMove(deltaX, 0.0f);

        // Then try Y axis movement (allowing X slide)
        safeMove(0.0f, deltaY);
    }

    // Tank vs Tank collisions (still use simple stay for now)
    for (size_t i = 0; i < allTanks.size(); ++i) {
        for (size_t j = i + 1; j < allTanks.size(); ++j) {
            if (CollisionManager::checkAABB(allTanks[i]->getBounds(), allTanks[j]->getBounds())) {
                allTanks[i]->stay();
                allTanks[j]->stay();
            }
        }
    }
}

} // namespace tank
