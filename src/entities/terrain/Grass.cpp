#include "entities/terrain/Grass.hpp"
#include "rendering/IRenderer.hpp"
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
    int x = static_cast<int>(position_.x);
    int y = static_cast<int>(position_.y);
    int w = static_cast<int>(width_);
    int h = static_cast<int>(height_);

    // Grass green color
    renderer.drawRect(x, y, w, h, 34, 139, 34, 255);  // Forest green

    // Draw grass pattern lines for texture
    for (int i = 2; i < w - 2; i += 4) {
        renderer.drawRect(x + i, y + 2, 2, h - 4, 0, 100, 0, 255);
    }
}

} // namespace tank
