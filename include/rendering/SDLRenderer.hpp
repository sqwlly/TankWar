#pragma once

#include "rendering/IRenderer.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <unordered_map>
#include <string>
#include <memory>

namespace tank {

/**
 * @brief SDL2 implementation of IRenderer
 * Handles all SDL2-specific rendering operations
 */
class SDLRenderer : public IRenderer {
public:
    SDLRenderer() = default;
    ~SDLRenderer() override;

    // IRenderer implementation
    bool initialize(const std::string& title, int width, int height) override;
    void shutdown() override;

    void clear() override;
    void present() override;

    void drawRectangle(const Rectangle& rect, const Constants::Color& color,
                       bool filled = true) override;

    SDL_Texture* loadTexture(const std::string& path) override;
    void drawTexture(SDL_Texture* texture, const Rectangle& dest) override;
    void drawTexture(SDL_Texture* texture, const Rectangle& src,
                     const Rectangle& dest) override;

    void drawText(const std::string& text, const Vector2& pos,
                  const Constants::Color& color, int fontSize = 16) override;

    int getWidth() const override { return width_; }
    int getHeight() const override { return height_; }

    void setDrawColor(const Constants::Color& color) override;

    // Convenience methods
    void clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;
    void drawRect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;

    // Sprite rendering
    void drawSprite(int srcX, int srcY, int srcW, int srcH,
                   int destX, int destY, int destW, int destH) override;
    void setSpriteSheet(const std::string& path) override;

    // SDL-specific accessors
    SDL_Renderer* getSDLRenderer() { return renderer_; }
    SDL_Window* getSDLWindow() { return window_; }

private:
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    SDL_Texture* spriteSheet_ = nullptr;
    int width_ = 0;
    int height_ = 0;

    // Font cache
    std::unordered_map<int, TTF_Font*> fontCache_;
    std::string defaultFontPath_;

    TTF_Font* getFont(int size);
    void clearFontCache();
};

} // namespace tank
