#pragma once

#include "utils/Rectangle.hpp"
#include "utils/Constants.hpp"

namespace tank {

/**
 * @brief Sprite sheet region definitions for tank_sprite.png
 *
 * Sprite sheet: 1088 x 442 pixels, 34px elements (32 cols x 13 rows)
 *
 * Coordinates from original Java source code:
 * Row 0-3 (y=0-135):   Player 1 tanks (yellow, 4 upgrade levels)
 * Row 4 (y=136):       UI elements (spawn, explosions, game over)
 * Row 5 (y=170):       Brick tiles (cols 4-18), Base (cols 19-20), Bullets (col 0-3)
 * Row 6 (y=204):       Steel (cols 0-3), Power-ups, player icons
 * Row 7 (y=238):       Water (cols 0-1), Grass (col 4), Shield (cols 13-15)
 * Row 8+ (y=272+):     Player 2 and Enemy tanks
 */
namespace Sprites {

constexpr int ELEMENT_SIZE = 34;
constexpr int HALF_SIZE = 17;

constexpr int TANK_SIZE = ELEMENT_SIZE;
constexpr int TERRAIN_SIZE = HALF_SIZE;
constexpr int BULLET_SIZE = 10;
constexpr int POWERUP_SIZE = ELEMENT_SIZE;
constexpr int EXPLOSION_SIZE = ELEMENT_SIZE;
constexpr int BIG_EXPLOSION_SIZE = ELEMENT_SIZE * 2;

namespace Tank {
    constexpr int P1_BASE_Y = 0;
    constexpr int P2_BASE_Y = 8 * ELEMENT_SIZE;  // 272

    constexpr int ENEMY_BASIC_Y = 8 * ELEMENT_SIZE;   // Row 8 - enemy tanks
    constexpr int ENEMY_FAST_Y = 8 * ELEMENT_SIZE;
    constexpr int ENEMY_POWER_Y = 8 * ELEMENT_SIZE;
    constexpr int ENEMY_ARMOR_Y = 8 * ELEMENT_SIZE;

    constexpr int DIR_UP_COL = 0;
    constexpr int DIR_DOWN_COL = 2;
    constexpr int DIR_LEFT_COL = 4;
    constexpr int DIR_RIGHT_COL = 6;

    constexpr int FLASH_OFFSET_X = 16 * ELEMENT_SIZE;  // 544

    inline Rectangle getFrame(int baseY, int directionCol, int frame, int level = 0) {
        int x = (level * 8 + directionCol + frame) * ELEMENT_SIZE;
        return Rectangle(static_cast<float>(x), static_cast<float>(baseY),
                        ELEMENT_SIZE, ELEMENT_SIZE);
    }
}

/**
 * @brief Terrain sprites - Coordinates from Java source
 *
 * Tile.java:   cut(4 * 34, 5 * 34, ...) -> Brick at (136, 170)
 * Steel.java:  cut(0, 6 * 34, ...)      -> Steel at (0, 204)
 * Water.java:  cut(0, 7 * 34, ...)      -> Water at (0, 238)
 * Grass.java:  cut(4 * 34, 7 * 34, ...) -> Grass at (136, 238)
 */
namespace Terrain {
    // Brick wall - row 5, starting col 4 (from Tile.java)
    constexpr int BRICK_X = 4 * ELEMENT_SIZE;   // 136
    constexpr int BRICK_Y = 5 * ELEMENT_SIZE;   // 170

    // Steel wall - row 6, col 0 (from Steel.java)
    constexpr int STEEL_X = 0;
    constexpr int STEEL_Y = 6 * ELEMENT_SIZE;   // 204

    // Water - row 7, cols 0-1 (from Water.java, 2 frames for animation)
    constexpr int WATER_X = 0;
    constexpr int WATER_Y = 7 * ELEMENT_SIZE;   // 238

    // Grass - row 7, col 4 (from Grass.java)
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
        // Ice is next to grass
        return Rectangle(static_cast<float>(GRASS_X + ELEMENT_SIZE),
                        static_cast<float>(GRASS_Y),
                        ELEMENT_SIZE, ELEMENT_SIZE);
    }
}

/**
 * @brief Base (Eagle) sprite - from Home.java
 * cut(19 * 34, 5 * 34, ...) -> (646, 170)
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
 * @brief Bullet sprites - from Bullet.java
 * cut(0, 5 * 34, ...) -> (0, 170)
 */
namespace Bullet {
    constexpr int BULLET_X = 0;
    constexpr int BULLET_Y = 5 * ELEMENT_SIZE;  // 170
    constexpr int SIZE = 8;

    inline Rectangle get(Direction dir) {
        int index = 0;
        switch (dir) {
            case Direction::Up:    index = 0; break;
            case Direction::Left:  index = 1; break;
            case Direction::Down:  index = 2; break;
            case Direction::Right: index = 3; break;
        }
        // Bullets are small, adjust offset within the element
        return Rectangle(static_cast<float>(index * SIZE + 4),
                        static_cast<float>(BULLET_Y + 4),
                        SIZE, SIZE);
    }
}

namespace PowerUp {
    constexpr int POWERUP_X = 7 * ELEMENT_SIZE;  // 238
    constexpr int POWERUP_Y = 6 * ELEMENT_SIZE;  // 204

    inline Rectangle get(PowerUpType type) {
        int index = static_cast<int>(type);
        return Rectangle(static_cast<float>(POWERUP_X + index * ELEMENT_SIZE),
                        static_cast<float>(POWERUP_Y),
                        ELEMENT_SIZE, ELEMENT_SIZE);
    }
}

/**
 * @brief Explosion sprites - from BulletBoom.java and TankBoom.java
 * BulletBoom: cut(20 * 34, 4 * 34, ...) -> (680, 136)
 * TankBoom:   cut(23 * 34, 4 * 34, ...) -> (782, 136)
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
 * @brief Spawn animation - from Born.java
 * cut(16 * 34, 4 * 34, ...) -> (544, 136)
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
 * @brief Shield/Invincible effect - from Invincible.java
 * cut(13 * 34, 7 * 34, ...) -> (442, 238)
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

namespace UI {
    // Enemy icon - from EnemyIcon.java: cut(1 * 34, 4 * 34, ...)
    constexpr int ENEMY_ICON_X = 1 * ELEMENT_SIZE;   // 34
    constexpr int ENEMY_ICON_Y = 4 * ELEMENT_SIZE;   // 136
    constexpr int ICON_SIZE = ELEMENT_SIZE;

    // Player icon - from PlayerIcon.java: cut(10 * 34, 6 * 34, ...)
    constexpr int LIFE_ICON_X = 10 * ELEMENT_SIZE;   // 340
    constexpr int LIFE_ICON_Y = 6 * ELEMENT_SIZE;    // 204

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
