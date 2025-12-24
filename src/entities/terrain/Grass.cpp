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
    Rectangle grassSrc = Sprites::Terrain::getGrass();
    int srcX = static_cast<int>(grassSrc.x);
    int srcY = static_cast<int>(grassSrc.y);
    int srcSize = static_cast<int>(grassSrc.width);

    int destX = static_cast<int>(position_.x);
    int destY = static_cast<int>(position_.y);

    // Render at tile size to match terrain grid
    int destSize = static_cast<int>(width_);

    renderer.drawSprite(srcX, srcY, srcSize, srcSize,
                       destX, destY, destSize, destSize);
}

} // namespace tank
