#include "ui/GameHUD.hpp"
#include "rendering/IRenderer.hpp"
#include <algorithm>
#include <cmath>

namespace tank {

// ============================================================================
// GameHUD Implementation
// ============================================================================

GameHUD::GameHUD()
    : remainingEnemies_(20)
    , player1Lives_(3)
    , player2Lives_(3)
    , player1HP_(100)
    , player1MaxHP_(100)
    , player2HP_(100)
    , player2MaxHP_(100)
    , currentLevel_(1)
    , score_(0)
    , twoPlayerMode_(false)
    , pulseTimer_(0.0f)
    , pulseAlpha_(1.0f)
{
}

void GameHUD::update(float deltaTime) {
    // Animate pulse effect for enemy icons
    pulseTimer_ += deltaTime * 3.0f;
    pulseAlpha_ = 0.7f + 0.3f * std::sin(pulseTimer_);
}

void GameHUD::render(IRenderer& renderer) {
    renderPanelBackground(renderer);
    renderEnemyIcons(renderer);
    renderPlayerInfo(renderer, 1, PANEL_X + PADDING, Constants::WINDOW_HEIGHT - 130);
    if (twoPlayerMode_) {
        renderPlayerInfo(renderer, 2, PANEL_X + PADDING, Constants::WINDOW_HEIGHT - 70);
    }
    renderLevelInfo(renderer);
}

void GameHUD::renderPanelBackground(IRenderer& renderer) {
    // Main panel with gradient effect (dark to darker)
    const auto& bgColor = Constants::UIColors::BG_PANEL;

    // Background
    renderer.drawRect(PANEL_X, 0, PANEL_WIDTH, Constants::WINDOW_HEIGHT,
                     bgColor.r, bgColor.g, bgColor.b, 255);

    // Left border highlight
    renderer.drawRect(PANEL_X, 0, 2, Constants::WINDOW_HEIGHT,
                     60, 65, 75, 255);

    // Subtle top accent line
    renderer.drawRect(PANEL_X + 2, 0, PANEL_WIDTH - 2, 2,
                     Constants::UIColors::PRIMARY.r,
                     Constants::UIColors::PRIMARY.g,
                     Constants::UIColors::PRIMARY.b, 100);
}

void GameHUD::renderEnemyIcons(IRenderer& renderer) {
    // Section title
    renderer.drawText("ENEMY",
                     Vector2(static_cast<float>(PANEL_X + PADDING), 8.0f),
                     Constants::UIColors::MENU_NORMAL, 10);

    // Draw enemy icons in a 2-column grid
    int startX = PANEL_X + PADDING;
    int startY = 28;
    int cols = 2;

    for (int i = 0; i < remainingEnemies_; ++i) {
        int col = i % cols;
        int row = i / cols;
        int x = startX + col * (ICON_SIZE + ICON_SPACING);
        int y = startY + row * (ICON_SIZE + ICON_SPACING);

        // Draw with pulse glow effect
        bool shouldGlow = (i < 4);  // First 4 enemies glow (next wave)
        drawEnemyIcon(renderer, x, y, ICON_SIZE, shouldGlow);
    }
}

void GameHUD::drawEnemyIcon(IRenderer& renderer, int x, int y, int size, bool glow) {
    // Glow effect for active enemies
    if (glow) {
        uint8_t alpha = static_cast<uint8_t>(80 * pulseAlpha_);
        renderer.drawRect(x - 2, y - 2, size + 4, size + 4,
                         Constants::UIColors::ENEMY_ICON_GLOW.r,
                         Constants::UIColors::ENEMY_ICON_GLOW.g,
                         Constants::UIColors::ENEMY_ICON_GLOW.b, alpha);
    }

    // Tank body
    renderer.drawRect(x, y + 3, size, size - 5,
                     Constants::UIColors::ENEMY_ICON.r,
                     Constants::UIColors::ENEMY_ICON.g,
                     Constants::UIColors::ENEMY_ICON.b, 255);

    // Turret
    int turretWidth = size / 3;
    int turretX = x + (size - turretWidth) / 2;
    renderer.drawRect(turretX, y, turretWidth, size / 2 + 2,
                     Constants::UIColors::ENEMY_ICON.r + 30,
                     Constants::UIColors::ENEMY_ICON.g,
                     Constants::UIColors::ENEMY_ICON.b, 255);
}

void GameHUD::renderPlayerInfo(IRenderer& renderer, int playerNum, int x, int y) {
    const Constants::Color& playerColor = (playerNum == 1)
        ? Constants::UIColors::PLAYER1
        : Constants::UIColors::PLAYER2;

    int hp = (playerNum == 1) ? player1HP_ : player2HP_;
    int maxHP = (playerNum == 1) ? player1MaxHP_ : player2MaxHP_;
    int lives = (playerNum == 1) ? player1Lives_ : player2Lives_;

    // Player label (P1 or P2)
    std::string label = "P" + std::to_string(playerNum);
    renderer.drawText(label,
                     Vector2(static_cast<float>(x), static_cast<float>(y)),
                     playerColor, 12);

    // Tank icon
    drawTankIcon(renderer, x + 24, y, 16, playerColor);

    // Lives count
    renderer.drawText("x" + std::to_string(lives),
                     Vector2(static_cast<float>(x + 42), static_cast<float>(y)),
                     Constants::COLOR_WHITE, 12);

    // Health bar below
    renderHealthBar(renderer, x, y + 18, hp, maxHP);
}

void GameHUD::drawTankIcon(IRenderer& renderer, int x, int y, int size, const Constants::Color& color) {
    // Tank body
    renderer.drawRect(x + 2, y + 4, size - 4, size - 6, color.r, color.g, color.b, 255);

    // Turret
    int turretW = size / 4;
    renderer.drawRect(x + (size - turretW) / 2, y + 2, turretW, size / 2,
                     color.r, color.g, color.b, 255);

    // Tracks
    renderer.drawRect(x, y + 4, 2, size - 6, color.r - 40, color.g - 40, color.b - 40, 255);
    renderer.drawRect(x + size - 2, y + 4, 2, size - 6, color.r - 40, color.g - 40, color.b - 40, 255);
}

void GameHUD::renderHealthBar(IRenderer& renderer, int x, int y, int hp, int maxHP) {
    const int barWidth = 52;
    const int barHeight = 5;

    // Background
    renderer.drawRect(x, y, barWidth, barHeight,
                     Constants::UIColors::HP_BG.r,
                     Constants::UIColors::HP_BG.g,
                     Constants::UIColors::HP_BG.b, 255);

    // Health fill
    float hpRatio = static_cast<float>(hp) / static_cast<float>(maxHP);
    int fillWidth = static_cast<int>(barWidth * hpRatio);

    // Color based on health percentage
    const Constants::Color& hpColor = (hpRatio > 0.6f) ? Constants::UIColors::HP_HIGH
                                    : (hpRatio > 0.3f) ? Constants::UIColors::HP_MED
                                    : Constants::UIColors::HP_LOW;

    if (fillWidth > 0) {
        renderer.drawRect(x, y, fillWidth, barHeight,
                         hpColor.r, hpColor.g, hpColor.b, 255);
    }

    // Border
    renderer.drawRect(x, y, barWidth, 1, 80, 80, 80, 255);  // Top
    renderer.drawRect(x, y + barHeight - 1, barWidth, 1, 80, 80, 80, 255);  // Bottom
}

void GameHUD::renderLevelInfo(IRenderer& renderer) {
    int x = PANEL_X + PADDING;
    int y = Constants::WINDOW_HEIGHT - 36;

    // Flag icon (stylized)
    // Pole
    renderer.drawRect(x + 2, y, 2, 20, 100, 100, 100, 255);
    // Flag
    renderer.drawRect(x + 4, y, 14, 10,
                     Constants::UIColors::ACCENT.r,
                     Constants::UIColors::ACCENT.g,
                     Constants::UIColors::ACCENT.b, 255);
    // Flag wave effect
    renderer.drawRect(x + 4, y + 3, 12, 4,
                     Constants::UIColors::ACCENT.r - 30,
                     Constants::UIColors::ACCENT.g - 20,
                     Constants::UIColors::ACCENT.b - 20, 255);

    // Level number
    renderer.drawText(std::to_string(currentLevel_),
                     Vector2(static_cast<float>(x + 22), static_cast<float>(y + 2)),
                     Constants::COLOR_WHITE, 14);
}

void GameHUD::renderScoreDisplay(IRenderer& renderer) {
    // Optional: render score at top of panel
    renderer.drawText("SCORE",
                     Vector2(static_cast<float>(PANEL_X + PADDING), 200.0f),
                     Constants::UIColors::MENU_NORMAL, 10);

    renderer.drawText(std::to_string(score_),
                     Vector2(static_cast<float>(PANEL_X + PADDING), 214.0f),
                     Constants::UIColors::PRIMARY, 12);
}

// ============================================================================
// GameOverOverlay Implementation
// ============================================================================

GameOverOverlay::GameOverOverlay()
    : active_(false)
    , animationComplete_(false)
    , animationProgress_(0.0f)
    , textY_(START_Y)
    , glowTimer_(0.0f)
{
}

void GameOverOverlay::start() {
    active_ = true;
    animationComplete_ = false;
    animationProgress_ = 0.0f;
    textY_ = START_Y;
    glowTimer_ = 0.0f;
    resetSelection();
}

void GameOverOverlay::reset() {
    active_ = false;
    animationComplete_ = false;
    animationProgress_ = 0.0f;
    textY_ = START_Y;
    glowTimer_ = 0.0f;
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
    if (!active_) return;

    glowTimer_ += deltaTime * 4.0f;

    if (!animationComplete_) {
        textY_ -= static_cast<int>(ANIMATION_SPEED * deltaTime);
        if (textY_ <= END_Y) {
            textY_ = END_Y;
            animationComplete_ = true;
        }
    }
}

void GameOverOverlay::render(IRenderer& renderer) {
    if (!active_) return;

    // Dark overlay with vignette effect
    renderer.drawRect(0, 0, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT,
                     0, 0, 0, 200);

    // "GAME OVER" text with glow
    int textX = Constants::WINDOW_WIDTH / 2 - 100;

    // Glow effect
    uint8_t glowAlpha = static_cast<uint8_t>(40 + 30 * std::sin(glowTimer_));
    renderer.drawRect(textX - 20, textY_ - 10, 240, 50,
                     255, 50, 50, glowAlpha);

    // Main text
    renderer.drawText("GAME", Vector2(static_cast<float>(textX), static_cast<float>(textY_)),
                     Constants::Color(255, 60, 60), 28);
    renderer.drawText("OVER", Vector2(static_cast<float>(textX + 100), static_cast<float>(textY_)),
                     Constants::Color(255, 60, 60), 28);

    if (animationComplete_) {
        const char* items[] = {"RESTART", "MAIN MENU"};
        const int selectedIndex = static_cast<int>(selectedItem_);
        const int menuX = Constants::WINDOW_WIDTH / 2 - 60;
        const int menuY = textY_ + 60;
        constexpr int ITEM_HEIGHT = 28;

        for (int i = 0; i < 2; ++i) {
            bool selected = (i == selectedIndex);
            Constants::Color color = selected
                ? Constants::UIColors::MENU_HIGHLIGHT
                : Constants::UIColors::MENU_NORMAL;

            if (selected) {
                // Selection indicator with animation
                float bounce = std::sin(glowTimer_ * 2.0f) * 2.0f;
                renderer.drawText(">",
                    Vector2(static_cast<float>(menuX - 20 + bounce), static_cast<float>(menuY + i * ITEM_HEIGHT)),
                    color, 18);

                // Highlight bar
                renderer.drawRect(menuX - 5, menuY + i * ITEM_HEIGHT - 2, 130, 22,
                                 color.r, color.g, color.b, 30);
            }

            renderer.drawText(items[i],
                Vector2(static_cast<float>(menuX), static_cast<float>(menuY + i * ITEM_HEIGHT)),
                color, 18);
        }

        // Help text
        renderer.drawText("UP/DOWN: select  ENTER: confirm",
            Vector2(static_cast<float>(menuX - 50), static_cast<float>(menuY + 2 * ITEM_HEIGHT + 20)),
            Constants::Color(120, 120, 120), 12);
    }
}

void GameOverOverlay::renderGlowingText(IRenderer& renderer, const std::string& text,
                                         int x, int y, int fontSize, const Constants::Color& color) {
    // Draw glow layers
    uint8_t glowAlpha = static_cast<uint8_t>(40 + 20 * std::sin(glowTimer_));
    Constants::Color glowColor(color.r, color.g, color.b, glowAlpha);

    for (int offset = 2; offset > 0; --offset) {
        renderer.drawText(text, Vector2(static_cast<float>(x - offset), static_cast<float>(y)), glowColor, fontSize);
        renderer.drawText(text, Vector2(static_cast<float>(x + offset), static_cast<float>(y)), glowColor, fontSize);
    }

    // Main text
    renderer.drawText(text, Vector2(static_cast<float>(x), static_cast<float>(y)), color, fontSize);
}

// ============================================================================
// PauseOverlay Implementation
// ============================================================================

void PauseOverlay::update(float deltaTime) {
    if (active_) {
        // Fade in
        if (fadeAlpha_ < 1.0f) {
            fadeAlpha_ += deltaTime * 5.0f;
            if (fadeAlpha_ > 1.0f) fadeAlpha_ = 1.0f;
        }
        // Slide in
        if (slideOffset_ > 0.0f) {
            slideOffset_ -= deltaTime * 400.0f;
            if (slideOffset_ < 0.0f) slideOffset_ = 0.0f;
        }
    } else {
        fadeAlpha_ = 0.0f;
        slideOffset_ = 50.0f;
    }
}

void PauseOverlay::setActive(bool active) {
    if (active && !active_) {
        fadeAlpha_ = 0.0f;
        slideOffset_ = 50.0f;
        resetSelection();
    }
    active_ = active;
}

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
    if (!active_ && fadeAlpha_ <= 0.0f) return;

