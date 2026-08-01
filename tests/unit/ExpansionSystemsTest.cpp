#include <gtest/gtest.h>

#define private public
#define protected public
#include "states/PlayingState.hpp"
#include "states/MenuState.hpp"
#undef private
#undef protected

#include "ai/AIBehavior.hpp"
#include "level/EnemyWaveGenerator.hpp"
#include "mocks/ScriptedInput.hpp"
#include "states/GameStateManager.hpp"

#include <filesystem>
#include <algorithm>

namespace tank::test {

TEST(ExpansionSystemsTest, EnemyTypesReceiveDistinctBehaviorStrategies) {
    GameStateManager manager;
    PlayingState state(manager, 1, false, false);
    state.enter();

    EnemyTank basic(Vector2(0.0f, 0.0f), EnemyType::Basic);
    EnemyTank fast(Vector2(0.0f, 0.0f), EnemyType::Fast);
    EnemyTank power(Vector2(0.0f, 0.0f), EnemyType::Power);
    EnemyTank heavy(Vector2(0.0f, 0.0f), EnemyType::Heavy);
    state.configureEnemyAI(basic);
    state.configureEnemyAI(fast);
    state.configureEnemyAI(power);
    state.configureEnemyAI(heavy);

    ASSERT_NE(basic.getAIBehavior(), nullptr);
    ASSERT_NE(fast.getAIBehavior(), nullptr);
    ASSERT_NE(power.getAIBehavior(), nullptr);
    ASSERT_NE(heavy.getAIBehavior(), nullptr);
    EXPECT_EQ(basic.getAIBehavior()->getType(), IAIBehavior::Type::Simple);
    EXPECT_EQ(fast.getAIBehavior()->getType(), IAIBehavior::Type::Pathfinding);
    EXPECT_EQ(power.getAIBehavior()->getType(), IAIBehavior::Type::Ranged);
    EXPECT_EQ(heavy.getAIBehavior()->getType(), IAIBehavior::Type::Direct);
}

TEST(ExpansionSystemsTest, SurvivalWavesHaveThreeThemesAndClassicEnemyTotal) {
    const auto waves = EnemyWaveGenerator::generateThreeWaves(1, GameDifficulty::Normal);

    ASSERT_EQ(waves.size(), 3u);
    EXPECT_EQ(waves[0].difficulty, EnemyWaveGenerator::Difficulty::Easy);
    EXPECT_EQ(waves[1].difficulty, EnemyWaveGenerator::Difficulty::Normal);
    EXPECT_EQ(waves[2].difficulty, EnemyWaveGenerator::Difficulty::Hard);

    size_t total = 0;
    for (const auto& wave : waves) {
        total += wave.enemies.size();
    }
    EXPECT_EQ(total, static_cast<size_t>(Constants::TOTAL_ENEMIES_PER_LEVEL));
}

TEST(ExpansionSystemsTest, EnemyKillAwardsScoreAndMultiKillUsesSameDamageEvent) {
    GameStateManager manager;
    PlayingState state(manager, 1, false, false);
    state.enter();
    state.player1_->update(1.0f);
    state.enemies_.clear();
    state.enemiesAlive_ = 2;

    int damageEvent = 0;
    for (int i = 0; i < 2; ++i) {
        auto enemy = std::make_unique<EnemyTank>(Vector2(100.0f + i * 40.0f, 100.0f), EnemyType::Basic);
        EnemyTank* rawEnemy = enemy.get();
        state.enemies_.push_back(std::move(enemy));
        state.registerEnemyDefeat(*rawEnemy, state.player1_.get(), &damageEvent);
        rawEnemy->die();
    }

    state.removeDeadEntities();

    EXPECT_EQ(state.player1_->getScore(), 400);
    EXPECT_EQ(manager.getPlayerScore(1), 400);
    EXPECT_EQ(manager.getHighScore(), 400);
    EXPECT_TRUE(state.enemies_.empty());
}

TEST(ExpansionSystemsTest, SettingsMenuChangesVolumeAndDifficulty) {
    GameStateManager manager;
    MenuState menu(manager);
    menu.enter();
    menu.selectedItem_ = MenuState::MenuItem::Settings;
    menu.confirmSelection();

    ASSERT_TRUE(menu.settingsOpen_);
    ScriptedInput input;
    input.setKeyDown(SDL_SCANCODE_RIGHT, true);
    menu.handleInput(input);
    input.advanceFrame();

    EXPECT_FLOAT_EQ(manager.getMasterVolume(), 1.0f);

    input.setKeyDown(SDL_SCANCODE_RIGHT, false);
    input.advanceFrame();
    input.setKeyDown(SDL_SCANCODE_DOWN, true);
    menu.handleInput(input);
    input.advanceFrame();
    input.setKeyDown(SDL_SCANCODE_DOWN, false);
    input.advanceFrame();
    input.setKeyDown(SDL_SCANCODE_RIGHT, true);
    menu.handleInput(input);

    EXPECT_EQ(manager.getDifficulty(), GameDifficulty::Hard);
}

TEST(ExpansionSystemsTest, StateManagerPersistsHighScoreAndUnlockedStageWhenConfigured) {
    const auto path = std::filesystem::temp_directory_path() / "TankGameStateManagerProgress.properties";
    std::error_code error;
    std::filesystem::remove(path, error);

    GameStateManager manager;
    ASSERT_TRUE(manager.loadProgress(path.string()));
    manager.recordHighScore(9000);
    manager.unlockCampaignLevel(3);
    manager.setDifficulty(GameDifficulty::Hard);
    manager.setMasterVolume(0.4f);

    GameStateManager restored;
    ASSERT_TRUE(restored.loadProgress(path.string()));
    EXPECT_EQ(restored.getHighScore(), 9000);
    EXPECT_EQ(restored.getCampaignStartLevel(), 4);
    EXPECT_EQ(restored.getDifficulty(), GameDifficulty::Hard);
    EXPECT_FLOAT_EQ(restored.getMasterVolume(), 0.4f);

    std::filesystem::remove(path, error);
}

TEST(ExpansionSystemsTest, RangedAIKeepsFiringBandAroundBase) {
    const Vector2 base(100.0f, 100.0f);

    // Far beyond MAX_RANGE: closes in towards the base.
    EnemyTank far(Vector2(100.0f, 300.0f), EnemyType::Power);
    RangedAI farAi;
    farAi.setTarget(base);
    farAi.update(far, 0.016f);
    EXPECT_EQ(far.getDirection(), Direction::Up);
    EXPECT_LT(far.getPosition().y, 300.0f);

    // Closer than MIN_RANGE: backs away from the base.
    EnemyTank close(Vector2(100.0f, 120.0f), EnemyType::Power);
    RangedAI closeAi;
    closeAi.setTarget(base);
    closeAi.update(close, 0.016f);
    EXPECT_EQ(close.getDirection(), Direction::Down);
    EXPECT_GT(close.getPosition().y, 120.0f);

    // Inside the band: holds position but faces the base.
    EnemyTank inBand(Vector2(100.0f, 200.0f), EnemyType::Power);
    RangedAI bandAi;
    bandAi.setTarget(base);
    bandAi.update(inBand, 0.016f);
    EXPECT_EQ(inBand.getDirection(), Direction::Up);
    EXPECT_FLOAT_EQ(inBand.getPosition().x, 100.0f);
    EXPECT_FLOAT_EQ(inBand.getPosition().y, 200.0f);
}

TEST(ExpansionSystemsTest, DirectAIAlwaysPressesTowardBase) {
    const Vector2 base(204.0f, 408.0f);

    // Far away: moves straight at the base.
    EnemyTank far(Vector2(100.0f, 100.0f), EnemyType::Heavy);
    DirectAI farAi;
    farAi.setTarget(base);
    farAi.update(far, 0.016f);
    EXPECT_EQ(far.getDirection(), Direction::Down);
    EXPECT_GT(far.getPosition().y, 100.0f);

    // Even point-blank it keeps pushing instead of holding a position.
    EnemyTank close(Vector2(200.0f, 400.0f), EnemyType::Heavy);
    DirectAI closeAi;
    closeAi.setTarget(base);
    closeAi.update(close, 0.016f);
    EXPECT_EQ(close.getDirection(), Direction::Down);
    EXPECT_GT(close.getPosition().y, 400.0f);
}

TEST(ExpansionSystemsTest, SurvivalWavesUseThemedCompositions) {
    const auto waves = EnemyWaveGenerator::generateThreeWaves(1, GameDifficulty::Normal);
    ASSERT_EQ(waves.size(), 3u);

    auto countType = [](const EnemyWaveGenerator::Wave& wave, EnemyType type) {
        return std::count_if(wave.enemies.begin(), wave.enemies.end(),
            [type](const EnemySpawnInfo& info) { return info.type == type; });
    };

    // Speed wave: pure fast tanks.
    EXPECT_EQ(waves[0].enemies.size(), 6u);
    EXPECT_EQ(countType(waves[0], EnemyType::Fast), 6);

    // Armour wave: heavy armour core.
    EXPECT_EQ(waves[1].enemies.size(), 7u);
    EXPECT_EQ(countType(waves[1], EnemyType::Heavy), 5);

    // Mixed wave (Normal, level 1): 3 Basic + 2 Fast + 2 Power.
    EXPECT_EQ(waves[2].enemies.size(), 7u);
    EXPECT_EQ(countType(waves[2], EnemyType::Basic), 3);
    EXPECT_EQ(countType(waves[2], EnemyType::Fast), 2);
    EXPECT_EQ(countType(waves[2], EnemyType::Power), 2);

    // Each wave dangles exactly one power-up carrier.
    for (const auto& wave : waves) {
        EXPECT_EQ(std::count_if(wave.enemies.begin(), wave.enemies.end(),
            [](const EnemySpawnInfo& info) { return info.hasPowerUp; }), 1);
    }
}

} // namespace tank::test
