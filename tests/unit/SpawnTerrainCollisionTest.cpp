#include <gtest/gtest.h>

#define private public
#define protected public
#include "states/PlayingState.hpp"
#undef private
#undef protected

#include "states/GameStateManager.hpp"
#include "states/MenuState.hpp"
#include "entities/terrain/SteelWall.hpp"
#include "collision/CollisionManager.hpp"
#include "mocks/ScriptedInput.hpp"

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

namespace tank::test {
namespace {

void pressKeyOnce(GameStateManager& manager, ScriptedInput& input, SDL_Scancode scancode) {
    input.setKeyDown(scancode, true);
    manager.handleInput(input);
    input.advanceFrame();
    input.setKeyDown(scancode, false);
    input.advanceFrame();
}

PlayingState* startCampaignLevel1(GameStateManager& manager, ScriptedInput& input) {
    manager.pushState(std::make_unique<MenuState>(manager));
    manager.update(0.0f);
    pressKeyOnce(manager, input, SDL_SCANCODE_1);
    manager.update(0.0f);  // -> StageState(1)
    manager.update(3.0f);
    manager.update(0.0f);  // -> PlayingState(1)
    return dynamic_cast<PlayingState*>(manager.getCurrentState());
}

// Writes a level file whose spawn cells are occupied by blocking terrain:
// steel on both player spawns, brick on the left enemy spawn, water on the
// center enemy spawn.
std::string writeSpawnBlockedLevelFile() {
    const std::string path = "test_spawn_clear_level.tmp";
    std::ofstream out(path);
    out << "@1111\n";
    std::vector<std::string> rows(Constants::GRID_HEIGHT, std::string(Constants::GRID_WIDTH, '0'));
    // Player-1 spawn cells (4..5, 24..25): steel
    rows[24][4] = '1'; rows[24][5] = '1'; rows[25][4] = '1'; rows[25][5] = '1';
    // Player-2 spawn cells (8..9, 24..25): steel
    rows[24][8] = '1'; rows[24][9] = '1'; rows[25][8] = '1'; rows[25][9] = '1';
    // Left enemy spawn cells (0..1, 0..1): brick
    rows[0][0] = '2'; rows[0][1] = '2'; rows[1][0] = '2'; rows[1][1] = '2';
    // Center enemy spawn cells (13..14, 0..1): water
    rows[0][13] = '3'; rows[0][14] = '3'; rows[1][13] = '3'; rows[1][14] = '3';
    for (const auto& row : rows) {
        out << row << '\n';
    }
    return path;
}

bool overlapsSolidTerrain(const PlayingState& state, const Rectangle& bounds) {
    for (const auto& terrain : state.terrains_) {
        if (!terrain->isActive() || terrain->isDestroyed() || terrain->isTankPassable()) {
            continue;
        }
        if (auto* brick = dynamic_cast<BrickWall*>(terrain.get())) {
            if (brick->intersectsSolid(bounds)) {
                return true;
            }
            continue;
        }
        if (CollisionManager::checkAABB(bounds, terrain->getBounds())) {
            return true;
        }
    }
    return false;
}

} // namespace

// Regression: tanks used to be excluded from terrain collision while the
// spawn animation played. Since tanks may move during the animation, they
// could drive into walls and end up stuck inside obstacles after respawn.
TEST(SpawnTerrainCollisionTest, SpawningTankCannotDriveIntoWall) {
    GameStateManager manager;
    ScriptedInput input;
    PlayingState* playing = startCampaignLevel1(manager, input);
    ASSERT_NE(playing, nullptr);
    ASSERT_NE(playing->player1_, nullptr);

    PlayerTank& tank = *playing->player1_;
    tank.respawn();
    ASSERT_TRUE(tank.isSpawning());

    // Place a steel wall directly above the spawn point
    const Vector2 spawnPos = tank.getSpawnPosition();
    playing->terrains_.push_back(
        std::make_unique<SteelWall>(Vector2(spawnPos.x, spawnPos.y - Constants::CELL_SIZE)));
    const ITerrain* wall = playing->terrains_.back().get();
    const Rectangle wallBounds = wall->getBounds();

    // Drive upward far longer than the spawn animation lasts
    for (int i = 0; i < 60; ++i) {
        tank.move(Direction::Up);
        playing->checkTankTerrainCollisions();

        EXPECT_FALSE(CollisionManager::checkAABB(tank.getBounds(), wallBounds))
            << "tank entered a wall during spawn animation (move " << i << ")";
    }

    // The tank must have stayed fully below the wall
    EXPECT_GE(tank.getBounds().y, wallBounds.y + wallBounds.height);
}

// Regression: initial spawn had no terrain sanitization at all, so a level
// (e.g. built in the construction editor) with blocking terrain on the spawn
// cells placed the player inside an obstacle from the first frame.
TEST(SpawnTerrainCollisionTest, LoadLevelClearsBlockingTerrainAtSpawnCells) {
    const std::string path = writeSpawnBlockedLevelFile();
    {
        GameStateManager manager;
        PlayingState state(manager, 1, /*twoPlayer=*/false, path);
        state.enter();
        ASSERT_NE(state.level_, nullptr);
        ASSERT_NE(state.player1_, nullptr);

        Level& level = *state.level_;
        // Player-1 spawn cells are sacred
        for (int y = 24; y <= 25; ++y) {
            for (int x = 4; x <= 5; ++x) {
                EXPECT_EQ(level.getTerrainAt(x, y), TerrainType::Empty)
                    << "player-1 spawn cell (" << x << "," << y << ") still blocked";
            }
        }
        // Enemy spawn points are cleared too - a blocked point would be
        // skipped forever and deadlock the stage.
        for (int y = 0; y <= 1; ++y) {
            for (int x = 0; x <= 1; ++x) {
                EXPECT_EQ(level.getTerrainAt(x, y), TerrainType::Empty)
                    << "left enemy spawn cell (" << x << "," << y << ") still blocked";
            }
            for (int x = 13; x <= 14; ++x) {
                EXPECT_EQ(level.getTerrainAt(x, y), TerrainType::Empty)
                    << "center enemy spawn cell (" << x << "," << y << ") still blocked";
            }
        }
        // The tank must not sit inside any solid terrain entity either
        EXPECT_FALSE(overlapsSolidTerrain(state, state.player1_->getBounds()));

        // Single-player: the unused player-2 spawn cells are ordinary map
        // terrain and stay untouched.
        EXPECT_EQ(level.getTerrainAt(8, 24), TerrainType::Steel);
    }
    std::remove(path.c_str());
}

TEST(SpawnTerrainCollisionTest, LoadLevelClearsPlayer2SpawnInTwoPlayerMode) {
    const std::string path = writeSpawnBlockedLevelFile();
    {
        GameStateManager manager;
        PlayingState state(manager, 1, /*twoPlayer=*/true, path);
        state.enter();
        ASSERT_NE(state.level_, nullptr);
        ASSERT_NE(state.player2_, nullptr);

        Level& level = *state.level_;
        for (int y = 24; y <= 25; ++y) {
            for (int x = 8; x <= 9; ++x) {
                EXPECT_EQ(level.getTerrainAt(x, y), TerrainType::Empty)
                    << "player-2 spawn cell (" << x << "," << y << ") still blocked";
            }
        }
        EXPECT_FALSE(overlapsSolidTerrain(state, state.player2_->getBounds()));
    }
    std::remove(path.c_str());
}

// Regression: fortify/restore used to materialize walls on top of tanks
// defending the base, burying them inside the obstacle.
TEST(SpawnTerrainCollisionTest, FortifyBaseDoesNotBuryTank) {
    GameStateManager manager;
    PlayingState state(manager, 1, /*twoPlayer=*/false, /*useWaveGenerator=*/false);
    state.enter();
    ASSERT_NE(state.player1_, nullptr);
    state.enemies_.clear();

    const Vector2 basePos = state.level_->getBasePosition();
    const int baseX = static_cast<int>(basePos.x) / Constants::CELL_SIZE;
    const int baseY = static_cast<int>(basePos.y) / Constants::CELL_SIZE;

    // Simulate the base-ring bricks having been shot away, then park the
    // player on the freed cells above-left of the base.
    Level& level = *state.level_;
    level.setTerrainAt(baseX - 1, baseY - 1, TerrainType::Empty);
    level.setTerrainAt(baseX, baseY - 1, TerrainType::Empty);
    level.setTerrainAt(baseX - 1, baseY, TerrainType::Empty);
    state.createTerrain();

    const Vector2 parked(static_cast<float>((baseX - 1) * Constants::CELL_SIZE),
                         static_cast<float>((baseY - 1) * Constants::CELL_SIZE));
    state.player1_->position_ = parked;
    state.player1_->previousPosition_ = parked;

    state.fortifyBase();

    EXPECT_EQ(level.getTerrainAt(baseX - 1, baseY - 1), TerrainType::Empty);
    EXPECT_EQ(level.getTerrainAt(baseX, baseY - 1), TerrainType::Empty);
    EXPECT_EQ(level.getTerrainAt(baseX - 1, baseY), TerrainType::Empty);
    // Unoccupied cells are fortified as usual
    EXPECT_EQ(level.getTerrainAt(baseX + 2, baseY - 1), TerrainType::Steel);
    EXPECT_EQ(level.getTerrainAt(baseX + 2, baseY), TerrainType::Steel);
    EXPECT_FALSE(overlapsSolidTerrain(state, state.player1_->getBounds()));
}

TEST(SpawnTerrainCollisionTest, RestoreFortifiedBaseDoesNotBuryTank) {
    GameStateManager manager;
    PlayingState state(manager, 1, /*twoPlayer=*/false, /*useWaveGenerator=*/false);
    state.enter();
    ASSERT_NE(state.player1_, nullptr);
    state.enemies_.clear();

    const Vector2 basePos = state.level_->getBasePosition();
    const int baseX = static_cast<int>(basePos.x) / Constants::CELL_SIZE;
    const int baseY = static_cast<int>(basePos.y) / Constants::CELL_SIZE;

    // Fortify with the player far away, then blast a gap into the steel ring
    // and park on the freed cells before the shield expires.
    state.fortifyBase();
    Level& level = *state.level_;
    level.setTerrainAt(baseX - 1, baseY - 1, TerrainType::Empty);
    level.setTerrainAt(baseX, baseY - 1, TerrainType::Empty);
    level.setTerrainAt(baseX - 1, baseY, TerrainType::Empty);
    state.createTerrain();

    const Vector2 parked(static_cast<float>((baseX - 1) * Constants::CELL_SIZE),
                         static_cast<float>((baseY - 1) * Constants::CELL_SIZE));
    state.player1_->position_ = parked;
    state.player1_->previousPosition_ = parked;

    state.restoreFortifiedBase();

    EXPECT_EQ(level.getTerrainAt(baseX - 1, baseY - 1), TerrainType::Empty);
    EXPECT_EQ(level.getTerrainAt(baseX, baseY - 1), TerrainType::Empty);
    EXPECT_EQ(level.getTerrainAt(baseX - 1, baseY), TerrainType::Empty);
    // Unoccupied cells are rebuilt as brick (classic behaviour)
    EXPECT_EQ(level.getTerrainAt(baseX + 2, baseY - 1), TerrainType::Brick);
    EXPECT_EQ(level.getTerrainAt(baseX + 2, baseY), TerrainType::Brick);
    EXPECT_FALSE(overlapsSolidTerrain(state, state.player1_->getBounds()));
}

} // namespace tank::test
