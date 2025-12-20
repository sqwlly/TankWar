#pragma once

#include "entities/terrain/Terrain.hpp"

namespace tank {

/**
 * @brief Water terrain - tanks cannot pass, bullets can
 */
class Water : public Terrain {
public:
    explicit Water(const Vector2& position);
    ~Water() override = default;

protected:
    void onUpdate(float deltaTime) override;
    void onRender(IRenderer& renderer) override;

private:
    float animationTimer_ = 0.0f;
    int animationFrame_ = 0;
};

} // namespace tank
