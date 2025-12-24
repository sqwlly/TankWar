#include "ui/GameHUD.hpp"
#include "rendering/IRenderer.hpp"
#include <algorithm>

namespace tank {

// GameHUD implementation
GameHUD::GameHUD()
    : remainingEnemies_(20)
    , player1Lives_(3)
    , player2Lives_(3)
    , currentLevel_(1)
    , twoPlayerMode_(false)
{
}

void GameHUD::render(IRenderer& renderer) {
    // Draw panel background
    renderer.drawRect(PANEL_X, 0, PANEL_WIDTH, Constants::WINDOW_HEIGHT,
                     99, 99, 99, 255);  // Gray background

    renderEnemyIcons(renderer);
    renderPlayerInfo(renderer);
    renderLevelInfo(renderer);
}

void GameHUD::renderEnemyIcons(IRenderer& renderer) {
    // Draw enemy icons (2 columns)
    int startX = PANEL_X + 10;
    int startY = 30;
    int cols = 2;

    for (int i = 0; i < remainingEnemies_; ++i) {
        int col = i % cols;
        int row = i / cols;
        int x = startX + col * (ICON_SIZE + ICON_SPACING);
        int y = startY + row * (ICON_SIZE + ICON_SPACING);

        // Draw enemy icon (small tank silhouette)
        renderer.drawRect(x, y, ICON_SIZE, ICON_SIZE, 0, 0, 0, 255);
        renderer.drawRect(x + 2, y + 4, ICON_SIZE - 4, ICON_SIZE - 6, 50, 50, 50, 255);
        renderer.drawRect(x + 6, y + 2, 4, 4, 50, 50, 50, 255);  // Turret
    }
}

void GameHUD::renderPlayerInfo(IRenderer& renderer) {
    int x = PANEL_X + 10;
    int y = Constants::WINDOW_HEIGHT - 120;

    // Player 1
    // "IP" label
    renderer.drawRect(x, y, 8, 16, 0, 0, 0, 255);  // I
    renderer.drawRect(x + 12, y, 12, 16, 0, 0, 0, 255);  // P

    // Player icon
    renderer.drawRect(x, y + 22, 16, 16, 0, 0, 0, 255);
    renderer.drawRect(x + 2, y + 24, 12, 12, 255, 200, 0, 255);

    // Lives count
    renderer.drawText(std::to_string(player1Lives_),
                     Vector2(static_cast<float>(x + 24), static_cast<float>(y + 20)),
                     Constants::COLOR_BLACK, 16);

    if (twoPlayerMode_) {
        y += 50;

        // Player 2 - "IIP"
        renderer.drawRect(x, y, 8, 16, 0, 0, 0, 255);  // I
        renderer.drawRect(x + 10, y, 8, 16, 0, 0, 0, 255);  // I
        renderer.drawRect(x + 20, y, 12, 16, 0, 0, 0, 255);  // P

        // Player 2 icon
        renderer.drawRect(x, y + 22, 16, 16, 0, 0, 0, 255);
        renderer.drawRect(x + 2, y + 24, 12, 12, 0, 200, 0, 255);

        // Lives count
        renderer.drawText(std::to_string(player2Lives_),
                         Vector2(static_cast<float>(x + 24), static_cast<float>(y + 20)),
                         Constants::COLOR_BLACK, 16);
    }
}

void GameHUD::renderLevelInfo(IRenderer& renderer) {
    int x = PANEL_X + 10;
    int y = Constants::WINDOW_HEIGHT - 50;

    // Flag icon
    renderer.drawRect(x, y, 20, 20, 0, 0, 0, 255);
    renderer.drawRect(x + 2, y + 2, 16, 10, 255, 100, 0, 255);  // Flag
    renderer.drawRect(x + 2, y + 2, 2, 16, 0, 0, 0, 255);  // Pole

    // Level number
    renderer.drawText(std::to_string(currentLevel_),
                     Vector2(static_cast<float>(x + 26), static_cast<float>(y)),
                     Constants::COLOR_BLACK, 16);
}

// GameOverOverlay implementation
GameOverOverlay::GameOverOverlay()
    : active_(false)
    , animationComplete_(false)
    , animationProgress_(0.0f)
    , textY_(START_Y)
{
}

void GameOverOverlay::start() {
    active_ = true;
    animationComplete_ = false;
    animationProgress_ = 0.0f;
    textY_ = START_Y;
    resetSelection();
}

void GameOverOverlay::reset() {
    active_ = false;
    animationComplete_ = false;
    animationProgress_ = 0.0f;
    textY_ = START_Y;
    resetSelection();
}

void GameOverOverlay::resetSelection() {
    selectedItem_ = MenuItem::Restart;
}

void GameOverOverlay::selectNextItem() {
    selectedItem_ = (selectedItem_ == MenuItem::Restart) ? MenuItem::MainMenu : MenuItem::Restart;
}

void GameOverOverlay::selectPreviousItem() {
    selectedItem_ = (selectedItem_ == MenuItem::Restart) ? MenuItem::MainMenu : MenuItem::Restart;
}

void GameOverOverlay::update(float deltaTime) {
    if (!active_ || animationComplete_) return;

    textY_ -= static_cast<int>(ANIMATION_SPEED * deltaTime);

    if (textY_ <= END_Y) {
        textY_ = END_Y;
        animationComplete_ = true;
    }
}

void GameOverOverlay::render(IRenderer& renderer) {
    if (!active_) return;

    // Semi-transparent overlay
    renderer.drawRect(0, 0, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT,
                     0, 0, 0, 128);

    // "GAME OVER" text
    int textX = Constants::WINDOW_WIDTH / 2 - 80;

    // Red "GAME" text
    renderer.drawText("GAME", Vector2(static_cast<float>(textX), static_cast<float>(textY_)),
                     Constants::Color(255, 0, 0), 24);

    // "OVER" text
    renderer.drawText("OVER", Vector2(static_cast<float>(textX + 90), static_cast<float>(textY_)),
                     Constants::Color(255, 0, 0), 24);

    if (animationComplete_) {
        const char* items[] = {"RESTART", "MAIN MENU"};
        const int selectedIndex = static_cast<int>(selectedItem_);
        const int menuX = Constants::WINDOW_WIDTH / 2 - 50;
        const int menuY = textY_ + 50;
        constexpr int ITEM_HEIGHT = 22;

        for (int i = 0; i < 2; ++i) {
            Constants::Color color = (i == selectedIndex) ? Constants::COLOR_WHITE : Constants::COLOR_GRAY;
            if (i == selectedIndex) {
                renderer.drawText(">", Vector2(static_cast<float>(menuX - 18), static_cast<float>(menuY + i * ITEM_HEIGHT)),
                                 color, 18);
            }
            renderer.drawText(items[i],
                             Vector2(static_cast<float>(menuX), static_cast<float>(menuY + i * ITEM_HEIGHT)),
                             color, 18);
        }

        renderer.drawText("UP/DOWN: select  ENTER: confirm  ESC: menu",
                         Vector2(static_cast<float>(menuX - 120), static_cast<float>(menuY + 2 * ITEM_HEIGHT + 18)),
                         Constants::COLOR_GRAY, 14);
    }
}

// PauseOverlay implementation
void PauseOverlay::resetSelection() {
    selectedItem_ = MenuItem::Continue;
}

void PauseOverlay::selectNextItem() {
    int current = static_cast<int>(selectedItem_);
    current = (current + 1) % 3;
    selectedItem_ = static_cast<MenuItem>(current);
}

void PauseOverlay::selectPreviousItem() {
    int current = static_cast<int>(selectedItem_);
    current = (current - 1 + 3) % 3;
    selectedItem_ = static_cast<MenuItem>(current);
}

void PauseOverlay::render(IRenderer& renderer) {
    if (!active_) return;

    // Dark overlay
    renderer.drawRect(0, 0, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT,
                     0, 0, 0, 180);

    // "PAUSE" text centered
    int textX = Constants::WINDOW_WIDTH / 2 - 50;
    int textY = Constants::WINDOW_HEIGHT / 2 - 80;

    renderer.drawText("PAUSE", Vector2(static_cast<float>(textX), static_cast<float>(textY)),
                     Constants::Color(255, 255, 255), 28);

    const char* items[] = {"CONTINUE", "RESTART", "MAIN MENU"};
    const int selectedIndex = static_cast<int>(selectedItem_);
    const int menuX = Constants::WINDOW_WIDTH / 2 - 60;
    const int menuY = textY + 50;
    constexpr int ITEM_HEIGHT = 22;

    for (int i = 0; i < 3; ++i) {
        Constants::Color color = (i == selectedIndex) ? Constants::COLOR_WHITE : Constants::COLOR_GRAY;
        if (i == selectedIndex) {
            renderer.drawText(">", Vector2(static_cast<float>(menuX - 18), static_cast<float>(menuY + i * ITEM_HEIGHT)),
                             color, 18);
        }
        renderer.drawText(items[i],
                         Vector2(static_cast<float>(menuX), static_cast<float>(menuY + i * ITEM_HEIGHT)),
                         color, 18);
    }

    renderer.drawText("UP/DOWN: select  ENTER: confirm  ESC: resume",
                     Vector2(static_cast<float>(menuX - 120), static_cast<float>(menuY + 3 * ITEM_HEIGHT + 18)),
                     Constants::COLOR_GRAY, 14);
}

} // namespace tank
