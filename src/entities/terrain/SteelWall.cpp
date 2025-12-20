#include "entities/terrain/SteelWall.hpp"

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

    // Silver/gray color for steel
    Constants::Color steelColor{192, 192, 192};
    renderer.drawRectangle(getBounds(), steelColor, true);
}

} // namespace tank
