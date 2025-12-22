#include "entities/tanks/EnemyTank.hpp"
#include "graphics/SpriteSheet.hpp"
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
    // Calculate sprite position based on direction and animation frame
    int dirCol = 0;
    switch (direction_) {
        case Direction::Up:    dirCol = Sprites::Tank::DIR_UP_COL; break;
        case Direction::Down:  dirCol = Sprites::Tank::DIR_DOWN_COL; break;
        case Direction::Left:  dirCol = Sprites::Tank::DIR_LEFT_COL; break;
        case Direction::Right: dirCol = Sprites::Tank::DIR_RIGHT_COL; break;
    }

    // Enemy type determines base Y position
    int baseY = Sprites::Tank::ENEMY_BASIC_Y;
    int typeOffset = static_cast<int>(enemyType_);

    // Flashing effect if carrying power-up
    int xOffset = 0;
    if (carriesPowerUp_) {
        // Alternate between normal and flash position
        xOffset = (animationFrame_ % 2) ? Sprites::Tank::FLASH_OFFSET_X : 0;
    }

    // Calculate source coordinates
    int srcX = (typeOffset * 8 + dirCol + animationFrame_) * Sprites::ELEMENT_SIZE + xOffset;
    int srcY = baseY;

    int destX = static_cast<int>(position_.x);
    int destY = static_cast<int>(position_.y);
    int destSize = static_cast<int>(width_);

    renderer.drawSprite(srcX, srcY, Sprites::ELEMENT_SIZE, Sprites::ELEMENT_SIZE,
                       destX, destY, destSize, destSize);
}

void EnemyTank::onDie() {
    // Enemy death effects handled by game state
}

void EnemyTank::onSpawn() {
    initializeStats();
}

} // namespace tank
