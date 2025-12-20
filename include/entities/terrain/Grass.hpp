#pragma once

#include "entities/terrain/Terrain.hpp"

namespace tank {

/**
 * @brief Grass terrain - tanks and bullets can pass through
 * Renders on top of tanks to hide them
 */
class Grass : public Terrain {
public:
    explicit Grass(const Vector2& position);
    ~Grass() override = default;

protected:
    void onUpdate(float deltaTime) override {}
    void onRender(IRenderer& renderer) override;
};

} // namespace tank
