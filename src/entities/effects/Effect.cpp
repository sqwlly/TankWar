#include "entities/effects/Effect.hpp"
#include "rendering/IRenderer.hpp"

namespace tank {

// Base Effect implementation
Effect::Effect(int x, int y, EffectType type)
    : Entity(Vector2(static_cast<float>(x), static_cast<float>(y)),
             static_cast<float>(Constants::ELEMENT_SIZE),
             static_cast<float>(Constants::ELEMENT_SIZE))
    , type_(type)
    , complete_(false)
    , playCount_(0)
    , targetPlayCount_(1)
{
    renderLayer_ = RenderLayer::Effects;
}

void Effect::update(float deltaTime) {
    if (complete_) return;

    animation_.update(deltaTime);

    if (animation_.hasPlayed(targetPlayCount_)) {
        complete_ = true;
        active_ = false;
        onComplete();
    }
}

void Effect::render(IRenderer& renderer) {
    // Base implementation - subclasses override
}

// SpawnEffect implementation
SpawnEffect::SpawnEffect(int x, int y)
    : Effect(x, y, EffectType::SpawnEffect)
    , currentFrame_(0)
    , frameTimer_(0.0f)
{
    targetPlayCount_ = 5;  // Play 5 times like original
    initAnimation();
}

void SpawnEffect::initAnimation() {
    // 4 frames for spawn animation
    for (int i = 0; i < 4; ++i) {
        Frame f(i * Constants::ELEMENT_SIZE, 0, Constants::ELEMENT_SIZE, Constants::ELEMENT_SIZE, 60);
        animation_.addFrame(f);
    }
    animation_.start();
}

void SpawnEffect::render(IRenderer& renderer) {
    if (complete_) return;

    int x = static_cast<int>(position_.x);
    int y = static_cast<int>(position_.y);
    int size = Constants::ELEMENT_SIZE;

    // Draw star-like spawn animation
    int frame = animation_.getCurrentFrameIndex();
    int offset = frame * 4;

    // Alternating pattern
    if (frame % 2 == 0) {
        renderer.drawRect(x + size/4 - offset, y + size/4 - offset,
                         size/2 + offset*2, size/2 + offset*2,
                         255, 255, 255, 200);
    } else {
        renderer.drawRect(x + size/3, y + size/3,
                         size/3, size/3, 255, 255, 255, 255);
        renderer.drawRect(x + 2, y + size/2 - 2, size - 4, 4, 255, 255, 255, 200);
        renderer.drawRect(x + size/2 - 2, y + 2, 4, size - 4, 255, 255, 255, 200);
    }
}

// BulletExplosion implementation
BulletExplosion::BulletExplosion(int x, int y)
    : Effect(x, y, EffectType::BulletExplosion)
    , currentFrame_(0)
    , frameTimer_(0.0f)
{
    targetPlayCount_ = 1;
    initAnimation();
}

void BulletExplosion::initAnimation() {
    // 3 frames for bullet explosion
    for (int i = 0; i < 3; ++i) {
        Frame f(i * Constants::ELEMENT_SIZE, 0, Constants::ELEMENT_SIZE, Constants::ELEMENT_SIZE, 25);
        animation_.addFrame(f);
    }
    animation_.start();
}

void BulletExplosion::render(IRenderer& renderer) {
    if (complete_) return;

    int x = static_cast<int>(position_.x);
    int y = static_cast<int>(position_.y);
    int size = Constants::ELEMENT_SIZE;

    int frame = animation_.getCurrentFrameIndex();
    int radius = 4 + frame * 6;

    // Draw expanding circle effect
    int cx = x + size / 2;
    int cy = y + size / 2;

    // Orange/yellow explosion
    renderer.drawRect(cx - radius, cy - radius, radius * 2, radius * 2,
                     255, 200 - frame * 50, 0, 255 - frame * 50);

    // Inner bright core
    if (frame < 2) {
        renderer.drawRect(cx - radius/2, cy - radius/2, radius, radius,
                         255, 255, 200, 255);
    }
}

// TankExplosion implementation
TankExplosion::TankExplosion(int x, int y)
    : Effect(x, y, EffectType::TankExplosion)
    , currentFrame_(0)
    , frameTimer_(0.0f)
{
    // Larger explosion size
    width_ = Constants::ELEMENT_SIZE * 2;
    height_ = Constants::ELEMENT_SIZE * 2;
    targetPlayCount_ = 3;
    initAnimation();
}

void TankExplosion::initAnimation() {
    // 2 frames for tank explosion, played 3 times
    for (int i = 0; i < 2; ++i) {
        Frame f(i * Constants::ELEMENT_SIZE * 2, 0,
                Constants::ELEMENT_SIZE * 2, Constants::ELEMENT_SIZE * 2, 75);
        animation_.addFrame(f);
    }
    animation_.start();
}

void TankExplosion::render(IRenderer& renderer) {
    if (complete_) return;

    int x = static_cast<int>(position_.x);
    int y = static_cast<int>(position_.y);
    int w = static_cast<int>(width_);
    int h = static_cast<int>(height_);

    int frame = animation_.getCurrentFrameIndex();

    // Large explosion effect
    int cx = x + w / 2;
    int cy = y + h / 2;

    if (frame == 0) {
        // First frame - expanding fireball
        renderer.drawRect(cx - 24, cy - 24, 48, 48, 255, 100, 0, 255);
        renderer.drawRect(cx - 16, cy - 16, 32, 32, 255, 200, 0, 255);
        renderer.drawRect(cx - 8, cy - 8, 16, 16, 255, 255, 200, 255);
    } else {
        // Second frame - larger explosion with debris
        renderer.drawRect(cx - 32, cy - 32, 64, 64, 200, 50, 0, 200);
        renderer.drawRect(cx - 20, cy - 20, 40, 40, 255, 150, 0, 255);
        renderer.drawRect(cx - 10, cy - 10, 20, 20, 255, 255, 100, 255);

        // Debris particles
        renderer.drawRect(cx - 28, cy - 8, 8, 8, 100, 100, 100, 255);
        renderer.drawRect(cx + 20, cy - 12, 6, 6, 100, 100, 100, 255);
        renderer.drawRect(cx - 4, cy + 22, 10, 6, 100, 100, 100, 255);
        renderer.drawRect(cx + 16, cy + 18, 8, 8, 100, 100, 100, 255);
    }
}

// InvincibilityEffect implementation
InvincibilityEffect::InvincibilityEffect(int x, int y, float duration)
    : Effect(x, y, EffectType::Invincibility)
    , duration_(duration)
    , elapsed_(0.0f)
    , expired_(false)
    , currentFrame_(0)
    , frameTimer_(0.0f)
{
    initAnimation();
}

void InvincibilityEffect::initAnimation() {
    // 2 frames alternating shield
    for (int i = 0; i < 2; ++i) {
        Frame f(0, 0, Constants::ELEMENT_SIZE, Constants::ELEMENT_SIZE, 50);
        animation_.addFrame(f);
    }
    animation_.start();
}

void InvincibilityEffect::update(float deltaTime) {
    elapsed_ += deltaTime;

    if (elapsed_ >= duration_) {
        expired_ = true;
        complete_ = true;
        active_ = false;
        return;
    }

    animation_.update(deltaTime);
}

void InvincibilityEffect::render(IRenderer& renderer) {
    if (expired_ || complete_) return;

    int x = static_cast<int>(position_.x);
    int y = static_cast<int>(position_.y);
    int size = Constants::ELEMENT_SIZE;

    int frame = animation_.getCurrentFrameIndex();

    // Alternating shield pattern
    if (frame == 0) {
        // Outer ring
        renderer.drawRect(x, y, size, 2, 255, 255, 255, 200);
        renderer.drawRect(x, y + size - 2, size, 2, 255, 255, 255, 200);
        renderer.drawRect(x, y, 2, size, 255, 255, 255, 200);
        renderer.drawRect(x + size - 2, y, 2, size, 255, 255, 255, 200);
    } else {
        // Inner pattern
        renderer.drawRect(x + 4, y + 4, size - 8, 2, 255, 255, 255, 180);
        renderer.drawRect(x + 4, y + size - 6, size - 8, 2, 255, 255, 255, 180);
        renderer.drawRect(x + 4, y + 4, 2, size - 8, 255, 255, 255, 180);
        renderer.drawRect(x + size - 6, y + 4, 2, size - 8, 255, 255, 255, 180);
    }
}

void InvincibilityEffect::setPosition(int x, int y) {
    position_.x = static_cast<float>(x);
    position_.y = static_cast<float>(y);
}

} // namespace tank
