#include <gtest/gtest.h>

#include "level/Level.hpp"
#include "level/LevelLoader.hpp"

#include <filesystem>
#include <fstream>

namespace tank {

namespace fs = std::filesystem;

static fs::path makeTempPath(const std::string& name) {
    const auto base = fs::temp_directory_path() / "tankgame_tests";
    fs::create_directories(base);
    return base / name;
}

TEST(LevelLoaderTest, SaveAndLoad_ExplicitEnemyPowerUps_RoundTrips) {
    Level level(1);
    level.clear();

    level.clearEnemySpawns();
    level.addEnemySpawn(EnemyType::Basic, true);
    level.addEnemySpawn(EnemyType::Fast, false);
    level.addEnemySpawn(EnemyType::Power, true);
    level.addEnemySpawn(EnemyType::Heavy, false);

    level.setTerrainAt(0, 0, TerrainType::Steel);
    level.setTerrainAt(1, 0, TerrainType::Brick);
    level.setTerrainAt(2, 0, TerrainType::Water);
    level.setTerrainAt(3, 0, TerrainType::Grass);

    LevelLoader loader;
    const fs::path path = makeTempPath("LevelLoaderTest_explicit_roundtrip.txt");
    ASSERT_TRUE(loader.saveToFile(level, path.string()));

    auto loaded = loader.loadFromFile(path.string(), 1);
    ASSERT_NE(loaded, nullptr);

    ASSERT_EQ(loaded->getEnemySpawnCount(), level.getEnemySpawnCount());
    for (size_t i = 0; i < level.getEnemySpawnCount(); ++i) {
        const auto* a = level.getEnemySpawn(i);
        const auto* b = loaded->getEnemySpawn(i);
        ASSERT_NE(a, nullptr);
        ASSERT_NE(b, nullptr);
        EXPECT_EQ(b->type, a->type);
        EXPECT_EQ(b->hasPowerUp, a->hasPowerUp);
    }

    EXPECT_EQ(loaded->getTerrainAt(0, 0), TerrainType::Steel);
    EXPECT_EQ(loaded->getTerrainAt(1, 0), TerrainType::Brick);
    EXPECT_EQ(loaded->getTerrainAt(2, 0), TerrainType::Water);
    EXPECT_EQ(loaded->getTerrainAt(3, 0), TerrainType::Grass);
}

TEST(LevelLoaderTest, Load_LegacyEnemyList_DerivesPowerUpsByIndex) {
    const fs::path path = makeTempPath("LevelLoaderTest_legacy_enemylist.txt");
    std::ofstream out(path);
    ASSERT_TRUE(out.is_open());

    out << "1111\n";
    for (int y = 0; y < Constants::GRID_HEIGHT; ++y) {
        out << std::string(Constants::GRID_WIDTH, '0') << "\n";
    }
    out.close();

    LevelLoader loader;
    auto loaded = loader.loadFromFile(path.string(), 1);
    ASSERT_NE(loaded, nullptr);
    ASSERT_EQ(loaded->getEnemySpawnCount(), 4u);

    EXPECT_FALSE(loaded->getEnemySpawn(0)->hasPowerUp);
    EXPECT_FALSE(loaded->getEnemySpawn(1)->hasPowerUp);
    EXPECT_FALSE(loaded->getEnemySpawn(2)->hasPowerUp);
    EXPECT_TRUE(loaded->getEnemySpawn(3)->hasPowerUp);
}

TEST(LevelLoaderTest, Load_Terrain_GrassAccepts4And5) {
    const fs::path path = makeTempPath("LevelLoaderTest_grass_compat.txt");
    std::ofstream out(path);
    ASSERT_TRUE(out.is_open());

    out << "1\n";
    for (int y = 0; y < Constants::GRID_HEIGHT; ++y) {
        std::string row(Constants::GRID_WIDTH, '0');
        if (y == 0) {
            row[0] = '4';
            row[1] = '5';
        }
        out << row << "\n";
    }
    out.close();

    LevelLoader loader;
    auto loaded = loader.loadFromFile(path.string(), 1);
    ASSERT_NE(loaded, nullptr);

    EXPECT_EQ(loaded->getTerrainAt(0, 0), TerrainType::Grass);
    EXPECT_EQ(loaded->getTerrainAt(1, 0), TerrainType::Grass);
}

} // namespace tank

