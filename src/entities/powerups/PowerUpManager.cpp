#include "entities/powerups/PowerUpManager.hpp"
#include "entities/tanks/PlayerTank.hpp"
#include <algorithm>

namespace tank {

void PowerUpManager::clear() {
    powerUps_.clear();
}

void PowerUpManager::update(float deltaTime) {
    for (auto& powerUp : powerUps_) {
        if (powerUp->isActive()) {
            powerUp->update(deltaTime);
        }
    }

    removeInactive();
}

void PowerUpManager::render(IRenderer& renderer) {
    for (auto& powerUp : powerUps_) {
        if (powerUp->isActive()) {
            powerUp->render(renderer);
        }
    }
}

void PowerUpManager::spawn(const Vector2& position, PowerUpType type) {
    powerUps_.push_back(std::make_unique<PowerUp>(
        static_cast<int>(position.x), static_cast<int>(position.y), type));
}

std::optional<PowerUpType> PowerUpManager::tryCollect(PlayerTank& player) {
    if (!player.isAlive() || player.isSpawning()) {
        return std::nullopt;
    }

    std::optional<PowerUpType> collected;
    const Rectangle playerBounds = player.getBounds();
    for (auto& powerUp : powerUps_) {
        if (!powerUp->isActive() || powerUp->isExpired()) {
            continue;
        }

        if (!playerBounds.intersects(powerUp->getBounds())) {
            continue;
        }

        collected = powerUp->getType();
        powerUp->collect();
        break;
    }

    // Container mutation stays outside the iteration above.
    removeInactive();
    return collected;
}

void PowerUpManager::removeInactive() {
    powerUps_.erase(
        std::remove_if(powerUps_.begin(), powerUps_.end(),
            [](const std::unique_ptr<PowerUp>& p) { return !p->isActive(); }),
        powerUps_.end()
    );
}

} // namespace tank
