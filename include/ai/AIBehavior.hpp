#pragma once

#include "ai/IAIBehavior.hpp"
#include "utils/Constants.hpp"
#include <vector>
#include <queue>
#include <random>

namespace tank {

class Level;

/**
 * @brief Simple random movement AI
 */
class SimpleAI : public IAIBehavior {
public:
    SimpleAI();
    ~SimpleAI() override = default;

    void update(EnemyTank& enemy, float deltaTime) override;
    void setTarget(const Vector2& target) override { targetPos_ = target; }

    Type getType() const override { return Type::Simple; }

private:
    Vector2 targetPos_;
    float directionTimer_;
    float fireTimer_;
    Direction currentDirection_;

    std::mt19937 rng_;

    static constexpr float DIRECTION_CHANGE_TIME = 1.5f;
    static constexpr float FIRE_INTERVAL = 0.8f;

    Direction getRandomDirection();
};

/**
 * @brief Pathfinding AI that navigates to base
 */
class PathfindingAI : public IAIBehavior {
public:
    PathfindingAI();
    ~PathfindingAI() override = default;

    void update(EnemyTank& enemy, float deltaTime) override;
    void setTarget(const Vector2& target) override { targetPos_ = target; }
    void setLevel(Level* level) { level_ = level; }

    Type getType() const override { return Type::Pathfinding; }

private:
    struct Node {
        int x, y;
        int distance;
        Direction dir;

        Node(int x, int y, int d, Direction dir)
            : x(x), y(y), distance(d), dir(dir) {}
    };

    Level* level_;
    Vector2 targetPos_;
    std::vector<Node> path_;
    int currentPathIndex_;
    float pathUpdateTimer_;
    float fireTimer_;

    std::mt19937 rng_;

    static constexpr float PATH_UPDATE_INTERVAL = 2.0f;
    static constexpr float FIRE_INTERVAL = 1.0f;
    static constexpr int GRID_SIZE = 26;

    void calculatePath(EnemyTank& enemy);
    bool isPassable(int x, int y) const;
    Direction getDirectionToNext(const EnemyTank& enemy) const;
};

} // namespace tank
