#pragma once

#include <cstdint>
#include <string>
#include "utils/Vector2.hpp"

namespace tank {

/**
 * @brief Game constants and configuration
 * Following the original Java game's values
 */
namespace Constants {

// Window settings
constexpr int WINDOW_WIDTH = 512;
constexpr int WINDOW_HEIGHT = 448;
constexpr const char* WINDOW_TITLE = "Tank Battle";

// Game area (excluding UI panel)
constexpr int GAME_WIDTH = 442;    // 26 * 17 = 442
constexpr int GAME_HEIGHT = 442;   // 26 * 17 = 442

// UI panel on the right
constexpr int UI_PANEL_WIDTH = WINDOW_WIDTH - GAME_WIDTH;

// Grid settings (26x26 cells, each 17 pixels)
constexpr int GRID_WIDTH = 26;
constexpr int GRID_HEIGHT = 26;
constexpr int CELL_SIZE = 17;
constexpr int ELEMENT_SIZE = 34;  // 2 cells = 1 element (tank size)

// Timing
constexpr float TARGET_FPS = 60.0f;
constexpr float FRAME_TIME = 1000.0f / TARGET_FPS;
constexpr float FIXED_DELTA_TIME = 1.0f / TARGET_FPS;

// Game timing (in milliseconds, from original Java)
constexpr int ENEMY_SPAWN_INTERVAL = 3500;
constexpr int POWERUP_SPAWN_INTERVAL = 13000;
constexpr int PLAYER_RESPAWN_DELAY = 1200;
constexpr int STOP_EFFECT_DURATION = 7000;
constexpr int INVINCIBLE_DURATION = 3000;

// Gameplay
constexpr int MAX_ENEMIES_ON_SCREEN = 4;  // Per player
constexpr int INITIAL_PLAYER_LIVES = 3;
constexpr int TOTAL_ENEMIES_PER_LEVEL = 20;
constexpr int MAX_LEVEL = 20;

// Tank stats
constexpr int PLAYER_DEFAULT_HP = 50;
constexpr int PLAYER_MAX_HP = 100;
constexpr int PLAYER_DEFAULT_SPEED = 2;
constexpr int PLAYER_DEFAULT_DEFENSE = 0;

constexpr int ENEMY_TYPE1_HP = 50;   // Basic
constexpr int ENEMY_TYPE2_HP = 50;   // Fast
constexpr int ENEMY_TYPE3_HP = 50;   // Shooter
constexpr int ENEMY_TYPE4_HP = 100;  // Heavy

// Bullet stats
constexpr int BULLET_DEFAULT_SPEED = 5;
constexpr int BULLET_DEFAULT_ATTACK = 50;

// Scoring
constexpr int ENEMY_REWARD[] = {100, 200, 300, 400};
constexpr int POWERUP_SCORE = 500;

// Animation
constexpr int SPAWN_ANIMATION_FRAMES = 4;
constexpr int SPAWN_ANIMATION_DELAY = 100;
constexpr int TANK_ANIMATION_DELAY = 50;
constexpr int EXPLOSION_ANIMATION_DELAY = 80;

// Colors (RGBA)
struct Color {
    uint8_t r, g, b, a;

    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : r(r), g(g), b(b), a(a) {}
};

constexpr Color COLOR_BLACK{0, 0, 0};
constexpr Color COLOR_WHITE{255, 255, 255};
constexpr Color COLOR_GRAY{99, 99, 99};      // #636363 from original
constexpr Color COLOR_RED{255, 0, 0};
constexpr Color COLOR_GREEN{0, 255, 0};

// Asset paths
namespace Paths {
    constexpr const char* ASSETS = "assets/";
    constexpr const char* IMAGES = "assets/images/";
    constexpr const char* AUDIO = "assets/audio/";
    constexpr const char* LEVELS = "assets/levels/";

    constexpr const char* SPRITE_SHEET = "assets/images/tank_sprite.png";
    constexpr const char* TEXTURE_ATLAS = "assets/images/texture_atlas.png";
    constexpr const char* TITLE_IMAGE = "assets/images/title.png";
}

} // namespace Constants

/**
 * @brief Direction enumeration
 */
enum class Direction {
    Up = 0,
    Right = 1,
    Down = 2,
    Left = 3
};

/**
 * @brief Get the opposite direction
 */
constexpr Direction oppositeDirection(Direction dir) {
    switch (dir) {
        case Direction::Up: return Direction::Down;
        case Direction::Down: return Direction::Up;
        case Direction::Left: return Direction::Right;
        case Direction::Right: return Direction::Left;
    }
    return dir;
}

/**
 * @brief Get direction vector
 */
constexpr Vector2 directionToVector(Direction dir) {
    switch (dir) {
        case Direction::Up: return {0, -1};
        case Direction::Down: return {0, 1};
        case Direction::Left: return {-1, 0};
        case Direction::Right: return {1, 0};
    }
    return {0, 0};
}

/**
 * @brief Game state types
 */
enum class StateType {
    Menu,
    Stage,      // Level transition
    Playing,
    Score,      // Score screen
    Construction // Level editor
};

/**
 * @brief Power-up types
 */
enum class PowerUpType {
    Star,       // Upgrade tank
    StopWatch,  // Freeze enemies
    Gun,        // Max level
    IronCap,    // Invincibility
    Bomb,       // Destroy all enemies
    Spade,      // Steel wall around base
    Tank        // Extra life
};

/**
 * @brief Enemy types
 */
enum class EnemyType {
    Basic = 0,  // Normal
    Fast = 1,   // Faster speed
    Power = 2,  // Higher level bullets
    Heavy = 3   // More HP
};

/**
 * @brief Terrain types for level data
 */
enum class TerrainType {
    Empty = 0,
    Steel = 1,
    Brick = 2,
    Water = 3,
    Grass = 4,
    Base = 5
};

/**
 * @brief Sound effect IDs
 */
enum class SoundId {
    BulletShot,
    BulletHitSteel,
    Explosion,
    PlayerMove,
    EnemyMove,
    GetBonus,
    BonusLife,
    Star,
    GameOver,
    StageStart,
    Pause
};

/**
 * @brief Render layers for proper draw order
 */
enum class RenderLayer {
    Background = 0,
    Water = 1,
    Terrain = 2,
    Base = 3,
    Tanks = 4,
    Bullets = 5,
    Effects = 6,
    Grass = 7,      // Grass renders on top of tanks
    PowerUps = 8,
    UI = 9
};

} // namespace tank
