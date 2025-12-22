/**
 * @file RectangleTest.cpp
 * @brief Unit tests for Rectangle collision detection
 */

#include <gtest/gtest.h>
#include "utils/Rectangle.hpp"

namespace tank {
namespace test {

class RectangleTest : public ::testing::Test {
protected:
    Rectangle rect1{0, 0, 10, 10};
    Rectangle rect2{5, 5, 10, 10};
    Rectangle rect3{15, 15, 10, 10};
};

// Test basic intersects functionality
TEST_F(RectangleTest, IntersectsOverlapping) {
    EXPECT_TRUE(rect1.intersects(rect2));
    EXPECT_TRUE(rect2.intersects(rect1));
}

TEST_F(RectangleTest, IntersectsNonOverlapping) {
    EXPECT_FALSE(rect1.intersects(rect3));
    EXPECT_FALSE(rect3.intersects(rect1));
}

// Edge case: adjacent rectangles should NOT intersect
TEST_F(RectangleTest, AdjacentRectanglesDoNotIntersect) {
    Rectangle a{0, 0, 10, 10};
    Rectangle b{10, 0, 10, 10};  // Exactly adjacent on right

    EXPECT_FALSE(a.intersects(b)) << "Adjacent rectangles should not intersect";
    EXPECT_FALSE(b.intersects(a));
}

// Edge case: touching at corner
TEST_F(RectangleTest, TouchingCornersDoNotIntersect) {
    Rectangle a{0, 0, 10, 10};
    Rectangle b{10, 10, 10, 10};  // Touches at (10, 10)

    EXPECT_FALSE(a.intersects(b));
}

// Edge case: same position
TEST_F(RectangleTest, SamePositionIntersects) {
    Rectangle a{0, 0, 10, 10};
    Rectangle b{0, 0, 10, 10};

    EXPECT_TRUE(a.intersects(b));
}

// Test contains point
TEST_F(RectangleTest, ContainsPointInside) {
    EXPECT_TRUE(rect1.contains(Vector2{5, 5}));
}

TEST_F(RectangleTest, ContainsPointOnEdge) {
    // Edge cases for contains - left and top are inclusive
    EXPECT_TRUE(rect1.contains(Vector2{0, 0}));
    // Right and bottom are exclusive
    EXPECT_FALSE(rect1.contains(Vector2{10, 5}));
    EXPECT_FALSE(rect1.contains(Vector2{5, 10}));
}

// Test moved() function
TEST_F(RectangleTest, MoveReturnsNewRectangle) {
    Rectangle moved = rect1.moved(Vector2{5, 5});

    EXPECT_EQ(moved.x, 5);
    EXPECT_EQ(moved.y, 5);
    EXPECT_EQ(moved.width, 10);
    EXPECT_EQ(moved.height, 10);

    // Original unchanged
    EXPECT_EQ(rect1.x, 0);
    EXPECT_EQ(rect1.y, 0);
}

// Test center calculation
TEST_F(RectangleTest, CenterCalculation) {
    Vector2 center = rect1.center();
    EXPECT_EQ(center.x, 5);
    EXPECT_EQ(center.y, 5);
}

} // namespace test
} // namespace tank
