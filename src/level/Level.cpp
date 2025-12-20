#include "level/Level.hpp"

namespace tank {

Level::Level(int levelNumber)
    : levelNumber_(levelNumber)
    , width_(Constants::GRID_WIDTH)
    , height_(Constants::GRID_HEIGHT)
    , player1Spawn_(4 * Constants::CELL_SIZE, (Constants::GRID_HEIGHT - 2) * Constants::CELL_SIZE)
    , player2Spawn_(8 * Constants::CELL_SIZE, (Constants::GRID_HEIGHT - 2) * Constants::CELL_SIZE)
    , basePosition_(Constants::GRID_WIDTH / 2 * Constants::CELL_SIZE, (Constants::GRID_HEIGHT - 2) * Constants::CELL_SIZE)
{
    initializeDefault();
}

void Level::initializeDefault() {
    // Initialize terrain map with empty tiles
    terrainMap_.resize(height_);
    for (auto& row : terrainMap_) {
        row.resize(width_, TerrainType::Empty);
    }

    // Default enemy spawn points (3 locations at top)
    enemySpawnPoints_.clear();
    enemySpawnPoints_.push_back(Vector2(0, 0));                                                     // Left top
    enemySpawnPoints_.push_back(Vector2((width_ / 2) * Constants::CELL_SIZE, 0));                   // Center top
    enemySpawnPoints_.push_back(Vector2((width_ - 2) * Constants::CELL_SIZE, 0));                   // Right top
}

void Level::setTerrainAt(int x, int y, TerrainType type) {
    if (x >= 0 && x < width_ && y >= 0 && y < height_) {
        terrainMap_[y][x] = type;
    }
}

TerrainType Level::getTerrainAt(int x, int y) const {
    if (x >= 0 && x < width_ && y >= 0 && y < height_) {
        return terrainMap_[y][x];
    }
    return TerrainType::Empty;
}

void Level::addEnemySpawn(EnemyType type, bool hasPowerUp) {
    enemySpawnList_.push_back({type, hasPowerUp});
}

void Level::clear() {
    for (auto& row : terrainMap_) {
        std::fill(row.begin(), row.end(), TerrainType::Empty);
    }
    enemySpawnList_.clear();
}

} // namespace tank
