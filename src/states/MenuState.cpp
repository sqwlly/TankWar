#include "states/MenuState.hpp"
#include "states/GameStateManager.hpp"
#include "input/InputManager.hpp"
#include <iostream>

namespace tank {

MenuState::MenuState(GameStateManager& manager)
    : stateManager_(manager)
{
}

void MenuState::enter() {
    std::cout << "Entering Menu State" << std::endl;
    selectedItem_ = MenuItem::SinglePlayer;
    cursorBlinkTimer_ = 0.0f;
    cursorVisible_ = true;
    upPressed_ = false;
    downPressed_ = false;
    confirmPressed_ = false;
}

void MenuState::exit() {
    std::cout << "Exiting Menu State" << std::endl;
}

void MenuState::update(float deltaTime) {
    // Cursor blink animation
    cursorBlinkTimer_ += deltaTime;
    if (cursorBlinkTimer_ >= 0.3f) {
        cursorBlinkTimer_ = 0.0f;
        cursorVisible_ = !cursorVisible_;
    }
}

void MenuState::render(IRenderer& renderer) {
    // Black background
    renderer.clear(0, 0, 0, 255);

    renderTitle(renderer);
    renderMenuItems(renderer);

    if (cursorVisible_) {
        renderCursor(renderer);
    }
}

void MenuState::handleInput(const InputManager& input) {
    // Handle up navigation
    bool upNow = input.isKeyDown(SDL_SCANCODE_UP) || input.isKeyDown(SDL_SCANCODE_W);
    if (upNow && !upPressed_) {
        selectPreviousItem();
    }
    upPressed_ = upNow;

    // Handle down navigation
    bool downNow = input.isKeyDown(SDL_SCANCODE_DOWN) || input.isKeyDown(SDL_SCANCODE_S);
    if (downNow && !downPressed_) {
        selectNextItem();
    }
    downPressed_ = downNow;

    // Handle confirm
    bool confirmNow = input.isKeyDown(SDL_SCANCODE_RETURN) || input.isKeyDown(SDL_SCANCODE_SPACE);
    if (confirmNow && !confirmPressed_) {
        confirmSelection();
    }
    confirmPressed_ = confirmNow;
}

void MenuState::selectNextItem() {
    int current = static_cast<int>(selectedItem_);
    current = (current + 1) % 3;
    selectedItem_ = static_cast<MenuItem>(current);
}

void MenuState::selectPreviousItem() {
    int current = static_cast<int>(selectedItem_);
    current = (current - 1 + 3) % 3;
    selectedItem_ = static_cast<MenuItem>(current);
}

void MenuState::confirmSelection() {
    std::cout << "Selected menu item: " << static_cast<int>(selectedItem_) << std::endl;

    switch (selectedItem_) {
        case MenuItem::SinglePlayer:
            stateManager_.changeToStage(1);
            break;

        case MenuItem::TwoPlayers:
            // Start two player game (stage 1, two player mode)
            stateManager_.changeToPlaying(1, true);
            break;

        case MenuItem::Construction:
            stateManager_.changeToConstruction(1);
            break;
    }
}

void MenuState::renderTitle(IRenderer& renderer) {
    // Title text
    renderer.drawText("TANK BATTLE", Vector2(170, 100), Constants::COLOR_WHITE, 32);
    renderer.drawText("C++ Edition", Vector2(190, 140), Constants::COLOR_GRAY, 16);
}

void MenuState::renderMenuItems(IRenderer& renderer) {
    const char* items[] = {
        "1 PLAYER",
        "2 PLAYERS",
        "CONSTRUCTION"
    };

    for (int i = 0; i < 3; ++i) {
        int y = MENU_START_Y + i * MENU_ITEM_HEIGHT;
        renderer.drawText(items[i], Vector2(180, static_cast<float>(y)), Constants::COLOR_WHITE, 20);
    }
}

void MenuState::renderCursor(IRenderer& renderer) {
    int cursorY = MENU_START_Y + static_cast<int>(selectedItem_) * MENU_ITEM_HEIGHT;

    // Draw tank icon as cursor
    renderer.drawRect(MENU_CURSOR_X, cursorY, 20, 20, 255, 255, 255, 255);
}

} // namespace tank
