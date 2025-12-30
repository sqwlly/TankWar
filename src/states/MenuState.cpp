#include "states/MenuState.hpp"
#include "states/GameStateManager.hpp"
#include "graphics/SpriteSheet.hpp"
#include "input/IInput.hpp"
#include <iostream>
#include <cmath>

namespace tank {

MenuState::MenuState(GameStateManager& manager)
    : stateManager_(manager)
{
}

void MenuState::enter() {
    std::cout << "Entering Menu State" << std::endl;
    selectedItem_ = MenuItem::Campaign;
    twoPlayerMode_ = false;
    animTimer_ = 0.0f;
    fadeAlpha_ = 0.0f;
    titleBounce_ = 0.0f;
    cursorPulse_ = 0.0f;
}

void MenuState::exit() {
    std::cout << "Exiting Menu State" << std::endl;
}

void MenuState::update(float deltaTime) {
    // Animation timers
    animTimer_ += deltaTime;
    cursorPulse_ += deltaTime * 5.0f;

    // Fade in effect
    if (fadeAlpha_ < 1.0f) {
        fadeAlpha_ += deltaTime * 2.0f;
        if (fadeAlpha_ > 1.0f) fadeAlpha_ = 1.0f;
    }

    // Title bounce effect
    titleBounce_ = std::sin(animTimer_ * 2.0f) * 3.0f;
}

void MenuState::render(IRenderer& renderer) {
    renderBackground(renderer);
    renderDecorations(renderer);
    renderTitle(renderer);
    renderMenuItems(renderer);
    renderFooter(renderer);
}

void MenuState::renderBackground(IRenderer& renderer) {
    // Dark gradient background
    renderer.clear(
        Constants::UIColors::BG_DARK.r,
        Constants::UIColors::BG_DARK.g,
        Constants::UIColors::BG_DARK.b, 255);

    // Subtle grid pattern for retro feel
    const int gridSize = 32;
    for (int y = 0; y < Constants::WINDOW_HEIGHT; y += gridSize) {
        for (int x = 0; x < Constants::WINDOW_WIDTH; x += gridSize) {
            uint8_t alpha = static_cast<uint8_t>(8 + 4 * std::sin(animTimer_ + x * 0.01f + y * 0.01f));
            renderer.drawRect(x, y, gridSize, 1, 60, 60, 70, alpha);
            renderer.drawRect(x, y, 1, gridSize, 60, 60, 70, alpha);
        }
    }
}

void MenuState::renderDecorations(IRenderer& renderer) {
    // Animated tank decorations on sides using actual sprites
    float tank1Y = 300 + std::sin(animTimer_ * 1.5f) * 10.0f;
    float tank2Y = 320 + std::cos(animTimer_ * 1.5f) * 10.0f;

    // Animation frame based on timer
    int animFrame = static_cast<int>(animTimer_ * 4.0f) % 2;

    // Left tank (Player 1, facing right)
    int leftX = 40;
    Rectangle p1Sprite = Sprites::Tank::getFrame(
        Sprites::Tank::P1_BASE_Y,
        Sprites::Tank::DIR_RIGHT_COL,
        animFrame, 0);
    renderer.drawSprite(
        static_cast<int>(p1Sprite.x), static_cast<int>(p1Sprite.y),
        static_cast<int>(p1Sprite.width), static_cast<int>(p1Sprite.height),
        leftX, static_cast<int>(tank1Y), 40, 40);

    // Right tank (Player 2, facing left)
    int rightX = Constants::WINDOW_WIDTH - 80;
    Rectangle p2Sprite = Sprites::Tank::getFrame(
        Sprites::Tank::P2_BASE_Y,
        Sprites::Tank::DIR_LEFT_COL,
        animFrame, 0);
    renderer.drawSprite(
        static_cast<int>(p2Sprite.x), static_cast<int>(p2Sprite.y),
        static_cast<int>(p2Sprite.width), static_cast<int>(p2Sprite.height),
        rightX, static_cast<int>(tank2Y), 40, 40);

    // Battle smoke/explosion decorations
    for (int i = 0; i < 3; ++i) {
        float offset = animTimer_ * 30.0f + i * 40.0f;
        int smokeX = 100 + static_cast<int>(offset) % 300;
        int smokeY = 380 + i * 15;
        uint8_t alpha = static_cast<uint8_t>(30 - i * 8);
        renderer.drawRect(smokeX, smokeY, 8 + i * 2, 4, 100, 100, 100, alpha);
    }
}

void MenuState::renderTitle(IRenderer& renderer) {
    int titleX = Constants::WINDOW_WIDTH / 2 - 120;
    int titleY = TITLE_Y + static_cast<int>(titleBounce_);

    // Title glow effect
    uint8_t glowAlpha = static_cast<uint8_t>(30 + 20 * std::sin(animTimer_ * 3.0f));
    renderer.drawRect(titleX - 15, titleY - 10, 260, 60,
                     Constants::UIColors::PRIMARY.r,
                     Constants::UIColors::PRIMARY.g,
                     Constants::UIColors::PRIMARY.b, glowAlpha);

    // Main title with gradient simulation (two-tone)
    renderer.drawText("TANK",
                     Vector2(static_cast<float>(titleX), static_cast<float>(titleY)),
                     Constants::UIColors::TITLE_TOP, 36);
    renderer.drawText("BATTLE",
                     Vector2(static_cast<float>(titleX + 10), static_cast<float>(titleY + 40)),
                     Constants::UIColors::TITLE_BOTTOM, 32);

    // Subtitle
    uint8_t subAlpha = static_cast<uint8_t>(180 * fadeAlpha_);
    renderer.drawText("C++ Edition",
                     Vector2(static_cast<float>(titleX + 70), static_cast<float>(titleY + 80)),
                     Constants::Color(150, 150, 150, subAlpha), 14);
}

void MenuState::renderMenuItems(IRenderer& renderer) {
    const char* items[] = {
        "CAMPAIGN",
        "SURVIVAL"
    };

    const char* descriptions[] = {
        "Play through 20 stages",
        "Endless wave mode"
    };

    for (int i = 0; i < MENU_ITEM_COUNT; ++i) {
        int y = MENU_START_Y + i * MENU_ITEM_HEIGHT;
        bool selected = (static_cast<int>(selectedItem_) == i);

        // Selection highlight background
        if (selected) {
            uint8_t highlightAlpha = static_cast<uint8_t>(40 + 20 * std::sin(cursorPulse_));
            renderer.drawRect(MENU_X - 40, y - 4, 220, 32,
                             Constants::UIColors::PRIMARY.r,
                             Constants::UIColors::PRIMARY.g,
                             Constants::UIColors::PRIMARY.b, highlightAlpha);

            // Left accent bar
            renderer.drawRect(MENU_X - 45, y - 4, 3, 32,
                             Constants::UIColors::PRIMARY.r,
                             Constants::UIColors::PRIMARY.g,
                             Constants::UIColors::PRIMARY.b, 200);
        }

        // Menu item number
        std::string numText = std::to_string(i + 1);
        Constants::Color numColor = selected
            ? Constants::UIColors::PRIMARY
            : Constants::Color(100, 100, 100);
        renderer.drawText(numText,
                         Vector2(static_cast<float>(MENU_X - 25), static_cast<float>(y)),
                         numColor, 18);

        // Menu item text
        Constants::Color textColor = selected
            ? Constants::UIColors::MENU_HIGHLIGHT
            : Constants::UIColors::MENU_NORMAL;
        renderer.drawText(items[i],
                         Vector2(static_cast<float>(MENU_X), static_cast<float>(y)),
                         textColor, 20);

        // Description text (only for selected item)
        if (selected) {
            renderer.drawText(descriptions[i],
                             Vector2(static_cast<float>(MENU_X), static_cast<float>(y + 18)),
                             Constants::Color(120, 120, 120), 11);
        }

        // Cursor for selected item
        if (selected) {
            renderCursor(renderer, MENU_X - 38, y + 6);
        }
    }

    // Player mode toggle
    int modeY = MENU_START_Y + MENU_ITEM_COUNT * MENU_ITEM_HEIGHT + 20;
    const char* modeText = twoPlayerMode_ ? "2 PLAYERS" : "1 PLAYER";
    Constants::Color modeColor = twoPlayerMode_
        ? Constants::UIColors::PLAYER2
        : Constants::UIColors::PLAYER1;

    renderer.drawText("MODE:",
                     Vector2(static_cast<float>(MENU_X - 25), static_cast<float>(modeY)),
                     Constants::Color(100, 100, 100), 14);
    renderer.drawText(modeText,
                     Vector2(static_cast<float>(MENU_X + 50), static_cast<float>(modeY)),
                     modeColor, 14);

    // Construction mode hint
    renderer.drawText("C - CONSTRUCTION",
                     Vector2(static_cast<float>(MENU_X - 25), static_cast<float>(modeY + 24)),
                     Constants::Color(80, 80, 80), 12);
}

void MenuState::renderCursor(IRenderer& renderer, int x, int y) {
    // Animated arrow cursor
    float bounce = std::sin(cursorPulse_) * 3.0f;
    int cursorX = x + static_cast<int>(bounce);

    // Draw arrow shape
    renderer.drawRect(cursorX, y, 12, 4,
                     Constants::UIColors::PRIMARY.r,
                     Constants::UIColors::PRIMARY.g,
                     Constants::UIColors::PRIMARY.b, 255);
    renderer.drawRect(cursorX + 8, y - 3, 4, 10,
                     Constants::UIColors::PRIMARY.r,
                     Constants::UIColors::PRIMARY.g,
                     Constants::UIColors::PRIMARY.b, 255);
}

void MenuState::renderFooter(IRenderer& renderer) {
    int footerY = Constants::WINDOW_HEIGHT - 30;

    // Controls hint
    renderer.drawText("UP/DOWN: Select   ENTER: Confirm   P: Toggle Players",
                     Vector2(80.0f, static_cast<float>(footerY)),
                     Constants::Color(80, 80, 80), 11);

    // Version/credits
    renderer.drawText("v1.0",
                     Vector2(static_cast<float>(Constants::WINDOW_WIDTH - 50), static_cast<float>(footerY)),
                     Constants::Color(60, 60, 60), 10);
}

void MenuState::handleInput(const IInput& input) {
    if (input.isKeyPressed(SDL_SCANCODE_C)) {
        stateManager_.changeToConstruction(1);
        return;
    }

    if (input.isKeyPressed(SDL_SCANCODE_P)) {
        twoPlayerMode_ = !twoPlayerMode_;
        return;
    }

    // Direct mode selection via number keys
    if (input.isKeyPressed(SDL_SCANCODE_1)) {
        selectedItem_ = MenuItem::Campaign;
        confirmSelection();
        return;
    }
    if (input.isKeyPressed(SDL_SCANCODE_2)) {
        selectedItem_ = MenuItem::Survival;
        confirmSelection();
        return;
    }

    // Navigate
    if (input.isKeyPressed(SDL_SCANCODE_UP) || input.isKeyPressed(SDL_SCANCODE_W)) {
        selectPreviousItem();
    } else if (input.isKeyPressed(SDL_SCANCODE_DOWN) || input.isKeyPressed(SDL_SCANCODE_S)) {
        selectNextItem();
    }

    // Confirm
    if (input.isKeyPressed(SDL_SCANCODE_RETURN) || input.isKeyPressed(SDL_SCANCODE_SPACE)) {
        confirmSelection();
    }
}

void MenuState::selectNextItem() {
    int current = static_cast<int>(selectedItem_);
    current = (current + 1) % MENU_ITEM_COUNT;
    selectedItem_ = static_cast<MenuItem>(current);
}

void MenuState::selectPreviousItem() {
    int current = static_cast<int>(selectedItem_);
    current = (current - 1 + MENU_ITEM_COUNT) % MENU_ITEM_COUNT;
    selectedItem_ = static_cast<MenuItem>(current);
}

void MenuState::confirmSelection() {
    std::cout << "Selected menu item: " << static_cast<int>(selectedItem_) << std::endl;

    switch (selectedItem_) {
        case MenuItem::Campaign:
            stateManager_.changeToStage(1, twoPlayerMode_);
            break;

        case MenuItem::Survival:
            stateManager_.changeToPlaying(1, /*twoPlayer=*/twoPlayerMode_, /*useWaveGenerator=*/true);
            break;
    }
}

} // namespace tank
