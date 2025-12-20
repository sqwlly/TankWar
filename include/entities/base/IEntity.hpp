#pragma once

#include "utils/Vector2.hpp"
#include "utils/Rectangle.hpp"

namespace tank {

/**
 * @brief Entity interface - base for all game objects (ISP)
 */
class IEntity {
public:
    virtual ~IEntity() = default;

    // Identity
    virtual int getId() const = 0;

    // Position
    virtual Vector2 getPosition() const = 0;
    virtual void setPosition(const Vector2& pos) = 0;

    // Bounds for collision
    virtual Rectangle getBounds() const = 0;

    // Active state
    virtual bool isActive() const = 0;
    virtual void setActive(bool active) = 0;

    // Update
    virtual void update(float deltaTime) = 0;
};

} // namespace tank
