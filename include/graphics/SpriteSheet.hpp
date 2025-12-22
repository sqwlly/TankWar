#pragma once

#include "utils/Rectangle.hpp"
#include "utils/Constants.hpp"

namespace tank {

/**
 * @brief Sprite sheet region definitions for tank_sprite.png
 *
 * Sprite sheet dimensions: 1088 x 442 pixels
 * Element size: 34 pixels (32 columns, 13 rows)
 *
 * Layout analysis from sprite image:
 * Row 0-3 (y=0-135): Player 1 tanks (yellow), 4 upgrade levels
 * Row 4 (y=136): UI elements (star, IP, GAME OVER, buttons, explosions)
 * Row 5 (y=170): Bullets, spawn effects, some icons
 * Row 6 (y=204): Water, steel wall, power-ups
 * Row 7 (y=238): Brick walls, grass
 * Row 8+ (y=272+): Enemy tanks (red/gray)
 */
namespace Sprites {

// Base element size
constexpr int ELEMENT_SIZE = 34;
constexpr int HALF_SIZE = 17;  // For terrain blocks

// Legacy compatibility
constexpr int TANK_SIZE = ELEMENT_SIZE;
constexpr int TERRAIN_SIZE = HALF_SIZE;
constexpr int BULLET_SIZE = 10;
constexpr int POWERUP_SIZE = ELEMENT_SIZE;
constexpr int EXPLOSION_SIZE = ELEMENT_SIZE;
constexpr int BIG_EXPLOSION_SIZE = ELEMENT_SIZE * 2;

/**
 * @brief Tank sprite regions
 */
namespace Tank {
    constexpr int P1_BASE_Y = 0;
    constexpr int P2_BASE_Y = 8 * ELEMENT_SIZE;  // 272

    // Enemy tanks at row 2 (y=68)
    constexpr int ENEMY_BASIC_Y = 2 * ELEMENT_SIZE;
    constexpr int ENEMY_FAST_Y = 2 * ELEMENT_SIZE;
    constexpr int ENEMY_POWER_Y = 2 * ELEMENT_SIZE;
    constexpr int ENEMY_ARMOR_Y = 0;

    constexpr int DIR_UP_COL = 0;
    constexpr int DIR_DOWN_COL = 2;
    constexpr int DIR_LEFT_COL = 4;
    constexpr int DIR_RIGHT_COL = 6;

    constexpr int FLASH_OFFSET_X = 24 * ELEMENT_SIZE;

    inline Rectangle getFrame(int baseY, int directionCol, int frame, int level = 0) {
        int x = (level * 8 + directionCol + frame) * ELEMENT_SIZE;
        return Rectangle(static_cast<float>(x), static_cast<float>(baseY),
                        ELEMENT_SIZE, ELEMENT_SIZE);
    }
}

/**
 * @brief Terrain sprite regions
 * Based on visual analysis of sprite sheet:
 * - Row 6 (y=204): Water (blue), Steel (gray)
 * - Row 7 (y=238): Brick (orange/brown), Grass (green)
 */
namespace Terrain {
    // Brick wall - row 7, first few columns (orange blocks)
    constexpr int BRICK_X = 0;
    constexpr int BRICK_Y = 7 * ELEMENT_SIZE;   // 238

    // Steel wall - row 6, after water (gray blocks)
    constexpr int STEEL_X = 2 * ELEMENT_SIZE;   // 68
    constexpr int STEEL_Y = 6 * ELEMENT_SIZE;   // 204

    // Water - row 6, first columns (blue animated)
    constexpr int WATER_X = 0;
    constexpr int WATER_Y = 6 * ELEMENT_SIZE;   // 204

    // Grass - row 7, after brick variants (green)
    constexpr int GRASS_X = 0;
    constexpr int GRASS_Y = 5 * ELEMENT_SIZE;   // 170 (small green block)

    inline Rectangle getBrick(int variant = 0) {
        return Rectangle(static_cast<float>(BRICK_X + variant * ELEMENT_SIZE),
                        static_cast<float>(BRICK_Y),
                        ELEMENT_SIZE, ELEMENT_SIZE);
    }

    inline Rectangle getSteel() {
        return Rectangle(static_cast<float>(STEEL_X),
                        static_cast<float>(STEEL_Y),
                        ELEMENT_SIZE, ELEMENT_SIZE);
    }

    inline Rectangle getWater(int frame) {
        return Rectangle(static_cast<float>(WATER_X + (frame % 2) * ELEMENT_SIZE),
                        static_cast<float>(WATER_Y),
                        ELEMENT_SIZE, ELEMENT_SIZE);
    }

    inline Rectangle getGrass() {
        // Grass is a smaller block (17x17)
        return Rectangle(static_cast<float>(GRASS_X + 2 * ELEMENT_SIZE),
                        static_cast<float>(GRASS_Y),
                        HALF_SIZE, HALF_SIZE);
    }

    inline Rectangle getIce() {
        return Rectangle(static_cast<float>(GRASS_X + 3 * ELEMENT_SIZE),
                        static_cast<float>(GRASS_Y),
                        ELEMENT_SIZE, ELEMENT_SIZE);
    }
}

/**
 * @brief Base (Eagle/Home) sprite
 * Located in row 5 area with other game elements
 */
namespace Base {
    // Base/Eagle - need to find exact position in sprite
    // Typically near terrain elements
    constexpr int BASE_X = 19 * ELEMENT_SIZE;   // 646
    constexpr int BASE_Y = 6 * ELEMENT_SIZE;    // 204 (row 6)
    constexpr int BASE_SIZE = ELEMENT_SIZE;

