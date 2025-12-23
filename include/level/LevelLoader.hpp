#pragma once

#include "level/Level.hpp"
#include <string>
#include <memory>
#include <fstream>

namespace tank {

/**
 * @brief Loads level data from file
 * File format:
 *   Line 1:
 *     - Legacy: enemy types only, each char 1-4 represents enemy type (power-up derived by index)
 *     - Explicit: starts with '@', then each enemy is 1-4 optionally followed by '*' to mark power-up
 *   Lines 2-27: 26x26 map grid
 *     0 = empty, 1 = steel, 2 = brick, 3 = water, 4/5 = grass
 */
class LevelLoader {
public:
    LevelLoader() = default;
    ~LevelLoader() = default;

    // Load level from file
    std::unique_ptr<Level> loadFromFile(const std::string& filePath, int levelNumber);

    // Load level by number (e.g., Level_1, Level_2, ...)
    std::unique_ptr<Level> loadLevel(int levelNumber, const std::string& basePath = "assets/levels/");

    // Save level to file
    bool saveToFile(const Level& level, const std::string& filePath);

    // Get total number of levels
    static int getTotalLevels() { return TOTAL_LEVELS; }

private:
    static constexpr int TOTAL_LEVELS = 20;

    // Parse enemy list from first line
    void parseEnemyList(const std::string& line, Level& level);

    // Parse map grid from lines
    void parseMapGrid(std::ifstream& file, Level& level);

    // Convert file value to terrain type
    TerrainType valueToTerrainType(int value);

    // Convert terrain type to file value
    int terrainTypeToValue(TerrainType type);
};

} // namespace tank
