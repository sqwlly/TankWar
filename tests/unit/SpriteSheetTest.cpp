/**
 * @file SpriteSheetTest.cpp
 * @brief Unit tests for SpriteSheet coordinate calculations
 */

#include <gtest/gtest.h>
#include "graphics/SpriteSheet.hpp"
#include "utils/Constants.hpp"

namespace tank {
namespace test {

class SpriteSheetTest : public ::testing::Test {
protected:
    static constexpr int ELEMENT_SIZE = Sprites::ELEMENT_SIZE;  // 34
    static constexpr int HALF_SIZE = Sprites::HALF_SIZE;        // 17

    static int directionCol(Direction dir) {
        switch (dir) {
            case Direction::Up: return Sprites::Tank::DIR_UP_COL;
            case Direction::Right: return Sprites::Tank::DIR_RIGHT_COL;
            case Direction::Down: return Sprites::Tank::DIR_DOWN_COL;
            case Direction::Left: return Sprites::Tank::DIR_LEFT_COL;
        }
        return Sprites::Tank::DIR_UP_COL;
    }
};

// Player tank sprite tests
TEST_F(SpriteSheetTest, PlayerTankLevel0Sprites) {
    // Level 0, Up direction, frame 0
    Rectangle sprite = Sprites::Tank::getFrame(Sprites::Tank::P1_BASE_Y, directionCol(Direction::Up), 0, 0);

    EXPECT_EQ(sprite.x, 0.0f);
    EXPECT_EQ(sprite.y, 0.0f);
    EXPECT_EQ(sprite.width, ELEMENT_SIZE);
    EXPECT_EQ(sprite.height, ELEMENT_SIZE);
}

TEST_F(SpriteSheetTest, PlayerTankLevelOffsetMovesBy8Sprites) {
    Rectangle level0 = Sprites::Tank::getFrame(Sprites::Tank::P1_BASE_Y, directionCol(Direction::Up), 0, 0);
    Rectangle level1 = Sprites::Tank::getFrame(Sprites::Tank::P1_BASE_Y, directionCol(Direction::Up), 0, 1);

    EXPECT_EQ(level1.x - level0.x, 8.0f * ELEMENT_SIZE);
    EXPECT_EQ(level1.y, level0.y);
}

TEST_F(SpriteSheetTest, PlayerTankDirectionOrder) {
    // Direction order matches Constants.hpp: Up, Right, Down, Left.
    // Each direction has 2 frames, col step is 2 sprites (see SpriteSheet.hpp).

    Rectangle up0 = Sprites::Tank::getFrame(Sprites::Tank::P1_BASE_Y, directionCol(Direction::Up), 0, 0);
    Rectangle up1 = Sprites::Tank::getFrame(Sprites::Tank::P1_BASE_Y, directionCol(Direction::Up), 1, 0);
    Rectangle right0 = Sprites::Tank::getFrame(Sprites::Tank::P1_BASE_Y, directionCol(Direction::Right), 0, 0);

    // Up frames: 0, 1
    EXPECT_EQ(up0.x, 0.0f);
    EXPECT_EQ(up1.x, static_cast<float>(ELEMENT_SIZE));

    // Right starts at col 2
    EXPECT_EQ(right0.x, 2.0f * ELEMENT_SIZE);
}

// Bullet sprite tests
TEST_F(SpriteSheetTest, BulletDirectionLayout) {
    // Bullets at y=170 (5 * ELEMENT_SIZE)
    // In current implementation, bullets are a single row of 4 sprites, each 8x8,
    // with a constant +4 offset inside the element.

    Rectangle up = Sprites::Bullet::get(Direction::Up);
    Rectangle down = Sprites::Bullet::get(Direction::Down);
    Rectangle left = Sprites::Bullet::get(Direction::Left);
    Rectangle right = Sprites::Bullet::get(Direction::Right);

    EXPECT_EQ(up.x, 4.0f);
    EXPECT_EQ(up.y, 5.0f * ELEMENT_SIZE + 4.0f);

    EXPECT_EQ(left.x, 12.0f);
    EXPECT_EQ(left.y, up.y);

    EXPECT_EQ(down.x, 20.0f);
    EXPECT_EQ(down.y, up.y);

    EXPECT_EQ(right.x, 28.0f);
    EXPECT_EQ(right.y, up.y);

    EXPECT_EQ(up.width, 8);
    EXPECT_EQ(up.height, 8);
}

// Terrain sprite tests
TEST_F(SpriteSheetTest, BrickWallSprite) {
    Rectangle brick = Sprites::Terrain::getBrick(0);

    EXPECT_EQ(brick.x, 4.0f * ELEMENT_SIZE);   // 136
    EXPECT_EQ(brick.y, 5.0f * ELEMENT_SIZE);   // 170
    EXPECT_EQ(brick.width, ELEMENT_SIZE);
    EXPECT_EQ(brick.height, ELEMENT_SIZE);
}

TEST_F(SpriteSheetTest, GrassSpriteIsHalfSize) {
    Rectangle grass = Sprites::Terrain::getGrass();

    EXPECT_EQ(grass.width, HALF_SIZE);   // 17
    EXPECT_EQ(grass.height, HALF_SIZE);  // 17
}

TEST_F(SpriteSheetTest, WaterAnimationFrames) {
    Rectangle water0 = Sprites::Terrain::getWater(0);
    Rectangle water1 = Sprites::Terrain::getWater(1);

    // Two frames side by side
    EXPECT_EQ(water1.x - water0.x, static_cast<float>(ELEMENT_SIZE));
}

// Effect sprite tests
TEST_F(SpriteSheetTest, SpawnAnimation) {
    for (int i = 0; i < 4; ++i) {
        Rectangle spawn = Sprites::Spawn::get(i);

        EXPECT_EQ(spawn.x, 16.0f * ELEMENT_SIZE + i * ELEMENT_SIZE);
        EXPECT_EQ(spawn.y, 4.0f * ELEMENT_SIZE);
    }
}

TEST_F(SpriteSheetTest, TankExplosionSize) {
    // Tank explosion is 68x68 (2 * ELEMENT_SIZE)
    Rectangle explosion = Sprites::Explosion::getBig(0);

    EXPECT_EQ(explosion.width, 2 * ELEMENT_SIZE);
    EXPECT_EQ(explosion.height, 2 * ELEMENT_SIZE);
}

// Base sprite tests
TEST_F(SpriteSheetTest, BaseNormalAndDestroyed) {
    Rectangle normal = Sprites::Base::getNormal();
    Rectangle destroyed = Sprites::Base::getDestroyed();

    // Destroyed is one sprite to the right
    EXPECT_EQ(destroyed.x - normal.x, static_cast<float>(ELEMENT_SIZE));
    EXPECT_EQ(destroyed.y, normal.y);
}

} // namespace test
} // namespace tank
