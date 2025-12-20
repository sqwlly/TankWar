#pragma once

#include "Vector2.hpp"
#include <algorithm>

namespace tank {

/**
 * @brief Axis-Aligned Bounding Box for collision detection
 */
struct Rectangle {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;

    constexpr Rectangle() = default;

    constexpr Rectangle(float x, float y, float width, float height)
        : x(x), y(y), width(width), height(height) {}

    constexpr Rectangle(const Vector2& pos, float width, float height)
        : x(pos.x), y(pos.y), width(width), height(height) {}

    constexpr Rectangle(const Vector2& pos, const Vector2& size)
        : x(pos.x), y(pos.y), width(size.x), height(size.y) {}

    // Getters
    [[nodiscard]] constexpr float left() const { return x; }
    [[nodiscard]] constexpr float right() const { return x + width; }
    [[nodiscard]] constexpr float top() const { return y; }
    [[nodiscard]] constexpr float bottom() const { return y + height; }

    [[nodiscard]] constexpr Vector2 position() const { return {x, y}; }
    [[nodiscard]] constexpr Vector2 size() const { return {width, height}; }
    [[nodiscard]] constexpr Vector2 center() const {
        return {x + width / 2.0f, y + height / 2.0f};
    }

    // Setters
    void setPosition(const Vector2& pos) {
        x = pos.x;
        y = pos.y;
    }

    void setCenter(const Vector2& center) {
        x = center.x - width / 2.0f;
        y = center.y - height / 2.0f;
    }

    // Collision detection
    [[nodiscard]] bool intersects(const Rectangle& other) const {
        return !(right() <= other.left() ||
                 left() >= other.right() ||
                 bottom() <= other.top() ||
                 top() >= other.bottom());
    }

    [[nodiscard]] bool contains(const Vector2& point) const {
        return point.x >= left() && point.x < right() &&
               point.y >= top() && point.y < bottom();
    }

    [[nodiscard]] bool contains(const Rectangle& other) const {
        return other.left() >= left() &&
               other.right() <= right() &&
               other.top() >= top() &&
               other.bottom() <= bottom();
    }

    // Get intersection rectangle
    [[nodiscard]] Rectangle intersection(const Rectangle& other) const {
        float l = std::max(left(), other.left());
        float t = std::max(top(), other.top());
        float r = std::min(right(), other.right());
        float b = std::min(bottom(), other.bottom());

        if (l < r && t < b) {
            return {l, t, r - l, b - t};
        }
        return {0, 0, 0, 0};
    }

    // Expand rectangle
    [[nodiscard]] Rectangle expanded(float amount) const {
        return {x - amount, y - amount, width + 2 * amount, height + 2 * amount};
    }

    // Move rectangle
    [[nodiscard]] Rectangle moved(const Vector2& offset) const {
        return {x + offset.x, y + offset.y, width, height};
    }

    // Comparison
    constexpr bool operator==(const Rectangle& other) const {
        return x == other.x && y == other.y &&
               width == other.width && height == other.height;
    }

    // Stream output
    friend std::ostream& operator<<(std::ostream& os, const Rectangle& r) {
        return os << "Rectangle(" << r.x << ", " << r.y << ", "
                  << r.width << ", " << r.height << ")";
    }
};

} // namespace tank
