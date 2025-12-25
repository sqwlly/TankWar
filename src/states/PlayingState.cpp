#include "states/PlayingState.hpp"
#include "states/GameStateManager.hpp"
#include "collision/handlers/BulletTerrainHandler.hpp"
#include "collision/handlers/BulletTankHandler.hpp"
#include "collision/handlers/TankTerrainHandler.hpp"
#include "collision/handlers/TankTankHandler.hpp"
#include "collision/handlers/BulletBulletHandler.hpp"
#include "entities/effects/Effect.hpp"
#include "input/IInput.hpp"
#include "input/PlayerInput.hpp"
#include "level/EnemyWaveGenerator.hpp"
#include "graphics/SpriteSheet.hpp"
#include "utils/DamageCalculator.hpp"
#include "ai/AIBehavior.hpp"
#include <array>
#include <algorithm>

namespace tank {

// Convert ms to seconds for spawn interval
constexpr float SPAWN_INTERVAL_SECONDS = Constants::ENEMY_SPAWN_INTERVAL / 1000.0f;

namespace {
PlayerInput readPlayer1Input(const IInput& input) {
    PlayerInput playerInput;
    playerInput.up = input.isKeyDown(SDL_SCANCODE_W) || input.isKeyDown(SDLK_w);
    playerInput.down = input.isKeyDown(SDL_SCANCODE_S) || input.isKeyDown(SDLK_s);
    playerInput.left = input.isKeyDown(SDL_SCANCODE_A) || input.isKeyDown(SDLK_a);
    playerInput.right = input.isKeyDown(SDL_SCANCODE_D) || input.isKeyDown(SDLK_d);
    playerInput.fire = input.isKeyDown(SDL_SCANCODE_SPACE) || input.isKeyDown(SDLK_SPACE);
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
}

void PlayingState::exit() {
    detachAllBulletOwners();
    bullets_.clear();
    enemies_.clear();
    terrains_.clear();
    effects_.clear();
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
        EnemyWaveGenerator::applyToLevel(*level_, currentLevel_);
    }

    enemiesSpawned_ = 0;
    enemiesAlive_ = 0;
    levelComplete_ = false;
    effects_.clear();

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

    if (twoPlayerMode_) {
        Vector2 spawn2 = level_->getPlayer2Spawn();
        player2_ = std::make_unique<PlayerTank>(2, spawn2);
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
        return;
    }

    if (paused_) return;

    // Enemy spawn timer
    enemySpawnTimer_ = std::min(enemySpawnTimer_ + deltaTime, SPAWN_INTERVAL_SECONDS);
    if (enemySpawnTimer_ >= SPAWN_INTERVAL_SECONDS &&
        enemiesAlive_ < maxEnemiesOnScreen_ &&
        enemiesSpawned_ < static_cast<int>(level_->getEnemySpawnList().size())) {
        if (spawnEnemy()) {
            enemySpawnTimer_ = 0.0f;
        }
    }

