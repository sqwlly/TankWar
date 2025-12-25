#include "entities/terrain/BrickWall.hpp"
#include "graphics/SpriteSheet.hpp"
#include <algorithm>
#include <cmath>

namespace tank {

BrickWall::BrickWall(const Vector2& position)
    : Terrain(position, TerrainType::Brick)
{
    // Brick wall is half cell size
    width_ = Constants::CELL_SIZE;
    height_ = Constants::CELL_SIZE;

    cornerStates_.fill(true);
    initializeCorners();
}

void BrickWall::initializeCorners() {
    // Split into 4 equal quadrants (float-based) to avoid uneven corner sizes (17 / 2 = 8.5).
    const float leftW = width_ / 2.0f;
    const float rightW = width_ - leftW;
    const float topH = height_ / 2.0f;
    const float bottomH = height_ - topH;

    corners_[0] = Rectangle(position_.x, position_.y, leftW, topH);                  // Top-Left
    corners_[1] = Rectangle(position_.x + leftW, position_.y, rightW, topH);         // Top-Right
    corners_[2] = Rectangle(position_.x, position_.y + topH, leftW, bottomH);        // Bottom-Left
    corners_[3] = Rectangle(position_.x + leftW, position_.y + topH, rightW, bottomH); // Bottom-Right
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
