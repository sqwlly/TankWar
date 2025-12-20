#include "entities/powerups/PowerUp.hpp"
#include "rendering/IRenderer.hpp"

namespace tank {

PowerUp::PowerUp(int x, int y, PowerUpType type)
    : Entity(Vector2(static_cast<float>(x), static_cast<float>(y)),
             static_cast<float>(Constants::ELEMENT_SIZE),
             static_cast<float>(Constants::ELEMENT_SIZE))
    , type_(type)
    , lifetime_(LIFETIME)
    , elapsedTime_(0.0f)
    , blinking_(false)
    , expired_(false)
{
    renderLayer_ = RenderLayer::PowerUps;
    initAnimation();
}

void PowerUp::initAnimation() {
    // Simple two-frame animation for visibility
    Frame f1(0, 0, Constants::ELEMENT_SIZE, Constants::ELEMENT_SIZE, 300);
    Frame f2(0, 0, Constants::ELEMENT_SIZE, Constants::ELEMENT_SIZE, 300);
    animation_.addFrame(f1);
    animation_.addFrame(f2);
    animation_.start();

    // Blink animation (faster)
    Frame b1(0, 0, Constants::ELEMENT_SIZE, Constants::ELEMENT_SIZE, 100);
    Frame b2(0, 0, Constants::ELEMENT_SIZE, Constants::ELEMENT_SIZE, 100);
    blinkAnimation_.addFrame(b1);
    blinkAnimation_.addFrame(b2);
}

void PowerUp::update(float deltaTime) {
    elapsedTime_ += deltaTime;

    if (elapsedTime_ >= lifetime_) {
        expired_ = true;
        active_ = false;
        return;
    }

    if (elapsedTime_ >= BLINK_START && !blinking_) {
        blinking_ = true;
        blinkAnimation_.start();
    }

    if (blinking_) {
        blinkAnimation_.update(deltaTime);
    } else {
        animation_.update(deltaTime);
    }
}

void PowerUp::render(IRenderer& renderer) {
    if (!active_ || expired_) return;

    // Skip every other frame when blinking
    if (blinking_ && blinkAnimation_.getCurrentFrameIndex() == 1) {
        return;
    }

    int x = static_cast<int>(position_.x);
    int y = static_cast<int>(position_.y);
    int w = static_cast<int>(width_);
    int h = static_cast<int>(height_);

    Constants::Color color = getColorForType();

    // Draw power-up background
    renderer.drawRect(x, y, w, h, 64, 64, 64, 255);

    // Draw power-up icon based on type
    int cx = x + w / 2;
    int cy = y + h / 2;

    switch (type_) {
        case PowerUpType::Star:
            // Yellow star
            renderer.drawRect(cx - 8, cy - 2, 16, 4, 255, 255, 0, 255);
            renderer.drawRect(cx - 2, cy - 8, 4, 16, 255, 255, 0, 255);
            renderer.drawRect(cx - 6, cy - 6, 4, 4, 255, 255, 0, 255);
            renderer.drawRect(cx + 2, cy - 6, 4, 4, 255, 255, 0, 255);
            renderer.drawRect(cx - 6, cy + 2, 4, 4, 255, 255, 0, 255);
            renderer.drawRect(cx + 2, cy + 2, 4, 4, 255, 255, 0, 255);
            break;

        case PowerUpType::StopWatch:
            // Blue clock
            renderer.drawRect(cx - 10, cy - 10, 20, 20, 100, 100, 255, 255);
            renderer.drawRect(cx - 2, cy - 8, 4, 10, 255, 255, 255, 255);
            renderer.drawRect(cx, cy - 2, 6, 4, 255, 255, 255, 255);
            break;

        case PowerUpType::Gun:
            // Red gun upgrade
            renderer.drawRect(cx - 12, cy - 4, 24, 8, 255, 100, 100, 255);
            renderer.drawRect(cx + 8, cy - 8, 4, 16, 255, 100, 100, 255);
            break;

        case PowerUpType::IronCap:
            // Silver helmet
            renderer.drawRect(cx - 10, cy - 6, 20, 12, 200, 200, 200, 255);
            renderer.drawRect(cx - 8, cy, 16, 6, 150, 150, 150, 255);
            break;

        case PowerUpType::Bomb:
            // Red bomb/grenade
            renderer.drawRect(cx - 8, cy - 8, 16, 16, 255, 50, 50, 255);
            renderer.drawRect(cx - 2, cy - 12, 4, 6, 100, 100, 100, 255);
            break;

        case PowerUpType::Spade:
            // Brown shovel
            renderer.drawRect(cx - 4, cy - 10, 8, 12, 139, 69, 19, 255);
            renderer.drawRect(cx - 8, cy, 16, 8, 150, 150, 150, 255);
            break;

        case PowerUpType::Tank:
            // Green extra life tank
            renderer.drawRect(cx - 10, cy - 6, 20, 12, 0, 200, 0, 255);
            renderer.drawRect(cx - 2, cy - 10, 4, 8, 0, 150, 0, 255);
            break;
    }
}

void PowerUp::collect() {
    active_ = false;
    expired_ = true;
}

Constants::Color PowerUp::getColorForType() const {
    switch (type_) {
        case PowerUpType::Star:     return Constants::Color(255, 255, 0);
        case PowerUpType::StopWatch: return Constants::Color(100, 100, 255);
        case PowerUpType::Gun:      return Constants::Color(255, 100, 100);
        case PowerUpType::IronCap:  return Constants::Color(200, 200, 200);
        case PowerUpType::Bomb:     return Constants::Color(255, 50, 50);
        case PowerUpType::Spade:    return Constants::Color(139, 69, 19);
        case PowerUpType::Tank:     return Constants::Color(0, 200, 0);
    }
    return Constants::COLOR_WHITE;
}

} // namespace tank
