#include "entities/terrain/Water.hpp"
#include "rendering/IRenderer.hpp"
#include "graphics/SpriteSheet.hpp"

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
    // Water sprite is 34x34, render at full size like Java
    Rectangle waterSrc = Sprites::Terrain::getWater(animationFrame_);
    int srcX = static_cast<int>(waterSrc.x);
    int srcY = static_cast<int>(waterSrc.y);
    int srcSize = static_cast<int>(waterSrc.width);

    int destX = static_cast<int>(position_.x);
    int destY = static_cast<int>(position_.y);

    // Render at ELEMENT_SIZE to match sprite exactly
    int destSize = Sprites::ELEMENT_SIZE;

    renderer.drawSprite(srcX, srcY, srcSize, srcSize, destX, destY, destSize, destSize);
}

} // namespace tank