    inline Rectangle getNormal() {
        return Rectangle(static_cast<float>(BASE_X), static_cast<float>(BASE_Y),
                        BASE_SIZE, BASE_SIZE);
    }

    inline Rectangle getDestroyed() {
        return Rectangle(static_cast<float>(BASE_X + BASE_SIZE), static_cast<float>(BASE_Y),
                        BASE_SIZE, BASE_SIZE);
    }
}

/**
 * @brief Bullet sprites - row 5, first element (small 10x10)
 */
namespace Bullet {
    constexpr int BULLET_X = 0;
    constexpr int BULLET_Y = 5 * ELEMENT_SIZE;  // 170
    constexpr int SIZE = 10;

    inline Rectangle get(Direction dir) {
        int index = 0;
        switch (dir) {
            case Direction::Up:    index = 0; break;
            case Direction::Down:  index = 1; break;
            case Direction::Left:  index = 2; break;
            case Direction::Right: index = 3; break;
        }
        int row = index / 2;
        int col = index % 2;
        return Rectangle(static_cast<float>(BULLET_X + col * SIZE),
                        static_cast<float>(BULLET_Y + row * SIZE),
                        SIZE, SIZE);
    }
}

/**
 * @brief Power-up sprites - row 6
 */
namespace PowerUp {
    constexpr int POWERUP_X = 12 * ELEMENT_SIZE;  // 408
    constexpr int POWERUP_Y = 6 * ELEMENT_SIZE;   // 204

    inline Rectangle get(PowerUpType type) {
        int index = static_cast<int>(type);
        return Rectangle(static_cast<float>(POWERUP_X + index * ELEMENT_SIZE),
                        static_cast<float>(POWERUP_Y),
                        ELEMENT_SIZE, ELEMENT_SIZE);
    }
}

/**
 * @brief Explosion animation - row 4
 */
namespace Explosion {
    constexpr int SMALL_X = 20 * ELEMENT_SIZE;
    constexpr int SMALL_Y = 4 * ELEMENT_SIZE;   // 136
    constexpr int BIG_X = 23 * ELEMENT_SIZE;
    constexpr int BIG_Y = 4 * ELEMENT_SIZE;

    inline Rectangle getSmall(int frame) {
        return Rectangle(static_cast<float>(SMALL_X + (frame % 3) * ELEMENT_SIZE),
                        static_cast<float>(SMALL_Y),
                        ELEMENT_SIZE, ELEMENT_SIZE);
    }

    inline Rectangle getBig(int frame) {
        return Rectangle(static_cast<float>(BIG_X + (frame % 2) * BIG_EXPLOSION_SIZE),
                        static_cast<float>(BIG_Y),
                        BIG_EXPLOSION_SIZE, BIG_EXPLOSION_SIZE);
    }
}

/**
 * @brief Spawn animation - row 4
 */
namespace Spawn {
    constexpr int SPAWN_X = 16 * ELEMENT_SIZE;
    constexpr int SPAWN_Y = 4 * ELEMENT_SIZE;   // 136

    inline Rectangle get(int frame) {
        return Rectangle(static_cast<float>(SPAWN_X + (frame % 4) * ELEMENT_SIZE),
                        static_cast<float>(SPAWN_Y),
                        ELEMENT_SIZE, ELEMENT_SIZE);
    }
}

/**
 * @brief Shield animation - row 5
 */
namespace Shield {
    constexpr int SHIELD_X = 13 * ELEMENT_SIZE;
    constexpr int SHIELD_Y = 5 * ELEMENT_SIZE;  // 170

    inline Rectangle get(int frame) {
        return Rectangle(static_cast<float>(SHIELD_X + (frame % 2) * ELEMENT_SIZE),
                        static_cast<float>(SHIELD_Y),
                        ELEMENT_SIZE, ELEMENT_SIZE);
    }
}

/**
 * @brief UI elements
 */
namespace UI {
    constexpr int ENEMY_ICON_X = 1 * ELEMENT_SIZE;
    constexpr int ENEMY_ICON_Y = 4 * ELEMENT_SIZE;
    constexpr int ICON_SIZE = ELEMENT_SIZE;

    constexpr int LIFE_ICON_X = 10 * ELEMENT_SIZE;
    constexpr int LIFE_ICON_Y = 6 * ELEMENT_SIZE;

    constexpr int FLAG_X = 11 * ELEMENT_SIZE;
    constexpr int FLAG_Y = 6 * ELEMENT_SIZE;
    constexpr int FLAG_W = ELEMENT_SIZE;
    constexpr int FLAG_H = ELEMENT_SIZE;

    inline Rectangle getEnemyIcon() {
        return Rectangle(static_cast<float>(ENEMY_ICON_X),
                        static_cast<float>(ENEMY_ICON_Y),
                        ICON_SIZE, ICON_SIZE);
    }

    inline Rectangle getLifeIcon() {
        return Rectangle(static_cast<float>(LIFE_ICON_X),
                        static_cast<float>(LIFE_ICON_Y),
                        ICON_SIZE, ICON_SIZE);
    }

    inline Rectangle getFlag() {
        return Rectangle(static_cast<float>(FLAG_X),
                        static_cast<float>(FLAG_Y),
                        FLAG_W, FLAG_H);
    }
}

} // namespace Sprites

} // namespace tank
