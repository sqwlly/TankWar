#include "ui/GameHUD.hpp"
#include "rendering/IRenderer.hpp"
#include "graphics/SpriteSheet.hpp"
#include <algorithm>
#include <cmath>

namespace tank {

// ============================================================================
// GameHUD Implementation - Using actual sprite sheet icons
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
    // Animate pulse effect
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
    // Main panel background - classic gray like original game
    renderer.drawRect(PANEL_X, 0, PANEL_WIDTH, Constants::WINDOW_HEIGHT,
                     Constants::COLOR_GRAY.r,
                     Constants::COLOR_GRAY.g,
                     Constants::COLOR_GRAY.b, 255);

    // Left border highlight
    renderer.drawRect(PANEL_X, 0, 2, Constants::WINDOW_HEIGHT,
                     120, 120, 120, 255);
}

void GameHUD::renderEnemyIcons(IRenderer& renderer) {
    // Draw enemy icons in a 2-column grid using actual sprite
    int startX = PANEL_X + PADDING;
    int startY = 24;
    int cols = 2;

    // Get enemy icon sprite from sheet
    Rectangle enemySprite = Sprites::UI::getEnemyIcon();

    for (int i = 0; i < remainingEnemies_; ++i) {
        int col = i % cols;
        int row = i / cols;
        int x = startX + col * (ICON_SIZE + ICON_SPACING);
        int y = startY + row * (ICON_SIZE + ICON_SPACING);

        // Draw actual enemy icon from sprite sheet
        renderer.drawSprite(
            static_cast<int>(enemySprite.x), static_cast<int>(enemySprite.y),
            static_cast<int>(enemySprite.width), static_cast<int>(enemySprite.height),
            x, y, ICON_SIZE, ICON_SIZE
        );
    }
}

void GameHUD::renderPlayerInfo(IRenderer& renderer, int playerNum, int x, int y) {
    // Get life icon sprite from sheet
    Rectangle lifeSprite = Sprites::UI::getLifeIcon();

    int lives = (playerNum == 1) ? player1Lives_ : player2Lives_;
    int hp = (playerNum == 1) ? player1HP_ : player2HP_;
    int maxHP = (playerNum == 1) ? player1MaxHP_ : player2MaxHP_;

    // Player label (IP or IIP style like original)
    std::string label = (playerNum == 1) ? "IP" : "IIP";
    renderer.drawText(label,
                     Vector2(static_cast<float>(x), static_cast<float>(y)),
                     Constants::COLOR_BLACK, 12);

    // Life icon from sprite sheet
    renderer.drawSprite(
        static_cast<int>(lifeSprite.x), static_cast<int>(lifeSprite.y),
        static_cast<int>(lifeSprite.width), static_cast<int>(lifeSprite.height),
        x, y + 18, ICON_SIZE, ICON_SIZE
    );

    // Lives count
    renderer.drawText(std::to_string(lives),
                     Vector2(static_cast<float>(x + ICON_SIZE + 4), static_cast<float>(y + 18)),
                     Constants::COLOR_BLACK, 14);

    // Health bar below (optional enhancement)
    renderHealthBar(renderer, x, y + 38, hp, maxHP);
}

void GameHUD::renderHealthBar(IRenderer& renderer, int x, int y, int hp, int maxHP) {
    const int barWidth = 48;
    const int barHeight = 4;

    // Background
    renderer.drawRect(x, y, barWidth, barHeight, 40, 40, 40, 255);

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
}

void GameHUD::renderLevelInfo(IRenderer& renderer) {
    int x = PANEL_X + PADDING;
    int y = Constants::WINDOW_HEIGHT - 44;

    // Flag icon from sprite sheet
    Rectangle flagSprite = Sprites::UI::getFlag();

    renderer.drawSprite(
        static_cast<int>(flagSprite.x), static_cast<int>(flagSprite.y),
        static_cast<int>(flagSprite.width), static_cast<int>(flagSprite.height),
        x, y, 20, 20
    );

    // Level number
    renderer.drawText(std::to_string(currentLevel_),
                     Vector2(static_cast<float>(x + 24), static_cast<float>(y + 2)),
                     Constants::COLOR_BLACK, 14);
}

