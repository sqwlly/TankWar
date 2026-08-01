#include "input/InputManager.hpp"

namespace tank {

InputManager::InputManager() {
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
    // IMPORTANT: Call SDL_PumpEvents() to update SDL's internal keyboard state.
    // This is required for SDL_GetKeyboardState() to work correctly.
    SDL_PumpEvents();

    // Clear per-frame edge flags.
    eventKeys_.fill(false);
    eventKeycodes_.clear();

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
                if (!event.key.repeat) {
                    eventKeys_[event.key.keysym.scancode] = true;
                    currentKeycodes_.insert(event.key.keysym.sym);
                    eventKeycodes_.insert(event.key.keysym.sym);
                    inputEvent.type = InputEvent::Type::KeyDown;
                    inputEvent.keycode = event.key.keysym.sym;
                }
                break;

            case SDL_KEYUP:
                currentKeycodes_.erase(event.key.keysym.sym);
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
        eventKeys_.fill(false);
        currentKeycodes_.clear();
        previousKeycodes_.clear();
        eventKeycodes_.clear();
        currentMouseButtons_.fill(false);
        previousMouseButtons_.fill(false);
        return;
    }
}

void InputManager::update() {
    previousMouseButtons_ = currentMouseButtons_;
    previousKeycodes_ = currentKeycodes_;
    eventKeys_.fill(false);
    eventKeycodes_.clear();
}

bool InputManager::isKeyDown(SDL_Keycode key) const {
    return currentKeycodes_.find(key) != currentKeycodes_.end();
}

bool InputManager::isKeyPressed(SDL_Keycode key) const {
    return eventKeycodes_.find(key) != eventKeycodes_.end();
}

bool InputManager::isKeyReleased(SDL_Keycode key) const {
    const bool isDownNow = currentKeycodes_.find(key) != currentKeycodes_.end();
    const bool wasDown = previousKeycodes_.find(key) != previousKeycodes_.end();
    return !isDownNow && wasDown;
}

bool InputManager::isKeyDown(SDL_Scancode scancode) const {
    if (scancode < SDL_NUM_SCANCODES) {
        // Use SDL's real-time keyboard state
        const Uint8* state = SDL_GetKeyboardState(NULL);
        return state && state[scancode] != 0;
    }
    return false;
}

bool InputManager::isKeyPressed(SDL_Scancode scancode) const {
    if (scancode < SDL_NUM_SCANCODES) {
        // Key was pressed this frame (edge trigger). The edge set alone is
        // authoritative here: also requiring the real-time state would drop
        // quick taps that press and release within a single frame.
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
