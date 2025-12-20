#include "collision/handlers/BulletBulletHandler.hpp"

namespace tank {

bool BulletBulletHandler::canHandle(IEntity& a, IEntity& b) const {
    Bullet* bulletA = dynamic_cast<Bullet*>(&a);
    Bullet* bulletB = dynamic_cast<Bullet*>(&b);
    return bulletA != nullptr && bulletB != nullptr;
}

bool BulletBulletHandler::handle(IEntity& a, IEntity& b) {
    Bullet* bulletA = dynamic_cast<Bullet*>(&a);
    Bullet* bulletB = dynamic_cast<Bullet*>(&b);

    if (!bulletA || !bulletB) return false;
    if (!bulletA->isAlive() || !bulletB->isAlive()) return false;

    // Both bullets destroy each other
    bulletA->hit();
    bulletA->die();
    bulletB->hit();
    bulletB->die();

    return true;
}

} // namespace tank
