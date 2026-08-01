#pragma once

#include "entities/powerups/PowerUp.hpp"
#include <memory>
#include <optional>
#include <vector>

namespace tank {

class IRenderer;
class PlayerTank;

/**
 * @brief Manages power-up lifecycle and reports collection events.
 */
class PowerUpManager {
public:
    PowerUpManager() = default;
    ~PowerUpManager() = default;

    void clear();
    void update(float deltaTime);
    void render(IRenderer& renderer);

    void spawn(const Vector2& position, PowerUpType type);

    // Returns the collected type, if any. Gameplay effects are deliberately
    // dispatched by PlayingState because several effects target the whole map.
    std::optional<PowerUpType> tryCollect(PlayerTank& player);

    size_t getCount() const { return powerUps_.size(); }

private:
    std::vector<std::unique_ptr<PowerUp>> powerUps_;

    void removeInactive();
};

} // namespace tank
