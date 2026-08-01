#pragma once

#include "rendering/IRenderer.hpp"
#include <vector>

namespace tank {
namespace test {

/**
 * @brief Mock renderer for testing without SDL dependencies
 */
class MockRenderer : public IRenderer {
public:
    bool initialize(const std::string&, int, int) override { return true; }
    void shutdown() override {}
    void clear() override {}
    void clear(uint8_t, uint8_t, uint8_t, uint8_t) override {}
    void present() override {}

    ::SDL_Texture* loadTexture(const std::string&) override { return nullptr; }
    void drawTexture(::SDL_Texture*, const Rectangle&) override {}
    void drawTexture(::SDL_Texture*, const Rectangle&, const Rectangle&) override {}

    void drawSprite(int srcX, int srcY, int srcW, int srcH,
                    int destX, int destY, int destW, int destH) override {
        lastDrawCall_ = {srcX, srcY, srcW, srcH, destX, destY, destW, destH};
        ++drawCallCount_;
    }

    void drawRectangle(const Rectangle&, const Constants::Color&, bool) override {}
    void drawText(const std::string&, const Vector2&, const Constants::Color&, int) override {}
    Vector2 measureText(const std::string&, int fontSize) override {
        return Vector2(0.0f, static_cast<float>(fontSize));
    }

    void setDrawColor(const Constants::Color&) override {}
    void drawRect(int x, int y, int w, int h,
                  uint8_t r, uint8_t g, uint8_t b, uint8_t a) override {
        rectCalls_.push_back({x, y, w, h, r, g, b, a});
    }
    void setSpriteSheet(const std::string&) override {}

    int getWidth() const override { return 512; }
    int getHeight() const override { return 448; }

    // Test helpers
    struct DrawCall {
        int srcX, srcY, srcW, srcH;
        int destX, destY, destW, destH;
    };

    struct RectCall {
        int x, y, w, h;
        uint8_t r, g, b, a;
    };

    DrawCall getLastDrawCall() const { return lastDrawCall_; }
    int getDrawCallCount() const { return drawCallCount_; }
    void resetDrawCallCount() { drawCallCount_ = 0; }
    const std::vector<RectCall>& getRectCalls() const { return rectCalls_; }

private:
    DrawCall lastDrawCall_{};
    int drawCallCount_ = 0;
    std::vector<RectCall> rectCalls_;
};

} // namespace test
} // namespace tank
