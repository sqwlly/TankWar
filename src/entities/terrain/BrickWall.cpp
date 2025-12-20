#include "entities/terrain/BrickWall.hpp"
#include "graphics/SpriteSheet.hpp"
#include <algorithm>

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
    float halfW = width_ / 2.0f;
    float halfH = height_ / 2.0f;

    corners_[0] = Rectangle(position_.x, position_.y, halfW, halfH);              // Top-Left
    corners_[1] = Rectangle(position_.x + halfW, position_.y, halfW, halfH);      // Top-Right
    corners_[2] = Rectangle(position_.x, position_.y + halfH, halfW, halfH);      // Bottom-Left
    corners_[3] = Rectangle(position_.x + halfW, position_.y + halfH, halfW, halfH); // Bottom-Right
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

    // Render each remaining corner using sprite
    Rectangle brickSrc = Sprites::Terrain::getBrick(0);
    int srcX = static_cast<int>(brickSrc.x);
    int srcY = static_cast<int>(brickSrc.y);
    int srcSize = Sprites::TERRAIN_SIZE;
    int destSize = static_cast<int>(width_ / 2.0f);

    for (int i = 0; i < 4; ++i) {
        if (cornerStates_[i]) {
            int destX = static_cast<int>(corners_[i].x);
            int destY = static_cast<int>(corners_[i].y);
            renderer.drawSprite(srcX, srcY, srcSize, srcSize, destX, destY, destSize, destSize);
        }
    }
}

} // namespace tank
