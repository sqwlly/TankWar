#pragma once

#include "utils/Vector2.hpp"

namespace tank {

class EnemyTank;

/**
 * @brief AI behavior interface (Strategy Pattern)
 * Allows different AI strategies to be swapped at runtime
 */
class IAIBehavior {
public:
    virtual ~IAIBehavior() = default;

    virtual void update(EnemyTank& enemy, float deltaTime) = 0;
    virtual void setTarget(const Vector2& target) = 0;

    enum class Type {
        Simple,
        Pathfinding
    };

    virtual Type getType() const = 0;
};

} // namespace tank
