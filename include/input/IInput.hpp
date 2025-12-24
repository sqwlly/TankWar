#pragma once

#include <SDL2/SDL.h>
#include <cstdint>

namespace tank {

/**
 * @brief Input query interface for gameplay/state logic (DIP)
 *
 * Production code uses InputManager; tests can provide scripted implementations.
 */
class IInput {
public:
    virtual ~IInput() = default;

    // Keyboard state
    virtual bool isKeyDown(SDL_Keycode key) const = 0;
    virtual bool isKeyDown(SDL_Scancode scancode) const = 0;
    virtual bool isKeyPressed(SDL_Keycode key) const = 0;
    virtual bool isKeyPressed(SDL_Scancode scancode) const = 0;
    virtual bool isKeyReleased(SDL_Keycode key) const = 0;

    // Mouse state
    virtual int getMouseX() const = 0;
    virtual int getMouseY() const = 0;
    virtual bool isMouseButtonDown(uint8_t button) const = 0;
    virtual bool isMouseButtonPressed(uint8_t button) const = 0;
    virtual bool isMouseButtonReleased(uint8_t button) const = 0;
};

} // namespace tank

