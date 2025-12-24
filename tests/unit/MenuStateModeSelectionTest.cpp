#include <gtest/gtest.h>

#include "states/GameStateManager.hpp"
#include "states/MenuState.hpp"
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

TEST(MenuStateModeSelectionTest, Press1StartsCampaign) {
    GameStateManager manager;
    manager.pushState(std::make_unique<MenuState>(manager));
    manager.update(0.0f);

    ASSERT_NE(manager.getCurrentState(), nullptr);
    ASSERT_EQ(manager.getCurrentState()->getType(), StateType::Menu);

    ScriptedInput input;
    pressKeyOnce(manager, input, SDL_SCANCODE_1);
    manager.update(0.0f);

    ASSERT_NE(manager.getCurrentState(), nullptr);
    EXPECT_EQ(manager.getCurrentState()->getType(), StateType::Stage);
}

TEST(MenuStateModeSelectionTest, Press2StartsSurvival) {
    GameStateManager manager;
    manager.pushState(std::make_unique<MenuState>(manager));
    manager.update(0.0f);

    ASSERT_NE(manager.getCurrentState(), nullptr);
    ASSERT_EQ(manager.getCurrentState()->getType(), StateType::Menu);

    ScriptedInput input;
    pressKeyOnce(manager, input, SDL_SCANCODE_2);
    manager.update(0.0f);

    ASSERT_NE(manager.getCurrentState(), nullptr);
    EXPECT_EQ(manager.getCurrentState()->getType(), StateType::Playing);
}

} // namespace tank::test

