#pragma once

#include "rendering/IRenderer.hpp"
#include "utils/Constants.hpp"

namespace tank {

class GameStateManager;
class InputManager;

/**
 * @brief Abstract game state interface (State Pattern)
 * Each game state (Menu, Playing, Score, etc.) implements this interface
 */
class IGameState {
public:
    virtual ~IGameState() = default;

    // Lifecycle
    virtual void enter() = 0;
    virtual void exit() = 0;

    // Update and render
    virtual void update(float deltaTime) = 0;
    virtual void render(IRenderer& renderer) = 0;

    // Input handling
    virtual void handleInput(const InputManager& input) = 0;

    // State type
    virtual StateType getType() const = 0;
};

} // namespace tank
