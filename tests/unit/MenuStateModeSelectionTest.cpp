#include <gtest/gtest.h>

#include "states/GameStateManager.hpp"
#include "states/MenuState.hpp"
#include "mocks/MockRenderer.hpp"
#include "mocks/ScriptedInput.hpp"

#define private public
#define protected public
#include "states/PlayingState.hpp"
#undef private
#undef protected

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

TEST(MenuStateModeSelectionTest, PressCStartsConstruction) {
    GameStateManager manager;
    manager.pushState(std::make_unique<MenuState>(manager));
    manager.update(0.0f);

    ASSERT_NE(manager.getCurrentState(), nullptr);
    ASSERT_EQ(manager.getCurrentState()->getType(), StateType::Menu);

    ScriptedInput input;
    pressKeyOnce(manager, input, SDL_SCANCODE_C);
    manager.update(0.0f);

    ASSERT_NE(manager.getCurrentState(), nullptr);
    EXPECT_EQ(manager.getCurrentState()->getType(), StateType::Construction);
}

TEST(MenuStateModeSelectionTest, Toggle2PThenStartSurvivalCreatesPlayer2) {
    GameStateManager manager;
    manager.pushState(std::make_unique<MenuState>(manager));
    manager.update(0.0f);

    ASSERT_NE(manager.getCurrentState(), nullptr);
    ASSERT_EQ(manager.getCurrentState()->getType(), StateType::Menu);

    ScriptedInput input;
    pressKeyOnce(manager, input, SDL_SCANCODE_P);
    pressKeyOnce(manager, input, SDL_SCANCODE_2);
    manager.update(0.0f);

    ASSERT_NE(manager.getCurrentState(), nullptr);
    ASSERT_EQ(manager.getCurrentState()->getType(), StateType::Playing);

    auto* playing = dynamic_cast<PlayingState*>(manager.getCurrentState());
    ASSERT_NE(playing, nullptr);
    EXPECT_TRUE(playing->twoPlayerMode_);
    EXPECT_NE(playing->player2_, nullptr);
}

TEST(MenuStateModeSelectionTest, MenuTankCountMatchesSelectedPlayerMode) {
    GameStateManager manager;
    MenuState menu(manager);
    menu.enter();

    MockRenderer renderer;
    menu.render(renderer);
    EXPECT_EQ(renderer.getDrawCallCount(), 1);
    const auto onePlayerTank = renderer.getLastDrawCall();
    EXPECT_GT(onePlayerTank.destX, 150);
    EXPECT_EQ(onePlayerTank.destY, 356);
    EXPECT_EQ(onePlayerTank.destW, 26);
    EXPECT_EQ(onePlayerTank.destH, 26);

    ScriptedInput input;
    input.setKeyDown(SDL_SCANCODE_P, true);
    menu.handleInput(input);

    renderer.resetDrawCallCount();
    menu.render(renderer);
    EXPECT_EQ(renderer.getDrawCallCount(), 2);
}

TEST(MenuStateModeSelectionTest, MuteShortcutAndButtonRestorePreviousVolume) {
    GameStateManager manager;
    manager.setMasterVolume(0.6f);
    MenuState menu(manager);
    menu.enter();
    ScriptedInput input;

    input.setKeyDown(SDL_SCANCODE_M, true);
    menu.handleInput(input);
    EXPECT_TRUE(manager.isMuted());

    input.advanceFrame();
    input.setKeyDown(SDL_SCANCODE_M, false);
    input.advanceFrame();
    input.setKeyDown(SDL_SCANCODE_M, true);
    menu.handleInput(input);
    EXPECT_FALSE(manager.isMuted());
    EXPECT_FLOAT_EQ(manager.getMasterVolume(), 0.6f);

    input.reset();
    input.setMousePosition(450, 20);
    input.setMouseButtonDown(SDL_BUTTON_LEFT, true);
    menu.handleInput(input);
    EXPECT_TRUE(manager.isMuted());
}

} // namespace tank::test
