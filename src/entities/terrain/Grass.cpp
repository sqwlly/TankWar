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
    // Grass sprite is 17x17 (HALF_SIZE), scale to destination size (17x17)
    Rectangle grassSrc = Sprites::Terrain::getGrass();
    int srcX = static_cast<int>(grassSrc.x);
    int srcY = static_cast<int>(grassSrc.y);
    int srcSize = static_cast<int>(grassSrc.width);

    int destX = static_cast<int>(position_.x);
    int destY = static_cast<int>(position_.y);
    int destSize = static_cast<int>(width_);

    // Render scaled sprite directly (17x17 -> 17x17, 1:1 mapping)
    renderer.drawSprite(srcX, srcY, srcSize, srcSize, destX, destY, destSize, destSize);
}

} // namespace tank
