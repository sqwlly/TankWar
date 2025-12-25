#include <gtest/gtest.h>

#define private public
#define protected public
#include "states/PlayingState.hpp"
#undef private
#undef protected

#include "entities/effects/Effect.hpp"
#include "states/GameStateManager.hpp"

namespace tank::test {

TEST(PlayingStateEffectsTest, BulletHitSpawnsBulletExplosion) {
    GameStateManager manager;
    PlayingState state(manager, /*levelNumber=*/1, /*twoPlayer=*/false, /*useWaveGenerator=*/false);
    state.enter();

    state.effects_.clear();
    state.bullets_.clear();
    state.enemies_.clear();
    state.terrains_.clear();
    state.base_.reset();

    state.terrains_.push_back(std::make_unique<BrickWall>(Vector2(0.0f, 0.0f)));
    state.bullets_.push_back(std::make_unique<Bullet>(Vector2(0.0f, 0.0f), Direction::Up, nullptr, 0));

    state.checkCollisions();

    bool foundBulletExplosion = false;
    for (const auto& effect : state.effects_) {
        if (dynamic_cast<BulletExplosion*>(effect.get()) != nullptr) {
            foundBulletExplosion = true;
            break;
        }
    }

    EXPECT_TRUE(foundBulletExplosion);
}

TEST(PlayingStateEffectsTest, TankDeathSpawnsTankExplosion) {
    GameStateManager manager;
    PlayingState state(manager, /*levelNumber=*/1, /*twoPlayer=*/false, /*useWaveGenerator=*/false);
    state.enter();

    state.effects_.clear();
    state.bullets_.clear();
    state.enemies_.clear();
    state.terrains_.clear();
    state.base_.reset();

    auto enemy = std::make_unique<EnemyTank>(Vector2(100.0f, 100.0f), EnemyType::Basic);
    state.enemies_.push_back(std::move(enemy));

    ASSERT_NE(state.player1_, nullptr);
    state.bullets_.push_back(std::make_unique<Bullet>(Vector2(100.0f, 100.0f), Direction::Up, state.player1_.get(), 0));

    state.checkCollisions();

    bool foundTankExplosion = false;
    bool foundBulletExplosion = false;
    for (const auto& effect : state.effects_) {
        foundTankExplosion |= dynamic_cast<TankExplosion*>(effect.get()) != nullptr;
        foundBulletExplosion |= dynamic_cast<BulletExplosion*>(effect.get()) != nullptr;
    }

    EXPECT_TRUE(foundTankExplosion);
    EXPECT_TRUE(foundBulletExplosion);
}

} // namespace tank::test

