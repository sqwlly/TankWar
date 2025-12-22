#pragma once

#include "utils/Rectangle.hpp"
#include "utils/Constants.hpp"

namespace tank {

/**
 * @brief Sprite sheet region definitions for tank_sprite.png
 *
 * Sprite sheet dimensions: 1088 x 442 pixels
 * Element size: 34 pixels (32 columns, 13 rows)
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
 *
 * Player 1 (Yellow): Row 0-3 (y=0), 8 frames per level × 4 levels = 32 frames
 * Player 2 (Green): Row 8 (y=272)
 *
 * Frame layout per level: UP1, UP2, DOWN1, DOWN2, LEFT1, LEFT2, RIGHT1, RIGHT2
 */
namespace Tank {
    // Player 1 base Y
    constexpr int P1_BASE_Y = 0;

    // Player 2 base Y (row 8)
    constexpr int P2_BASE_Y = 8 * ELEMENT_SIZE;  // 272

    // Enemy tanks start at row 2
    constexpr int ENEMY_BASIC_Y = 2 * ELEMENT_SIZE;   // 68
    constexpr int ENEMY_FAST_Y = 2 * ELEMENT_SIZE;    // Same row, different columns
    constexpr int ENEMY_POWER_Y = 2 * ELEMENT_SIZE;
    constexpr int ENEMY_ARMOR_Y = 0;  // Armored tanks at row 0, different x

    // Direction column offsets (2 frames each)
    constexpr int DIR_UP_COL = 0;
    constexpr int DIR_DOWN_COL = 2;
    constexpr int DIR_LEFT_COL = 4;
    constexpr int DIR_RIGHT_COL = 6;

    // Flashing enemy offset
    constexpr int FLASH_OFFSET_X = 24 * ELEMENT_SIZE;  // 816

    inline Rectangle getFrame(int baseY, int directionCol, int frame, int level = 0) {
        int x = (level * 8 + directionCol + frame) * ELEMENT_SIZE;
        int y = baseY;
        return Rectangle(static_cast<float>(x), static_cast<float>(y),
                        ELEMENT_SIZE, ELEMENT_SIZE);
    }
}

/**
 * @brief Terrain sprite regions
 *
 * Brick: (136, 170) - row 5, column 4
 * Steel: (0, 204) - row 6, column 0
 * Water: (0, 238) - row 7, column 0, 2 frames
 * Grass: (136, 238) - row 7, column 4
 */
namespace Terrain {
    // Brick wall
    constexpr int BRICK_X = 4 * ELEMENT_SIZE;   // 136
    constexpr int BRICK_Y = 5 * ELEMENT_SIZE;   // 170

    // Steel wall
    constexpr int STEEL_X = 0;
    constexpr int STEEL_Y = 6 * ELEMENT_SIZE;   // 204

    // Water (animated)
    constexpr int WATER_X = 0;
    constexpr int WATER_Y = 7 * ELEMENT_SIZE;   // 238

    // Grass
    constexpr int GRASS_X = 4 * ELEMENT_SIZE;   // 136
    constexpr int GRASS_Y = 7 * ELEMENT_SIZE;   // 238

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
        return Rectangle(static_cast<float>(GRASS_X),
                        static_cast<float>(GRASS_Y),
                        HALF_SIZE, HALF_SIZE);
    }

    inline Rectangle getIce() {
        // Ice is at same row as grass, next to it
        return Rectangle(static_cast<float>(GRASS_X + ELEMENT_SIZE),
                        static_cast<float>(GRASS_Y),
                        ELEMENT_SIZE, ELEMENT_SIZE);
    }
}

/**
 * @brief Base (Eagle) sprite
 * Position: (646, 170) - row 5, column 19
 */
namespace Base {
    constexpr int BASE_X = 19 * ELEMENT_SIZE;   // 646
    constexpr int BASE_Y = 5 * ELEMENT_SIZE;    // 170
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
 * @brief Bullet sprites
 * Position: (0, 170) - row 5, column 0
 * 4 directions in a 2x2 grid, each 10x10 pixels
 */
namespace Bullet {
    constexpr int BULLET_X = 0;
    constexpr int BULLET_Y = 5 * ELEMENT_SIZE;  // 170
    constexpr int SIZE = 10;

    inline Rectangle get(Direction dir) {
        // Bullets arranged: UP=0, DOWN=1, LEFT=2, RIGHT=3
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
 * @brief Power-up sprites
 * Position: (408, 204) - row 6, column 12
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
 * @brief Explosion animation frames
 * Small explosion: (680, 136) - row 4, column 20
 * Big explosion: (782, 136) - row 4, column 23
 */
namespace Explosion {
    constexpr int SMALL_X = 20 * ELEMENT_SIZE;  // 680
    constexpr int SMALL_Y = 4 * ELEMENT_SIZE;   // 136
    constexpr int BIG_X = 23 * ELEMENT_SIZE;    // 782
    constexpr int BIG_Y = 4 * ELEMENT_SIZE;     // 136

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
 * @brief Spawn animation (star effect)
 * Position: (544, 136) - row 4, column 16
 */
namespace Spawn {
    constexpr int SPAWN_X = 16 * ELEMENT_SIZE;  // 544
    constexpr int SPAWN_Y = 4 * ELEMENT_SIZE;   // 136

    inline Rectangle get(int frame) {
        return Rectangle(static_cast<float>(SPAWN_X + (frame % 4) * ELEMENT_SIZE),
                        static_cast<float>(SPAWN_Y),
                        ELEMENT_SIZE, ELEMENT_SIZE);
    }
}

/**
 * @brief Shield animation (invincibility)
 * Position: (442, 238) - row 7, column 13
 */
namespace Shield {
    constexpr int SHIELD_X = 13 * ELEMENT_SIZE;  // 442
    constexpr int SHIELD_Y = 7 * ELEMENT_SIZE;   // 238

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
    // Enemy icon: (34, 136) - row 4, column 1
    constexpr int ENEMY_ICON_X = 1 * ELEMENT_SIZE;   // 34
    constexpr int ENEMY_ICON_Y = 4 * ELEMENT_SIZE;   // 136
    constexpr int ICON_SIZE = ELEMENT_SIZE;

    // Player icon: (340, 204) - row 6, column 10
    constexpr int LIFE_ICON_X = 10 * ELEMENT_SIZE;   // 340
    constexpr int LIFE_ICON_Y = 6 * ELEMENT_SIZE;    // 204

    // Flag
    constexpr int FLAG_X = 11 * ELEMENT_SIZE;        // 374
    constexpr int FLAG_Y = 6 * ELEMENT_SIZE;         // 204
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
