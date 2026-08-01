#include <gtest/gtest.h>

#define private public
#define protected public
#include "states/PlayingState.hpp"
#undef private
#undef protected

#include "ai/AIBehavior.hpp"
#include "states/GameStateManager.hpp"

namespace tank::test {

class PowerUpEffectsTest : public ::testing::Test {
protected:
    GameStateManager manager_;
    PlayingState state_{manager_, /*levelNumber=*/1, /*twoPlayer=*/false, /*useWaveGenerator=*/false};

    void SetUp() override {
        state_.enter();
        ASSERT_NE(state_.player1_, nullptr);
        state_.player1_->update(1.0f);
        state_.enemies_.clear();
        state_.bullets_.clear();
        state_.effects_.clear();
        state_.enemiesAlive_ = 0;
    }
};

TEST_F(PowerUpEffectsTest, GunImmediatelySetsMaximumTankLevel) {
    state_.applyPowerUp(*state_.player1_, PowerUpType::Gun);

    EXPECT_EQ(state_.player1_->getLevel(), 3);
}

TEST_F(PowerUpEffectsTest, IronCapAndTankApplyPlayerScopedEffects) {
    state_.player1Lives_ = Constants::MAX_PLAYER_LIVES - 1;
    state_.applyPowerUp(*state_.player1_, PowerUpType::IronCap);
    state_.applyPowerUp(*state_.player1_, PowerUpType::Tank);
    state_.applyPowerUp(*state_.player1_, PowerUpType::Tank);

    EXPECT_TRUE(state_.player1_->isInvincible());
    EXPECT_EQ(state_.player1Lives_, Constants::MAX_PLAYER_LIVES);
}

TEST_F(PowerUpEffectsTest, StopWatchFreezesEnemyUpdates) {
    auto enemy = std::make_unique<EnemyTank>(Vector2(100.0f, 100.0f), EnemyType::Basic);
    enemy->setAIBehavior(std::make_unique<SimpleAI>());
    EnemyTank* enemyPtr = enemy.get();
    state_.enemies_.push_back(std::move(enemy));
    state_.enemiesAlive_ = 1;

    const Vector2 before = enemyPtr->getPosition();
    state_.applyPowerUp(*state_.player1_, PowerUpType::StopWatch);
    state_.updateEntities(0.5f);

    EXPECT_GT(state_.freezeTimer_, 0.0f);
    EXPECT_EQ(enemyPtr->getPosition().x, before.x);
    EXPECT_EQ(enemyPtr->getPosition().y, before.y);
}

TEST_F(PowerUpEffectsTest, BombClearsEnemiesWithoutPowerUpChainDrops) {
    for (int i = 0; i < 2; ++i) {
        auto enemy = std::make_unique<EnemyTank>(Vector2(80.0f + i * 50.0f, 100.0f), EnemyType::Basic);
        enemy->setCarriesPowerUp(true);
        state_.enemies_.push_back(std::move(enemy));
    }
    state_.enemiesAlive_ = 2;

    state_.applyPowerUp(*state_.player1_, PowerUpType::Bomb);
    state_.removeDeadEntities();

    EXPECT_TRUE(state_.enemies_.empty());
    EXPECT_EQ(state_.enemiesAlive_, 0);
    EXPECT_EQ(state_.powerUpManager_.getCount(), 0u);
    EXPECT_EQ(manager_.getEnemyKillCount(EnemyType::Basic), 2);
    int explosionCount = 0;
    for (const auto& effect : state_.effects_) {
        explosionCount += dynamic_cast<TankExplosion*>(effect.get()) != nullptr;
    }
    EXPECT_EQ(explosionCount, 2);
}

TEST_F(PowerUpEffectsTest, SpadeFortifiesAndRestoresBasePerimeter) {
    ASSERT_NE(state_.level_, nullptr);
    const Vector2 base = state_.level_->getBasePosition();
    const int baseX = static_cast<int>(base.x) / Constants::CELL_SIZE;
    const int baseY = static_cast<int>(base.y) / Constants::CELL_SIZE;

    // The 2x2 base occupies (baseX..baseX+1, baseY..baseY+1); the classic
    // wall is 4 cells directly above + 2 cells down each side.
    const std::vector<std::pair<int, int>> expected = {
        {baseX - 1, baseY - 1}, {baseX, baseY - 1},
        {baseX + 1, baseY - 1}, {baseX + 2, baseY - 1},
        {baseX - 1, baseY}, {baseX + 2, baseY},
        {baseX - 1, baseY + 1}, {baseX + 2, baseY + 1}
    };

    state_.applyPowerUp(*state_.player1_, PowerUpType::Spade);

    ASSERT_EQ(state_.fortifiedCells_.size(), 8u);
    for (const auto& [x, y] : expected) {
        EXPECT_EQ(state_.level_->getTerrainAt(x, y), TerrainType::Steel)
            << "cell (" << x << "," << y << ")";
    }
    // The two cells directly above the base must be protected.
    EXPECT_EQ(state_.level_->getTerrainAt(baseX, baseY - 1), TerrainType::Steel);
    EXPECT_EQ(state_.level_->getTerrainAt(baseX + 1, baseY - 1), TerrainType::Steel);

    state_.updateTimedPowerUps(Constants::POWERUP_BASE_FORTIFY_DURATION);

    EXPECT_TRUE(state_.fortifiedCells_.empty());
    for (const auto& [x, y] : expected) {
        EXPECT_EQ(state_.level_->getTerrainAt(x, y), TerrainType::Brick)
            << "cell (" << x << "," << y << ")";
    }
}

TEST_F(PowerUpEffectsTest, SpadeDoesNotResurrectDestroyedBricks) {
    // Find a brick wall away from the base and destroy two of its corners.
    BrickWall* target = nullptr;
    for (const auto& terrain : state_.terrains_) {
        if (auto* brick = dynamic_cast<BrickWall*>(terrain.get())) {
            target = brick;
            break;
        }
    }
    ASSERT_NE(target, nullptr);

    const int cellX = static_cast<int>(target->getPosition().x) / Constants::CELL_SIZE;
    const int cellY = static_cast<int>(target->getPosition().y) / Constants::CELL_SIZE;
    target->cornerStates_[0] = false;  // TL destroyed
    target->cornerStates_[1] = false;  // TR destroyed

    // Fortifying rebuilds all terrain entities from the level map; the map
    // must first absorb the live entity state, or destroyed corners return.
    state_.applyPowerUp(*state_.player1_, PowerUpType::Spade);

    EXPECT_EQ(state_.level_->getTerrainAt(cellX, cellY), TerrainType::Empty);
    EXPECT_EQ(state_.level_->getTerrainAt(cellX + 1, cellY), TerrainType::Empty);
    // Surviving corners of the same block must survive the rebuild.
    EXPECT_EQ(state_.level_->getTerrainAt(cellX, cellY + 1), TerrainType::Brick);
    EXPECT_EQ(state_.level_->getTerrainAt(cellX + 1, cellY + 1), TerrainType::Brick);
}

TEST_F(PowerUpEffectsTest, SpadeDoesNotResurrectFullyClearedBrickBlocks) {
    // Destroy a whole 2x2 brick block; removeDeadEntities erases the entity.
    BrickWall* target = nullptr;
    for (const auto& terrain : state_.terrains_) {
        if (auto* brick = dynamic_cast<BrickWall*>(terrain.get())) {
            target = brick;
            break;
        }
    }
    ASSERT_NE(target, nullptr);

    const int cellX = static_cast<int>(target->getPosition().x) / Constants::CELL_SIZE;
    const int cellY = static_cast<int>(target->getPosition().y) / Constants::CELL_SIZE;
    target->cornerStates_.fill(false);
    state_.removeDeadEntities();

    state_.applyPowerUp(*state_.player1_, PowerUpType::Spade);

    for (int dy = 0; dy < 2; ++dy) {
        for (int dx = 0; dx < 2; ++dx) {
            EXPECT_EQ(state_.level_->getTerrainAt(cellX + dx, cellY + dy), TerrainType::Empty)
                << "cell (" << cellX + dx << "," << cellY + dy << ")";
        }
    }
}

TEST_F(PowerUpEffectsTest, UnfrozenEnemyMovesAndFires) {
    // Control case: without StopWatch the same enemy setup must act.
    auto enemy = std::make_unique<EnemyTank>(Vector2(100.0f, 100.0f), EnemyType::Basic);
    enemy->setAIBehavior(std::make_unique<SimpleAI>());
    EnemyTank* enemyPtr = enemy.get();
    state_.enemies_.push_back(std::move(enemy));
    state_.enemiesAlive_ = 1;

    const Vector2 before = enemyPtr->getPosition();
    state_.updateEntities(1.0f);  // SimpleAI fires after 0.8s

    EXPECT_FALSE(state_.bullets_.empty());
    EXPECT_TRUE(enemyPtr->getPosition().x != before.x ||
                enemyPtr->getPosition().y != before.y);
}

TEST_F(PowerUpEffectsTest, StopWatchAlsoStopsEnemyFire) {
    auto enemy = std::make_unique<EnemyTank>(Vector2(100.0f, 100.0f), EnemyType::Basic);
    enemy->setAIBehavior(std::make_unique<SimpleAI>());
    state_.enemies_.push_back(std::move(enemy));
    state_.enemiesAlive_ = 1;

    state_.applyPowerUp(*state_.player1_, PowerUpType::StopWatch);
    state_.updateEntities(1.0f);  // would exceed the 0.8s fire interval

    EXPECT_GT(state_.freezeTimer_, 0.0f);
    EXPECT_TRUE(state_.bullets_.empty());
}

TEST_F(PowerUpEffectsTest, TankPowerUpGrantsLifeToCollectingPlayer2) {
    GameStateManager twoPlayerManager;
    PlayingState twoPlayer(twoPlayerManager, 1, /*twoPlayer=*/true, /*useWaveGenerator=*/false);
    twoPlayer.enter();
    ASSERT_NE(twoPlayer.player2_, nullptr);
    twoPlayer.player2_->update(1.0f);

    const int p1Before = twoPlayer.player1Lives_;
    const int p2Before = twoPlayer.player2Lives_;
    twoPlayer.applyPowerUp(*twoPlayer.player2_, PowerUpType::Tank);

    EXPECT_EQ(twoPlayer.player2Lives_, p2Before + 1);
    EXPECT_EQ(twoPlayer.player1Lives_, p1Before);
}

TEST_F(PowerUpEffectsTest, DropWeightsCoverAllTypesWithStarMostCommon) {
    std::array<int, 7> counts{};
    constexpr int DRAWS = 10000;
    for (int i = 0; i < DRAWS; ++i) {
        ++counts[static_cast<int>(state_.chooseRandomPowerUp())];
    }

    // Every type (weights 10-30%) must appear in 10k draws.
    for (int count : counts) {
        EXPECT_GT(count, 0);
    }
    // Star (weight 30) dominates any 10-weight row by roughly 3x.
    EXPECT_EQ(counts[static_cast<int>(PowerUpType::Star)],
              *std::max_element(counts.begin(), counts.end()));
    EXPECT_GT(counts[static_cast<int>(PowerUpType::Star)],
              counts[static_cast<int>(PowerUpType::Bomb)] * 2);
}

} // namespace tank::test
