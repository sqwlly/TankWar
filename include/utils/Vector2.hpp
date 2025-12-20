#pragma once

#include <cmath>
#include <iostream>

namespace tank {

/**
 * @brief 2D Vector class for positions and velocities
 * Immutable design with operator overloads
 */
struct Vector2 {
    float x = 0.0f;
    float y = 0.0f;

    constexpr Vector2() = default;
    constexpr Vector2(float x, float y) : x(x), y(y) {}

    // Arithmetic operators
    constexpr Vector2 operator+(const Vector2& other) const {
        return {x + other.x, y + other.y};
    }

    constexpr Vector2 operator-(const Vector2& other) const {
        return {x - other.x, y - other.y};
    }

    constexpr Vector2 operator*(float scalar) const {
        return {x * scalar, y * scalar};
    }

    constexpr Vector2 operator/(float scalar) const {
        return {x / scalar, y / scalar};
    }

    // Compound assignment operators
    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vector2& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    // Comparison operators
    constexpr bool operator==(const Vector2& other) const {
        return x == other.x && y == other.y;
    }

    constexpr bool operator!=(const Vector2& other) const {
        return !(*this == other);
    }

    // Utility functions
    [[nodiscard]] float length() const {
        return std::sqrt(x * x + y * y);
    }

    [[nodiscard]] float lengthSquared() const {
        return x * x + y * y;
    }

    [[nodiscard]] Vector2 normalized() const {
        float len = length();
        if (len > 0) {
            return {x / len, y / len};
        }
        return {0, 0};
    }

    [[nodiscard]] float dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }

    [[nodiscard]] float distance(const Vector2& other) const {
        return (*this - other).length();
    }

    // Static constants
    static constexpr Vector2 zero() { return {0.0f, 0.0f}; }
    static constexpr Vector2 one() { return {1.0f, 1.0f}; }
    static constexpr Vector2 up() { return {0.0f, -1.0f}; }
    static constexpr Vector2 down() { return {0.0f, 1.0f}; }
    static constexpr Vector2 left() { return {-1.0f, 0.0f}; }
    static constexpr Vector2 right() { return {1.0f, 0.0f}; }

    // Stream output
    friend std::ostream& operator<<(std::ostream& os, const Vector2& v) {
        return os << "Vector2(" << v.x << ", " << v.y << ")";
    }
};

// Non-member operator for scalar * Vector2
constexpr Vector2 operator*(float scalar, const Vector2& v) {
    return v * scalar;
}

} // namespace tank
