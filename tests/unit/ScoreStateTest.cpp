#include <gtest/gtest.h>

#define private public
#define protected public
#include "states/ScoreState.hpp"
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

TEST(ScoreStateTest, SkipAnimationThenContinueGoesToNextStageOnVictory) {
    GameStateManager manager;

    auto score = std::make_unique<ScoreState>(manager, /*levelNumber=*/1, /*victory=*/true);
    score->setKillCounts(/*type1=*/1, /*type2=*/0, /*type3=*/0, /*type4=*/0);
    manager.pushState(std::move(score));
    manager.update(0.0f);

    ASSERT_NE(manager.getCurrentState(), nullptr);
    ASSERT_EQ(manager.getCurrentState()->getType(), StateType::Score);

    auto* scoreState = dynamic_cast<ScoreState*>(manager.getCurrentState());
    ASSERT_NE(scoreState, nullptr);
    EXPECT_FALSE(scoreState->animationComplete_);

    ScriptedInput input;
    pressKeyOnce(manager, input, SDL_SCANCODE_RETURN);  // Skip animation.

    ASSERT_EQ(manager.getCurrentState()->getType(), StateType::Score);
    EXPECT_TRUE(scoreState->animationComplete_);
    EXPECT_EQ(scoreState->displayedKills_[0], 1);
    EXPECT_EQ(scoreState->displayedTotal_, 1);

    pressKeyOnce(manager, input, SDL_SCANCODE_RETURN);  // Continue.
    manager.update(0.0f);                               // Apply pending change.

    ASSERT_NE(manager.getCurrentState(), nullptr);
    ASSERT_EQ(manager.getCurrentState()->getType(), StateType::Stage);

    auto* stage = dynamic_cast<StageState*>(manager.getCurrentState());
    ASSERT_NE(stage, nullptr);
    EXPECT_EQ(stage->levelNumber_, 2);
}

TEST(ScoreStateTest, SkipAnimationThenContinueReturnsToMenuOnDefeat) {
    GameStateManager manager;

    auto score = std::make_unique<ScoreState>(manager, /*levelNumber=*/3, /*victory=*/false);
    score->setKillCounts(/*type1=*/0, /*type2=*/1, /*type3=*/0, /*type4=*/0);
    manager.pushState(std::move(score));
    manager.update(0.0f);

    ASSERT_NE(manager.getCurrentState(), nullptr);
    ASSERT_EQ(manager.getCurrentState()->getType(), StateType::Score);

    ScriptedInput input;
    pressKeyOnce(manager, input, SDL_SCANCODE_RETURN);  // Skip animation.
    pressKeyOnce(manager, input, SDL_SCANCODE_RETURN);  // Continue.
    manager.update(0.0f);

    ASSERT_NE(manager.getCurrentState(), nullptr);
    EXPECT_EQ(manager.getCurrentState()->getType(), StateType::Menu);
}

} // namespace tank::test

