#include <gtest/gtest.h>

#define private public
#define protected public
#include "states/PlayingState.hpp"
#undef private
#undef protected

#include "states/GameStateManager.hpp"

namespace tank::test {

TEST(PlayerTankUpgradeTest, CollectStarUpgradesPlayerTankLevel) {
    GameStateManager manager;
    PlayingState state(manager, /*levelNumber=*/1, /*twoPlayer=*/false, /*useWaveGenerator=*/false);
    state.enter();

    ASSERT_NE(state.player1_, nullptr);
    state.player1_->update(1.0f);  // finish spawn animation

    state.enemies_.clear();
    state.bullets_.clear();
    state.terrains_.clear();
    state.base_.reset();
    state.effects_.clear();
    state.powerUpManager_.clear();

    const int beforeLevel = state.player1_->getLevel();
    state.powerUpManager_.spawn(state.player1_->getPosition(), PowerUpType::Star);

    state.checkCollisions();

    EXPECT_EQ(state.player1_->getLevel(), beforeLevel + 1);
    EXPECT_EQ(state.powerUpManager_.getCount(), 0u);
}

} // namespace tank::test

