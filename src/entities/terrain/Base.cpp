#include "entities/terrain/Base.hpp"
#include "rendering/IRenderer.hpp"
#include "graphics/SpriteSheet.hpp"
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
    Rectangle baseSrc = destroyed_ ? Sprites::Base::getDestroyed() : Sprites::Base::getNormal();
    int srcX = static_cast<int>(baseSrc.x);
    int srcY = static_cast<int>(baseSrc.y);
    int srcSize = Sprites::Base::BASE_SIZE;

    int destX = static_cast<int>(position_.x);
    int destY = static_cast<int>(position_.y);
    int destSize = static_cast<int>(width_);

    renderer.drawSprite(srcX, srcY, srcSize, srcSize, destX, destY, destSize, destSize);
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
