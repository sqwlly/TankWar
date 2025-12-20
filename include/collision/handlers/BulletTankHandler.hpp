#pragma once

#include "collision/ICollisionHandler.hpp"
#include "entities/projectiles/Bullet.hpp"
#include "entities/tanks/ITank.hpp"
#include "entities/tanks/PlayerTank.hpp"
#include "entities/tanks/EnemyTank.hpp"

namespace tank {

/**
 * @brief Handles bullet-tank collisions
 */
class BulletTankHandler : public ICollisionHandler {
public:
    bool canHandle(IEntity& a, IEntity& b) const override;
    bool handle(IEntity& a, IEntity& b) override;

private:
    void applyDamage(Bullet& bullet, ITank& tank);
};

} // namespace tank
