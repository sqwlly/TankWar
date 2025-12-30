#pragma once

#include "states/IGameState.hpp"

namespace tank {

class GameStateManager;

/**
 * @brief Stage transition state with cinematic animation
 *
 * Features:
 * - Curtain wipe animation
 * - Stage number with glow effect
 * - Player info display
 * - Smooth fade transitions
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

    // Animation state
    float displayTime_;
    float curtainProgress_;  // 0.0 = closed, 1.0 = open
    float textScale_;
    float glowPulse_;
    bool readyToTransition_;

    static constexpr float DISPLAY_DURATION = 2.5f;
    static constexpr float CURTAIN_SPEED = 2.0f;
    static constexpr float TEXT_SCALE_SPEED = 3.0f;

    void renderCurtains(IRenderer& renderer);
    void renderStageInfo(IRenderer& renderer);
    void renderPlayerIndicators(IRenderer& renderer);
};

} // namespace tank
