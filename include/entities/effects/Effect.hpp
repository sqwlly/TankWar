#pragma once

#include "entities/base/Entity.hpp"
#include "graphics/Animation.hpp"
#include "utils/Constants.hpp"

namespace tank {

/**
 * @brief Type of visual effect
 */
enum class EffectType {
    SpawnEffect,        // Tank spawn animation
    BulletExplosion,    // Small explosion when bullet hits
    TankExplosion,      // Large explosion when tank dies
    Invincibility       // Shield effect around tank
};

/**
 * @brief Base class for visual effects
 */
class Effect : public Entity {
public:
    Effect(int x, int y, EffectType type);
    ~Effect() override = default;

    void update(float deltaTime) override;
    void render(IRenderer& renderer) override;

    EffectType getEffectType() const { return type_; }
    bool isComplete() const { return complete_; }

    RenderLayer getRenderLayer() const { return RenderLayer::Effects; }

protected:
    EffectType type_;
    Animation animation_;
    bool complete_;
    int playCount_;         // Number of times to play
    int targetPlayCount_;   // Target play count before complete

    virtual void initAnimation() = 0;
    virtual void onComplete() {}
};

/**
 * @brief Spawn effect (star animation when tank appears)
 */
class SpawnEffect : public Effect {
public:
    SpawnEffect(int x, int y);
    void render(IRenderer& renderer) override;

protected:
    void initAnimation() override;

private:
    int currentFrame_;
    float frameTimer_;
};

/**
 * @brief Small explosion when bullet hits something
 */
class BulletExplosion : public Effect {
public:
    BulletExplosion(int x, int y);
    void render(IRenderer& renderer) override;

protected:
    void initAnimation() override;

private:
    int currentFrame_;
    float frameTimer_;
};

/**
 * @brief Large explosion when tank is destroyed
 */
class TankExplosion : public Effect {
public:
    TankExplosion(int x, int y);
    void render(IRenderer& renderer) override;

protected:
    void initAnimation() override;

private:
    int currentFrame_;
    float frameTimer_;
};

/**
 * @brief Invincibility shield effect
 */
class InvincibilityEffect : public Effect {
public:
    InvincibilityEffect(int x, int y, float duration);

    void update(float deltaTime) override;
    void render(IRenderer& renderer) override;

    void setPosition(int x, int y);
    bool isExpired() const { return expired_; }

protected:
    void initAnimation() override;

private:
    float duration_;
    float elapsed_;
    bool expired_;
    int currentFrame_;
    float frameTimer_;
};

} // namespace tank
