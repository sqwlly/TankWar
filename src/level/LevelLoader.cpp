#include "level/LevelLoader.hpp"
#include <sstream>
#include <iostream>

namespace tank {

std::unique_ptr<Level> LevelLoader::loadFromFile(const std::string& filePath, int levelNumber) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open level file: " << filePath << std::endl;
        return nullptr;
    }

    auto level = std::make_unique<Level>(levelNumber);

    // Read first line (enemy types)
    std::string enemyLine;
    if (std::getline(file, enemyLine)) {
        parseEnemyList(enemyLine, *level);
    }

    // Read map grid
    parseMapGrid(file, *level);

    file.close();
    return level;
}

std::unique_ptr<Level> LevelLoader::loadLevel(int levelNumber, const std::string& basePath) {
    std::string filePath = basePath + "Level_" + std::to_string(levelNumber);
    return loadFromFile(filePath, levelNumber);
}

bool LevelLoader::saveToFile(const Level& level, const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to create level file: " << filePath << std::endl;
        return false;
    }

    // Write enemy list
    const auto& enemies = level.getEnemySpawnList();
    for (const auto& enemy : enemies) {
        int value = static_cast<int>(enemy.type) + 1;  // 1-based in file
        file << value;
    }
    file << "\n";

    // Write map grid
    const auto& terrainMap = level.getTerrainMap();
    for (int y = 0; y < level.getHeight(); ++y) {
        for (int x = 0; x < level.getWidth(); ++x) {
            int value = terrainTypeToValue(terrainMap[y][x]);
            file << value;
        }
        file << "\n";
    }

    file.close();
    return true;
}

void LevelLoader::parseEnemyList(const std::string& line, Level& level) {
    level.clearEnemySpawns();

    for (char c : line) {
        if (c >= '1' && c <= '4') {
            int typeIndex = c - '1';  // Convert to 0-based
            EnemyType type = static_cast<EnemyType>(typeIndex);

            // Every 4th enemy has a power-up (original game logic)
            bool hasPowerUp = (level.getEnemySpawnList().size() % 4 == 3);

            level.addEnemySpawn(type, hasPowerUp);
        }
    }
}

void LevelLoader::parseMapGrid(std::ifstream& file, Level& level) {
    std::string line;
    int y = 0;

    while (std::getline(file, line) && y < level.getHeight()) {
        for (int x = 0; x < static_cast<int>(line.length()) && x < level.getWidth(); ++x) {
            int value = line[x] - '0';
            TerrainType type = valueToTerrainType(value);
            level.setTerrainAt(x, y, type);
        }
        ++y;
    }
}

TerrainType LevelLoader::valueToTerrainType(int value) {
    switch (value) {
        case 0: return TerrainType::Empty;
        case 1: return TerrainType::Steel;
        case 2: return TerrainType::Brick;
        case 3: return TerrainType::Grass;
        case 5: return TerrainType::Water;
        default: return TerrainType::Empty;
    }
}

int LevelLoader::terrainTypeToValue(TerrainType type) {
    switch (type) {
        case TerrainType::Empty: return 0;
        case TerrainType::Steel: return 1;
        case TerrainType::Brick: return 2;
        case TerrainType::Grass: return 3;
        case TerrainType::Water: return 5;
        case TerrainType::Base:  return 0;  // Base is special, not in grid
        default: return 0;
    }
}

} // namespace tank
