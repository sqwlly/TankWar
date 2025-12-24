#pragma once

#include "entities/terrain/Terrain.hpp"
#include <array>

namespace tank {

/**
 * @brief Destructible brick wall with 4-corner state
 * Each corner can be destroyed independently
 */
class BrickWall : public Terrain {
public:
    BrickWall(const Vector2& position);
    ~BrickWall() override = default;

    // ITerrain overrides
    void takeDamage(int damage, const Rectangle& hitBox) override;
    bool isDestroyed() const override;

    // Corner state
    bool isCornerAlive(int index) const { return cornerStates_[index]; }
    Rectangle getCornerBounds(int index) const { return corners_[index]; }
    bool intersectsSolid(const Rectangle& box) const;
    int getSpriteIndex() const;

protected:
    void onRender(IRenderer& renderer) override;

private:
    std::array<bool, 4> cornerStates_;  // TL, TR, BL, BR
    std::array<Rectangle, 4> corners_;

    void initializeCorners();
};

} // namespace tank
