#include <gtest/gtest.h>

#include "utils/ProgressStore.hpp"

#include <filesystem>
#include <fstream>

namespace tank::test {
namespace {

std::filesystem::path testPath(const char* name) {
    return std::filesystem::temp_directory_path() / name;
}

} // namespace

TEST(ProgressStoreTest, MissingFileReturnsSafeDefaults) {
    const auto path = testPath("TankGameProgressStoreMissing.properties");
    std::error_code error;
    std::filesystem::remove(path, error);

    const ProgressData progress = ProgressStore(path.string()).load();

    EXPECT_EQ(progress.highScore, 0);
    EXPECT_EQ(progress.levelToPlay, 1);
    EXPECT_FLOAT_EQ(progress.masterVolume, 1.0f);
    EXPECT_EQ(progress.difficulty, GameDifficulty::Normal);
}

TEST(ProgressStoreTest, SaveRoundTripPreservesUnknownProperties) {
    const auto path = testPath("TankGameProgressStoreRoundTrip.properties");
    {
        std::ofstream original(path);
        ASSERT_TRUE(original.is_open());
        original << "customKey=keep-me\n";
        original << "highScore=100\n";
        original << "levelToPlay=2\n";
    }

    ProgressStore store(path.string());
    ProgressData progress = store.load();
    progress.highScore = 5600;
    progress.levelToPlay = 4;
    progress.masterVolume = 0.35f;
    progress.difficulty = GameDifficulty::Hard;
    ASSERT_TRUE(store.save(progress));

    const ProgressData restored = store.load();
    EXPECT_EQ(restored.highScore, 5600);
    EXPECT_EQ(restored.levelToPlay, 4);
    EXPECT_FLOAT_EQ(restored.masterVolume, 0.35f);
    EXPECT_EQ(restored.difficulty, GameDifficulty::Hard);

    std::ifstream saved(path);
    std::string contents((std::istreambuf_iterator<char>(saved)), std::istreambuf_iterator<char>());
    EXPECT_NE(contents.find("customKey=keep-me"), std::string::npos);

    std::error_code error;
    std::filesystem::remove(path, error);
}

TEST(ProgressStoreTest, CorruptValuesFallBackToDefaults) {
    const auto path = testPath("TankGameProgressStoreCorrupt.properties");
    {
        std::ofstream original(path);
        ASSERT_TRUE(original.is_open());
        original << "highScore=abc\n";
        original << "levelToPlay=xyz\n";
        original << "masterVolume=lots\n";
        original << "difficulty=hard\n";
    }

    const ProgressData progress = ProgressStore(path.string()).load();

    EXPECT_EQ(progress.highScore, 0);
    EXPECT_EQ(progress.levelToPlay, 1);
    EXPECT_FLOAT_EQ(progress.masterVolume, 1.0f);
    EXPECT_EQ(progress.difficulty, GameDifficulty::Normal);

    std::error_code error;
    std::filesystem::remove(path, error);
}

TEST(ProgressStoreTest, OutOfRangeValuesAreClampedOnLoad) {
    const auto path = testPath("TankGameProgressStoreClamp.properties");
    {
        std::ofstream original(path);
        ASSERT_TRUE(original.is_open());
        original << "highScore=-50\n";
        original << "levelToPlay=99\n";
        original << "masterVolume=5.0\n";
        original << "difficulty=9\n";
    }

    const ProgressData progress = ProgressStore(path.string()).load();

    EXPECT_EQ(progress.highScore, 0);
    EXPECT_EQ(progress.levelToPlay, Constants::MAX_LEVEL);
    EXPECT_FLOAT_EQ(progress.masterVolume, 1.0f);
    EXPECT_EQ(progress.difficulty, GameDifficulty::Hard);

    std::error_code error;
    std::filesystem::remove(path, error);
}

} // namespace tank::test
