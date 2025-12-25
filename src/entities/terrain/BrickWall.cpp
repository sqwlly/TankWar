#include "entities/terrain/BrickWall.hpp"
#include "graphics/SpriteSheet.hpp"
#include <algorithm>
#include <cmath>

namespace tank {

BrickWall::BrickWall(const Vector2& position)
    : BrickWall(position, std::array<bool, 4>{true, true, true, true})
{
}

BrickWall::BrickWall(const Vector2& position, const std::array<bool, 4>& cornerStates)
    : Terrain(position, TerrainType::Brick)
    , cornerStates_(cornerStates)
{
    // Brick wall is a full element (2x2 half-cells), each corner is a half-cell (17px).
    width_ = Constants::ELEMENT_SIZE;
    height_ = Constants::ELEMENT_SIZE;
    initializeCorners();

    if (isDestroyed()) {
        active_ = false;
    }
}

void BrickWall::initializeCorners() {
    const float leftW = width_ / 2.0f;
    const float topH = height_ / 2.0f;

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
    // Convert corner states to sprite index
    // Matches original Java Tile.diversity() mapping
    int index = 0;
    for (int i = 0; i < 4; ++i) {
        if (cornerStates_[i]) {
            index |= (1 << i);
        }
    }

    // Map to sprite indices (from original Java code)
    static const int spriteMap[16] = {
        15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14
    };

    return spriteMap[index];
}

void BrickWall::onRender(IRenderer& renderer) {
    if (isDestroyed()) return;

    // Get sprite based on current state (like Java Tile.java)
    // The sprite sheet has 15 different brick states at row 5, starting col 4
    int spriteIndex = getSpriteIndex();

    // Source sprite: each brick state is 34x34, arranged horizontally
    int srcX = Sprites::Terrain::BRICK_X + spriteIndex * Sprites::ELEMENT_SIZE;
    int srcY = Sprites::Terrain::BRICK_Y;

    int destX = static_cast<int>(position_.x);
    int destY = static_cast<int>(position_.y);

    // Render at tile size to match collision bounds
    int destSize = static_cast<int>(width_);

    renderer.drawSprite(srcX, srcY, Sprites::ELEMENT_SIZE, Sprites::ELEMENT_SIZE,
                       destX, destY, destSize, destSize);
}

} // namespace tank
