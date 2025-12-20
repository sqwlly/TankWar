#pragma once

#include "entities/terrain/Terrain.hpp"

namespace tank {

/**
 * @brief Indestructible steel wall
 * Only level 3 bullets can destroy it
 */
class SteelWall : public Terrain {
public:
    SteelWall(const Vector2& position);
    ~SteelWall() override = default;

    void takeDamage(int damage, const Rectangle& hitBox) override;
    bool isDestroyed() const override { return destroyed_; }

    // Steel can be destroyed by level 3 bullets
    void setDestructible(bool destructible) { canBeDestroyed_ = destructible; }

protected:
    void onRender(IRenderer& renderer) override;

private:
    bool destroyed_ = false;
    bool canBeDestroyed_ = false;  // Only true for level 3 bullets
};

} // namespace tank
