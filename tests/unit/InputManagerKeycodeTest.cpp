#include <gtest/gtest.h>

#define private public
#include "input/InputManager.hpp"
#undef private

namespace tank::test {

TEST(InputManagerKeycodeTest, KeycodeQueriesUseKeycodeState) {
    InputManager input;

    input.currentKeys_.fill(false);
    input.previousKeys_.fill(false);
    input.eventKeys_.fill(false);

    input.currentKeycodes_.clear();
    input.previousKeycodes_.clear();
    input.eventKeycodes_.clear();

    input.currentKeycodes_.insert(SDLK_w);
    input.eventKeycodes_.insert(SDLK_w);

    EXPECT_TRUE(input.isKeyDown(SDLK_w));
    EXPECT_TRUE(input.isKeyPressed(SDLK_w));
    EXPECT_FALSE(input.isKeyDown(SDL_SCANCODE_W));

    input.previousKeycodes_.insert(SDLK_w);
    input.currentKeycodes_.clear();
    EXPECT_TRUE(input.isKeyReleased(SDLK_w));
}

} // namespace tank::test

