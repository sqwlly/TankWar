#pragma once

#include "collision/ICollisionHandler.hpp"
#include "entities/projectiles/Bullet.hpp"
#include "entities/terrain/ITerrain.hpp"
#include "entities/terrain/BrickWall.hpp"
#include "entities/terrain/SteelWall.hpp"
#include "entities/terrain/Water.hpp"
#include "entities/terrain/Base.hpp"

namespace tank {

/**
 * @brief Handles bullet-terrain collisions
 */
class BulletTerrainHandler : public ICollisionHandler {
public:
    bool canHandle(IEntity& a, IEntity& b) const override;
    bool handle(IEntity& a, IEntity& b) override;

private:
    void handleBulletBrick(Bullet& bullet, BrickWall& brick);
    void handleBulletSteel(Bullet& bullet, SteelWall& steel);
    void handleBulletBase(Bullet& bullet, Base& base);
};

} // namespace tank
