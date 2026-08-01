#pragma once

#include "states/IGameState.hpp"
#include "level/Level.hpp"
#include "level/LevelLoader.hpp"
#include "collision/CollisionManager.hpp"
#include "entities/tanks/PlayerTank.hpp"
#include "entities/tanks/EnemyTank.hpp"
#include "entities/terrain/BrickWall.hpp"
#include "entities/terrain/SteelWall.hpp"
#include "entities/terrain/Water.hpp"
#include "entities/terrain/Grass.hpp"
#include "entities/terrain/Base.hpp"
#include "entities/projectiles/Bullet.hpp"
#include "entities/effects/Effect.hpp"
#include "entities/powerups/PowerUpManager.hpp"
#include "ui/GameHUD.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace tank {

class GameStateManager;

/**
 * @brief Main gameplay state
 */
class PlayingState : public IGameState {
public:
    explicit PlayingState(GameStateManager& manager, int levelNumber = 1, bool twoPlayer = false, bool useWaveGenerator = false);
    PlayingState(GameStateManager& manager, int levelNumber, bool twoPlayer, const std::string& levelFilePath, bool useWaveGenerator = false);
    ~PlayingState() override = default;

    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render(IRenderer& renderer) override;
    void handleInput(const IInput& input) override;

    StateType getType() const override { return StateType::Playing; }

    // Game control
    void pauseGame() { paused_ = true; pauseOverlay_.setActive(true); pauseOverlay_.resetSelection(); }
    void resumeGame() { paused_ = false; pauseOverlay_.setActive(false); }
    bool isPaused() const { return paused_; }

    // Level management
    int getCurrentLevel() const { return currentLevel_; }
    void nextLevel();

    // Add entities
    void addBullet(std::unique_ptr<Bullet> bullet);
    bool spawnEnemy();

private:
    GameStateManager& stateManager_;

    // Level data
    int currentLevel_;
    bool twoPlayerMode_;
    bool useWaveGenerator_;
    std::string levelFilePath_;
    std::unique_ptr<Level> level_;
    LevelLoader levelLoader_;

    // Entities
    std::unique_ptr<PlayerTank> player1_;
    std::unique_ptr<PlayerTank> player2_;
    std::vector<std::unique_ptr<EnemyTank>> enemies_;
    std::vector<std::unique_ptr<Bullet>> bullets_;
    std::vector<std::unique_ptr<ITerrain>> terrains_;
    std::unique_ptr<Base> base_;
    std::vector<std::unique_ptr<Effect>> effects_;
    PowerUpManager powerUpManager_;

    // Timed global power-up state.
    float freezeTimer_ = 0.0f;
    float baseFortifyTimer_ = 0.0f;
    struct FortifiedCell {
        int x;
        int y;
    };
    std::vector<FortifiedCell> fortifiedCells_;

    // Enemy deaths are resolved in removeDeadEntities(), after collision and
    // power-up handling have completed for the frame.
    struct EnemyDefeat {
        PlayerTank* owner = nullptr;
        const void* damageSource = nullptr;
        bool preventsMultiplier = false;
    };
    std::unordered_map<EnemyTank*, EnemyDefeat> enemyDefeats_;

    // Collision
    CollisionManager collisionManager_;

    // Game state
    bool paused_;
    bool gameOver_;
    bool levelComplete_;

    // Enemy spawning
    float enemySpawnTimer_;
    int enemiesSpawned_;
    int enemiesAlive_;
    int maxEnemiesOnScreen_;
    int currentSpawnPoint_;

    // Player lives
    int player1Lives_;
    int player2Lives_;
    float player1RespawnTimer_ = 0.0f;
    float player2RespawnTimer_ = 0.0f;
    static constexpr float RESPAWN_CHECK_INTERVAL = 0.5f;  // Check every 0.5 seconds

    // UI components
    GameHUD hud_;
    GameOverOverlay gameOverOverlay_;
    PauseOverlay pauseOverlay_;

    // Debug mode
    bool debugMode_ = false;

    // Methods
    void loadLevel();
    void createTerrain();
    void createPlayers();
    void setupCollisionHandlers();

    void updateEntities(float deltaTime);
    void updateTimedPowerUps(float deltaTime);
    void updateEffects(float deltaTime);
    void checkCollisions();
    void checkTankTerrainCollisions();
    void removeDeadEntities();
    void checkGameState(float deltaTime);

    void handlePlayer1Input(const IInput& input);
    void handlePlayer2Input(const IInput& input);

    void openPauseMenu();
    void resumeFromPause();
    void restartLevel();
    void handlePauseMenuInput(const IInput& input);
    void handleGameOverMenuInput(const IInput& input);

    void renderTerrain(IRenderer& renderer);
    void renderEntities(IRenderer& renderer);
    void renderUI(IRenderer& renderer);
    void renderDebugBounds(IRenderer& renderer);

    void handleTankShooting(Tank& tank);
    Vector2 calculateBulletSpawnPosition(const Tank& tank) const;
    void detachBulletsFromTank(ITank* tank);
    void detachAllBulletOwners();
    bool isTankSpawnAreaFree(const Vector2& position) const;

    void applyPowerUp(PlayerTank& player, PowerUpType type);
    void fortifyBase();
    void restoreFortifiedBase();
    // Copies live destructible-terrain state (brick corners / steel) back into
    // the level map. Bullets only damage terrain entities, so the map is stale
    // until synced; call before any full createTerrain() rebuild.
    void syncDestructibleTerrainToMap();
    PowerUpType chooseRandomPowerUp();
    void registerEnemyDefeat(EnemyTank& enemy, PlayerTank* owner,
                             const void* damageSource = nullptr, bool fromBomb = false);
    void configureEnemyAI(EnemyTank& enemy);
};

} // namespace tank
