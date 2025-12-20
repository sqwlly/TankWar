#include "graphics/Animation.hpp"
#include "rendering/IRenderer.hpp"
#include <algorithm>

namespace tank {

// SpriteSheet implementation
SpriteSheet::SpriteSheet(int sheetWidth, int sheetHeight, int spriteWidth, int spriteHeight)
    : sheetWidth_(sheetWidth)
    , sheetHeight_(sheetHeight)
    , spriteWidth_(spriteWidth)
    , spriteHeight_(spriteHeight)
{
    spritesPerRow_ = sheetWidth_ / spriteWidth_;
    int rows = sheetHeight_ / spriteHeight_;
    spriteCount_ = spritesPerRow_ * rows;
}

Frame SpriteSheet::getSprite(int index) const {
    index = index % spriteCount_;
    int x = (index % spritesPerRow_) * spriteWidth_;
    int y = (index / spritesPerRow_) * spriteHeight_;
    return Frame(x, y, spriteWidth_, spriteHeight_, 0);
}

// Animation implementation
Animation::Animation()
    : currentFrame_(0)
    , frameDelay_(100)
    , elapsedTime_(0.0f)
    , timesPlayed_(0)
    , stopped_(true)
{
}

Animation::Animation(const std::vector<Frame>& frames, int frameDelay)
    : frames_(frames)
    , currentFrame_(0)
    , frameDelay_(frameDelay)
    , elapsedTime_(0.0f)
    , timesPlayed_(0)
    , stopped_(true)
{
}

void Animation::addFrame(const Frame& frame) {
    frames_.push_back(frame);
}

void Animation::start() {
    if (!stopped_ || frames_.empty()) return;
    stopped_ = false;
}

void Animation::stop() {
    if (frames_.empty()) return;
    stopped_ = true;
}

void Animation::restart() {
    if (frames_.empty()) return;
    stopped_ = false;
    currentFrame_ = 0;
    elapsedTime_ = 0.0f;
}

void Animation::reset() {
    stopped_ = true;
    currentFrame_ = 0;
    elapsedTime_ = 0.0f;
    timesPlayed_ = 0;
}

void Animation::update(float deltaTime) {
    if (stopped_ || frames_.empty()) return;

    elapsedTime_ += deltaTime * 1000.0f;  // Convert to ms

    if (elapsedTime_ >= frameDelay_) {
        elapsedTime_ -= frameDelay_;
        ++currentFrame_;

        if (currentFrame_ >= static_cast<int>(frames_.size())) {
            currentFrame_ = 0;
            ++timesPlayed_;
        }
    }
}

const Frame& Animation::getCurrentFrame() const {
    static Frame emptyFrame(0, 0, 0, 0, 0);
    if (frames_.empty()) return emptyFrame;
    return frames_[currentFrame_];
}

// Sprite implementation
Sprite::Sprite()
    : srcX_(0), srcY_(0), width_(0), height_(0)
{
}

Sprite::Sprite(int srcX, int srcY, int width, int height)
    : srcX_(srcX), srcY_(srcY), width_(width), height_(height)
{
}

void Sprite::setSource(int srcX, int srcY, int width, int height) {
    srcX_ = srcX;
    srcY_ = srcY;
    width_ = width;
    height_ = height;
}

void Sprite::render(IRenderer& renderer, int x, int y) const {
    renderer.drawSprite(srcX_, srcY_, width_, height_, x, y, width_, height_);
}

void Sprite::render(IRenderer& renderer, int x, int y, int w, int h) const {
    renderer.drawSprite(srcX_, srcY_, width_, height_, x, y, w, h);
}

} // namespace tank
