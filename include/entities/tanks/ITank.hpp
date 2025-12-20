#pragma once

#include "entities/base/IEntity.hpp"
#include "entities/base/IMovable.hpp"
#include "entities/base/IRenderable.hpp"
#include "entities/base/IDamageable.hpp"

namespace tank {

/**
 * @brief Tank interface combining entity capabilities (ISP)
 * Tanks are movable, damageable, and renderable entities
 */
class ITank : public IEntity, public IMovable, public IRenderable, public IDamageable {
public:
    ~ITank() override = default;

    // Shooting
    virtual void shoot() = 0;
    virtual bool canShoot() const = 0;
    virtual void onBulletDestroyed() = 0;

    // Level/upgrade
    virtual int getLevel() const = 0;
    virtual void upgrade() = 0;

    // Spawn
    virtual void spawn(const Vector2& position) = 0;
    virtual bool isSpawning() const = 0;
};

} // namespace tank
