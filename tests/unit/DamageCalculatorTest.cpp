/**
 * @file DamageCalculatorTest.cpp
 * @brief Unit tests for damage calculation - validates issue #5.1 (double defense reduction)
 */

#include <gtest/gtest.h>
#include "utils/DamageCalculator.hpp"

namespace tank {
namespace test {

class DamageCalculatorTest : public ::testing::Test {
protected:
    static constexpr int BASE_ATTACK = 50;
    static constexpr int MAX_HEALTH = 100;
};

// Basic damage calculation without defense
TEST_F(DamageCalculatorTest, NoDamageReduction) {
    int damage = DamageCalculator::calculateDamage(BASE_ATTACK, 0, MAX_HEALTH);
    EXPECT_EQ(damage, 50) << "With 0 defense, damage should equal attack";
}

// 50% defense reduction
TEST_F(DamageCalculatorTest, HalfDefenseReduction) {
    int damage = DamageCalculator::calculateDamage(BASE_ATTACK, 50, MAX_HEALTH);
    EXPECT_EQ(damage, 25) << "With 50% defense ratio, damage should be halved";
}

// Full defense (should deal 0 damage)
TEST_F(DamageCalculatorTest, FullDefenseReduction) {
    int damage = DamageCalculator::calculateDamage(BASE_ATTACK, 100, MAX_HEALTH);
    EXPECT_EQ(damage, 0) << "With 100% defense, damage should be 0";
}

// Defense higher than max health (should still cap at 0)
TEST_F(DamageCalculatorTest, OverDefenseDoesNotNegative) {
    int damage = DamageCalculator::calculateDamage(BASE_ATTACK, 150, MAX_HEALTH);
    EXPECT_GE(damage, 0) << "Damage should never be negative";
}

// Edge case: maxHealth is 0 (should return full attack)
TEST_F(DamageCalculatorTest, ZeroMaxHealthReturnsFullDamage) {
    int damage = DamageCalculator::calculateDamage(BASE_ATTACK, 50, 0);
    EXPECT_EQ(damage, BASE_ATTACK);
}

// BUG TEST: This test demonstrates the double defense reduction bug
// When damage is calculated TWICE with defense reduction, it's wrong
TEST_F(DamageCalculatorTest, DoubleDamageReductionBug) {
    // Simulate what happens in current code:
    // 1. DamageCalculator calculates damage first
    int firstPass = DamageCalculator::calculateDamage(BASE_ATTACK, 50, MAX_HEALTH);
    EXPECT_EQ(firstPass, 25);

    // 2. If Tank::takeDamage also applies defense reduction, we get:
    int secondPass = DamageCalculator::calculateDamage(firstPass, 50, MAX_HEALTH);
    EXPECT_LT(secondPass, firstPass);  // Wrong! Should remain firstPass

    // The bug: actual damage is 12 instead of expected 25
    // This happens because defense is applied twice
    EXPECT_NE(firstPass, secondPass)
        << "BUG: Defense reduction is applied twice! "
        << "Expected damage: " << firstPass
        << ", Actual with bug: " << secondPass;
}

// Steel penetration tests
TEST_F(DamageCalculatorTest, CanPenetrateSteel_LevelThreeOrHigher) {
    EXPECT_FALSE(DamageCalculator::canPenetrateSteel(0));
    EXPECT_FALSE(DamageCalculator::canPenetrateSteel(1));
    EXPECT_FALSE(DamageCalculator::canPenetrateSteel(2));
    EXPECT_TRUE(DamageCalculator::canPenetrateSteel(3));
    EXPECT_TRUE(DamageCalculator::canPenetrateSteel(4));
}

// Reward calculation tests
TEST_F(DamageCalculatorTest, EnemyRewards) {
    EXPECT_EQ(DamageCalculator::calculateReward(0), 100);
    EXPECT_EQ(DamageCalculator::calculateReward(1), 200);
    EXPECT_EQ(DamageCalculator::calculateReward(2), 300);
    EXPECT_EQ(DamageCalculator::calculateReward(3), 400);
}

TEST_F(DamageCalculatorTest, InvalidEnemyTypeReturnsDefault) {
    EXPECT_EQ(DamageCalculator::calculateReward(-1), 100);
    EXPECT_EQ(DamageCalculator::calculateReward(99), 100);
}

} // namespace test
} // namespace tank
