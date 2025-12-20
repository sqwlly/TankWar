#pragma once

#include "entities/tanks/Tank.hpp"
#include "input/InputManager.hpp"

namespace tank {

/**
 * @brief Player-controlled tank
 * Supports Player 1 (WASD + Space) and Player 2 (Arrows + Numpad0)
 */
class PlayerTank : public Tank {
public:
    PlayerTank(int playerId, const Vector2& spawnPosition);
    ~PlayerTank() override = default;

    // Player identity
    int getPlayerId() const { return playerId_; }

    // Score
    int getScore() const { return score_; }
    void addScore(int points) { score_ += points; }
    void resetScore() { score_ = 0; }

    // Invincibility
    void makeInvincible(float duration);
    bool isInvincible() const { return invincibleTimer_ > 0; }

    // Respawn
    void respawn();
    void resetLevel() { level_ = 0; }

    // Input handling
    void handleInput(const InputManager::PlayerInput& input);

protected:
    void onUpdate(float deltaTime) override;
    void onRender(IRenderer& renderer) override;
    void onShoot() override;
    void onDie() override;
    void onSpawn() override;
    void onUpgrade() override;

private:
    int playerId_;
    int score_ = 0;

    // Invincibility
    float invincibleTimer_ = 0.0f;
    bool invincibleVisible_ = true;
    float invincibleBlinkTimer_ = 0.0f;

    // Spawn position for respawn
    Vector2 spawnPosition_;

    // Input state
    bool wantsToShoot_ = false;
};

} // namespace tank
