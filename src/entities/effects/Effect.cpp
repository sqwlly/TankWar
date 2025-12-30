#include "entities/effects/Effect.hpp"
#include "rendering/IRenderer.hpp"
#include "graphics/SpriteSheet.hpp"

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

    // Use spawn sprite from sprite sheet
    int frame = animation_.getCurrentFrameIndex();
    Rectangle sprite = Sprites::Spawn::get(frame);

    renderer.drawSprite(
        static_cast<int>(sprite.x), static_cast<int>(sprite.y),
        static_cast<int>(sprite.width), static_cast<int>(sprite.height),
        x, y, size, size);
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

    // Use small explosion sprite from sprite sheet
    int frame = animation_.getCurrentFrameIndex();
    Rectangle sprite = Sprites::Explosion::getSmall(frame);

    renderer.drawSprite(
        static_cast<int>(sprite.x), static_cast<int>(sprite.y),
        static_cast<int>(sprite.width), static_cast<int>(sprite.height),
        x, y, size, size);
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

    // Use big explosion sprite from sprite sheet
    int frame = animation_.getCurrentFrameIndex();
    Rectangle sprite = Sprites::Explosion::getBig(frame);

    renderer.drawSprite(
        static_cast<int>(sprite.x), static_cast<int>(sprite.y),
        static_cast<int>(sprite.width), static_cast<int>(sprite.height),
        x, y, w, h);
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

    // Use shield sprite from sprite sheet
    int frame = animation_.getCurrentFrameIndex();
    Rectangle sprite = Sprites::Shield::get(frame);

    renderer.drawSprite(
        static_cast<int>(sprite.x), static_cast<int>(sprite.y),
        static_cast<int>(sprite.width), static_cast<int>(sprite.height),
        x, y, size, size);
}

void InvincibilityEffect::setPosition(int x, int y) {
    position_.x = static_cast<float>(x);
    position_.y = static_cast<float>(y);
}

} // namespace tank
