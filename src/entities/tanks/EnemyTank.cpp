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
    // Direction order matches Java: UP(0), RIGHT(2), DOWN(4), LEFT(6)
    int dirCol = 0;
    switch (direction_) {
        case Direction::Up:    dirCol = Sprites::Tank::DIR_UP_COL; break;    // 0
        case Direction::Right: dirCol = Sprites::Tank::DIR_RIGHT_COL; break; // 2
        case Direction::Down:  dirCol = Sprites::Tank::DIR_DOWN_COL; break;  // 4
        case Direction::Left:  dirCol = Sprites::Tank::DIR_LEFT_COL; break;  // 6
    }

    // Enemy type determines the sprite set (0-2 for basic, fast, power; special for heavy)
    int baseY = Sprites::Tank::ENEMY_BASIC_Y;  // Row 2 (y=68)
    int typeOffset = static_cast<int>(enemyType_);

    // Flashing effect if carrying power-up
    int xOffset = 0;
    if (carriesPowerUp_) {
        // Alternate between normal and flash position
        xOffset = (animationFrame_ % 2) ? Sprites::Tank::FLASH_OFFSET_X : 0;
    }

    // Calculate source coordinates
    // Each enemy type has 8 columns (4 directions * 2 frames)
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
