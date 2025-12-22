#pragma once

#include "rendering/IRenderer.hpp"

namespace tank {
namespace test {

/**
 * @brief Mock renderer for testing without SDL dependencies
 */
class MockRenderer : public IRenderer {
public:
    bool initialize() override { return true; }
    void shutdown() override {}
    void clear() override {}
    void clear(int r, int g, int b, int a) override {}
    void present() override {}

    bool loadTexture(const std::string& path) override { return true; }
    void drawTexture(const std::string& path, int x, int y) override {}
    void drawTexture(const std::string& path, int srcX, int srcY, int srcW, int srcH,
                     int destX, int destY, int destW, int destH) override {}

    void drawSprite(int srcX, int srcY, int srcW, int srcH,
                    int destX, int destY, int destW, int destH) override {
        lastDrawCall_ = {srcX, srcY, srcW, srcH, destX, destY, destW, destH};
        ++drawCallCount_;
    }

    void drawRectangle(int x, int y, int w, int h, bool filled) override {}
    void drawText(const std::string& text, int x, int y, int fontSize) override {}

    int getWidth() const override { return 512; }
    int getHeight() const override { return 448; }

    // Test helpers
    struct DrawCall {
        int srcX, srcY, srcW, srcH;
        int destX, destY, destW, destH;
    };

    DrawCall getLastDrawCall() const { return lastDrawCall_; }
    int getDrawCallCount() const { return drawCallCount_; }
    void resetDrawCallCount() { drawCallCount_ = 0; }

private:
    DrawCall lastDrawCall_{};
    int drawCallCount_ = 0;
};

} // namespace test
} // namespace tank
