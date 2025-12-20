#include "collision/CollisionManager.hpp"

namespace tank {

void CollisionManager::addHandler(std::unique_ptr<ICollisionHandler> handler) {
    handlers_.push_back(std::move(handler));
}

bool CollisionManager::checkAABB(const Rectangle& a, const Rectangle& b) {
    return a.intersects(b);
}

void CollisionManager::dispatchCollision(IEntity& a, IEntity& b) {
    for (auto& handler : handlers_) {
        if (handler->canHandle(a, b)) {
            if (handler->handle(a, b)) {
                return; // Collision handled
            }
        }
        // Try reverse order
        if (handler->canHandle(b, a)) {
            if (handler->handle(b, a)) {
                return;
            }
        }
    }
}

} // namespace tank