void GameHUD::renderScoreDisplay(IRenderer& renderer) {
    // Optional: render score at top of panel
    renderer.drawText("HI-",
                     Vector2(static_cast<float>(PANEL_X + PADDING), 4.0f),
                     Constants::COLOR_BLACK, 10);

    renderer.drawText(std::to_string(score_),
                     Vector2(static_cast<float>(PANEL_X + PADDING + 20), 4.0f),
                     Constants::COLOR_BLACK, 10);
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

    // Semi-transparent overlay
    renderer.drawRect(0, 0, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT,
                     0, 0, 0, 180);

    // "GAME OVER" text
    int textX = Constants::WINDOW_WIDTH / 2 - 80;

    renderer.drawText("GAME", Vector2(static_cast<float>(textX), static_cast<float>(textY_)),
                     Constants::COLOR_RED, 24);
    renderer.drawText("OVER", Vector2(static_cast<float>(textX + 90), static_cast<float>(textY_)),
                     Constants::COLOR_RED, 24);

    if (animationComplete_) {
        const char* items[] = {"RESTART", "MAIN MENU"};
        const int selectedIndex = static_cast<int>(selectedItem_);
        const int menuX = Constants::WINDOW_WIDTH / 2 - 50;
        const int menuY = textY_ + 50;
        constexpr int ITEM_HEIGHT = 24;

        for (int i = 0; i < 2; ++i) {
            bool selected = (i == selectedIndex);
            Constants::Color color = selected ? Constants::COLOR_WHITE : Constants::COLOR_GRAY;

            if (selected) {
                renderer.drawText(">",
                    Vector2(static_cast<float>(menuX - 18), static_cast<float>(menuY + i * ITEM_HEIGHT)),
                    color, 16);
            }

            renderer.drawText(items[i],
                Vector2(static_cast<float>(menuX), static_cast<float>(menuY + i * ITEM_HEIGHT)),
                color, 16);
        }

        renderer.drawText("UP/DOWN: select  ENTER: confirm",
            Vector2(static_cast<float>(menuX - 60), static_cast<float>(menuY + 2 * ITEM_HEIGHT + 15)),
            Constants::COLOR_GRAY, 11);
    }
}

void GameOverOverlay::renderGlowingText(IRenderer& renderer, const std::string& text,
                                         int x, int y, int fontSize, const Constants::Color& color) {
    renderer.drawText(text, Vector2(static_cast<float>(x), static_cast<float>(y)), color, fontSize);
}

// ============================================================================
// PauseOverlay Implementation
// ============================================================================

void PauseOverlay::update(float deltaTime) {
    if (active_) {
        if (fadeAlpha_ < 1.0f) {
            fadeAlpha_ += deltaTime * 5.0f;
            if (fadeAlpha_ > 1.0f) fadeAlpha_ = 1.0f;
        }
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

    int textX = Constants::WINDOW_WIDTH / 2 - 50;
    int textY = Constants::WINDOW_HEIGHT / 2 - 80;

    // "PAUSE" text
    renderer.drawText("PAUSE",
                     Vector2(static_cast<float>(textX), static_cast<float>(textY)),
                     Constants::COLOR_WHITE, 28);

    // Menu items
    const char* items[] = {"CONTINUE", "RESTART", "MAIN MENU"};
    const int selectedIndex = static_cast<int>(selectedItem_);
    const int menuX = Constants::WINDOW_WIDTH / 2 - 60;
    const int menuY = textY + 50;
    constexpr int ITEM_HEIGHT = 24;

    for (int i = 0; i < 3; ++i) {
        renderMenuItem(renderer, items[i], menuX, menuY + i * ITEM_HEIGHT,
                       i == selectedIndex, 0.0f);
    }

    // Help text
    renderer.drawText("UP/DOWN: select  ENTER: confirm  ESC: resume",
        Vector2(static_cast<float>(menuX - 80), static_cast<float>(menuY + 3 * ITEM_HEIGHT + 15)),
        Constants::COLOR_GRAY, 11);
}

void PauseOverlay::renderMenuItem(IRenderer& renderer, const char* text, int x, int y,
                                   bool selected, float offset) {
    Constants::Color color = selected ? Constants::COLOR_WHITE : Constants::COLOR_GRAY;

    if (selected) {
        renderer.drawText(">", Vector2(static_cast<float>(x - 18), static_cast<float>(y)), color, 16);
    }

    renderer.drawText(text, Vector2(static_cast<float>(x), static_cast<float>(y)), color, 16);
}

} // namespace tank