    updateEntities(deltaTime);
    checkCollisions();
    removeDeadEntities();
    checkGameState();
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
    for (auto& enemy : enemies_) {
        if (enemy->isAlive()) {
            enemy->update(deltaTime);
            handleTankShooting(*enemy);
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

    // Tank vs Terrain collisions (must happen before any other checks)
    checkTankTerrainCollisions();

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

    // Bullets vs Tanks
    std::vector<ITank*> allTanks;
    if (player1_ && player1_->isAlive()) allTanks.push_back(player1_.get());
    if (player2_ && player2_->isAlive()) allTanks.push_back(player2_.get());
    for (auto& enemy : enemies_) {
        if (enemy->isAlive()) allTanks.push_back(enemy.get());
    }

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
    int deadEnemies = 0;
    enemies_.erase(
        std::remove_if(enemies_.begin(), enemies_.end(),
            [this, &deadEnemies](const std::unique_ptr<EnemyTank>& e) {
                if (!e->isAlive()) {
                    detachBulletsFromTank(e.get());
                    ++deadEnemies;
                    return true;
                }
                return false;
            }),
        enemies_.end()
    );
    enemiesAlive_ -= deadEnemies;

    // Remove destroyed terrain
    terrains_.erase(
        std::remove_if(terrains_.begin(), terrains_.end(),
            [](const std::unique_ptr<ITerrain>& t) { return t->isDestroyed(); }),
        terrains_.end()
    );
}

void PlayingState::checkGameState() {
    // Check base destruction
    if (base_ && !base_->isAlive()) {
        gameOver_ = true;
        gameOverOverlay_.start();
        return;
    }

    // Check player deaths
    bool player1Dead = !player1_ || (!player1_->isAlive() && player1Lives_ <= 0);
    bool player2Dead = !twoPlayerMode_ || !player2_ || (!player2_->isAlive() && player2Lives_ <= 0);

    if (player1Dead && player2Dead) {
        gameOver_ = true;
        gameOverOverlay_.start();
        return;
    }

    // Handle player respawn
    if (player1_ && !player1_->isAlive() && player1Lives_ > 0) {
        --player1Lives_;
        player1_->respawn();
    }

    if (twoPlayerMode_ && player2_ && !player2_->isAlive() && player2Lives_ > 0) {
        --player2Lives_;
        player2_->respawn();
    }

    // Check level complete
    if (!levelComplete_ &&
        enemiesAlive_ == 0 &&
        enemiesSpawned_ >= static_cast<int>(level_->getEnemySpawnList().size())) {
        levelComplete_ = true;

        int nextLevel = currentLevel_ + 1;
        if (nextLevel > LevelLoader::getTotalLevels()) {
            nextLevel = 1;
        }

        stateManager_.changeToStage(nextLevel, twoPlayerMode_, useWaveGenerator_);
    }
}

void PlayingState::handleInput(const IInput& input) {
    if (!levelFilePath_.empty() && input.isKeyPressed(SDL_SCANCODE_F6)) {
        stateManager_.popState();
        return;
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

    // Render sidebar with remaining enemies, lives, etc.
    hud_.render(renderer);
}

void PlayingState::handleTankShooting(Tank& tank) {
    if (!tank.consumeShotRequest()) {
        return;
    }

    Vector2 spawnPos = calculateBulletSpawnPosition(tank);
    auto bullet = std::make_unique<Bullet>(spawnPos, tank.getDirection(), &tank, tank.getLevel());
    addBullet(std::move(bullet));
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

    // Set AI behavior for enemy tank
    enemy->setAIBehavior(std::make_unique<SimpleAI>());

    // Set power-up carrying based on spawn info
    if (info.hasPowerUp) {
        enemy->setCarriesPowerUp(true);
    }

    // Initialize spawn animation
    enemy->spawn(chosenPoint);

    enemies_.push_back(std::move(enemy));
    ++enemiesSpawned_;
    ++enemiesAlive_;

    // Rotate spawn points
    currentSpawnPoint_ = (chosenIndex + 1) % spawnPointCount;
    return true;
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
    constexpr float TANK_SIZE = Constants::ELEMENT_SIZE - 2.0f;
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
    // Collect all tanks
    std::vector<Tank*> allTanks;
    if (player1_ && player1_->isAlive() && !player1_->isSpawning()) {
        allTanks.push_back(player1_.get());
    }
    if (player2_ && player2_->isAlive() && !player2_->isSpawning()) {
        allTanks.push_back(player2_.get());
    }
    for (auto& enemy : enemies_) {
        if (enemy->isAlive() && !enemy->isSpawning()) {
            allTanks.push_back(enemy.get());
        }
    }

    // Check each tank against terrain and base
    for (Tank* tank : allTanks) {
        Rectangle tankBounds = tank->getBounds();
        bool collided = false;

        // Check against base
        if (base_ && base_->isAlive()) {
            if (CollisionManager::checkAABB(tankBounds, base_->getBounds())) {
                tank->stay();
                collided = true;
            }
        }

        if (collided) continue;

        // Check against terrain
        for (auto& terrain : terrains_) {
            if (terrain->isTankPassable()) continue;
            if (terrain->isDestroyed()) continue;

            if (auto* brick = dynamic_cast<BrickWall*>(terrain.get())) {
                if (!brick->intersectsSolid(tankBounds)) {
                    continue;
                }
                tank->stay();
                break;
            }
            if (CollisionManager::checkAABB(tankBounds, terrain->getBounds())) {
                tank->stay();
                break;
            }
        }
    }

    // Tank vs Tank collisions
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
