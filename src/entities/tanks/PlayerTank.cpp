#include "entities/tanks/PlayerTank.hpp"
#include "core/ServiceLocator.hpp"

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

    // Base rendering is handled by Tank::render()
    // Here we could add player-specific effects
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
