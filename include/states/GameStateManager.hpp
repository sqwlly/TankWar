#pragma once

#include "states/IGameState.hpp"
#include "utils/ProgressStore.hpp"
#include <array>
#include <memory>
#include <stack>

namespace tank {

class IInput;

/**
 * @brief Manages game states with stack-based navigation
 * Supports push/pop for overlays and change for transitions
 */
class GameStateManager {
public:
    GameStateManager() = default;
    ~GameStateManager() = default;

    // State management
    void pushState(std::unique_ptr<IGameState> state);
    void popState();
    void changeState(std::unique_ptr<IGameState> state);

    // Convenience methods for state transitions
    void changeToMenu();
    void changeToStage(int levelNumber, bool twoPlayer = false, bool useWaveGenerator = false);
    void changeToPlaying(int levelNumber, bool twoPlayer, bool useWaveGenerator = false);
    void changeToScore(int levelNumber, bool victory, bool twoPlayer = false, bool useWaveGenerator = false);
    void changeToConstruction(int levelNumber = 1);

    // Update and render
    void update(float deltaTime);
    void render(IRenderer& renderer);
    void handleInput(const IInput& input);

    // Access current state
    IGameState* getCurrentState();
    const IGameState* getCurrentState() const;

    // State query
    bool isEmpty() const { return states_.empty(); }
    size_t stateCount() const { return states_.size(); }

    // Player level persistence across level transitions
    int getPlayer1Level() const { return player1Level_; }
    int getPlayer2Level() const { return player2Level_; }
    void setPlayerLevels(int p1Level, int p2Level) {
        player1Level_ = p1Level;
        player2Level_ = p2Level;
    }
    void resetPlayerLevels() {
        player1Level_ = 0;
        player2Level_ = 0;
    }

    // Session scores persist between campaign stages but are not written until
    // they exceed the existing high score.
    void beginRun();
    void addPlayerScore(int playerId, int points);
    int getPlayerScore(int playerId) const;
    void recordEnemyKill(EnemyType type);
    int getEnemyKillCount(EnemyType type) const;

    // Progress is explicitly loaded by Game after services are initialized.
    // This keeps unit tests independent from the repository's real save file.
    bool loadProgress(const std::string& filePath = Constants::Paths::PROGRESS);
    int getHighScore() const { return progress_.highScore; }
    int getCampaignStartLevel() const { return progress_.levelToPlay; }
    GameDifficulty getDifficulty() const { return progress_.difficulty; }
    float getMasterVolume() const { return progress_.masterVolume; }
    void recordHighScore(int score);
    void unlockCampaignLevel(int completedLevel);
    void setDifficulty(GameDifficulty difficulty);
    void setMasterVolume(float volume);

private:
    std::stack<std::unique_ptr<IGameState>> states_;

    // Pending operations (executed at end of frame)
    std::unique_ptr<IGameState> pendingPush_;
    bool pendingPop_ = false;
    std::unique_ptr<IGameState> pendingChange_;

    // Player level persistence (preserved across level transitions)
    int player1Level_ = 0;
    int player2Level_ = 0;
    int player1Score_ = 0;
    int player2Score_ = 0;
    std::array<int, 4> enemyKillCounts_{};

    ProgressStore progressStore_;
    ProgressData progress_;
    bool progressLoaded_ = false;

    void processPendingOperations();
    void saveProgress();
};

} // namespace tank