    // Dark overlay
    uint8_t overlayAlpha = static_cast<uint8_t>(180 * fadeAlpha_);
    renderer.drawRect(0, 0, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT,
                     0, 0, 0, overlayAlpha);

    int textX = Constants::WINDOW_WIDTH / 2 - 60;
    int textY = Constants::WINDOW_HEIGHT / 2 - 80;
    int offset = static_cast<int>(slideOffset_);

    // Title box
    renderer.drawRect(textX - 20 - offset, textY - 10, 160, 40,
                     Constants::UIColors::BG_PANEL.r,
                     Constants::UIColors::BG_PANEL.g,
                     Constants::UIColors::BG_PANEL.b,
                     static_cast<uint8_t>(240 * fadeAlpha_));

    // Border accent
    renderer.drawRect(textX - 20 - offset, textY - 10, 160, 3,
                     Constants::UIColors::PRIMARY.r,
                     Constants::UIColors::PRIMARY.g,
                     Constants::UIColors::PRIMARY.b,
                     static_cast<uint8_t>(255 * fadeAlpha_));

    // "PAUSE" text
    uint8_t textAlpha = static_cast<uint8_t>(255 * fadeAlpha_);
    renderer.drawText("PAUSED",
                     Vector2(static_cast<float>(textX - offset), static_cast<float>(textY)),
                     Constants::Color(255, 255, 255, textAlpha), 24);

