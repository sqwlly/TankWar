#pragma once

#include "entities/base/Entity.hpp"
#include "graphics/Animation.hpp"
#include "utils/Constants.hpp"

namespace tank {

/**
 * @brief Power-up collectible item
 * Spawned when destroying enemy tanks with special marking
 */
class PowerUp : public Entity {
public:
    PowerUp(int x, int y, PowerUpType type);
    ~PowerUp() override = default;

    void update(float deltaTime) override;
    void render(IRenderer& renderer) override;

    PowerUpType getType() const { return type_; }
    bool isExpired() const { return expired_; }

    // Collect the power-up
    void collect();

    RenderLayer getRenderLayer() const { return RenderLayer::PowerUps; }

private:
    PowerUpType type_;
    Animation animation_;
    Animation blinkAnimation_;

    float lifetime_;           // Total lifetime
    float elapsedTime_;        // Time since spawn
    bool blinking_;            // Near expiration
    bool expired_;

    static constexpr float LIFETIME = 10.0f;        // 10 seconds
    static constexpr float BLINK_START = 7.0f;      // Start blinking at 7s

    void initAnimation();
    Constants::Color getColorForType() const;
};

} // namespace tank
