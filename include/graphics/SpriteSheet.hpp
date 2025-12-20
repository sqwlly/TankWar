#pragma once

#include "utils/Rectangle.hpp"
#include "utils/Constants.hpp"

namespace tank {

/**
 * @brief Sprite sheet region definitions for tank_sprite.png
 * Standard Battle City sprite layout: 16x16 tanks, 8x8 terrain blocks
 */
namespace Sprites {

// Sprite dimensions
constexpr int TANK_SIZE = 16;
constexpr int TERRAIN_SIZE = 8;
constexpr int BULLET_SIZE = 8;
constexpr int POWERUP_SIZE = 16;
constexpr int EXPLOSION_SIZE = 16;
constexpr int BIG_EXPLOSION_SIZE = 32;

// Scale factor for rendering (original is 16x16, we render at 32x32)
constexpr int SCALE = 2;

/**
 * @brief Tank sprite regions
 * Layout: 8 columns per direction (2 animation frames x 4 upgrade levels)
 * Rows: Up, Left, Down, Right for each tank type
 */
namespace Tank {
    // Player 1 (Yellow) - rows 0-3
    constexpr int P1_ROW_UP = 0;
    constexpr int P1_ROW_LEFT = 0;
    constexpr int P1_ROW_DOWN = 0;
    constexpr int P1_ROW_RIGHT = 0;

    // Direction offsets within a row (each direction has 2 frames)
    constexpr int DIR_UP_COL = 0;
    constexpr int DIR_LEFT_COL = 2;
    constexpr int DIR_DOWN_COL = 4;
    constexpr int DIR_RIGHT_COL = 6;

    // Player 1 tank rows (4 rows for 4 upgrade levels)
    constexpr int P1_BASE_Y = 0;

    // Player 2 (Green) - rows 8-11
    constexpr int P2_BASE_Y = 128;  // 8 * 16

    // Enemy tanks start at row 4
    constexpr int ENEMY_BASE_Y = 64;  // 4 * 16

    // Enemy types (each has 4 rows for directions)
    constexpr int ENEMY_BASIC_Y = 64;
    constexpr int ENEMY_FAST_Y = 128;
    constexpr int ENEMY_POWER_Y = 192;
    constexpr int ENEMY_ARMOR_Y = 256;

    // Flashing enemy (carries powerup) - add 128 to X
    constexpr int FLASH_OFFSET_X = 128;

    inline Rectangle getFrame(int baseY, int directionCol, int frame, int level = 0) {
        int x = (directionCol + frame) * TANK_SIZE + (level * 128);
        int y = baseY;
        return Rectangle(static_cast<float>(x), static_cast<float>(y),
                        TANK_SIZE, TANK_SIZE);
    }
}

/**
 * @brief Terrain sprite regions
 * 8x8 pixel terrain blocks
 */
namespace Terrain {
    // Terrain Y position in sprite sheet
    constexpr int TERRAIN_Y = 256;

    // Brick wall (4 variants for damage states)
    constexpr int BRICK_X = 256;
    constexpr int BRICK_Y = 64;

    // Steel wall
    constexpr int STEEL_X = 256;
    constexpr int STEEL_Y = 72;

    // Water (animated, 2 frames)
    constexpr int WATER_X = 256;
    constexpr int WATER_Y = 80;

    // Grass (renders on top)
    constexpr int GRASS_X = 264;
    constexpr int GRASS_Y = 72;

    // Ice
    constexpr int ICE_X = 272;
    constexpr int ICE_Y = 72;

    inline Rectangle getBrick(int variant = 0) {
        return Rectangle(BRICK_X + variant * TERRAIN_SIZE, BRICK_Y,
                        TERRAIN_SIZE, TERRAIN_SIZE);
    }

    inline Rectangle getSteel() {
        return Rectangle(STEEL_X, STEEL_Y, TERRAIN_SIZE, TERRAIN_SIZE);
    }

    inline Rectangle getWater(int frame) {
        return Rectangle(WATER_X + frame * TERRAIN_SIZE, WATER_Y,
                        TERRAIN_SIZE, TERRAIN_SIZE);
    }

    inline Rectangle getGrass() {
        return Rectangle(GRASS_X, GRASS_Y, TERRAIN_SIZE, TERRAIN_SIZE);
    }

