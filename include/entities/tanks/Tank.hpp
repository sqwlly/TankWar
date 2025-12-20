#pragma once

#include "entities/tanks/ITank.hpp"
#include "utils/Vector2.hpp"
#include "utils/Rectangle.hpp"
#include "utils/Constants.hpp"
#include <atomic>

namespace tank {

// Forward declaration
class PlayingState;

/**
 * @brief Base tank class with common functionality
 * Subclasses: PlayerTank, EnemyTank
 */
class Tank : public ITank {
public:
    Tank(const Vector2& position);
    ~Tank() override = default;

    // IEntity implementation
    int getId() const override { return id_; }
    Vector2 getPosition() const override { return position_; }
    void setPosition(const Vector2& pos) override { position_ = pos; }
    Rectangle getBounds() const override;
    bool isActive() const override { return active_; }
    void setActive(bool active) override { active_ = active; }
    void update(float deltaTime) override;

    // IMovable implementation
    void move(Direction direction) override;
    float getSpeed() const override { return speed_; }
    void setSpeed(float speed) override { speed_ = speed; }
    Direction getDirection() const override { return direction_; }
    void setDirection(Direction dir) override { direction_ = dir; }

    // IRenderable implementation
    void render(IRenderer& renderer) override;
    RenderLayer getRenderLayer() const override { return RenderLayer::Tanks; }

    // IDamageable implementation
    int getHealth() const override { return health_; }
    int getMaxHealth() const override { return maxHealth_; }
    int getDefense() const override { return defense_; }
    void setDefense(int defense) override { defense_ = defense; }
    void takeDamage(int amount) override;
    void heal(int amount) override;
    bool isAlive() const override { return health_ > 0; }
    void die() override;

    // ITank implementation
    void shoot() override;
    bool canShoot() const override { return bulletCount_ > 0; }
    void onBulletDestroyed() override;
    int getLevel() const override { return level_; }
    void upgrade() override;
    void spawn(const Vector2& position) override;
    bool isSpawning() const override { return spawning_; }

    // Stay in place (collision response)
    void stay();

protected:
    // Subclass hooks
    virtual void onUpdate(float deltaTime) {}
    virtual void onRender(IRenderer& renderer) {}
    virtual void onShoot() {}
    virtual void onDie() {}
    virtual void onUpgrade() {}
    virtual void onSpawn() {}

    // ID
    int id_;
    static std::atomic<int> nextId_;

    // Position and movement
    Vector2 position_;
    Vector2 previousPosition_;
    float speed_ = Constants::PLAYER_DEFAULT_SPEED;
    Direction direction_ = Direction::Up;

    // Dimensions
    float width_ = Constants::ELEMENT_SIZE - 2;
    float height_ = Constants::ELEMENT_SIZE - 2;

    // State
    bool active_ = true;
    bool spawning_ = false;
    float spawnTimer_ = 0.0f;

    // Combat
    int health_ = Constants::PLAYER_DEFAULT_HP;
    int maxHealth_ = Constants::PLAYER_MAX_HP;
    int defense_ = Constants::PLAYER_DEFAULT_DEFENSE;

    // Shooting
    int bulletCount_ = 1;
    int maxBullets_ = 1;
    int level_ = 0;

    // Animation
    int animationFrame_ = 0;
    float animationTimer_ = 0.0f;
    static constexpr float ANIMATION_SPEED = 0.1f;  // seconds per frame
};

} // namespace tank
