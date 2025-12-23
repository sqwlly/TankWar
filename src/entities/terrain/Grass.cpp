#include "entities/terrain/Grass.hpp"
#include "rendering/IRenderer.hpp"
#include "graphics/SpriteSheet.hpp"
#include "utils/Constants.hpp"

namespace tank {

Grass::Grass(const Vector2& position)
    : Terrain(position, TerrainType::Grass)
{
    tankPassable_ = true;
    bulletPassable_ = true;
    destructible_ = false;
    renderLayer_ = RenderLayer::Grass;
}

void Grass::onRender(IRenderer& renderer) {
    // Grass source sprite is at (136, 238), use full 34x34 area
    int srcX = Sprites::Terrain::GRASS_X;
    int srcY = Sprites::Terrain::GRASS_Y;

    int destX = static_cast<int>(position_.x);
    int destY = static_cast<int>(position_.y);

    // Render at ELEMENT_SIZE to match sprite exactly
    int destSize = Sprites::ELEMENT_SIZE;

    renderer.drawSprite(srcX, srcY, Sprites::ELEMENT_SIZE, Sprites::ELEMENT_SIZE,
                       destX, destY, destSize, destSize);
}

} // namespace tank
