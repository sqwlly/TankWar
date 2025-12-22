#include "input/InputManager.hpp"

namespace tank {

InputManager::InputManager() {
    currentKeys_.fill(false);
    previousKeys_.fill(false);
    initializeKeyMappings();
}

void InputManager::initializeKeyMappings() {
    // Player 1: WASD + Space
    playerMappings_[0] = {
        SDLK_w,      // up
        SDLK_s,      // down
        SDLK_a,      // left
        SDLK_d,      // right
        SDLK_SPACE   // fire
    };

    // Player 2: Arrow keys + Numpad 0
    playerMappings_[1] = {
        SDLK_UP,     // up
        SDLK_DOWN,   // down
        SDLK_LEFT,   // left
        SDLK_RIGHT,  // right
        SDLK_KP_0    // fire (numpad 0)
    };
}

void InputManager::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        InputEvent inputEvent;

        switch (event.type) {
            case SDL_QUIT:
                quit_ = true;
                inputEvent.type = InputEvent::Type::Quit;
                break;

            case SDL_KEYDOWN:
                if (!event.key.repeat) {
                    inputEvent.type = InputEvent::Type::KeyDown;
                    inputEvent.keycode = event.key.keysym.sym;
                }
                break;

            case SDL_KEYUP:
                inputEvent.type = InputEvent::Type::KeyUp;
                inputEvent.keycode = event.key.keysym.sym;
                break;

            case SDL_MOUSEBUTTONDOWN:
                inputEvent.type = InputEvent::Type::MouseButtonDown;
                inputEvent.mouseX = event.button.x;
                inputEvent.mouseY = event.button.y;
                inputEvent.mouseButton = event.button.button;
                break;

            case SDL_MOUSEBUTTONUP:
                inputEvent.type = InputEvent::Type::MouseButtonUp;
                inputEvent.mouseX = event.button.x;
                inputEvent.mouseY = event.button.y;
                inputEvent.mouseButton = event.button.button;
                break;

            case SDL_MOUSEMOTION:
                inputEvent.type = InputEvent::Type::MouseMove;
                inputEvent.mouseX = event.motion.x;
                inputEvent.mouseY = event.motion.y;
                break;
        }

        if (inputEvent.type != InputEvent::Type::None && eventCallback_) {
            eventCallback_(inputEvent);
        }
    }

    // Use SDL_GetKeyboardState for reliable real-time key detection
    const Uint8* keyState = SDL_GetKeyboardState(nullptr);
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        currentKeys_[i] = keyState[i] != 0;
    }
}

void InputManager::update() {
    previousKeys_ = currentKeys_;
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
        return currentKeys_[scancode] && !previousKeys_[scancode];
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
        return currentKeys_[scancode] && !previousKeys_[scancode];
    }
    return false;
}

InputManager::PlayerInput InputManager::getPlayer1Input() const {
    PlayerInput input;
    // Use scancodes directly for reliable input detection
    input.up = isKeyDown(SDL_SCANCODE_W);
    input.down = isKeyDown(SDL_SCANCODE_S);
    input.left = isKeyDown(SDL_SCANCODE_A);
    input.right = isKeyDown(SDL_SCANCODE_D);
    input.fire = isKeyDown(SDL_SCANCODE_SPACE);
    return input;
}

InputManager::PlayerInput InputManager::getPlayer2Input() const {
    PlayerInput input;
    // Use scancodes directly for reliable input detection
    input.up = isKeyDown(SDL_SCANCODE_UP);
    input.down = isKeyDown(SDL_SCANCODE_DOWN);
    input.left = isKeyDown(SDL_SCANCODE_LEFT);
    input.right = isKeyDown(SDL_SCANCODE_RIGHT);
    input.fire = isKeyDown(SDL_SCANCODE_KP_0) || isKeyDown(SDL_SCANCODE_RCTRL);
    return input;
}

} // namespace tank
