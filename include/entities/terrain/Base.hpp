#pragma once

#include "entities/base/Entity.hpp"
#include "entities/base/IDamageable.hpp"
#include <algorithm>

namespace tank {

class IRenderer;

/**
 * @brief Player base (eagle) - must be protected
 * Game ends if base is destroyed
 */
class Base : public Entity, public IDamageable {
public:
    Base(int x, int y);
    ~Base() override = default;

    void update(float deltaTime) override;
    void render(IRenderer& renderer) override;

    // IDamageable implementation
    void takeDamage(int damage) override;
    void takeDamage(int damage, const Rectangle& hitBox);
    int getHealth() const override { return health_; }
    int getMaxHealth() const override { return maxHealth_; }
    int getDefense() const override { return defense_; }
    void setDefense(int defense) override { defense_ = defense; }
    void heal(int amount) override { health_ = std::min(health_ + amount, maxHealth_); }
    bool isAlive() const override { return active_; }
    void die() override;

    // Base-specific
    bool isDestroyed() const { return destroyed_; }
    void reset();

    RenderLayer getRenderLayer() const override { return RenderLayer::Base; }

private:
    int health_;
    int maxHealth_;
    int defense_;
    bool destroyed_;

    // Visual state
    bool showDestroyedSprite_;
};

} // namespace tank
