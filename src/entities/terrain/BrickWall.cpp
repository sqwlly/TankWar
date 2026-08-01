#include "entities/terrain/BrickWall.hpp"
#include "graphics/SpriteSheet.hpp"
#include <algorithm>
#include <cmath>
#include <set>

namespace tank {

BrickWall::BrickWall(const Vector2& position)
    : BrickWall(position, std::array<bool, 4>{true, true, true, true})
{
}

BrickWall::BrickWall(const Vector2& position, const std::array<bool, 4>& cornerStates)
    : Terrain(position, TerrainType::Brick)
    , cornerStates_(cornerStates)
{
    // Brick wall is a full element (2x2 half-cells), each corner is 17x17
    width_ = Constants::ELEMENT_SIZE;  // 34x34 total
    height_ = Constants::ELEMENT_SIZE;
    initializeCorners();

    if (isDestroyed()) {
        active_ = false;
    }
}

void BrickWall::initializeCorners() {
    // Each corner is 17x17 (CELL_SIZE)
    const float leftW = width_ / 2.0f;  // 17
    const float topH = height_ / 2.0f;  // 17

    corners_[0] = Rectangle(position_.x, position_.y, leftW, topH);                  // Top-Left
    corners_[1] = Rectangle(position_.x + leftW, position_.y, leftW, topH);          // Top-Right
    corners_[2] = Rectangle(position_.x, position_.y + topH, leftW, topH);           // Bottom-Left
    corners_[3] = Rectangle(position_.x + leftW, position_.y + topH, leftW, topH);   // Bottom-Right
}

void BrickWall::takeDamage(int damage, const Rectangle& hitBox) {
    for (int i = 0; i < 4; ++i) {
        if (cornerStates_[i] && corners_[i].intersects(hitBox)) {
            cornerStates_[i] = false;
        }
    }

    if (isDestroyed()) {
        active_ = false;
    }
}

bool BrickWall::isDestroyed() const {
    return std::none_of(cornerStates_.begin(), cornerStates_.end(),
                        [](bool alive) { return alive; });
}

bool BrickWall::intersectsSolid(const Rectangle& box) const {
    for (int i = 0; i < 4; ++i) {
        if (cornerStates_[i] && corners_[i].intersects(box)) {
            return true;
        }
    }
    return false;
}

int BrickWall::getSpriteIndex() const {
    // The sprite sheet represents DESTROYED corners, not alive corners
    // Sprite 0 = all corners intact, Sprite 1-14 = various destruction patterns
    // Need to convert from alive-corner states to destroyed-corner states

    if (cornerStates_[0] && cornerStates_[1] && cornerStates_[2] && cornerStates_[3]) {
        // All corners alive -> use sprite 0 (full brick)
        return 0;
    }

    // Count how many corners are destroyed
    int destroyedCount = 0;
    int destroyedTL = cornerStates_[0] ? 0 : 1;
    int destroyedTR = cornerStates_[1] ? 0 : 1;
    int destroyedBL = cornerStates_[2] ? 0 : 1;
    int destroyedBR = cornerStates_[3] ? 0 : 1;
    destroyedCount = destroyedTL + destroyedTR + destroyedBL + destroyedBR;

    // Map destruction patterns to sprite indices based on sprite sheet layout
    // Sprite sheet order: 0 (none) -> 1 (TL) -> 2 (TR) -> 3 (BL) -> 4 (BR) ->
    // 5 (TL+TR) -> 6 (TL+BL) -> 7 (TL+BR) -> 8 (TR+BL) -> 9 (TR+BR) -> 10 (BL+BR) ->
    // 11 (TL+TR+BL) -> 12 (TL+TR+BR) -> 13 (TL+BL+BR) -> 14 (TR+BL+BR)

    if (destroyedCount == 1) {
        if (destroyedTL) return 1;
        if (destroyedTR) return 2;
        if (destroyedBL) return 3;
        if (destroyedBR) return 4;
    }
    else if (destroyedCount == 2) {
        if (destroyedTL && destroyedTR) return 5;
        if (destroyedTL && destroyedBL) return 6;
        if (destroyedTL && destroyedBR) return 7;
        if (destroyedTR && destroyedBL) return 8;
        if (destroyedTR && destroyedBR) return 9;
        if (destroyedBL && destroyedBR) return 10;
    }
    else if (destroyedCount == 3) {
        if (!destroyedTL) return 14;  // TL alive (TR+BL+BR destroyed) -> sprite 14
        if (!destroyedTR) return 13;  // TR alive (TL+BL+BR destroyed) -> sprite 13
        if (!destroyedBL) return 12;  // BL alive (TL+TR+BR destroyed) -> sprite 12
        if (!destroyedBR) return 11;  // BR alive (TL+TR+BL destroyed) -> sprite 11
    }

    // Fallback (shouldn't reach here if logic is correct)
    return 0;
}

void BrickWall::onRender(IRenderer& renderer) {
    if (isDestroyed()) return;

    // Render each alive corner as a 17x17 tile
    // Use slight overlap to prevent gaps between corners
    constexpr int HALF_SIZE = Constants::CELL_SIZE;  // 17

    // Render each alive corner from the full-brick tile (Sprites::Terrain::BRICK_X).
    // getSpriteIndex() is currently unused: per-corner rendering already
    // reflects the destruction state, so variant tiles are not required.
    int baseSrcX = Sprites::Terrain::BRICK_X;
    int baseSrcY = Sprites::Terrain::BRICK_Y;

    int baseX = static_cast<int>(position_.x);
    int destY = static_cast<int>(position_.y);

    // Render each corner that's still alive
    for (int i = 0; i < 4; ++i) {
        if (!cornerStates_[i]) continue;  // Skip destroyed corners

        int destX, destY_corner;
        switch (i) {
            case 0:  // Top-Left
                destX = baseX;
                destY_corner = destY;
                break;
            case 1:  // Top-Right
                destX = baseX + HALF_SIZE;
                destY_corner = destY;
                break;
            case 2:  // Bottom-Left
                destX = baseX;
                destY_corner = destY + HALF_SIZE;
                break;
            case 3:  // Bottom-Right
                destX = baseX + HALF_SIZE;
                destY_corner = destY + HALF_SIZE;
                break;
            default:
                continue;
        }

        // Draw the corner with slight overlap (render as 18x18 to fill gaps)
        // Source is still 17x17 from the sprite, but dest is 18x18
        renderer.drawSprite(baseSrcX, baseSrcY, HALF_SIZE, HALF_SIZE,
                           destX, destY_corner, HALF_SIZE + 1, HALF_SIZE + 1);
    }
}

} // namespace tank
