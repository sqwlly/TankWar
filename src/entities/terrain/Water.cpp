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
    // Water sprite is 34x34 in source, but we use 17x17 with overlap
    constexpr int HALF_SIZE = Constants::CELL_SIZE;  // 17

    Rectangle waterSrc = Sprites::Terrain::getWater(animationFrame_);
    int srcX = static_cast<int>(waterSrc.x);
    int srcY = static_cast<int>(waterSrc.y);

    int destX = static_cast<int>(position_.x);
    int destY = static_cast<int>(position_.y);

    // Render source 17x17 to dest 18x18 (with overlap to prevent gaps)
    renderer.drawSprite(srcX, srcY, HALF_SIZE, HALF_SIZE,
                       destX, destY, HALF_SIZE + 1, HALF_SIZE + 1);
}

} // namespace tank
