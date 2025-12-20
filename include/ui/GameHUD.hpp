#pragma once

#include "utils/Vector2.hpp"
#include "utils/Constants.hpp"
#include <vector>

namespace tank {

class IRenderer;

/**
 * @brief Game HUD (Heads-Up Display) for showing game info
 */
class GameHUD {
public:
    GameHUD();
    ~GameHUD() = default;

    void render(IRenderer& renderer);

    // Update display values
    void setRemainingEnemies(int count) { remainingEnemies_ = count; }
    void setPlayer1Lives(int lives) { player1Lives_ = lives; }
    void setPlayer2Lives(int lives) { player2Lives_ = lives; }
    void setCurrentLevel(int level) { currentLevel_ = level; }
    void setTwoPlayerMode(bool enabled) { twoPlayerMode_ = enabled; }

private:
    int remainingEnemies_;
    int player1Lives_;
    int player2Lives_;
    int currentLevel_;
    bool twoPlayerMode_;

    // UI Layout constants
    static constexpr int PANEL_X = Constants::GAME_WIDTH;
    static constexpr int PANEL_WIDTH = Constants::UI_PANEL_WIDTH;
    static constexpr int ICON_SIZE = 16;
    static constexpr int ICON_SPACING = 4;

    void renderEnemyIcons(IRenderer& renderer);
    void renderPlayerInfo(IRenderer& renderer);
    void renderLevelInfo(IRenderer& renderer);
};

/**
 * @brief Game Over overlay
 */
class GameOverOverlay {
public:
    GameOverOverlay();

    void update(float deltaTime);
    void render(IRenderer& renderer);
    void reset();

    void start();
    bool isAnimationComplete() const { return animationComplete_; }

private:
    bool active_;
    bool animationComplete_;
    float animationProgress_;
    int textY_;

    static constexpr float ANIMATION_SPEED = 100.0f;  // Pixels per second
    static constexpr int START_Y = Constants::WINDOW_HEIGHT;
    static constexpr int END_Y = Constants::WINDOW_HEIGHT / 2 - 20;
};

/**
 * @brief Pause menu overlay
 */
class PauseOverlay {
public:
    PauseOverlay() : active_(false) {}

    void render(IRenderer& renderer);
    void setActive(bool active) { active_ = active; }
    bool isActive() const { return active_; }

private:
    bool active_;
};

} // namespace tank
