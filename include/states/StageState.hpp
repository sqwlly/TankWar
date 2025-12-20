#pragma once

#include "states/IGameState.hpp"

namespace tank {

class GameStateManager;

/**
 * @brief Stage transition state - shows "STAGE X" before level starts
 */
class StageState : public IGameState {
public:
    StageState(GameStateManager& manager, int levelNumber);
    ~StageState() override = default;

    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render(IRenderer& renderer) override;
    void handleInput(const InputManager& input) override;

    StateType getType() const override { return StateType::Stage; }

private:
    GameStateManager& stateManager_;
    int levelNumber_;
    float displayTime_;
    bool waitingForInput_;

    static constexpr float DISPLAY_DURATION = 2.0f;  // 2 seconds
};

} // namespace tank
