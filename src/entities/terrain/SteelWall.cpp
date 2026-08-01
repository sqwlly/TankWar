#include "entities/terrain/SteelWall.hpp"
#include "graphics/SpriteSheet.hpp"

namespace tank {

SteelWall::SteelWall(const Vector2& position)
    : Terrain(position, TerrainType::Steel)
{
    // Steel wall is 17x17 (single cell)
    width_ = Constants::CELL_SIZE;
    height_ = Constants::CELL_SIZE;
    destructible_ = false;
}

void SteelWall::takeDamage(int damage, const Rectangle& hitBox) {
    if (canBeDestroyed_ && damage >= 100) {
        destroyed_ = true;
        active_ = false;
    }
}

void SteelWall::onRender(IRenderer& renderer) {
    if (destroyed_) return;

    // Steel sprite is 34x34 in source, but we render at 17x17 (CELL_SIZE)
    // Source is the top-left 17x17 portion of the 34x34 steel sprite
    constexpr int HALF_SIZE = Constants::CELL_SIZE;  // 17

    Rectangle steelSrc = Sprites::Terrain::getSteel();
    int srcX = static_cast<int>(steelSrc.x);
    int srcY = static_cast<int>(steelSrc.y);

    int destX = static_cast<int>(position_.x);
    int destY = static_cast<int>(position_.y);

    // Render source 17x17 to dest 17x17 (with slight overlap to prevent gaps)
    renderer.drawSprite(srcX, srcY, HALF_SIZE, HALF_SIZE,
                       destX, destY, HALF_SIZE + 1, HALF_SIZE + 1);
}

} // namespace tank
