#pragma once

#include "states/IGameState.hpp"

namespace tank {

class GameStateManager;

/**
 * @brief Stage transition state - shows "STAGE X" before level starts
 */
class StageState : public IGameState {
public:
    StageState(GameStateManager& manager, int levelNumber, bool twoPlayer = false, bool useWaveGenerator = false);
    ~StageState() override = default;

    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render(IRenderer& renderer) override;
    void handleInput(const IInput& input) override;

    StateType getType() const override { return StateType::Stage; }

private:
    GameStateManager& stateManager_;
    int levelNumber_;
    bool twoPlayerMode_;
    bool useWaveGenerator_;
    float displayTime_;
    bool waitingForInput_;

    static constexpr float DISPLAY_DURATION = 2.0f;  // 2 seconds
};

} // namespace tank
