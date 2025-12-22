#include "entities/tanks/PlayerTank.hpp"
#include "core/ServiceLocator.hpp"
#include "graphics/SpriteSheet.hpp"
#include <iostream>

namespace tank {

PlayerTank::PlayerTank(int playerId, const Vector2& spawnPosition)
    : Tank(spawnPosition)
    , playerId_(playerId)
    , spawnPosition_(spawnPosition)
{
    // Initial spawn with invincibility
    spawn(spawnPosition);
    makeInvincible(Constants::INVINCIBLE_DURATION / 1000.0f);
    std::cout << "[PlayerTank] Created at (" << spawnPosition.x << ", " << spawnPosition.y
              << "), spawning=" << spawning_ << ", spawnTimer=" << spawnTimer_ << std::endl;
}

void PlayerTank::handleInput(const InputManager::PlayerInput& input) {
    if (spawning_) {
        std::cout << "[PlayerTank] Still spawning, ignoring input" << std::endl;
        return;
    }

    // Debug: print when any movement key is detected
    bool anyMovement = input.up || input.down || input.left || input.right;
    if (anyMovement) {
        std::cout << "[PlayerTank] Movement input detected: "
                  << "U=" << input.up << " D=" << input.down
                  << " L=" << input.left << " R=" << input.right
                  << " Pos before=(" << position_.x << "," << position_.y << ")" << std::endl;
    }

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

    if (anyMovement) {
        std::cout << "[PlayerTank] Pos after=(" << position_.x << "," << position_.y << ")" << std::endl;
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
        case Direction::Up:    dirCol = Sprites::Tank::DIR_UP_COL; break;
        case Direction::Down:  dirCol = Sprites::Tank::DIR_DOWN_COL; break;
        case Direction::Left:  dirCol = Sprites::Tank::DIR_LEFT_COL; break;
        case Direction::Right: dirCol = Sprites::Tank::DIR_RIGHT_COL; break;
    }

    // Player 1: row 0, Player 2: row 8
    int baseY = (playerId_ == 1) ? Sprites::Tank::P1_BASE_Y : Sprites::Tank::P2_BASE_Y;

    // Get sprite frame using the Tank::getFrame helper
    Rectangle srcRect = Sprites::Tank::getFrame(baseY, dirCol, animationFrame_, level_);

    int destX = static_cast<int>(position_.x);
    int destY = static_cast<int>(position_.y);
    int destSize = static_cast<int>(width_);

    renderer.drawSprite(
        static_cast<int>(srcRect.x), static_cast<int>(srcRect.y),
        static_cast<int>(srcRect.width), static_cast<int>(srcRect.height),
        destX, destY, destSize, destSize
    );

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
