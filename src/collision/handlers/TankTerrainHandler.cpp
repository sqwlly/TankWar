#include "collision/handlers/TankTerrainHandler.hpp"

namespace tank {

bool TankTerrainHandler::canHandle(IEntity& a, IEntity& b) const {
    ITank* tank = dynamic_cast<ITank*>(&a);
    ITerrain* terrain = dynamic_cast<ITerrain*>(&b);
    return tank != nullptr && terrain != nullptr;
}

bool TankTerrainHandler::handle(IEntity& a, IEntity& b) {
    Tank* tank = dynamic_cast<Tank*>(&a);
    ITerrain* terrain = dynamic_cast<ITerrain*>(&b);

    if (!tank || !terrain) return false;

    // Tanks can pass through grass
    if (terrain->isTankPassable()) {
        return false;
    }

    // Try smooth movement
    if (!applySmoothMovement(*tank, b.getBounds())) {
        // Can't slide, stay in place
        tank->stay();
    }

    return true;
}

bool TankTerrainHandler::applySmoothMovement(Tank& tank, const Rectangle& obstacle) {
    Direction dir = tank.getDirection();
    Rectangle tankBounds = tank.getBounds();

    // Try to slide around the obstacle
    int slideAmount = static_cast<int>(tank.getBounds().width / 3);

    switch (dir) {
        case Direction::Up:
        case Direction::Down: {
            // Try sliding left/right
            for (int i = 1; i <= slideAmount; ++i) {
                // Try right
                Rectangle tryRight = tankBounds.moved(Vector2(i, 0));
                if (!tryRight.intersects(obstacle)) {
                    tank.setPosition(tank.getPosition() + Vector2(i, 0));
                    return true;
                }
                // Try left
                Rectangle tryLeft = tankBounds.moved(Vector2(-i, 0));
                if (!tryLeft.intersects(obstacle)) {
                    tank.setPosition(tank.getPosition() + Vector2(-i, 0));
                    return true;
                }
            }
            break;
        }

        case Direction::Left:
        case Direction::Right: {
            // Try sliding up/down
            for (int i = 1; i <= slideAmount; ++i) {
                // Try up
                Rectangle tryUp = tankBounds.moved(Vector2(0, -i));
                if (!tryUp.intersects(obstacle)) {
                    tank.setPosition(tank.getPosition() + Vector2(0, -i));
                    return true;
                }
                // Try down
                Rectangle tryDown = tankBounds.moved(Vector2(0, i));
                if (!tryDown.intersects(obstacle)) {
                    tank.setPosition(tank.getPosition() + Vector2(0, i));
                    return true;
                }
            }
            break;
        }
    }

    return false;
}

} // namespace tank
