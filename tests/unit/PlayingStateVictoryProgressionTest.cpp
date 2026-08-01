#include <gtest/gtest.h>

#define private public
#define protected public
#include "states/PlayingState.hpp"
#include "states/StageState.hpp"
#undef private
#undef protected

#include "mocks/ScriptedInput.hpp"
#include "states/GameStateManager.hpp"

namespace tank::test {
namespace {

void pressKeyOnce(GameStateManager& manager, ScriptedInput& input, SDL_Scancode scancode) {
    input.setKeyDown(scancode, true);
    manager.handleInput(input);
    input.advanceFrame();
    input.setKeyDown(scancode, false);
    input.advanceFrame();
}

} // namespace

TEST(PlayingStateVictoryProgressionTest, VictoryShowsScoreThenNextStageAndPreservesTwoPlayer) {
    GameStateManager manager;
    PlayingState state(manager, /*levelNumber=*/1, /*twoPlayer=*/true);

    state.level_ = std::make_unique<Level>(1);
    state.player1_ = std::make_unique<PlayerTank>(1, Vector2(0.0f, 0.0f));
    state.player2_ = std::make_unique<PlayerTank>(2, Vector2(34.0f, 0.0f));
    state.enemiesAlive_ = 0;
    state.enemiesSpawned_ = 0;
    state.levelComplete_ = false;

    state.update(0.016f);
    manager.update(0.0f);  // Apply pending change -> ScoreState

    ASSERT_NE(manager.getCurrentState(), nullptr);
    ASSERT_EQ(manager.getCurrentState()->getType(), StateType::Score);

    // Skip tally animation, then continue -> StageState for the next level.
    ScriptedInput input;
    pressKeyOnce(manager, input, SDL_SCANCODE_RETURN);
    pressKeyOnce(manager, input, SDL_SCANCODE_RETURN);
    manager.update(0.0f);

    ASSERT_NE(manager.getCurrentState(), nullptr);
    EXPECT_EQ(manager.getCurrentState()->getType(), StateType::Stage);

    auto* stage = dynamic_cast<StageState*>(manager.getCurrentState());
    ASSERT_NE(stage, nullptr);
    EXPECT_EQ(stage->levelNumber_, 2);
    EXPECT_TRUE(stage->twoPlayerMode_);

    // Fast-forward StageState -> PlayingState
    manager.update(3.0f);
    manager.update(0.0f);

    ASSERT_NE(manager.getCurrentState(), nullptr);
    EXPECT_EQ(manager.getCurrentState()->getType(), StateType::Playing);
}

} // namespace tank::test
