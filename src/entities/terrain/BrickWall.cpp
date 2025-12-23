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
    // For 17x17 brick, split into 4 corners with proper coverage
    // Left corners: width = ceil(17/2) = 9, Right corners: width = floor(17/2) = 8
    // Top corners: height = ceil(17/2) = 9, Bottom corners: height = floor(17/2) = 8
    float leftW = std::ceil(width_ / 2.0f);
    float rightW = std::floor(width_ / 2.0f);
    float topH = std::ceil(height_ / 2.0f);
    float bottomH = std::floor(height_ / 2.0f);

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

    // Java renders at ELEMENT_SIZE to match sprite exactly
    int destSize = Sprites::ELEMENT_SIZE;

    renderer.drawSprite(srcX, srcY, Sprites::ELEMENT_SIZE, Sprites::ELEMENT_SIZE,
                       destX, destY, destSize, destSize);
}

} // namespace tank
