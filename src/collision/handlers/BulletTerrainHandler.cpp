#include "collision/handlers/BulletTerrainHandler.hpp"

namespace tank {

bool BulletTerrainHandler::canHandle(IEntity& a, IEntity& b) const {
    Bullet* bullet = dynamic_cast<Bullet*>(&a);
    ITerrain* terrain = dynamic_cast<ITerrain*>(&b);
    return bullet != nullptr && terrain != nullptr;
}

bool BulletTerrainHandler::handle(IEntity& a, IEntity& b) {
    Bullet* bullet = dynamic_cast<Bullet*>(&a);
    ITerrain* terrain = dynamic_cast<ITerrain*>(&b);

    if (!bullet || !terrain) return false;
    if (!bullet->isAlive()) return false;

    // Bullets pass through water and grass
    if (terrain->isBulletPassable()) {
        return false;
    }

    // Handle different terrain types
    if (auto* brick = dynamic_cast<BrickWall*>(&b)) {
        handleBulletBrick(*bullet, *brick);
        return true;
    }

    if (auto* steel = dynamic_cast<SteelWall*>(&b)) {
        handleBulletSteel(*bullet, *steel);
        return true;
    }

    if (auto* base = dynamic_cast<Base*>(&b)) {
        handleBulletBase(*bullet, *base);
        return true;
    }

    // Generic terrain hit
    bullet->hit();
    bullet->die();
    return true;
}

void BulletTerrainHandler::handleBulletBrick(Bullet& bullet, BrickWall& brick) {
    brick.takeDamage(bullet.getAttack(), bullet.getBounds());
    bullet.hit();
    bullet.die();
}

void BulletTerrainHandler::handleBulletSteel(Bullet& bullet, SteelWall& steel) {
    // Only level 3 bullets can destroy steel
    if (bullet.getLevel() >= 3) {
        steel.setDestructible(true);
        steel.takeDamage(bullet.getAttack(), bullet.getBounds());
    }
    bullet.hit();
    bullet.die();
    // Play steel hit sound
}

void BulletTerrainHandler::handleBulletBase(Bullet& bullet, Base& base) {
    base.takeDamage(bullet.getAttack(), bullet.getBounds());
    bullet.hit();
    bullet.die();
}

} // namespace tank
