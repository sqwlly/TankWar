#pragma once

#include "utils/Constants.hpp"
#include "utils/Rectangle.hpp"

namespace tank {

// Forward declaration
class IRenderer;

/**
 * @brief Terrain interface (ISP)
 * Extended to include update/render capabilities for polymorphic handling
 */
class ITerrain {
public:
    virtual ~ITerrain() = default;

    // Terrain properties
    virtual TerrainType getTerrainType() const = 0;
    virtual bool isTankPassable() const = 0;
    virtual bool isBulletPassable() const = 0;
    virtual bool isDestructible() const = 0;

    // Damage handling (for destructible terrain)
    virtual void takeDamage(int damage, const Rectangle& hitBox) = 0;
    virtual bool isDestroyed() const = 0;

    // Update and render (allows polymorphic handling without dynamic_cast)
    virtual void update(float deltaTime) = 0;
    virtual void render(IRenderer& renderer) = 0;

    // Bounds and layer for collision and rendering
    virtual Rectangle getBounds() const = 0;
    virtual RenderLayer getRenderLayer() const = 0;
    virtual bool isActive() const = 0;
};

} // namespace tank
