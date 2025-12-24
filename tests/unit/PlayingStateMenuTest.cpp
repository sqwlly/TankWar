#include <gtest/gtest.h>

// White-box access for gameOver_ toggling in tests.
#define private public
#define protected public
#include "states/PlayingState.hpp"
#undef private
#undef protected

#include "states/GameStateManager.hpp"
#include "mocks/ScriptedInput.hpp"

namespace tank::test {
namespace {

void pressKeyOnce(PlayingState& state, ScriptedInput& input, SDL_Scancode scancode) {
    input.setKeyDown(scancode, true);
    state.handleInput(input);
    input.advanceFrame();
    input.setKeyDown(scancode, false);
    input.advanceFrame();
}

} // namespace

TEST(PlayingStateMenuTest, PauseMenuContinueThenRestart) {
    GameStateManager manager;
    PlayingState state(manager, /*levelNumber=*/1, /*twoPlayer=*/false);
    ScriptedInput input;

    EXPECT_FALSE(state.isPaused());

    pressKeyOnce(state, input, SDL_SCANCODE_ESCAPE);
    EXPECT_TRUE(state.isPaused());

    // Default selection is CONTINUE
    pressKeyOnce(state, input, SDL_SCANCODE_RETURN);
    EXPECT_FALSE(state.isPaused());

    // Pause again and choose RESTART
    pressKeyOnce(state, input, SDL_SCANCODE_ESCAPE);
    EXPECT_TRUE(state.isPaused());
    pressKeyOnce(state, input, SDL_SCANCODE_DOWN);
    pressKeyOnce(state, input, SDL_SCANCODE_RETURN);

    manager.update(0.0f);
    ASSERT_NE(manager.getCurrentState(), nullptr);
    EXPECT_EQ(manager.getCurrentState()->getType(), StateType::Playing);
}

TEST(PlayingStateMenuTest, PauseMenuReturnToMainMenu) {
    GameStateManager manager;
    PlayingState state(manager, /*levelNumber=*/1, /*twoPlayer=*/false);
    ScriptedInput input;

    pressKeyOnce(state, input, SDL_SCANCODE_ESCAPE);
    EXPECT_TRUE(state.isPaused());

    // CONTINUE -> RESTART -> MAIN MENU
    pressKeyOnce(state, input, SDL_SCANCODE_DOWN);
    pressKeyOnce(state, input, SDL_SCANCODE_DOWN);
    pressKeyOnce(state, input, SDL_SCANCODE_RETURN);

    manager.update(0.0f);
    ASSERT_NE(manager.getCurrentState(), nullptr);
    EXPECT_EQ(manager.getCurrentState()->getType(), StateType::Menu);
}

TEST(PlayingStateMenuTest, GameOverEscapeReturnsToMenu) {
    GameStateManager manager;
    PlayingState state(manager, /*levelNumber=*/1, /*twoPlayer=*/false);
    ScriptedInput input;

    state.gameOver_ = true;
    state.gameOverOverlay_.start();

    pressKeyOnce(state, input, SDL_SCANCODE_ESCAPE);

    manager.update(0.0f);
    ASSERT_NE(manager.getCurrentState(), nullptr);
    EXPECT_EQ(manager.getCurrentState()->getType(), StateType::Menu);
}

} // namespace tank::test

