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
#include "ui/GameHUD.hpp"
#include <vector>
#include <memory>
#include <string>

namespace tank {

class GameStateManager;

/**
 * @brief Main gameplay state
 */
class PlayingState : public IGameState {
public:
    explicit PlayingState(GameStateManager& manager, int levelNumber = 1, bool twoPlayer = false);
    PlayingState(GameStateManager& manager, int levelNumber, bool twoPlayer, const std::string& levelFilePath);
    ~PlayingState() override = default;

    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render(IRenderer& renderer) override;
    void handleInput(const InputManager& input) override;

    StateType getType() const override { return StateType::Playing; }

    // Game control
    void pauseGame() { paused_ = true; }
    void resumeGame() { paused_ = false; }
    bool isPaused() const { return paused_; }

    // Level management
    int getCurrentLevel() const { return currentLevel_; }
    void nextLevel();

    // Add entities
    void addBullet(std::unique_ptr<Bullet> bullet);
    void spawnEnemy();

private:
    GameStateManager& stateManager_;

    // Level data
    int currentLevel_;
    bool twoPlayerMode_;
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

    // UI components
    GameHUD hud_;
    GameOverOverlay gameOverOverlay_;
    PauseOverlay pauseOverlay_;

    // Methods
    void loadLevel();
    void createTerrain();
    void createPlayers();
    void setupCollisionHandlers();

    void updateEntities(float deltaTime);
    void checkCollisions();
    void checkTankTerrainCollisions();
    void removeDeadEntities();
    void checkGameState();

    void handlePlayer1Input(const InputManager& input);
    void handlePlayer2Input(const InputManager& input);

    void renderTerrain(IRenderer& renderer);
    void renderEntities(IRenderer& renderer);
    void renderUI(IRenderer& renderer);

    void handleTankShooting(Tank& tank);
    Vector2 calculateBulletSpawnPosition(const Tank& tank) const;
    void detachBulletsFromTank(ITank* tank);
    void detachAllBulletOwners();
};

} // namespace tank
