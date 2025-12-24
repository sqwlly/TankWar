#include <gtest/gtest.h>

#define private public
#define protected public
#include "states/PlayingState.hpp"
#include "states/StageState.hpp"
#undef private
#undef protected

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

TEST(CampaignFirstLevelFlowTest, Press1FromMenuStartsStage1ThenPlayingLevel1WithoutWaveGenerator) {
    GameStateManager manager;
    manager.pushState(std::make_unique<MenuState>(manager));
    manager.update(0.0f);

    ASSERT_NE(manager.getCurrentState(), nullptr);
    ASSERT_EQ(manager.getCurrentState()->getType(), StateType::Menu);

    ScriptedInput input;
    pressKeyOnce(manager, input, SDL_SCANCODE_1);
    manager.update(0.0f);  // Apply pending change -> StageState

    ASSERT_NE(manager.getCurrentState(), nullptr);
    ASSERT_EQ(manager.getCurrentState()->getType(), StateType::Stage);

    auto* stage = dynamic_cast<StageState*>(manager.getCurrentState());
    ASSERT_NE(stage, nullptr);
    EXPECT_EQ(stage->levelNumber_, 1);

    // Fast-forward StageState -> PlayingState.
    manager.update(3.0f);
    manager.update(0.0f);

    ASSERT_NE(manager.getCurrentState(), nullptr);
    ASSERT_EQ(manager.getCurrentState()->getType(), StateType::Playing);

    auto* playing = dynamic_cast<PlayingState*>(manager.getCurrentState());
    ASSERT_NE(playing, nullptr);
    EXPECT_EQ(playing->getCurrentLevel(), 1);
    EXPECT_FALSE(playing->useWaveGenerator_);

    ASSERT_NE(playing->level_, nullptr);
    EXPECT_EQ(static_cast<int>(playing->level_->getEnemySpawnCount()), 20)
        << "assets/levels/Level_1 first line defines 20 enemies";
}

TEST(CampaignFirstLevelFlowTest, VictoryFromLevel1TransitionsToStage2ThenPlayingLevel2WithoutWaveGenerator) {
    GameStateManager manager;
    manager.pushState(std::make_unique<MenuState>(manager));
    manager.update(0.0f);

    ScriptedInput input;
    pressKeyOnce(manager, input, SDL_SCANCODE_1);
    manager.update(0.0f);  // -> StageState(1)

    // StageState(1) -> PlayingState(1)
    manager.update(3.0f);
    manager.update(0.0f);

    ASSERT_NE(manager.getCurrentState(), nullptr);
    ASSERT_EQ(manager.getCurrentState()->getType(), StateType::Playing);

    auto* playing = dynamic_cast<PlayingState*>(manager.getCurrentState());
    ASSERT_NE(playing, nullptr);
    ASSERT_NE(playing->level_, nullptr);
    EXPECT_EQ(playing->getCurrentLevel(), 1);
    EXPECT_FALSE(playing->useWaveGenerator_);

    // Force victory condition for level 1.
    const int spawnCount = static_cast<int>(playing->level_->getEnemySpawnCount());
    ASSERT_GT(spawnCount, 0);
    playing->bullets_.clear();
    playing->enemies_.clear();
    playing->enemiesAlive_ = 0;
    playing->enemiesSpawned_ = spawnCount;
    playing->levelComplete_ = false;

    playing->checkGameState();
    manager.update(0.0f);  // Apply pending change -> StageState(2)

    ASSERT_NE(manager.getCurrentState(), nullptr);
    ASSERT_EQ(manager.getCurrentState()->getType(), StateType::Stage);

    auto* stage = dynamic_cast<StageState*>(manager.getCurrentState());
    ASSERT_NE(stage, nullptr);
    EXPECT_EQ(stage->levelNumber_, 2);
    EXPECT_FALSE(stage->useWaveGenerator_);

    // StageState(2) -> PlayingState(2)
    manager.update(3.0f);
    manager.update(0.0f);

    ASSERT_NE(manager.getCurrentState(), nullptr);
    ASSERT_EQ(manager.getCurrentState()->getType(), StateType::Playing);

    auto* playing2 = dynamic_cast<PlayingState*>(manager.getCurrentState());
    ASSERT_NE(playing2, nullptr);
    ASSERT_NE(playing2->level_, nullptr);
    EXPECT_EQ(playing2->getCurrentLevel(), 2);
    EXPECT_FALSE(playing2->useWaveGenerator_);
    EXPECT_EQ(static_cast<int>(playing2->level_->getEnemySpawnCount()), 20)
        << "assets/levels/Level_2 first line defines 20 enemies";
}

} // namespace tank::test
