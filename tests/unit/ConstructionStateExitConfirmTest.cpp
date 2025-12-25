#include <gtest/gtest.h>

#include "states/ConstructionState.hpp"
#include "states/GameStateManager.hpp"
#include "mocks/ScriptedInput.hpp"

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

TEST(ConstructionStateExitConfirmTest, DirtyLevelRequiresSecondEscapeToReturnToMenu) {
    GameStateManager manager;
    manager.pushState(std::make_unique<ConstructionState>(manager, /*levelNumber=*/1));
    manager.update(0.0f);

    ASSERT_NE(manager.getCurrentState(), nullptr);
    ASSERT_EQ(manager.getCurrentState()->getType(), StateType::Construction);

    ScriptedInput input;
    pressKeyOnce(manager, input, SDL_SCANCODE_N);  // New level -> dirty.

    pressKeyOnce(manager, input, SDL_SCANCODE_ESCAPE);
    manager.update(0.0f);
    ASSERT_NE(manager.getCurrentState(), nullptr);
    EXPECT_EQ(manager.getCurrentState()->getType(), StateType::Construction);

    pressKeyOnce(manager, input, SDL_SCANCODE_ESCAPE);
    manager.update(0.0f);
    ASSERT_NE(manager.getCurrentState(), nullptr);
    EXPECT_EQ(manager.getCurrentState()->getType(), StateType::Menu);
}

} // namespace tank::test

