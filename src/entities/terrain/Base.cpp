#include "entities/terrain/Base.hpp"
#include "rendering/IRenderer.hpp"
#include "utils/Constants.hpp"

namespace tank {

Base::Base(int x, int y)
    : Entity(Vector2(static_cast<float>(x), static_cast<float>(y)),
             static_cast<float>(Constants::ELEMENT_SIZE),
             static_cast<float>(Constants::ELEMENT_SIZE))
    , health_(100)
    , maxHealth_(100)
    , defense_(0)
    , destroyed_(false)
    , showDestroyedSprite_(false)
{
    renderLayer_ = RenderLayer::Base;
}

void Base::update(float deltaTime) {
    // Base has no update logic beyond visual state
}

void Base::render(IRenderer& renderer) {
    int x = static_cast<int>(position_.x);
    int y = static_cast<int>(position_.y);
    int w = static_cast<int>(width_);
    int h = static_cast<int>(height_);

    if (!destroyed_) {
        // Draw eagle/base alive
        // Base plate
        renderer.drawRect(x, y, w, h, 128, 128, 128, 255);

        // Eagle body (triangle shape approximation)
        int cx = x + w / 2;

        // Eagle wings
        renderer.drawRect(x + 2, y + h/3, w - 4, h/3, 255, 200, 0, 255);

        // Eagle head
        renderer.drawRect(cx - 4, y + 4, 8, 10, 255, 200, 0, 255);

        // Eagle beak
        renderer.drawRect(cx - 2, y + 2, 4, 4, 255, 100, 0, 255);
    } else {
        // Draw destroyed base (white flag / rubble)
        renderer.drawRect(x, y, w, h, 64, 64, 64, 255);

        // X mark for destroyed
        renderer.drawRect(x + 4, y + 4, w - 8, 4, 200, 50, 50, 255);
        renderer.drawRect(x + 4, y + h - 8, w - 8, 4, 200, 50, 50, 255);
        renderer.drawRect(x + 4, y + 4, 4, h - 8, 200, 50, 50, 255);
        renderer.drawRect(x + w - 8, y + 4, 4, h - 8, 200, 50, 50, 255);
    }
}

void Base::takeDamage(int damage) {
    if (destroyed_) return;

    int actualDamage = std::max(0, damage - defense_);
    health_ -= actualDamage;

    if (health_ <= 0) {
        health_ = 0;
        die();
    }
}

void Base::takeDamage(int damage, const Rectangle& hitBox) {
    takeDamage(damage);
}

void Base::die() {
    if (!destroyed_) {
        destroyed_ = true;
        active_ = false;
        showDestroyedSprite_ = true;
        // Trigger game over in PlayingState
    }
}

void Base::reset() {
    health_ = maxHealth_;
    defense_ = 0;
    destroyed_ = false;
    active_ = true;
    showDestroyedSprite_ = false;
}

} // namespace tank
