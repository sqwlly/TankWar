#include "entities/tanks/PlayerTank.hpp"
#include "core/ServiceLocator.hpp"
#include "graphics/SpriteSheet.hpp"

namespace tank {

PlayerTank::PlayerTank(int playerId, const Vector2& spawnPosition)
    : Tank(spawnPosition)
    , playerId_(playerId)
    , spawnPosition_(spawnPosition)
{
    // Initial spawn with invincibility
    spawn(spawnPosition);
    makeInvincible(Constants::INVINCIBLE_DURATION / 1000.0f);
}

void PlayerTank::handleInput(const InputManager::PlayerInput& input) {
    if (spawning_) return;

    // Movement
    if (input.up) {
        move(Direction::Up);
    } else if (input.down) {
        move(Direction::Down);
    } else if (input.left) {
        move(Direction::Left);
    } else if (input.right) {
        move(Direction::Right);
    }

    // Shooting
    wantsToShoot_ = input.fire;
}

void PlayerTank::onUpdate(float deltaTime) {
    // Handle invincibility timer
    if (invincibleTimer_ > 0) {
        invincibleTimer_ -= deltaTime;

        // Blink effect
        invincibleBlinkTimer_ += deltaTime;
        if (invincibleBlinkTimer_ >= 0.1f) {
            invincibleBlinkTimer_ = 0;
            invincibleVisible_ = !invincibleVisible_;
        }
    } else {
        invincibleVisible_ = true;
    }

    // Handle shooting
    if (wantsToShoot_ && canShoot()) {
        shoot();
        wantsToShoot_ = false;
    }
}

void PlayerTank::onRender(IRenderer& renderer) {
    // Skip rendering during invincibility blink
    if (isInvincible() && !invincibleVisible_) {
        return;
    }

    // Calculate sprite position based on direction and animation frame
    int dirCol = 0;
    switch (direction_) {
        case Direction::Up:    dirCol = 0; break;
        case Direction::Left:  dirCol = 2; break;
        case Direction::Down:  dirCol = 4; break;
        case Direction::Right: dirCol = 6; break;
    }

    // Player 1: yellow (row 0), Player 2: green (row 8)
    int baseRow = (playerId_ == 1) ? 0 : 8;
    // Add level offset (each level adds 2 columns per direction set)
    int levelOffset = level_ * 8;

    int srcX = (dirCol + animationFrame_) * Sprites::TANK_SIZE + levelOffset;
    int srcY = baseRow * Sprites::TANK_SIZE;

    int destX = static_cast<int>(position_.x);
    int destY = static_cast<int>(position_.y);
    int destSize = static_cast<int>(width_);

    renderer.drawSprite(srcX, srcY, Sprites::TANK_SIZE, Sprites::TANK_SIZE,
                       destX, destY, destSize, destSize);

    // Render shield effect if invincible
    if (isInvincible()) {
        Rectangle shieldRect = Sprites::Shield::get(animationFrame_);
        renderer.drawSprite(
            static_cast<int>(shieldRect.x), static_cast<int>(shieldRect.y),
            static_cast<int>(shieldRect.width), static_cast<int>(shieldRect.height),
            destX, destY, destSize, destSize
        );
    }
}

void PlayerTank::onShoot() {
    // Play shooting sound
    // ServiceLocator::getAudio().playSound(SoundId::BulletShot);

    // Bullet creation will be handled by the game state
}

void PlayerTank::onDie() {
    // Play death sound and effects
    // Death handling will be done by the game state
}

void PlayerTank::onSpawn() {
    // Make invincible after spawn
    makeInvincible(Constants::INVINCIBLE_DURATION / 1000.0f);
}

void PlayerTank::onUpgrade() {
    // Play upgrade sound
    // ServiceLocator::getAudio().playSound(SoundId::Star);
}

void PlayerTank::makeInvincible(float duration) {
    invincibleTimer_ = duration;
    invincibleBlinkTimer_ = 0;
    invincibleVisible_ = true;
}

void PlayerTank::respawn() {
    resetLevel();
    spawn(spawnPosition_);
}

} // namespace tank
