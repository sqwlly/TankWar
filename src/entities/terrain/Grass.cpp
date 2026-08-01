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
    // Grass sprite is 34x34 in source, but we use 17x17 with overlap
    constexpr int HALF_SIZE = Constants::CELL_SIZE;  // 17

    Rectangle grassSrc = Sprites::Terrain::getGrass();
    int srcX = static_cast<int>(grassSrc.x);
    int srcY = static_cast<int>(grassSrc.y);

    int destX = static_cast<int>(position_.x);
    int destY = static_cast<int>(position_.y);

    // Render source 17x17 to dest 18x18 (with overlap to prevent gaps)
    renderer.drawSprite(srcX, srcY, HALF_SIZE, HALF_SIZE,
                       destX, destY, HALF_SIZE + 1, HALF_SIZE + 1);
}

} // namespace tank