    // Menu items
    const char* items[] = {"CONTINUE", "RESTART", "MAIN MENU"};
    const int selectedIndex = static_cast<int>(selectedItem_);
    const int menuX = Constants::WINDOW_WIDTH / 2 - 60;
    const int menuY = textY + 50;
    constexpr int ITEM_HEIGHT = 26;

    for (int i = 0; i < 3; ++i) {
        renderMenuItem(renderer, items[i], menuX, menuY + i * ITEM_HEIGHT,
                       i == selectedIndex, static_cast<float>(offset));
    }

    // Help text
    uint8_t helpAlpha = static_cast<uint8_t>(180 * fadeAlpha_);
    renderer.drawText("UP/DOWN: select  ENTER: confirm  ESC: resume",
        Vector2(static_cast<float>(menuX - 80 - offset), static_cast<float>(menuY + 3 * ITEM_HEIGHT + 15)),
        Constants::Color(100, 100, 100, helpAlpha), 11);
}

void PauseOverlay::renderMenuItem(IRenderer& renderer, const char* text, int x, int y,
                                   bool selected, float offset) {
    int adjustedX = x - static_cast<int>(offset);
    uint8_t alpha = static_cast<uint8_t>(255 * fadeAlpha_);

    Constants::Color color = selected
        ? Constants::Color(Constants::UIColors::MENU_HIGHLIGHT.r,
                          Constants::UIColors::MENU_HIGHLIGHT.g,
                          Constants::UIColors::MENU_HIGHLIGHT.b, alpha)
        : Constants::Color(Constants::UIColors::MENU_NORMAL.r,
                          Constants::UIColors::MENU_NORMAL.g,
                          Constants::UIColors::MENU_NORMAL.b, alpha);

    if (selected) {
        // Selection highlight bar
        renderer.drawRect(adjustedX - 5, y - 2, 140, 22,
                         Constants::UIColors::PRIMARY.r,
                         Constants::UIColors::PRIMARY.g,
                         Constants::UIColors::PRIMARY.b,
                         static_cast<uint8_t>(40 * fadeAlpha_));

        // Arrow indicator
        renderer.drawText(">", Vector2(static_cast<float>(adjustedX - 18), static_cast<float>(y)), color, 16);
    }

    renderer.drawText(text, Vector2(static_cast<float>(adjustedX), static_cast<float>(y)), color, 16);
}

} // namespace tank
