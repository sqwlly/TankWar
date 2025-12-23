#pragma once

#include "utils/Constants.hpp"
#include <SDL2/SDL.h>
#include <array>
#include <functional>

namespace tank {

/**
 * @brief Input event wrapper
 */
struct InputEvent {
    enum class Type {
        None,
        KeyDown,
        KeyUp,
        MouseButtonDown,
        MouseButtonUp,
        MouseMove,
        Quit
    };

    Type type = Type::None;
    SDL_Keycode keycode = 0;
    int mouseX = 0;
    int mouseY = 0;
    int mouseButton = 0;
};

/**
 * @brief Key mapping for a player
 */
struct KeyMapping {
    SDL_Scancode up;
    SDL_Scancode down;
    SDL_Scancode left;
    SDL_Scancode right;
    SDL_Scancode fire;
};

/**
 * @brief Input manager - handles SDL events and keyboard state
 */
class InputManager {
public:
    InputManager();

    // Process all pending SDL events
    void processEvents();

    // Check if quit was requested
    bool shouldQuit() const { return quit_; }

    // Keyboard state
    bool isKeyDown(SDL_Keycode key) const;
    bool isKeyDown(SDL_Scancode scancode) const;  // Scancode version
    bool isKeyPressed(SDL_Keycode key) const;  // Just pressed this frame
    bool isKeyPressed(SDL_Scancode scancode) const;  // Scancode version
    bool isKeyReleased(SDL_Keycode key) const; // Just released this frame

    // Player input (simplified struct)
    struct PlayerInput {
        bool up = false;
        bool down = false;
        bool left = false;
        bool right = false;
        bool fire = false;
    };

    PlayerInput getPlayer1Input() const;
    PlayerInput getPlayer2Input() const;

    // Event callback
    using EventCallback = std::function<void(const InputEvent&)>;
    void setEventCallback(EventCallback callback) { eventCallback_ = callback; }

    // Update (call at end of frame)
    void update();

private:
    bool quit_ = false;
    EventCallback eventCallback_;

    // Keyboard state
    std::array<bool, SDL_NUM_SCANCODES> currentKeys_{};
    std::array<bool, SDL_NUM_SCANCODES> previousKeys_{};

    // Player key mappings
    std::array<KeyMapping, 2> playerMappings_;

    void initializeKeyMappings();
    SDL_Scancode keyToScancode(SDL_Keycode key) const;
};

} // namespace tank
