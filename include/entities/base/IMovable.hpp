#pragma once

#include "utils/Constants.hpp"

namespace tank {

/**
 * @brief Movable interface - for entities that can move (ISP)
 */
class IMovable {
public:
    virtual ~IMovable() = default;

    virtual void move(Direction direction) = 0;
    virtual float getSpeed() const = 0;
    virtual void setSpeed(float speed) = 0;
    virtual Direction getDirection() const = 0;
    virtual void setDirection(Direction dir) = 0;
};

} // namespace tank
