#include "level/EnemyWaveGenerator.hpp"

namespace tank {

namespace {
void addEnemies(std::vector<EnemySpawnInfo>& out, EnemyType type, int count, bool lastHasPowerUp = false) {
    for (int i = 0; i < count; ++i) {
        const bool hasPowerUp = lastHasPowerUp && (i == count - 1);
        out.push_back(EnemySpawnInfo{type, hasPowerUp});
    }
}
} // namespace

std::vector<EnemyWaveGenerator::Wave> EnemyWaveGenerator::generateThreeWaves(int levelNumber) {
    (void)levelNumber;

    std::vector<Wave> waves;
    waves.reserve(3);

    // Wave 1 (Easy): mostly basic tanks
    Wave easy;
    easy.difficulty = Difficulty::Easy;
    addEnemies(easy.enemies, EnemyType::Basic, 4, /*lastHasPowerUp=*/true);
    waves.push_back(std::move(easy));

    // Wave 2 (Normal): mix basic/fast
    Wave normal;
    normal.difficulty = Difficulty::Normal;
    addEnemies(normal.enemies, EnemyType::Basic, 2);
    addEnemies(normal.enemies, EnemyType::Fast, 2, /*lastHasPowerUp=*/true);
    waves.push_back(std::move(normal));

    // Wave 3 (Hard): mix fast/power/heavy
    Wave hard;
    hard.difficulty = Difficulty::Hard;
    addEnemies(hard.enemies, EnemyType::Fast, 1);
    addEnemies(hard.enemies, EnemyType::Power, 2);
    addEnemies(hard.enemies, EnemyType::Heavy, 1, /*lastHasPowerUp=*/true);
    waves.push_back(std::move(hard));

    return waves;
}

void EnemyWaveGenerator::applyToLevel(Level& level, int levelNumber) {
    level.clearEnemySpawns();

    const auto waves = generateThreeWaves(levelNumber);
    for (const auto& wave : waves) {
        for (const auto& enemy : wave.enemies) {
            level.addEnemySpawn(enemy.type, enemy.hasPowerUp);
        }
    }
}

} // namespace tank

