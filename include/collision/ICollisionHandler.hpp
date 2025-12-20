#pragma once

#include "entities/base/IEntity.hpp"

namespace tank {

/**
 * @brief Collision handler interface (Strategy Pattern + Chain of Responsibility)
 * Each handler deals with specific collision types
 */
class ICollisionHandler {
public:
    virtual ~ICollisionHandler() = default;

    // Check if this handler can process the collision
    virtual bool canHandle(IEntity& a, IEntity& b) const = 0;

    // Process the collision, return true if collision was handled
    virtual bool handle(IEntity& a, IEntity& b) = 0;
};

} // namespace tank
