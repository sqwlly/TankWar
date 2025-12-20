#pragma once

#include <vector>
#include <cstdint>

namespace tank {

struct SDL_Texture;
class IRenderer;

/**
 * @brief Single frame of animation
 */
struct Frame {
    int srcX;       // Source X in sprite sheet
    int srcY;       // Source Y in sprite sheet
    int width;      // Frame width
    int height;     // Frame height
    int duration;   // Frame duration in milliseconds

    Frame(int x, int y, int w, int h, int dur)
        : srcX(x), srcY(y), width(w), height(h), duration(dur) {}
};

/**
 * @brief Sprite sheet for managing texture atlases
 */
class SpriteSheet {
public:
    SpriteSheet(int sheetWidth, int sheetHeight, int spriteWidth, int spriteHeight);

    Frame getSprite(int index) const;
    int getSpriteCount() const { return spriteCount_; }
    int getSpriteWidth() const { return spriteWidth_; }
    int getSpriteHeight() const { return spriteHeight_; }

private:
    int sheetWidth_;
    int sheetHeight_;
    int spriteWidth_;
    int spriteHeight_;
    int spritesPerRow_;
    int spriteCount_;
};

/**
 * @brief Animation controller for sprite-based animations
 */
class Animation {
public:
    Animation();
    Animation(const std::vector<Frame>& frames, int frameDelay);

    void addFrame(const Frame& frame);
    void setFrameDelay(int delayMs) { frameDelay_ = delayMs; }

    void start();
    void stop();
    void restart();
    void reset();

    void update(float deltaTime);

    const Frame& getCurrentFrame() const;
    int getCurrentFrameIndex() const { return currentFrame_; }
    bool isStopped() const { return stopped_; }
    bool hasPlayedOnce() const { return timesPlayed_ > 0; }
    bool hasPlayed(int times) const { return timesPlayed_ >= times; }
    int getTimesPlayed() const { return timesPlayed_; }

private:
    std::vector<Frame> frames_;
    int currentFrame_;
    int frameDelay_;        // Delay between frames in ms
    float elapsedTime_;     // Time accumulated
    int timesPlayed_;
    bool stopped_;
};

/**
 * @brief Static sprite (single frame from sprite sheet)
 */
class Sprite {
public:
    Sprite();
    Sprite(int srcX, int srcY, int width, int height);

    void setSource(int srcX, int srcY, int width, int height);
    void render(IRenderer& renderer, int x, int y) const;
    void render(IRenderer& renderer, int x, int y, int w, int h) const;

    int getSrcX() const { return srcX_; }
    int getSrcY() const { return srcY_; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }

private:
    int srcX_;
    int srcY_;
    int width_;
    int height_;
};

} // namespace tank
