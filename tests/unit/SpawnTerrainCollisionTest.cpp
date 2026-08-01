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

} // namespace tank::test
