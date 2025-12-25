#include "input/InputManager.hpp"

namespace tank {

InputManager::InputManager() {
    currentKeys_.fill(false);
    previousKeys_.fill(false);
    eventKeys_.fill(false);
    currentMouseButtons_.fill(false);
    previousMouseButtons_.fill(false);
    initializeKeyMappings();
}

void InputManager::initializeKeyMappings() {
    // Player 1: WASD + Space
    playerMappings_[0] = {
        SDL_SCANCODE_W,      // up
        SDL_SCANCODE_S,      // down
        SDL_SCANCODE_A,      // left
        SDL_SCANCODE_D,      // right
        SDL_SCANCODE_SPACE   // fire
    };

    // Player 2: Arrow keys + Enter
    playerMappings_[1] = {
        SDL_SCANCODE_UP,      // up
        SDL_SCANCODE_DOWN,    // down
        SDL_SCANCODE_LEFT,    // left
        SDL_SCANCODE_RIGHT,   // right
        SDL_SCANCODE_RETURN   // fire
    };
}

void InputManager::processEvents() {
    // Ensure SDL internal input state is refreshed every frame, even if the event queue is empty.
    SDL_PumpEvents();

    // Clear per-frame edge flags.
    eventKeys_.fill(false);

    SDL_Event event;
    bool shouldClearKeyboardState = false;
    while (SDL_PollEvent(&event)) {
        InputEvent inputEvent;

        switch (event.type) {
            case SDL_QUIT:
                quit_ = true;
                inputEvent.type = InputEvent::Type::Quit;
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.scancode < SDL_NUM_SCANCODES) {
                    currentKeys_[event.key.keysym.scancode] = true;
                    if (!event.key.repeat) {
                        eventKeys_[event.key.keysym.scancode] = true;
                    }
                }
                if (!event.key.repeat) {
                    inputEvent.type = InputEvent::Type::KeyDown;
                    inputEvent.keycode = event.key.keysym.sym;
                }
                break;

            case SDL_KEYUP:
                if (event.key.keysym.scancode < SDL_NUM_SCANCODES) {
                    currentKeys_[event.key.keysym.scancode] = false;
                }
                inputEvent.type = InputEvent::Type::KeyUp;
                inputEvent.keycode = event.key.keysym.sym;
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST ||
                    event.window.event == SDL_WINDOWEVENT_HIDDEN) {
                    shouldClearKeyboardState = true;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                inputEvent.type = InputEvent::Type::MouseButtonDown;
                inputEvent.mouseX = event.button.x;
                inputEvent.mouseY = event.button.y;
                inputEvent.mouseButton = event.button.button;
                mouseX_ = event.button.x;
                mouseY_ = event.button.y;
                if (event.button.button < MOUSE_BUTTON_COUNT) {
                    currentMouseButtons_[event.button.button] = true;
                }
                break;

            case SDL_MOUSEBUTTONUP:
                inputEvent.type = InputEvent::Type::MouseButtonUp;
                inputEvent.mouseX = event.button.x;
                inputEvent.mouseY = event.button.y;
                inputEvent.mouseButton = event.button.button;
                mouseX_ = event.button.x;
                mouseY_ = event.button.y;
                if (event.button.button < MOUSE_BUTTON_COUNT) {
                    currentMouseButtons_[event.button.button] = false;
                }
                break;

            case SDL_MOUSEMOTION:
                inputEvent.type = InputEvent::Type::MouseMove;
                inputEvent.mouseX = event.motion.x;
                inputEvent.mouseY = event.motion.y;
                mouseX_ = event.motion.x;
                mouseY_ = event.motion.y;
                break;
        }

        if (inputEvent.type != InputEvent::Type::None && eventCallback_) {
            eventCallback_(inputEvent);
        }
    }

    if (shouldClearKeyboardState) {
        currentKeys_.fill(false);
        previousKeys_.fill(false);
        eventKeys_.fill(false);
        currentMouseButtons_.fill(false);
        previousMouseButtons_.fill(false);
        return;
    }
}

void InputManager::update() {
    previousKeys_ = currentKeys_;
    previousMouseButtons_ = currentMouseButtons_;
    eventKeys_.fill(false);
}

SDL_Scancode InputManager::keyToScancode(SDL_Keycode key) const {
    return SDL_GetScancodeFromKey(key);
}

bool InputManager::isKeyDown(SDL_Keycode key) const {
    SDL_Scancode scancode = keyToScancode(key);
    if (scancode < SDL_NUM_SCANCODES) {
        return currentKeys_[scancode];
    }
    return false;
}

bool InputManager::isKeyPressed(SDL_Keycode key) const {
    SDL_Scancode scancode = keyToScancode(key);
    if (scancode < SDL_NUM_SCANCODES) {
        return eventKeys_[scancode];
    }
    return false;
}

bool InputManager::isKeyReleased(SDL_Keycode key) const {
    SDL_Scancode scancode = keyToScancode(key);
    if (scancode < SDL_NUM_SCANCODES) {
        return !currentKeys_[scancode] && previousKeys_[scancode];
    }
    return false;
}

bool InputManager::isKeyDown(SDL_Scancode scancode) const {
    if (scancode < SDL_NUM_SCANCODES) {
        return currentKeys_[scancode];
    }
    return false;
}

bool InputManager::isKeyPressed(SDL_Scancode scancode) const {
    if (scancode < SDL_NUM_SCANCODES) {
        return eventKeys_[scancode];
    }
    return false;
}

bool InputManager::isMouseButtonDown(uint8_t button) const {
    if (button < MOUSE_BUTTON_COUNT) {
        return currentMouseButtons_[button];
    }
    return false;
}

bool InputManager::isMouseButtonPressed(uint8_t button) const {
    if (button < MOUSE_BUTTON_COUNT) {
        return currentMouseButtons_[button] && !previousMouseButtons_[button];
    }
    return false;
}

bool InputManager::isMouseButtonReleased(uint8_t button) const {
    if (button < MOUSE_BUTTON_COUNT) {
        return !currentMouseButtons_[button] && previousMouseButtons_[button];
    }
    return false;
}

PlayerInput InputManager::getPlayer1Input() const {
    PlayerInput input;

    const KeyMapping& mapping = playerMappings_[0];
    input.up = isKeyDown(mapping.up) || isKeyDown(SDLK_w);
    input.down = isKeyDown(mapping.down) || isKeyDown(SDLK_s);
    input.left = isKeyDown(mapping.left) || isKeyDown(SDLK_a);
    input.right = isKeyDown(mapping.right) || isKeyDown(SDLK_d);
    input.fire = isKeyDown(mapping.fire) || isKeyDown(SDLK_SPACE);

    return input;
}

PlayerInput InputManager::getPlayer2Input() const {
    PlayerInput input;
    const KeyMapping& mapping = playerMappings_[1];
    input.up = isKeyDown(mapping.up);
    input.down = isKeyDown(mapping.down);
    input.left = isKeyDown(mapping.left);
    input.right = isKeyDown(mapping.right);
    input.fire = isKeyDown(mapping.fire) || isKeyDown(SDL_SCANCODE_KP_ENTER) || isKeyDown(SDL_SCANCODE_RCTRL);
    return input;
}

} // namespace tank
