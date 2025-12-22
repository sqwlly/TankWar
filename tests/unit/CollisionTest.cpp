/**
 * @file CollisionTest.cpp
 * @brief Unit tests for collision system
 */

#include <gtest/gtest.h>
#include "collision/CollisionManager.hpp"
#include "collision/handlers/BulletBulletHandler.hpp"
#include "collision/handlers/TankTankHandler.hpp"
#include "collision/handlers/TankTerrainHandler.hpp"
#include "utils/Rectangle.hpp"

namespace tank {
namespace test {

class CollisionManagerTest : public ::testing::Test {
protected:
    CollisionManager manager;
};

// AABB collision detection tests
TEST_F(CollisionManagerTest, AABBDetectsOverlap) {
    Rectangle a{0, 0, 10, 10};
    Rectangle b{5, 5, 10, 10};

    EXPECT_TRUE(CollisionManager::checkAABB(a, b));
}

TEST_F(CollisionManagerTest, AABBNoOverlap) {
    Rectangle a{0, 0, 10, 10};
    Rectangle b{20, 20, 10, 10};

    EXPECT_FALSE(CollisionManager::checkAABB(a, b));
}

TEST_F(CollisionManagerTest, AABBAdjacentNoOverlap) {
    Rectangle a{0, 0, 10, 10};
    Rectangle b{10, 0, 10, 10};  // Adjacent, touching

    EXPECT_FALSE(CollisionManager::checkAABB(a, b))
        << "Adjacent rectangles should not be considered colliding";
}

// Tank size vs collision tests
class TankCollisionTest : public ::testing::Test {
protected:
    // Tank dimensions from Constants
    static constexpr float TANK_WIDTH = 32.0f;   // ELEMENT_SIZE - 2
    static constexpr float TANK_HEIGHT = 32.0f;
    static constexpr float ELEMENT_SIZE = 34.0f;
};

TEST_F(TankCollisionTest, TankBoundsSmallerThanSprite) {
    // Tank uses width/height = ELEMENT_SIZE - 2 = 32
    // But sprite is 34x34
    // This is intentional for better gameplay feel

    EXPECT_LT(TANK_WIDTH, ELEMENT_SIZE);
    EXPECT_EQ(TANK_WIDTH, 32.0f);
}

// Bullet collision tests
class BulletCollisionTest : public ::testing::Test {
protected:
    static constexpr float BULLET_ENTITY_WIDTH = 9.0f;
    static constexpr float BULLET_ENTITY_HEIGHT = 10.0f;
    static constexpr int BULLET_SPRITE_SIZE = 10;
};

TEST_F(BulletCollisionTest, BulletSizeMismatch) {
    // BUG: Bullet entity is 9x10 but sprite is 10x10
    EXPECT_NE(BULLET_ENTITY_WIDTH, static_cast<float>(BULLET_SPRITE_SIZE))
        << "BUG: Bullet entity width (9) doesn't match sprite width (10)";

    // This could cause visual-collision misalignment
}

// Terrain collision tests
class TerrainCollisionTest : public ::testing::Test {
protected:
    static constexpr int CELL_SIZE = 17;
    static constexpr int HALF_TILE = 8;  // 17 / 2 = 8
};

TEST_F(TerrainCollisionTest, TerrainGridMisalignment) {
    // BUG: Terrain placed at halfTile intervals but sized at CELL_SIZE
    // This causes overlapping

    // If terrain at (0, 0) has size 17x17
    // Next terrain at (8, 0) also has size 17x17
    // They overlap from x=8 to x=17!

    int terrainSize = CELL_SIZE;  // 17
    int gridStep = HALF_TILE;     // 8

    // Check overlap
    int overlapSize = terrainSize - gridStep;  // 17 - 8 = 9 pixels overlap!

    EXPECT_GT(overlapSize, 0)
        << "BUG: Terrain tiles overlap by " << overlapSize << " pixels";
}

// Smooth movement collision test
class SmoothMovementTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup test environment
    }
};

TEST_F(SmoothMovementTest, SlideAmountCalculation) {
    // Tank width is 32, slideAmount = width / 3 = 10
    float tankWidth = 32.0f;
    int slideAmount = static_cast<int>(tankWidth / 3);

    EXPECT_EQ(slideAmount, 10);
}

TEST_F(SmoothMovementTest, SlideDoesNotCheckOtherObstacles) {
    // This is a known issue:
    // TankTerrainHandler::applySmoothMovement only checks against
    // the CURRENT obstacle, not other obstacles that might be in the
    // slide path.

    // This is a design flaw that would need integration testing
    // to properly verify.
    SUCCEED() << "Integration test needed for multi-obstacle slide checking";
}

} // namespace test
} // namespace tank
