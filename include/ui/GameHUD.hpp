#pragma once

#include "utils/Vector2.hpp"
#include "utils/Constants.hpp"
#include <vector>

namespace tank {

class IRenderer;

/**
 * @brief Game HUD (Heads-Up Display) with improved visual design
 *
 * Features:
 * - Animated enemy icons with pulse effect
 * - Health bar with color gradient
 * - Player info section with icons
 * - Level/flag indicator
 */
class GameHUD {
public:
    GameHUD();
    ~GameHUD() = default;

    void update(float deltaTime);
    void render(IRenderer& renderer);

    // Update display values
    void setRemainingEnemies(int count) { remainingEnemies_ = count; }
    void setPlayer1Lives(int lives) { player1Lives_ = lives; }
    void setPlayer2Lives(int lives) { player2Lives_ = lives; }
    void setPlayer1HP(int hp, int maxHP) { player1HP_ = hp; player1MaxHP_ = maxHP; }
    void setPlayer2HP(int hp, int maxHP) { player2HP_ = hp; player2MaxHP_ = maxHP; }
    void setCurrentLevel(int level) { currentLevel_ = level; }
    void setTwoPlayerMode(bool enabled) { twoPlayerMode_ = enabled; }
    void setScore(int score) { score_ = score; }

private:
    int remainingEnemies_;
    int player1Lives_;
    int player2Lives_;
    int player1HP_;
    int player1MaxHP_;
    int player2HP_;
    int player2MaxHP_;
    int currentLevel_;
    int score_;
    bool twoPlayerMode_;

    // Animation state
    float pulseTimer_;
    float pulseAlpha_;

    // UI Layout constants
    static constexpr int PANEL_X = Constants::GAME_WIDTH;
    static constexpr int PANEL_WIDTH = Constants::UI_PANEL_WIDTH;
    static constexpr int ICON_SIZE = 14;
    static constexpr int ICON_SPACING = 3;
    static constexpr int PADDING = 8;

    void renderPanelBackground(IRenderer& renderer);
    void renderEnemyIcons(IRenderer& renderer);
    void renderPlayerInfo(IRenderer& renderer, int playerNum, int x, int y);
    void renderHealthBar(IRenderer& renderer, int x, int y, int hp, int maxHP);
    void renderLevelInfo(IRenderer& renderer);
    void renderScoreDisplay(IRenderer& renderer);

    // Helper to draw a stylized tank icon
    void drawTankIcon(IRenderer& renderer, int x, int y, int size, const Constants::Color& color);
    void drawEnemyIcon(IRenderer& renderer, int x, int y, int size, bool glow);
};

/**
 * @brief Game Over overlay with improved visuals
 */
class GameOverOverlay {
public:
    GameOverOverlay();

    void update(float deltaTime);
    void render(IRenderer& renderer);
    void reset();

    void start();
    bool isAnimationComplete() const { return animationComplete_; }

    enum class MenuItem {
        Restart = 0,
        MainMenu = 1
    };

    void resetSelection();
    void selectNextItem();
    void selectPreviousItem();
    void setSelectedItem(MenuItem item) { selectedItem_ = item; }
    MenuItem getSelectedItem() const { return selectedItem_; }

private:
    bool active_;
    bool animationComplete_;
    float animationProgress_;
    int textY_;
    float glowTimer_;
    MenuItem selectedItem_ = MenuItem::Restart;

    static constexpr float ANIMATION_SPEED = 120.0f;  // Pixels per second
    static constexpr int START_Y = Constants::WINDOW_HEIGHT;
    static constexpr int END_Y = Constants::WINDOW_HEIGHT / 2 - 40;

    void renderGlowingText(IRenderer& renderer, const std::string& text,
                           int x, int y, int fontSize, const Constants::Color& color);
};

/**
 * @brief Pause menu overlay with modern design
 */
class PauseOverlay {
public:
    enum class MenuItem {
        Continue = 0,
        Restart = 1,
        MainMenu = 2
    };

    PauseOverlay() : active_(false), fadeAlpha_(0.0f), slideOffset_(0.0f) {}

    void update(float deltaTime);
    void render(IRenderer& renderer);
    void setActive(bool active);
    bool isActive() const { return active_; }

    void resetSelection();
    void selectNextItem();
    void selectPreviousItem();
    void setSelectedItem(MenuItem item) { selectedItem_ = item; }
    MenuItem getSelectedItem() const { return selectedItem_; }

private:
    bool active_;
    float fadeAlpha_;
    float slideOffset_;
    MenuItem selectedItem_ = MenuItem::Continue;

    void renderMenuItem(IRenderer& renderer, const char* text, int x, int y,
                        bool selected, float offset);
};

} // namespace tank
