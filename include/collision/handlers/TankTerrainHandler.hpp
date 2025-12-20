#pragma once

#include "collision/ICollisionHandler.hpp"
#include "entities/tanks/ITank.hpp"
#include "entities/tanks/Tank.hpp"
#include "entities/terrain/ITerrain.hpp"

namespace tank {

/**
 * @brief Handles tank-terrain collisions with smooth movement
 */
class TankTerrainHandler : public ICollisionHandler {
public:
    bool canHandle(IEntity& a, IEntity& b) const override;
    bool handle(IEntity& a, IEntity& b) override;

private:
    // Smooth movement: try to slide around obstacles
    bool applySmoothMovement(Tank& tank, const Rectangle& obstacle);
};

} // namespace tank
