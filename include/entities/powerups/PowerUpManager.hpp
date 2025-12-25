#pragma once

#include "entities/powerups/PowerUp.hpp"
#include <memory>
#include <vector>

namespace tank {

class IRenderer;
class PlayerTank;

/**
 * @brief Manages power-up lifecycle and applies collected effects (minimal: Star->upgrade)
 */
class PowerUpManager {
public:
    PowerUpManager() = default;
    ~PowerUpManager() = default;

    void clear();
    void update(float deltaTime);
    void render(IRenderer& renderer);

    void spawn(const Vector2& position, PowerUpType type);

    // Returns true if a power-up was collected and applied this call.
    bool tryCollect(PlayerTank& player);

    size_t getCount() const { return powerUps_.size(); }

private:
    std::vector<std::unique_ptr<PowerUp>> powerUps_;

    void removeInactive();
    bool applyToPlayer(PlayerTank& player, PowerUpType type);
};

} // namespace tank

