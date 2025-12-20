#include "entities/terrain/Terrain.hpp"

namespace tank {

Terrain::Terrain(const Vector2& position, TerrainType type)
    : Entity(position, Constants::CELL_SIZE, Constants::CELL_SIZE)
    , type_(type)
{
    // Set properties based on type
    switch (type) {
        case TerrainType::Empty:
            tankPassable_ = true;
            bulletPassable_ = true;
            active_ = false;
            break;

        case TerrainType::Brick:
            tankPassable_ = false;
            bulletPassable_ = false;
            destructible_ = true;
            renderLayer_ = RenderLayer::Terrain;
            break;

        case TerrainType::Steel:
            tankPassable_ = false;
            bulletPassable_ = false;
            destructible_ = false;  // Only level 3 bullets can destroy
            renderLayer_ = RenderLayer::Terrain;
            break;

        case TerrainType::Water:
            tankPassable_ = false;
            bulletPassable_ = true;
            renderLayer_ = RenderLayer::Water;
            break;

        case TerrainType::Grass:
            tankPassable_ = true;
            bulletPassable_ = true;
            renderLayer_ = RenderLayer::Grass;  // Renders on top
            break;

        case TerrainType::Base:
            tankPassable_ = false;
            bulletPassable_ = false;
            destructible_ = true;
            renderLayer_ = RenderLayer::Base;
            break;
    }
}

} // namespace tank
