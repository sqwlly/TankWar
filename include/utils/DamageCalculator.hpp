#pragma once

#include <algorithm>
#include <cmath>

namespace tank {

/**
 * @brief Utility class for damage calculations (SRP)
 * Centralizes all damage-related formulas for consistency
 */
class DamageCalculator {
public:
    /**
     * @brief Calculate actual damage after defense reduction
     * @param attack Base attack value
     * @param defense Target's defense value
     * @param maxHealth Target's max health (for ratio calculation)
     * @return Actual damage to apply
     */
    static int calculateDamage(int attack, int defense, int maxHealth) {
        if (maxHealth <= 0) return attack;

        float defenseRatio = static_cast<float>(defense) / static_cast<float>(maxHealth);
        float multiplier = std::max(0.0f, 1.0f - defenseRatio);
        return static_cast<int>(std::round(attack * multiplier));
    }

    /**
     * @brief Calculate reward points for destroying an enemy
     * @param enemyTypeIndex Index of enemy type (0-3)
     * @return Reward points
     */
    static int calculateReward(int enemyTypeIndex) {
        static constexpr int REWARDS[] = {100, 200, 300, 400};
        if (enemyTypeIndex < 0 || enemyTypeIndex > 3) return 100;
        return REWARDS[enemyTypeIndex];
    }

    /**
     * @brief Check if attack can penetrate steel
     * @param bulletLevel Level of the bullet
     * @return true if can destroy steel
     */
    static bool canPenetrateSteel(int bulletLevel) {
        return bulletLevel >= 3;
    }

private:
    DamageCalculator() = delete;  // Static utility class
};

} // namespace tank
