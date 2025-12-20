#pragma once

#include "entities/base/Entity.hpp"
#include "entities/base/IMovable.hpp"
#include "utils/Constants.hpp"

namespace tank {

class ITank;

/**
 * @brief Bullet projectile entity
 */
class Bullet : public Entity, public IMovable {
public:
    Bullet(const Vector2& position, Direction direction, ITank* owner, int level = 0);
    ~Bullet() override = default;

    // IMovable implementation
    void move(Direction direction) override;
    float getSpeed() const override { return speed_; }
    void setSpeed(float speed) override { speed_ = speed; }
    Direction getDirection() const override { return direction_; }
    void setDirection(Direction dir) override { direction_ = dir; }

    // Bullet specific
    ITank* getOwner() const { return owner_; }
    int getAttack() const { return attack_; }
    int getLevel() const { return level_; }

    void hit() { hitTarget_ = true; }
    bool hasHitTarget() const { return hitTarget_; }

    void die();
    bool isAlive() const { return !hitTarget_ && active_; }

    // Check if bullet is out of bounds
    bool isOutOfBounds() const;

    RenderLayer getRenderLayer() const override { return RenderLayer::Bullets; }

protected:
    void onUpdate(float deltaTime) override;
    void onRender(IRenderer& renderer) override;

private:
    ITank* owner_;
    Direction direction_;
    float speed_;
    int attack_;
    int level_;
    bool hitTarget_ = false;
};

} // namespace tank
