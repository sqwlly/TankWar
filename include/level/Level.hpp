#pragma once

#include "utils/Constants.hpp"
#include "entities/terrain/ITerrain.hpp"
#include <vector>
#include <memory>
#include <string>

namespace tank {

/**
 * @brief Enemy spawn configuration
 */
struct EnemySpawnInfo {
    EnemyType type;
    bool hasPowerUp;
};

/**
 * @brief Level data container
 */
class Level {
public:
    Level(int levelNumber = 1);
    ~Level() = default;

    // Getters
    int getLevelNumber() const { return levelNumber_; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }

    // Terrain access
    const std::vector<std::vector<TerrainType>>& getTerrainMap() const { return terrainMap_; }
    void setTerrainAt(int x, int y, TerrainType type);
    TerrainType getTerrainAt(int x, int y) const;

    // Enemy configuration
    const std::vector<EnemySpawnInfo>& getEnemySpawnList() const { return enemySpawnList_; }
    void addEnemySpawn(EnemyType type, bool hasPowerUp = false);
    void clearEnemySpawns() { enemySpawnList_.clear(); }

    // Player spawn positions
    Vector2 getPlayer1Spawn() const { return player1Spawn_; }
    Vector2 getPlayer2Spawn() const { return player2Spawn_; }
    void setPlayer1Spawn(const Vector2& pos) { player1Spawn_ = pos; }
    void setPlayer2Spawn(const Vector2& pos) { player2Spawn_ = pos; }

    // Enemy spawn points (3 locations at top)
    const std::vector<Vector2>& getEnemySpawnPoints() const { return enemySpawnPoints_; }
    void addEnemySpawnPoint(const Vector2& pos) { enemySpawnPoints_.push_back(pos); }

    // Base position
    Vector2 getBasePosition() const { return basePosition_; }
    void setBasePosition(const Vector2& pos) { basePosition_ = pos; }

    // Initialize with default values
    void initializeDefault();

    // Clear level data
    void clear();

private:
    int levelNumber_;
    int width_;
    int height_;

    std::vector<std::vector<TerrainType>> terrainMap_;
    std::vector<EnemySpawnInfo> enemySpawnList_;
    std::vector<Vector2> enemySpawnPoints_;

    Vector2 player1Spawn_;
    Vector2 player2Spawn_;
    Vector2 basePosition_;
};

} // namespace tank
