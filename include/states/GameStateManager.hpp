#pragma once

#include "states/IGameState.hpp"
#include <memory>
#include <stack>

namespace tank {

class InputManager;

/**
 * @brief Manages game states with stack-based navigation
 * Supports push/pop for overlays and change for transitions
 */
class GameStateManager {
public:
    GameStateManager() = default;
    ~GameStateManager() = default;

    // State management
    void pushState(std::unique_ptr<IGameState> state);
    void popState();
    void changeState(std::unique_ptr<IGameState> state);

    // Convenience methods for state transitions
    void changeToMenu();
    void changeToStage(int levelNumber);
    void changeToPlaying(int levelNumber, bool twoPlayer);
    void changeToScore(int levelNumber, bool victory);

    // Update and render
    void update(float deltaTime);
    void render(IRenderer& renderer);
    void handleInput(const InputManager& input);

    // Access current state
    IGameState* getCurrentState();
    const IGameState* getCurrentState() const;

    // State query
    bool isEmpty() const { return states_.empty(); }
    size_t stateCount() const { return states_.size(); }

private:
    std::stack<std::unique_ptr<IGameState>> states_;

    // Pending operations (executed at end of frame)
    std::unique_ptr<IGameState> pendingPush_;
    bool pendingPop_ = false;
    std::unique_ptr<IGameState> pendingChange_;

    void processPendingOperations();
};

} // namespace tank