    inline Rectangle getIce() {
        return Rectangle(ICE_X, ICE_Y, TERRAIN_SIZE, TERRAIN_SIZE);
    }
}

/**
 * @brief Base (Eagle) sprite
 */
namespace Base {
    constexpr int BASE_X = 304;
    constexpr int BASE_Y = 64;
    constexpr int BASE_SIZE = 16;

    // Normal base
    inline Rectangle getNormal() {
        return Rectangle(BASE_X, BASE_Y, BASE_SIZE, BASE_SIZE);
    }

    // Destroyed base
    inline Rectangle getDestroyed() {
        return Rectangle(BASE_X + BASE_SIZE, BASE_Y, BASE_SIZE, BASE_SIZE);
    }
}

/**
 * @brief Bullet sprites (4 directions)
 */
namespace Bullet {
    constexpr int BULLET_X = 320;
    constexpr int BULLET_Y = 100;

    inline Rectangle get(Direction dir) {
        int offset = 0;
        switch (dir) {
            case Direction::Up:    offset = 0; break;
            case Direction::Left:  offset = 1; break;
            case Direction::Down:  offset = 2; break;
            case Direction::Right: offset = 3; break;
        }
        return Rectangle(BULLET_X + offset * BULLET_SIZE, BULLET_Y,
                        BULLET_SIZE, BULLET_SIZE);
    }
}

/**
 * @brief Power-up sprites
 */
namespace PowerUp {
    constexpr int POWERUP_X = 256;
    constexpr int POWERUP_Y = 112;

    // Power-up types in order
    inline Rectangle get(PowerUpType type) {
        int index = static_cast<int>(type);
        return Rectangle(POWERUP_X + index * POWERUP_SIZE, POWERUP_Y,
                        POWERUP_SIZE, POWERUP_SIZE);
    }
}

/**
 * @brief Explosion animation frames
 */
namespace Explosion {
    constexpr int SMALL_X = 256;
    constexpr int SMALL_Y = 128;
    constexpr int BIG_X = 304;
    constexpr int BIG_Y = 128;

    // Small explosion (bullet hit) - 3 frames
    inline Rectangle getSmall(int frame) {
        return Rectangle(SMALL_X + frame * EXPLOSION_SIZE, SMALL_Y,
                        EXPLOSION_SIZE, EXPLOSION_SIZE);
    }

    // Big explosion (tank destroyed) - 2 frames
    inline Rectangle getBig(int frame) {
        return Rectangle(BIG_X + frame * BIG_EXPLOSION_SIZE, BIG_Y,
                        BIG_EXPLOSION_SIZE, BIG_EXPLOSION_SIZE);
    }
}

/**
 * @brief Spawn animation (star effect)
 */
namespace Spawn {
    constexpr int SPAWN_X = 256;
    constexpr int SPAWN_Y = 96;

    inline Rectangle get(int frame) {
        return Rectangle(SPAWN_X + frame * POWERUP_SIZE, SPAWN_Y,
                        POWERUP_SIZE, POWERUP_SIZE);
    }
}

/**
 * @brief Shield animation (invincibility)
 */
namespace Shield {
    constexpr int SHIELD_X = 256;
    constexpr int SHIELD_Y = 144;

    inline Rectangle get(int frame) {
        return Rectangle(SHIELD_X + (frame % 2) * POWERUP_SIZE, SHIELD_Y,
                        POWERUP_SIZE, POWERUP_SIZE);
    }
}

/**
 * @brief UI elements
 */
namespace UI {
    // Enemy icon for HUD
    constexpr int ENEMY_ICON_X = 320;
    constexpr int ENEMY_ICON_Y = 192;
    constexpr int ICON_SIZE = 8;

    // Player life icon
    constexpr int LIFE_ICON_X = 320;
    constexpr int LIFE_ICON_Y = 200;

    // Flag icon
    constexpr int FLAG_X = 336;
    constexpr int FLAG_Y = 184;
    constexpr int FLAG_W = 16;
    constexpr int FLAG_H = 16;

    inline Rectangle getEnemyIcon() {
        return Rectangle(ENEMY_ICON_X, ENEMY_ICON_Y, ICON_SIZE, ICON_SIZE);
    }

    inline Rectangle getLifeIcon() {
        return Rectangle(LIFE_ICON_X, LIFE_ICON_Y, ICON_SIZE, ICON_SIZE);
    }

    inline Rectangle getFlag() {
        return Rectangle(FLAG_X, FLAG_Y, FLAG_W, FLAG_H);
    }
}

} // namespace Sprites

} // namespace tank
