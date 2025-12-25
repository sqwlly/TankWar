#include <gtest/gtest.h>

#define private public
#define protected public
#include "states/PlayingState.hpp"
#undef private
#undef protected

#include "entities/terrain/BrickWall.hpp"
#include "mocks/ScriptedInput.hpp"
#include "states/GameStateManager.hpp"

namespace tank::test {

TEST(PlayerMovementAndBrickWallTest, Player1MovesUpWhenHoldingW) {
    GameStateManager manager;
    PlayingState state(manager, /*levelNumber=*/1, /*twoPlayer=*/false, /*useWaveGenerator=*/false);
    state.enter();

    ASSERT_NE(state.player1_, nullptr);
    state.player1_->update(1.0f);  // finish spawn animation

    const Vector2 before = state.player1_->getPosition();

    ScriptedInput input;
    input.setKeyDown(SDL_SCANCODE_W, true);
    state.handleInput(input);
    state.update(0.016f);

    const Vector2 after = state.player1_->getPosition();
    EXPECT_LT(after.y, before.y);
}

TEST(PlayerMovementAndBrickWallTest, BrickWallCornersSplitEvenly) {
    BrickWall brick(Vector2(0.0f, 0.0f));

    const Rectangle tl = brick.getCornerBounds(0);
    const Rectangle tr = brick.getCornerBounds(1);
    const Rectangle bl = brick.getCornerBounds(2);
    const Rectangle br = brick.getCornerBounds(3);

    EXPECT_FLOAT_EQ(tl.width, tr.width);
    EXPECT_FLOAT_EQ(tl.width, bl.width);
    EXPECT_FLOAT_EQ(tl.width, br.width);

    EXPECT_FLOAT_EQ(tl.height, tr.height);
    EXPECT_FLOAT_EQ(tl.height, bl.height);
    EXPECT_FLOAT_EQ(tl.height, br.height);

    EXPECT_FLOAT_EQ(tl.width + tr.width, static_cast<float>(Constants::ELEMENT_SIZE));
    EXPECT_FLOAT_EQ(tl.height + bl.height, static_cast<float>(Constants::ELEMENT_SIZE));
}

} // namespace tank::test
