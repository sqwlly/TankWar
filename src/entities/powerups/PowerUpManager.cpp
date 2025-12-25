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

bool PowerUpManager::tryCollect(PlayerTank& player) {
    if (!player.isAlive() || player.isSpawning()) {
        return false;
    }

    const Rectangle playerBounds = player.getBounds();
    for (auto& powerUp : powerUps_) {
        if (!powerUp->isActive() || powerUp->isExpired()) {
            continue;
        }

        if (!playerBounds.intersects(powerUp->getBounds())) {
            continue;
        }

        const PowerUpType type = powerUp->getType();
        powerUp->collect();
        applyToPlayer(player, type);
        removeInactive();
        return true;
    }

    removeInactive();
    return false;
}

void PowerUpManager::removeInactive() {
    powerUps_.erase(
        std::remove_if(powerUps_.begin(), powerUps_.end(),
            [](const std::unique_ptr<PowerUp>& p) { return !p->isActive(); }),
        powerUps_.end()
    );
}

bool PowerUpManager::applyToPlayer(PlayerTank& player, PowerUpType type) {
    switch (type) {
        case PowerUpType::Star:
            player.upgrade();
            return true;
        default:
            return false;
    }
}

} // namespace tank

