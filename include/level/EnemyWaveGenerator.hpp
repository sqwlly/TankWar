#pragma once

#include "level/Level.hpp"
#include <vector>

namespace tank {

/**
 * @brief Simple wave-based enemy spawn list generator.
 *
 * Generates 3 waves with increasing difficulty and can apply them to a Level.
 */
class EnemyWaveGenerator {
public:
    enum class Difficulty {
        Easy = 0,
        Normal = 1,
        Hard = 2
    };

    struct Wave {
        Difficulty difficulty;
        std::vector<EnemySpawnInfo> enemies;
    };

    static std::vector<Wave> generateThreeWaves(
        int levelNumber, GameDifficulty difficulty = GameDifficulty::Normal);
    static void applyToLevel(Level& level, int levelNumber,
                             GameDifficulty difficulty = GameDifficulty::Normal);
};

} // namespace tank
