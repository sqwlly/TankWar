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
};

// Player tank sprite tests
TEST_F(SpriteSheetTest, PlayerTankLevel0Sprites) {
    // Level 0, Up direction, frame 0
    Rectangle sprite = Sprites::PlayerTank::getFrame(0, Direction::Up, 0);

    EXPECT_EQ(sprite.x, 0.0f);
    EXPECT_EQ(sprite.y, 0.0f);
    EXPECT_EQ(sprite.width, ELEMENT_SIZE);
    EXPECT_EQ(sprite.height, ELEMENT_SIZE);
}

TEST_F(SpriteSheetTest, PlayerTankLevel3UsesCorrectRow) {
    // Level 3 uses a separate row at y=272
    Rectangle sprite = Sprites::PlayerTank::getFrame(3, Direction::Up, 0);

    EXPECT_EQ(sprite.y, 8.0f * ELEMENT_SIZE);  // 272
}

TEST_F(SpriteSheetTest, PlayerTankDirectionOrder) {
    // Direction order: Up=0, Down=1, Left=2, Right=3
    // Each direction has 2 frames

    Rectangle up0 = Sprites::PlayerTank::getFrame(0, Direction::Up, 0);
    Rectangle up1 = Sprites::PlayerTank::getFrame(0, Direction::Up, 1);
    Rectangle down0 = Sprites::PlayerTank::getFrame(0, Direction::Down, 0);

    // Up frames: 0, 1
    EXPECT_EQ(up0.x, 0.0f);
    EXPECT_EQ(up1.x, static_cast<float>(ELEMENT_SIZE));

    // Down frames: 2, 3
    EXPECT_EQ(down0.x, 2.0f * ELEMENT_SIZE);
}

// Enemy tank sprite tests
TEST_F(SpriteSheetTest, EnemyTankBasicTypes) {
    // Basic enemies at y=68 (2 * ELEMENT_SIZE)
    Rectangle basic = Sprites::EnemyTank::getFrame(0, Direction::Up, 0);

    EXPECT_EQ(basic.y, 2.0f * ELEMENT_SIZE);
}

TEST_F(SpriteSheetTest, EnemyTankArmorType) {
    // Armor (type 3) at x=816 (24 * ELEMENT_SIZE)
    Rectangle armor = Sprites::EnemyTank::getFrame(3, Direction::Up, 0);

    EXPECT_EQ(armor.x, 24.0f * ELEMENT_SIZE);
    EXPECT_EQ(armor.y, 0.0f);
}

TEST_F(SpriteSheetTest, EnemyTankFlashingOffset) {
    // Flashing enemies add 24 * ELEMENT_SIZE offset
    Rectangle normal = Sprites::EnemyTank::getFrame(0, Direction::Up, 0, false);
    Rectangle flash = Sprites::EnemyTank::getFrame(0, Direction::Up, 0, true);

    EXPECT_EQ(flash.x - normal.x, 24.0f * ELEMENT_SIZE);
}

// Bullet sprite tests
TEST_F(SpriteSheetTest, BulletDirectionLayout) {
    // Bullets at y=170 (5 * ELEMENT_SIZE)
    // Arranged in 2x2 grid, each 10x10

    Rectangle up = Sprites::Bullet::get(Direction::Up);
    Rectangle down = Sprites::Bullet::get(Direction::Down);
    Rectangle left = Sprites::Bullet::get(Direction::Left);
    Rectangle right = Sprites::Bullet::get(Direction::Right);

    // Up: row=0, col=0
    EXPECT_EQ(up.x, 0.0f);
    EXPECT_EQ(up.y, 5.0f * ELEMENT_SIZE);

    // Down: row=0, col=1
    EXPECT_EQ(down.x, 10.0f);
    EXPECT_EQ(down.y, 5.0f * ELEMENT_SIZE);

    // Left: row=1, col=0
    EXPECT_EQ(left.x, 0.0f);
    EXPECT_EQ(left.y, 5.0f * ELEMENT_SIZE + 10.0f);

    // Right: row=1, col=1
    EXPECT_EQ(right.x, 10.0f);
    EXPECT_EQ(right.y, 5.0f * ELEMENT_SIZE + 10.0f);

    // All should be 10x10
    EXPECT_EQ(up.width, 10);
    EXPECT_EQ(up.height, 10);
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
    Rectangle explosion = Sprites::TankExplosion::get(0);

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
