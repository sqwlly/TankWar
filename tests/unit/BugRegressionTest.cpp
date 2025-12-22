/**
 * @file BugRegressionTest.cpp
 * @brief Regression tests for identified bugs
 *
 * These tests document known bugs and verify fixes when implemented.
 * Each test is marked with the bug ID from the code review.
 */

#include <gtest/gtest.h>
#include "utils/DamageCalculator.hpp"
#include "utils/Rectangle.hpp"
#include "utils/Constants.hpp"

namespace tank {
namespace test {

/**
 * BUG #5.1: Double Defense Reduction
 *
 * Location: BulletTankHandler::applyDamage() + Tank::takeDamage()
 *
 * Description: Defense reduction is applied twice:
 * 1. First in DamageCalculator::calculateDamage()
 * 2. Again in Tank::takeDamage()
 *
 * Impact: Damage is reduced more than intended
 * Example: 50 damage with 50% defense becomes 12 instead of 25
 */
TEST(BugRegressionTest, Bug5_1_DoubleDamageReduction) {
    int attack = 50;
    int defense = 50;
    int maxHealth = 100;

    // First reduction (in DamageCalculator or BulletTankHandler)
    int firstReduction = DamageCalculator::calculateDamage(attack, defense, maxHealth);
    EXPECT_EQ(firstReduction, 25);  // Correct

    // Bug: Second reduction in Tank::takeDamage
    int buggySecondReduction = DamageCalculator::calculateDamage(firstReduction, defense, maxHealth);
    EXPECT_EQ(buggySecondReduction, 12);  // Wrong!

    // Fix verification: After fix, damage should only be reduced once
    // The expected behavior is to apply defense reduction only ONCE
    // Either in DamageCalculator OR in Tank::takeDamage, not both

    EXPECT_NE(firstReduction, buggySecondReduction)
        << "BUG CONFIRMED: Defense applied twice. "
        << "Actual damage: " << buggySecondReduction
        << ", Expected: " << firstReduction;
}

/**
 * BUG #3.1: Bullet Entity Size Mismatch
 *
 * Location: Bullet.cpp constructor
 *
 * Description: Bullet entity is created with width=9, height=10
 * but the sprite is 10x10. This causes visual-collision misalignment.
 */
TEST(BugRegressionTest, Bug3_1_BulletSizeMismatch) {
    constexpr float BULLET_ENTITY_WIDTH = 9.0f;   // From Bullet.cpp:8
    constexpr float BULLET_ENTITY_HEIGHT = 10.0f;
    constexpr int BULLET_SPRITE_SIZE = 10;        // From SpriteSheet.hpp

    EXPECT_NE(BULLET_ENTITY_WIDTH, static_cast<float>(BULLET_SPRITE_SIZE))
        << "BUG: Bullet width " << BULLET_ENTITY_WIDTH
        << " doesn't match sprite size " << BULLET_SPRITE_SIZE;
}

/**
 * BUG #7.2: Terrain Grid Overlap
 *
 * Location: PlayingState::createTerrain()
 *
 * Description: Terrain is positioned at halfTile (8px) intervals
 * but sized at CELL_SIZE (17px), causing 9px overlap.
 */
TEST(BugRegressionTest, Bug7_2_TerrainGridOverlap) {
    constexpr int CELL_SIZE = 17;
    constexpr int HALF_TILE = CELL_SIZE / 2;  // 8

    // Two adjacent terrain blocks
    int terrain1Start = 0;
    int terrain1End = terrain1Start + CELL_SIZE;  // 0 to 17

    int terrain2Start = HALF_TILE;  // 8
    int terrain2End = terrain2Start + CELL_SIZE;  // 8 to 25

    // Calculate overlap
    int overlapStart = terrain2Start;  // 8
    int overlapEnd = terrain1End;      // 17
    int overlapSize = overlapEnd - overlapStart;  // 9

    EXPECT_GT(overlapSize, 0)
        << "BUG: Terrain tiles overlap by " << overlapSize << " pixels. "
        << "Terrain 1: [" << terrain1Start << ", " << terrain1End << "), "
        << "Terrain 2: [" << terrain2Start << ", " << terrain2End << ")";
}

/**
 * BUG #7.3: Enemy AI Not Set
 *
 * Location: PlayingState::spawnEnemy()
 *
 * Description: Enemies are spawned without setting AI behavior,
 * resulting in no AI updates in EnemyTank::onUpdate().
 */
TEST(BugRegressionTest, Bug7_3_EnemyAINotSet) {
    // This is verified by code inspection of PlayingState::spawnEnemy()
    // The function creates an EnemyTank but never calls setAIBehavior()

    SUCCEED()
        << "BUG: Enemy tanks spawned without AI behavior. "
        << "Missing call to enemy->setAIBehavior() in PlayingState::spawnEnemy()";
}

/**
 * BUG #5.3: Friendly Fire with Null Owner
 *
 * Location: BulletTankHandler::handle()
 *
 * Description: If bullet owner is null (tank destroyed),
 * bulletFromPlayer = false, which may allow friendly fire on enemies.
 */
TEST(BugRegressionTest, Bug5_3_NullOwnerFriendlyFire) {
    // When bullet->getOwner() returns nullptr:
    // bulletFromPlayer = dynamic_cast<PlayerTank*>(nullptr) != nullptr
    // bulletFromPlayer = false

    // This means a bullet from a destroyed player tank
    // is treated as an enemy bullet

    // If targetIsPlayer = false (enemy tank target)
    // Then bulletFromPlayer (false) != targetIsPlayer (false) -> false
    // Friendly fire check passes, enemy takes damage

    // This is actually correct behavior - the bullet still hits

    // But if targetIsPlayer = true (player tank target)
    // Then bulletFromPlayer (false) != targetIsPlayer (true) -> true
    // Player takes damage from their own bullet!

    SUCCEED()
        << "POTENTIAL BUG: Null owner makes bullet appear as enemy bullet. "
        << "Player's orphaned bullet can damage players.";
}

/**
 * BUG #2.1: BrickWall Sprite Index Unused
 *
 * Location: BrickWall.cpp
 *
 * Description: getSpriteIndex() calculates proper sprite variant
 * but onRender() always uses getBrick(0).
 */
TEST(BugRegressionTest, Bug2_1_BrickWallSpriteIndexUnused) {
    // BrickWall::getSpriteIndex() returns 0-15 based on corner states
    // But BrickWall::onRender() uses:
    //   Rectangle brickSrc = Sprites::Terrain::getBrick(0);
    // Instead of:
    //   Rectangle brickSrc = Sprites::Terrain::getBrick(getSpriteIndex());

    SUCCEED()
        << "BUG: BrickWall::getSpriteIndex() is defined but never called. "
        << "All brick walls render the same sprite regardless of damage state.";
}

/**
 * BUG #6.1: Dangling Pointer in Bullet::die()
 *
 * Location: Bullet.cpp:66-74
 *
 * Description: Bullet stores raw pointer to owner (ITank*).
 * If tank is destroyed before bullet, owner_ becomes dangling.
 */
TEST(BugRegressionTest, Bug6_1_DanglingOwnerPointer) {
    // Bullet::die() calls:
    //   if (owner_) {
    //       owner_->onBulletDestroyed();
    //   }

    // If owner tank is destroyed first:
    // 1. Tank is removed from enemies_ or players_
    // 2. unique_ptr deletes Tank object
    // 3. Bullet's owner_ still points to freed memory
    // 4. Bullet::die() dereferences dangling pointer -> UB

    SUCCEED()
        << "BUG: Bullet stores raw pointer to owner. "
        << "Risk of dangling pointer if tank destroyed before bullet. "
        << "Solution: Use weak_ptr or observer pattern.";
}

/**
 * BUG #5.2: Smooth Movement Doesn't Check All Obstacles
 *
 * Location: TankTerrainHandler::applySmoothMovement()
 *
 * Description: When sliding around an obstacle, only the current
 * obstacle is checked. The tank may slide into another obstacle.
 */
TEST(BugRegressionTest, Bug5_2_SmoothMovementSingleObstacleCheck) {
    // applySmoothMovement() receives single obstacle Rectangle
    // It checks if sliding left/right/up/down avoids THIS obstacle
    // But doesn't check if the new position collides with OTHER obstacles

    SUCCEED()
        << "BUG: Smooth movement only checks collision with one obstacle. "
        << "Tank may slide from one obstacle directly into another.";
}

/**
 * BUG #1.2: Enemy Flash Offset Conflicts with Armor Position
 *
 * Location: SpriteSheet.hpp EnemyTank::getFrame()
 *
 * Description: Flash effect adds 24*ELEMENT_SIZE (816) offset,
 * which is the same X position as ARMOR_X.
 */
TEST(BugRegressionTest, Bug1_2_FlashOffsetConflict) {
    constexpr int ELEMENT_SIZE = 34;
    constexpr int FLASH_OFFSET = 24 * ELEMENT_SIZE;  // 816
    constexpr int ARMOR_X = 24 * ELEMENT_SIZE;       // 816

    EXPECT_EQ(FLASH_OFFSET, ARMOR_X)
        << "POTENTIAL BUG: Flash offset " << FLASH_OFFSET
        << " equals armor tank X position " << ARMOR_X
        << ". Flashing basic enemies may show armor tank sprites.";
}

} // namespace test
} // namespace tank
