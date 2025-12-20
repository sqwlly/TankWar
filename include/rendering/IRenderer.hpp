#pragma once

#include "utils/Rectangle.hpp"
#include "utils/Constants.hpp"
#include <string>
#include <memory>

struct SDL_Texture;

namespace tank {

/**
 * @brief Abstract renderer interface (DIP - Dependency Inversion)
 * High-level modules depend on this interface, not concrete SDL implementation
 */
class IRenderer {
public:
    virtual ~IRenderer() = default;

    // Lifecycle
    virtual bool initialize(const std::string& title, int width, int height) = 0;
    virtual void shutdown() = 0;

    // Frame operations
    virtual void clear() = 0;
    virtual void present() = 0;

    // Drawing primitives
    virtual void drawRectangle(const Rectangle& rect, const Constants::Color& color, bool filled = true) = 0;

    // Texture operations
    virtual SDL_Texture* loadTexture(const std::string& path) = 0;
    virtual void drawTexture(SDL_Texture* texture, const Rectangle& dest) = 0;
    virtual void drawTexture(SDL_Texture* texture, const Rectangle& src, const Rectangle& dest) = 0;

    // Text rendering
    virtual void drawText(const std::string& text, const Vector2& pos,
                          const Constants::Color& color, int fontSize = 16) = 0;

    // Window access
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;

    // Set draw color
    virtual void setDrawColor(const Constants::Color& color) = 0;

    // Convenience methods with raw color values
    virtual void clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a) = 0;
    virtual void drawRect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) = 0;

    // Sprite rendering from texture atlas
    virtual void drawSprite(int srcX, int srcY, int srcW, int srcH,
                           int destX, int destY, int destW, int destH) = 0;

    // Texture management
    virtual void setSpriteSheet(const std::string& path) = 0;
};

} // namespace tank
