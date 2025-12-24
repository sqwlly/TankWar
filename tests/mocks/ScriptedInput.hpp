#pragma once

#include "input/IInput.hpp"
#include <algorithm>
#include <array>

namespace tank::test {

/**
 * @brief Scriptable input source for deterministic gameplay tests.
 *
 * Call advanceFrame() once per simulated frame to update pressed/released edges.
 */
class ScriptedInput final : public tank::IInput {
public:
    ScriptedInput() {
        reset();
    }

    void reset() {
        currentKeys_.fill(false);
        previousKeys_.fill(false);
        currentMouseButtons_.fill(false);
        previousMouseButtons_.fill(false);
        mouseX_ = 0;
        mouseY_ = 0;
    }

    void advanceFrame() {
        previousKeys_ = currentKeys_;
        previousMouseButtons_ = currentMouseButtons_;
    }

    void setKeyDown(SDL_Scancode scancode, bool down) {
        if (scancode < SDL_NUM_SCANCODES) {
            currentKeys_[scancode] = down;
        }
    }

    void setKeyDown(SDL_Keycode keycode, bool down) {
        setKeyDown(SDL_GetScancodeFromKey(keycode), down);
    }

    void setMousePosition(int x, int y) {
        mouseX_ = x;
        mouseY_ = y;
    }

    void setMouseButtonDown(uint8_t button, bool down) {
        if (button < MOUSE_BUTTON_COUNT) {
            currentMouseButtons_[button] = down;
        }
    }

    bool isKeyDown(SDL_Keycode key) const override {
        return isKeyDown(SDL_GetScancodeFromKey(key));
    }

    bool isKeyDown(SDL_Scancode scancode) const override {
        if (scancode < SDL_NUM_SCANCODES) {
            return currentKeys_[scancode];
        }
        return false;
    }

    bool isKeyPressed(SDL_Keycode key) const override {
        return isKeyPressed(SDL_GetScancodeFromKey(key));
    }

    bool isKeyPressed(SDL_Scancode scancode) const override {
        if (scancode < SDL_NUM_SCANCODES) {
            return currentKeys_[scancode] && !previousKeys_[scancode];
        }
        return false;
    }

    bool isKeyReleased(SDL_Keycode key) const override {
        SDL_Scancode scancode = SDL_GetScancodeFromKey(key);
        if (scancode < SDL_NUM_SCANCODES) {
            return !currentKeys_[scancode] && previousKeys_[scancode];
        }
        return false;
    }

    int getMouseX() const override { return mouseX_; }
    int getMouseY() const override { return mouseY_; }

    bool isMouseButtonDown(uint8_t button) const override {
        if (button < MOUSE_BUTTON_COUNT) {
            return currentMouseButtons_[button];
        }
        return false;
    }

    bool isMouseButtonPressed(uint8_t button) const override {
        if (button < MOUSE_BUTTON_COUNT) {
            return currentMouseButtons_[button] && !previousMouseButtons_[button];
        }
        return false;
    }

    bool isMouseButtonReleased(uint8_t button) const override {
        if (button < MOUSE_BUTTON_COUNT) {
            return !currentMouseButtons_[button] && previousMouseButtons_[button];
        }
        return false;
    }

private:
    static constexpr int MOUSE_BUTTON_COUNT = 8;

    std::array<bool, SDL_NUM_SCANCODES> currentKeys_{};
    std::array<bool, SDL_NUM_SCANCODES> previousKeys_{};
    std::array<bool, MOUSE_BUTTON_COUNT> currentMouseButtons_{};
    std::array<bool, MOUSE_BUTTON_COUNT> previousMouseButtons_{};
    int mouseX_ = 0;
    int mouseY_ = 0;
};

} // namespace tank::test

