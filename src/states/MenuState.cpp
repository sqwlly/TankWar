#include "states/MenuState.hpp"
#include "states/GameStateManager.hpp"
#include "input/IInput.hpp"
#include <iostream>

namespace tank {

MenuState::MenuState(GameStateManager& manager)
    : stateManager_(manager)
{
}

void MenuState::enter() {
    std::cout << "Entering Menu State" << std::endl;
    selectedItem_ = MenuItem::Campaign;
    cursorBlinkTimer_ = 0.0f;
    cursorVisible_ = true;
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

void MenuState::handleInput(const IInput& input) {
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
            stateManager_.changeToStage(1);
            break;

        case MenuItem::Survival:
            stateManager_.changeToPlaying(1, /*twoPlayer=*/false);
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
        "1 \xE6\x88\x98\xE5\xBD\xB9",
        "2 \xE6\x97\xA0\xE5\xB0\xBD"
    };

    for (int i = 0; i < MENU_ITEM_COUNT; ++i) {
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
