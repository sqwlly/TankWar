#pragma once

#include "entities/tanks/Tank.hpp"
#include "ai/IAIBehavior.hpp"
#include "utils/Constants.hpp"
#include <memory>

namespace tank {

/**
 * @brief AI-controlled enemy tank
 */
class EnemyTank : public Tank {
public:
    EnemyTank(const Vector2& position, EnemyType type);
    ~EnemyTank() override = default;

    // Enemy type
    EnemyType getEnemyType() const { return enemyType_; }
    int getTypeIndex() const { return static_cast<int>(enemyType_); }

    // Reward for destroying this enemy
    int getReward() const;

    // AI behavior
    void setAIBehavior(std::unique_ptr<IAIBehavior> behavior);
    IAIBehavior* getAIBehavior() { return aiBehavior_.get(); }

    // Movement steps
    int getStep() const { return step_; }
    void setStep(int step) { step_ = step; }
    void decrementStep() { step_--; }

    // Random shooting
    void randomFire();
    bool shouldFire() const { return fireChance_ >= 1 && fireChance_ <= 5; }

    // Power-up carrying
    void setCarriesPowerUp(bool carries) { carriesPowerUp_ = carries; }
    bool carriesPowerUp() const { return carriesPowerUp_; }

protected:
    void onUpdate(float deltaTime) override;
    void onRender(IRenderer& renderer) override;
    void onDie() override;
    void onSpawn() override;

private:
    EnemyType enemyType_;
    std::unique_ptr<IAIBehavior> aiBehavior_;

    int step_ = 0;
    int fireChance_ = 0;
    bool carriesPowerUp_ = false;

    void initializeStats();
};

} // namespace tank
