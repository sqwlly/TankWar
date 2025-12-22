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

    // Java renders at ELEMENT_SIZE / 2 + 2 = 19 for grass
    // But we use ELEMENT_SIZE + 1 = 35 to match other terrain
    int destSize = Sprites::ELEMENT_SIZE + 1;

    renderer.drawSprite(srcX, srcY, Sprites::ELEMENT_SIZE, Sprites::ELEMENT_SIZE,
                       destX, destY, destSize, destSize);
}

} // namespace tank
