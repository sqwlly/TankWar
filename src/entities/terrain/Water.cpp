#include "entities/terrain/Water.hpp"
#include "rendering/IRenderer.hpp"

namespace tank {

Water::Water(const Vector2& position)
    : Terrain(position, TerrainType::Water)
{
    renderLayer_ = RenderLayer::Water;
    tankPassable_ = false;
    bulletPassable_ = true;
}

void Water::onUpdate(float deltaTime) {
    // Water animation
    animationTimer_ += deltaTime;
    if (animationTimer_ >= 0.5f) {
        animationTimer_ = 0.0f;
        animationFrame_ = (animationFrame_ + 1) % 2;
    }
}

void Water::onRender(IRenderer& renderer) {
    int x = static_cast<int>(position_.x);
    int y = static_cast<int>(position_.y);
    int w = static_cast<int>(width_);
    int h = static_cast<int>(height_);

    // Blue water color with animation variation
    int shade = animationFrame_ == 0 ? 200 : 180;
    renderer.drawRect(x, y, w, h, 0, 100, shade, 255);

    // Add wave pattern
    if (animationFrame_ == 0) {
        renderer.drawRect(x + 2, y + h/3, w - 4, 2, 100, 150, 255, 200);
    } else {
        renderer.drawRect(x + 2, y + 2*h/3, w - 4, 2, 100, 150, 255, 200);
    }
}

} // namespace tank
