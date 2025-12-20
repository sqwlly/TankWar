#pragma once

#include "entities/base/Entity.hpp"
#include "entities/terrain/ITerrain.hpp"

namespace tank {

/**
 * @brief Base terrain class
 * Inherits from Entity for position/bounds and ITerrain for terrain behavior
 */
class Terrain : public Entity, public ITerrain {
public:
    Terrain(const Vector2& position, TerrainType type);
    ~Terrain() override = default;

    // ITerrain implementation - terrain properties
    TerrainType getTerrainType() const override { return type_; }
    bool isTankPassable() const override { return tankPassable_; }
    bool isBulletPassable() const override { return bulletPassable_; }
    bool isDestructible() const override { return destructible_; }
    void takeDamage(int damage, const Rectangle& hitBox) override {}
    bool isDestroyed() const override { return !active_; }

    // ITerrain implementation - forwarded from Entity
    void update(float deltaTime) override { Entity::update(deltaTime); }
    void render(IRenderer& renderer) override { Entity::render(renderer); }
    Rectangle getBounds() const override { return Entity::getBounds(); }
    RenderLayer getRenderLayer() const override { return renderLayer_; }
    bool isActive() const override { return active_; }

protected:
    TerrainType type_;
    bool tankPassable_ = false;
    bool bulletPassable_ = false;
    bool destructible_ = false;
};

} // namespace tank
