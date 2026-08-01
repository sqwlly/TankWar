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

std::vector<EnemyWaveGenerator::Wave> EnemyWaveGenerator::generateThreeWaves(
    int levelNumber, GameDifficulty difficulty) {

    std::vector<Wave> waves;
    waves.reserve(3);

    const bool hardMode = difficulty == GameDifficulty::Hard;
    const bool easyMode = difficulty == GameDifficulty::Easy;
    const int levelPressure = levelNumber >= 10 ? 1 : 0;

    // A speed wave teaches tracking, an armour wave rewards positioning, and
    // the final mixed wave combines both. Keep the classic 20-enemy total.
    Wave speed;
    speed.difficulty = Difficulty::Easy;
    addEnemies(speed.enemies, easyMode ? EnemyType::Basic : EnemyType::Fast, 6, true);
    waves.push_back(std::move(speed));

    Wave armour;
    armour.difficulty = Difficulty::Normal;
    addEnemies(armour.enemies, hardMode ? EnemyType::Heavy : EnemyType::Basic, 2);
    addEnemies(armour.enemies, easyMode ? EnemyType::Basic : EnemyType::Heavy, 5, true);
    waves.push_back(std::move(armour));

    Wave mixed;
    mixed.difficulty = Difficulty::Hard;
    addEnemies(mixed.enemies, EnemyType::Basic, easyMode ? 3 : 2);
    addEnemies(mixed.enemies, EnemyType::Fast, 2 + levelPressure);
    addEnemies(mixed.enemies, EnemyType::Power, hardMode ? 3 : 2);
    addEnemies(mixed.enemies, hardMode ? EnemyType::Heavy : EnemyType::Basic,
               easyMode ? 0 : 1, true);
    // Keep wave size stable after pressure replaces a basic tank with a fast one.
    while (mixed.enemies.size() > 7) {
        mixed.enemies.erase(mixed.enemies.begin());
    }
    while (mixed.enemies.size() < 7) {
        mixed.enemies.push_back({EnemyType::Basic, false});
    }
    if (!mixed.enemies.empty()) {
        mixed.enemies.back().hasPowerUp = true;
    }
    waves.push_back(std::move(mixed));

    return waves;
}

void EnemyWaveGenerator::applyToLevel(Level& level, int levelNumber, GameDifficulty difficulty) {
    level.clearEnemySpawns();

    const auto waves = generateThreeWaves(levelNumber, difficulty);
    for (const auto& wave : waves) {
        for (const auto& enemy : wave.enemies) {
            level.addEnemySpawn(enemy.type, enemy.hasPowerUp);
        }
    }
}

} // namespace tank
