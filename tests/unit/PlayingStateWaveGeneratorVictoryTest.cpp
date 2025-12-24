#include <gtest/gtest.h>

#define private public
#define protected public
#include "states/PlayingState.hpp"
#include "states/StageState.hpp"
#undef private
#undef protected

#include "states/GameStateManager.hpp"

namespace tank::test {

TEST(PlayingStateWaveGeneratorVictoryTest, WaveGeneratorEnabled_VictoryTransitionsToNextLevelPlaying) {
    GameStateManager manager;
    manager.changeToPlaying(/*levelNumber=*/1, /*twoPlayer=*/true, /*useWaveGenerator=*/true);
    manager.update(0.0f);  // Apply pending change -> PlayingState

    ASSERT_NE(manager.getCurrentState(), nullptr);
    ASSERT_EQ(manager.getCurrentState()->getType(), StateType::Playing);

    auto* playing = dynamic_cast<PlayingState*>(manager.getCurrentState());
    ASSERT_NE(playing, nullptr);
    EXPECT_TRUE(playing->useWaveGenerator_);

    ASSERT_NE(playing->level_, nullptr);
    const int spawnCount = static_cast<int>(playing->level_->getEnemySpawnCount());
    ASSERT_GT(spawnCount, 0);

    // Simulate end-of-level condition: all enemies have been spawned and defeated.
    playing->enemies_.clear();
    playing->bullets_.clear();
    playing->enemiesAlive_ = 0;
    playing->enemiesSpawned_ = spawnCount;
    playing->levelComplete_ = false;

    manager.update(0.016f);
    manager.update(0.0f);  // Apply pending change -> StageState

    ASSERT_NE(manager.getCurrentState(), nullptr);
    EXPECT_EQ(manager.getCurrentState()->getType(), StateType::Stage);

    auto* stage = dynamic_cast<StageState*>(manager.getCurrentState());
    ASSERT_NE(stage, nullptr);
    EXPECT_EQ(stage->levelNumber_, 2);
    EXPECT_TRUE(stage->twoPlayerMode_);

    // Fast-forward StageState -> next PlayingState.
    manager.update(3.0f);
    manager.update(0.0f);

    ASSERT_NE(manager.getCurrentState(), nullptr);
    EXPECT_EQ(manager.getCurrentState()->getType(), StateType::Playing);

    auto* nextPlaying = dynamic_cast<PlayingState*>(manager.getCurrentState());
    ASSERT_NE(nextPlaying, nullptr);
    EXPECT_EQ(nextPlaying->getCurrentLevel(), 2);
    EXPECT_TRUE(nextPlaying->useWaveGenerator_);
}

} // namespace tank::test
