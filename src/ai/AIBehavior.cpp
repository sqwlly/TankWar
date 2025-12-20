#include "ai/AIBehavior.hpp"
#include "entities/tanks/EnemyTank.hpp"
#include "level/Level.hpp"
#include <ctime>
#include <climits>
#include <algorithm>

namespace tank {

// SimpleAI implementation
SimpleAI::SimpleAI()
    : directionTimer_(0.0f)
    , fireTimer_(0.0f)
    , currentDirection_(Direction::Down)
    , rng_(static_cast<unsigned>(std::time(nullptr)))
{
}

void SimpleAI::update(EnemyTank& enemy, float deltaTime) {
    directionTimer_ += deltaTime;
    fireTimer_ += deltaTime;

    // Change direction periodically
    if (directionTimer_ >= DIRECTION_CHANGE_TIME) {
        directionTimer_ = 0.0f;
        currentDirection_ = getRandomDirection();
    }

    // Move in current direction
    enemy.move(currentDirection_);

    // Fire periodically
    if (fireTimer_ >= FIRE_INTERVAL) {
        fireTimer_ = 0.0f;
        enemy.shoot();
    }
}

Direction SimpleAI::getRandomDirection() {
    std::uniform_int_distribution<int> dist(0, 3);
    return static_cast<Direction>(dist(rng_));
}

// PathfindingAI implementation
PathfindingAI::PathfindingAI()
    : level_(nullptr)
    , currentPathIndex_(0)
    , pathUpdateTimer_(0.0f)
    , fireTimer_(0.0f)
    , rng_(static_cast<unsigned>(std::time(nullptr)))
{
}

void PathfindingAI::update(EnemyTank& enemy, float deltaTime) {
    pathUpdateTimer_ += deltaTime;
    fireTimer_ += deltaTime;

    // Recalculate path periodically
    if (pathUpdateTimer_ >= PATH_UPDATE_INTERVAL || path_.empty()) {
        pathUpdateTimer_ = 0.0f;
        calculatePath(enemy);
    }

    // Follow path
    if (!path_.empty() && currentPathIndex_ < static_cast<int>(path_.size())) {
        Direction dir = getDirectionToNext(enemy);
        enemy.move(dir);

        // Check if reached current waypoint
        const Node& target = path_[currentPathIndex_];
        int cellSize = Constants::CELL_SIZE;
        int targetX = target.x * cellSize;
        int targetY = target.y * cellSize;
        int enemyX = static_cast<int>(enemy.getPosition().x);
        int enemyY = static_cast<int>(enemy.getPosition().y);

        if (std::abs(enemyX - targetX) < 4 && std::abs(enemyY - targetY) < 4) {
            ++currentPathIndex_;
        }
    } else {
        // No path or reached end, move randomly
        std::uniform_int_distribution<int> dist(0, 3);
        enemy.move(static_cast<Direction>(dist(rng_)));
    }

    // Fire periodically
    if (fireTimer_ >= FIRE_INTERVAL) {
        fireTimer_ = 0.0f;
        enemy.shoot();
    }
}

void PathfindingAI::calculatePath(EnemyTank& enemy) {
    if (!level_) return;

    path_.clear();
    currentPathIndex_ = 0;

    int cellSize = Constants::CELL_SIZE;
    int startX = static_cast<int>(enemy.getPosition().x) / cellSize;
    int startY = static_cast<int>(enemy.getPosition().y) / cellSize;
    int endX = static_cast<int>(targetPos_.x) / cellSize;
    int endY = static_cast<int>(targetPos_.y) / cellSize;

    // BFS pathfinding
    std::vector<std::vector<int>> distance(GRID_SIZE, std::vector<int>(GRID_SIZE, INT_MAX));
    std::vector<std::vector<bool>> visited(GRID_SIZE, std::vector<bool>(GRID_SIZE, false));
    std::queue<Node> queue;

    // Start from target (reverse BFS)
    queue.push(Node(endX, endY, 0, Direction::Down));
    distance[endY][endX] = 0;
    visited[endY][endX] = true;

    int dx[] = {0, 1, 0, -1};
    int dy[] = {-1, 0, 1, 0};
    Direction dirs[] = {Direction::Up, Direction::Right, Direction::Down, Direction::Left};

    while (!queue.empty()) {
        Node current = queue.front();
        queue.pop();

        for (int i = 0; i < 4; ++i) {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];

            if (nx >= 0 && nx < GRID_SIZE && ny >= 0 && ny < GRID_SIZE &&
                !visited[ny][nx] && isPassable(nx, ny)) {
                visited[ny][nx] = true;
                distance[ny][nx] = distance[current.y][current.x] + 1;
                queue.push(Node(nx, ny, distance[ny][nx], dirs[i]));
            }
        }
    }

    // Trace path from start to target
    if (distance[startY][startX] == INT_MAX) return;  // No path

    int x = startX;
    int y = startY;

    while (x != endX || y != endY) {
        int minDist = distance[y][x];
        int nextX = x, nextY = y;
        Direction nextDir = Direction::Down;

        for (int i = 0; i < 4; ++i) {
            int nx = x + dx[i];
            int ny = y + dy[i];

            if (nx >= 0 && nx < GRID_SIZE && ny >= 0 && ny < GRID_SIZE) {
                if (distance[ny][nx] < minDist) {
                    minDist = distance[ny][nx];
                    nextX = nx;
                    nextY = ny;
                    nextDir = dirs[i];
                }
            }
        }

        if (nextX == x && nextY == y) break;  // Stuck

        path_.push_back(Node(nextX, nextY, minDist, nextDir));
        x = nextX;
        y = nextY;
    }
}

bool PathfindingAI::isPassable(int x, int y) const {
    if (!level_) return false;
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) return false;

    const auto& terrainMap = level_->getTerrainMap();

    // Check 2x2 tiles (tank size)
    for (int dy = 0; dy < 2; ++dy) {
        for (int dx = 0; dx < 2; ++dx) {
            int tx = x + dx;
            int ty = y + dy;
            if (tx >= GRID_SIZE || ty >= GRID_SIZE) continue;

            TerrainType type = terrainMap[ty][tx];
            // Steel and water block tanks
            if (type == TerrainType::Steel || type == TerrainType::Water) {
                return false;
            }
        }
    }
    return true;
}

Direction PathfindingAI::getDirectionToNext(const EnemyTank& enemy) const {
    if (path_.empty() || currentPathIndex_ >= static_cast<int>(path_.size())) {
        return Direction::Down;
    }

    const Node& target = path_[currentPathIndex_];
    int cellSize = Constants::CELL_SIZE;
    int targetX = target.x * cellSize;
    int targetY = target.y * cellSize;
    int enemyX = static_cast<int>(enemy.getPosition().x);
    int enemyY = static_cast<int>(enemy.getPosition().y);

    int diffX = targetX - enemyX;
    int diffY = targetY - enemyY;

    // Prioritize larger difference
    if (std::abs(diffX) > std::abs(diffY)) {
        return (diffX > 0) ? Direction::Right : Direction::Left;
    } else {
        return (diffY > 0) ? Direction::Down : Direction::Up;
    }
}

} // namespace tank
