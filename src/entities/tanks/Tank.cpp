#include "entities/tanks/Tank.hpp"
#include "graphics/SpriteSheet.hpp"
#include <algorithm>

namespace tank {

std::atomic<int> Tank::nextId_{0};

Tank::Tank(const Vector2& position)
    : id_(nextId_++)
    , position_(position)
    , previousPosition_(position)
{
}

Rectangle Tank::getBounds() const {
    return Rectangle(position_.x, position_.y, width_, height_);
}

void Tank::update(float deltaTime) {
    if (!active_) return;

    // Handle spawn animation
    if (spawning_) {
        spawnTimer_ -= deltaTime;
        if (spawnTimer_ <= 0) {
            spawning_ = false;
        }
        return;
    }

    // Update animation frame when moving
    animationTimer_ += deltaTime;
    if (animationTimer_ >= ANIMATION_SPEED) {
        animationTimer_ = 0.0f;
        animationFrame_ = (animationFrame_ + 1) % 2;
    }

    onUpdate(deltaTime);
}

void Tank::render(IRenderer& renderer) {
    if (!active_) return;

    // During spawning, render spawn effect
    if (spawning_) {
        int spawnFrame = static_cast<int>((Constants::SPAWN_ANIMATION_FRAMES -
            spawnTimer_ / (Constants::SPAWN_ANIMATION_DELAY / 1000.0f))) % 4;
        Rectangle srcRect = Sprites::Spawn::get(spawnFrame);
        int x = static_cast<int>(position_.x);
        int y = static_cast<int>(position_.y);
        renderer.drawSprite(
            static_cast<int>(srcRect.x), static_cast<int>(srcRect.y),
            static_cast<int>(srcRect.width), static_cast<int>(srcRect.height),
            x, y, static_cast<int>(width_), static_cast<int>(height_)
        );
        return;
    }

    // Let subclass render the actual tank sprite
    onRender(renderer);
}

void Tank::move(Direction direction) {
    previousPosition_ = position_;
    direction_ = direction;

    Vector2 delta = directionToVector(direction) * speed_;

    // Calculate new position with boundary checks
    float newX = position_.x + delta.x;
    float newY = position_.y + delta.y;

    // Clamp to game boundaries
    newX = std::clamp(newX, 0.0f, static_cast<float>(Constants::GAME_WIDTH - width_));
    newY = std::clamp(newY, 0.0f, static_cast<float>(Constants::GAME_HEIGHT - height_));

    position_.x = newX;
    position_.y = newY;
}

void Tank::stay() {
    position_ = previousPosition_;
}

void Tank::takeDamage(int amount) {
    // Apply defense reduction
    float defenseMultiplier = 1.0f - (static_cast<float>(defense_) / static_cast<float>(maxHealth_));
    int actualDamage = static_cast<int>(amount * defenseMultiplier);

    health_ = std::max(0, health_ - actualDamage);

    if (health_ <= 0) {
        die();
    }
}

void Tank::heal(int amount) {
    health_ = std::min(maxHealth_, health_ + amount);
}

void Tank::die() {
    health_ = 0;
    active_ = false;
    onDie();
}

void Tank::shoot() {
    if (!canShoot()) return;

    bulletCount_--;
    onShoot();
}

void Tank::onBulletDestroyed() {
    if (bulletCount_ < maxBullets_) {
        bulletCount_++;
    }
}

void Tank::upgrade() {
    if (level_ < 3) {
        level_++;
        onUpgrade();

        // Level 3 (ultimate): extra bullet, more HP
        if (level_ == 3) {
            maxBullets_ = 2;
            defense_ = 20;
            health_ = maxHealth_;
        }
    }
}

void Tank::spawn(const Vector2& position) {
    position_ = position;
    previousPosition_ = position;
    health_ = Constants::PLAYER_DEFAULT_HP;
    direction_ = Direction::Up;
    spawning_ = true;
    spawnTimer_ = Constants::SPAWN_ANIMATION_FRAMES * Constants::SPAWN_ANIMATION_DELAY / 1000.0f;
    active_ = true;
    bulletCount_ = 1;
    maxBullets_ = 1;
    defense_ = 0;

    onSpawn();
}

} // namespace tank
