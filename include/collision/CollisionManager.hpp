#pragma once

#include "collision/ICollisionHandler.hpp"
#include "entities/base/IEntity.hpp"
#include <vector>
#include <memory>
#include <functional>

namespace tank {

/**
 * @brief Manages collision detection and dispatches to handlers
 */
class CollisionManager {
public:
    CollisionManager() = default;
    ~CollisionManager() = default;

    // Add a collision handler
    void addHandler(std::unique_ptr<ICollisionHandler> handler);

    // Check collisions between two entity lists
    template<typename ListA, typename ListB>
    void checkCollisions(ListA& listA, ListB& listB);

    // Check collisions within a single list
    template<typename List>
    void checkCollisionsInternal(List& list);

    // AABB collision check
    static bool checkAABB(const Rectangle& a, const Rectangle& b);

private:
    std::vector<std::unique_ptr<ICollisionHandler>> handlers_;

    void dispatchCollision(IEntity& a, IEntity& b);
};

// Template implementations
template<typename ListA, typename ListB>
void CollisionManager::checkCollisions(ListA& listA, ListB& listB) {
    for (auto& a : listA) {
        if (!a->isActive()) continue;

        for (auto& b : listB) {
            if (!b->isActive()) continue;
            if (a.get() == b.get()) continue;

            if (checkAABB(a->getBounds(), b->getBounds())) {
                dispatchCollision(*a, *b);
            }
        }
    }
}

template<typename List>
void CollisionManager::checkCollisionsInternal(List& list) {
    for (size_t i = 0; i < list.size(); ++i) {
        if (!list[i]->isActive()) continue;

        for (size_t j = i + 1; j < list.size(); ++j) {
            if (!list[j]->isActive()) continue;

            if (checkAABB(list[i]->getBounds(), list[j]->getBounds())) {
                dispatchCollision(*list[i], *list[j]);
            }
        }
    }
}

} // namespace tank
