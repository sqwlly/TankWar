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
    Rectangle waterSrc = Sprites::Terrain::getWater(animationFrame_);
    int srcX = static_cast<int>(waterSrc.x);
    int srcY = static_cast<int>(waterSrc.y);
    int srcSize = Sprites::TERRAIN_SIZE;

    int destX = static_cast<int>(position_.x);
    int destY = static_cast<int>(position_.y);
    int destSize = static_cast<int>(width_);

    // Water is rendered as 2x2 blocks
    int halfDest = destSize / 2;
    renderer.drawSprite(srcX, srcY, srcSize, srcSize, destX, destY, halfDest, halfDest);
    renderer.drawSprite(srcX, srcY, srcSize, srcSize, destX + halfDest, destY, halfDest, halfDest);
    renderer.drawSprite(srcX, srcY, srcSize, srcSize, destX, destY + halfDest, halfDest, halfDest);
    renderer.drawSprite(srcX, srcY, srcSize, srcSize, destX + halfDest, destY + halfDest, halfDest, halfDest);
}

} // namespace tank
