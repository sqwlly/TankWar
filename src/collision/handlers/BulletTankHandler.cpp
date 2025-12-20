#include "collision/handlers/BulletTankHandler.hpp"

namespace tank {

bool BulletTankHandler::canHandle(IEntity& a, IEntity& b) const {
    Bullet* bullet = dynamic_cast<Bullet*>(&a);
    ITank* tank = dynamic_cast<ITank*>(&b);
    return bullet != nullptr && tank != nullptr;
}

bool BulletTankHandler::handle(IEntity& a, IEntity& b) {
    Bullet* bullet = dynamic_cast<Bullet*>(&a);
    ITank* tank = dynamic_cast<ITank*>(&b);

    if (!bullet || !tank) return false;
    if (!bullet->isAlive() || !tank->isAlive()) return false;

    // Don't hit owner
    if (bullet->getOwner() == tank) return false;

    // Check friendly fire (player bullets shouldn't hit players, enemy bullets shouldn't hit enemies)
    bool bulletFromPlayer = dynamic_cast<PlayerTank*>(bullet->getOwner()) != nullptr;
    bool targetIsPlayer = dynamic_cast<PlayerTank*>(tank) != nullptr;

    if (bulletFromPlayer == targetIsPlayer) {
        return false; // Friendly fire disabled
    }

    // Check invincibility for players
    if (auto* player = dynamic_cast<PlayerTank*>(tank)) {
        if (player->isInvincible()) {
            bullet->die();
            return true;
        }
    }

    applyDamage(*bullet, *tank);
    return true;
}

void BulletTankHandler::applyDamage(Bullet& bullet, ITank& tank) {
    // Calculate damage with defense reduction
    float defenseMultiplier = 1.0f - (static_cast<float>(tank.getDefense()) /
                                       static_cast<float>(tank.getMaxHealth()));
    int damage = static_cast<int>(bullet.getAttack() * defenseMultiplier);

    tank.takeDamage(damage);
    bullet.die();
}

} // namespace tank
