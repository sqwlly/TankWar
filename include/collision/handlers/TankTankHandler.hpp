#pragma once

#include "collision/ICollisionHandler.hpp"
#include "entities/tanks/ITank.hpp"
#include "entities/tanks/Tank.hpp"

namespace tank {

/**
 * @brief Handles tank-tank collisions
 * Both tanks stop at collision point
 */
class TankTankHandler : public ICollisionHandler {
public:
    bool canHandle(IEntity& a, IEntity& b) const override;
    bool handle(IEntity& a, IEntity& b) override;

private:
    void separateTanks(Tank& tankA, Tank& tankB);
};

} // namespace tank
