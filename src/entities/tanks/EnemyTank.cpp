#include "entities/tanks/EnemyTank.hpp"
#include <random>

namespace tank {

// Random number generator
static std::random_device rd;
static std::mt19937 gen(rd());

EnemyTank::EnemyTank(const Vector2& position, EnemyType type)
    : Tank(position)
    , enemyType_(type)
{
    direction_ = Direction::Down;
    initializeStats();
}

void EnemyTank::initializeStats() {
    switch (enemyType_) {
        case EnemyType::Basic:
            health_ = Constants::ENEMY_TYPE1_HP;
            speed_ = 2;
            break;

        case EnemyType::Fast:
            health_ = Constants::ENEMY_TYPE2_HP;
            speed_ = 3;  // Faster
            break;

        case EnemyType::Power:
            health_ = Constants::ENEMY_TYPE3_HP;
            speed_ = 2;
            level_ = 1;  // Higher level bullets
            break;

        case EnemyType::Heavy:
            health_ = Constants::ENEMY_TYPE4_HP;
            maxHealth_ = 100;
            defense_ = 50;
            speed_ = 1;  // Slower but tanky
            break;
    }
}

int EnemyTank::getReward() const {
    int index = std::min(static_cast<int>(enemyType_), 3);
    return Constants::ENEMY_REWARD[index];
}

void EnemyTank::setAIBehavior(std::unique_ptr<IAIBehavior> behavior) {
    aiBehavior_ = std::move(behavior);
}

void EnemyTank::randomFire() {
    std::uniform_int_distribution<> dist(0, step_ > 0 ? step_ : 10);
    fireChance_ = dist(gen);
}

void EnemyTank::onUpdate(float deltaTime) {
    if (aiBehavior_) {
        aiBehavior_->update(*this, deltaTime);
    }
}

void EnemyTank::onRender(IRenderer& renderer) {
    // Enemy-specific rendering
    // Color based on type
    Constants::Color color = Constants::COLOR_GRAY;
    switch (enemyType_) {
        case EnemyType::Basic: color = {128, 128, 128}; break;
        case EnemyType::Fast: color = {255, 128, 0}; break;
        case EnemyType::Power: color = {128, 0, 255}; break;
        case EnemyType::Heavy: color = {0, 128, 0}; break;
    }

    Rectangle bounds = getBounds();
    renderer.drawRectangle(bounds, color, true);
}

void EnemyTank::onDie() {
    // Enemy death effects handled by game state
}

void EnemyTank::onSpawn() {
    initializeStats();
}

} // namespace tank
