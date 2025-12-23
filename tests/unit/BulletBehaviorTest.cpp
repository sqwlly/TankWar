#include <gtest/gtest.h>

// We need white-box access to PlayingState internals (players/bullets)
#define private public
#define protected public
#include "states/PlayingState.hpp"
#undef private
#undef protected

#include "states/GameStateManager.hpp"
#include "entities/projectiles/Bullet.hpp"
#include "input/InputManager.hpp"

namespace tank {
namespace test {

class BulletBehaviorTest : public ::testing::Test {
protected:
    static constexpr float kDelta = 0.016f;
};

TEST_F(BulletBehaviorTest, PlayerFireShouldCreateBulletInPlayingState) {
    GameStateManager manager;
    PlayingState state(manager, /*levelNumber=*/1, /*twoPlayer=*/false);

    state.level_ = std::make_unique<Level>(1);
    state.player1_ = std::make_unique<PlayerTank>(1, Vector2(0.0f, 0.0f));
    state.bullets_.clear();

    ASSERT_TRUE(state.bullets_.empty()) << "test precondition";

    state.player1_->update(1.0f);  // finish spawn animation

    InputManager::PlayerInput fireInput;
    fireInput.fire = true;
    state.player1_->handleInput(fireInput);
    state.updateEntities(kDelta);

    EXPECT_FALSE(state.bullets_.empty())
        << "Player firing should enqueue a Bullet inside PlayingState";
}

TEST_F(BulletBehaviorTest, BulletsClearOwnerWhenTankDetached) {
    GameStateManager manager;
    PlayingState state(manager, /*levelNumber=*/1, /*twoPlayer=*/false);
    state.level_ = std::make_unique<Level>(1);
    state.player1_ = std::make_unique<PlayerTank>(1, Vector2(0.0f, 0.0f));

    auto bullet = std::make_unique<Bullet>(Vector2(0.0f, 0.0f), Direction::Up, state.player1_.get(), 0);
    Bullet* bulletPtr = bullet.get();
    state.bullets_.push_back(std::move(bullet));

    state.detachBulletsFromTank(state.player1_.get());

    EXPECT_EQ(nullptr, bulletPtr->getOwner())
        << "Bullet owner pointer must be cleared once tank is removed from the state";
}

}  // namespace test
}  // namespace tank
