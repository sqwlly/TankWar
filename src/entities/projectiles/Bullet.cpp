#include "entities/projectiles/Bullet.hpp"
#include "entities/tanks/ITank.hpp"
#include "graphics/SpriteSheet.hpp"

namespace tank {

Bullet::Bullet(const Vector2& position, Direction direction, ITank* owner, int level)
    : Entity(position, 9, 10)
    , owner_(owner)
    , direction_(direction)
    , level_(level)
{
    renderLayer_ = RenderLayer::Bullets;

    // Base stats
    speed_ = Constants::BULLET_DEFAULT_SPEED;
    attack_ = Constants::BULLET_DEFAULT_ATTACK;

    // Level upgrades
    if (level >= 1) {
        speed_ *= 2;
    }
    if (level == 2) {
        attack_ += 50;
    } else if (level >= 3) {
        attack_ += 100;
    }
}

void Bullet::move(Direction direction) {
    direction_ = direction;
    Vector2 delta = directionToVector(direction) * speed_;
    position_ += delta;
}

void Bullet::onUpdate(float deltaTime) {
    if (hitTarget_ || !active_) return;

    // Move in current direction
    move(direction_);

    // Check bounds
    if (isOutOfBounds()) {
        die();
    }
}

void Bullet::onRender(IRenderer& renderer) {
    if (!isAlive()) return;

    Rectangle bulletSrc = Sprites::Bullet::get(direction_);
    int srcX = static_cast<int>(bulletSrc.x);
    int srcY = static_cast<int>(bulletSrc.y);
    int srcSize = Sprites::BULLET_SIZE;

    int destX = static_cast<int>(position_.x);
    int destY = static_cast<int>(position_.y);
    int destW = static_cast<int>(width_);
    int destH = static_cast<int>(height_);

    renderer.drawSprite(srcX, srcY, srcSize, srcSize, destX, destY, destW, destH);
}

void Bullet::die() {
    hitTarget_ = true;
    active_ = false;

    // Notify owner that bullet is destroyed
    if (owner_) {
        owner_->onBulletDestroyed();
    }

    // Explosion effect will be added later
}

bool Bullet::isOutOfBounds() const {
    return position_.x < 0 ||
           position_.x >= Constants::GAME_WIDTH ||
           position_.y < 0 ||
           position_.y >= Constants::GAME_HEIGHT;
}

} // namespace tank
