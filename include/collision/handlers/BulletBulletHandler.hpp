#pragma once

#include "collision/ICollisionHandler.hpp"
#include "entities/projectiles/Bullet.hpp"

namespace tank {

/**
 * @brief Handles bullet-bullet collisions
 * Both bullets are destroyed on collision
 */
class BulletBulletHandler : public ICollisionHandler {
public:
    bool canHandle(IEntity& a, IEntity& b) const override;
    bool handle(IEntity& a, IEntity& b) override;
};

} // namespace tank
