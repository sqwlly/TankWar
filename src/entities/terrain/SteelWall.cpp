#include "entities/terrain/SteelWall.hpp"
#include "graphics/SpriteSheet.hpp"

namespace tank {

SteelWall::SteelWall(const Vector2& position)
    : Terrain(position, TerrainType::Steel)
{
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

    // Steel sprite is 34x34
    Rectangle steelSrc = Sprites::Terrain::getSteel();
    int srcX = static_cast<int>(steelSrc.x);
    int srcY = static_cast<int>(steelSrc.y);
    int srcSize = static_cast<int>(steelSrc.width);

    int destX = static_cast<int>(position_.x);
    int destY = static_cast<int>(position_.y);

    // Render at tile size to match collision bounds
    int destSize = static_cast<int>(width_);

    renderer.drawSprite(srcX, srcY, srcSize, srcSize, destX, destY, destSize, destSize);
}

} // namespace tank
