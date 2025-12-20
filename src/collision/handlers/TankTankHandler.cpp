#include "collision/handlers/TankTankHandler.hpp"

namespace tank {

bool TankTankHandler::canHandle(IEntity& a, IEntity& b) const {
    ITank* tankA = dynamic_cast<ITank*>(&a);
    ITank* tankB = dynamic_cast<ITank*>(&b);
    return tankA != nullptr && tankB != nullptr;
}

bool TankTankHandler::handle(IEntity& a, IEntity& b) {
    Tank* tankA = dynamic_cast<Tank*>(&a);
    Tank* tankB = dynamic_cast<Tank*>(&b);

    if (!tankA || !tankB) return false;
    if (!tankA->isAlive() || !tankB->isAlive()) return false;

    separateTanks(*tankA, *tankB);
    return true;
}

void TankTankHandler::separateTanks(Tank& tankA, Tank& tankB) {
    // Both tanks stay at their previous positions
    tankA.stay();
    tankB.stay();
}

} // namespace tank
