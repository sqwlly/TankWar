#include "entities/projectiles/Bullet.hpp"
#include "entities/tanks/ITank.hpp"
#include "graphics/SpriteSheet.hpp"

namespace tank {

Bullet::Bullet(const Vector2& position, Direction direction, ITank* owner, int level)
    : Entity(position, static_cast<float>(Sprites::Bullet::SIZE), static_cast<float>(Sprites::Bullet::SIZE))
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

    // Classic bullet: a white rectangle elongated along the travel direction.
    // Drawn procedurally - the sprite sheet has no directional bullet sprites.
    int x = static_cast<int>(position_.x);
    int y = static_cast<int>(position_.y);
    int w = static_cast<int>(width_);
    int h = static_cast<int>(height_);

    int bodyW = w;
    int bodyH = h;
    if (direction_ == Direction::Up || direction_ == Direction::Down) {
        bodyW = w / 2;  // Vertical: narrow and tall
    } else {
        bodyH = h / 2;  // Horizontal: wide and short
    }

    renderer.drawRect(x + (w - bodyW) / 2, y + (h - bodyH) / 2,
                      bodyW, bodyH, 255, 255, 255, 255);
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

void Bullet::clearOwner() {
    owner_ = nullptr;
}

bool Bullet::isOutOfBounds() const {
    return position_.x < 0 ||
           position_.x >= Constants::GAME_WIDTH ||
           position_.y < 0 ||
           position_.y >= Constants::GAME_HEIGHT;
}

} // namespace tank
