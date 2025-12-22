#include "rendering/SDLRenderer.hpp"
#include <iostream>

namespace tank {

SDLRenderer::~SDLRenderer() {
    shutdown();
}

bool SDLRenderer::initialize(const std::string& title, int width, int height) {
    width_ = width;
    height_ = height;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Initialize SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image initialization failed: " << IMG_GetError() << std::endl;
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() < 0) {
        std::cerr << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
        return false;
    }

    // Create window
    window_ = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );

    if (!window_) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create renderer
    renderer_ = SDL_CreateRenderer(
        window_,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer_) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Set default font path
    defaultFontPath_ = "assets/joystix.ttf";

    std::cout << "SDL Renderer initialized successfully" << std::endl;
    return true;
}

void SDLRenderer::shutdown() {
    clearFontCache();

    // Destroy sprite sheet texture
    if (spriteSheet_) {
        SDL_DestroyTexture(spriteSheet_);
        spriteSheet_ = nullptr;
    }

    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }

    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void SDLRenderer::clear() {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);
}

void SDLRenderer::present() {
    SDL_RenderPresent(renderer_);
}

void SDLRenderer::setDrawColor(const Constants::Color& color) {
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
}

void SDLRenderer::drawRectangle(const Rectangle& rect, const Constants::Color& color, bool filled) {
    SDL_Rect sdlRect = {
        static_cast<int>(rect.x),
        static_cast<int>(rect.y),
        static_cast<int>(rect.width),
        static_cast<int>(rect.height)
    };

    setDrawColor(color);

    if (filled) {
        SDL_RenderFillRect(renderer_, &sdlRect);
    } else {
        SDL_RenderDrawRect(renderer_, &sdlRect);
    }
}

SDL_Texture* SDLRenderer::loadTexture(const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "Failed to load image: " << path << " - " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    return texture;
}

void SDLRenderer::drawTexture(SDL_Texture* texture, const Rectangle& dest) {
    if (!texture) return;

    SDL_Rect destRect = {
        static_cast<int>(dest.x),
        static_cast<int>(dest.y),
        static_cast<int>(dest.width),
        static_cast<int>(dest.height)
    };

    SDL_RenderCopy(renderer_, texture, nullptr, &destRect);
}

void SDLRenderer::drawTexture(SDL_Texture* texture, const Rectangle& src, const Rectangle& dest) {
    if (!texture) return;

    SDL_Rect srcRect = {
        static_cast<int>(src.x),
        static_cast<int>(src.y),
        static_cast<int>(src.width),
        static_cast<int>(src.height)
    };

    SDL_Rect destRect = {
        static_cast<int>(dest.x),
        static_cast<int>(dest.y),
        static_cast<int>(dest.width),
        static_cast<int>(dest.height)
    };

    SDL_RenderCopy(renderer_, texture, &srcRect, &destRect);
}

void SDLRenderer::drawText(const std::string& text, const Vector2& pos,
                           const Constants::Color& color, int fontSize) {
    TTF_Font* font = getFont(fontSize);
    if (!font) return;

    SDL_Color sdlColor = {color.r, color.g, color.b, color.a};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), sdlColor);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    if (texture) {
        SDL_Rect destRect = {
            static_cast<int>(pos.x),
            static_cast<int>(pos.y),
            surface->w,
            surface->h
        };
        SDL_RenderCopy(renderer_, texture, nullptr, &destRect);
        SDL_DestroyTexture(texture);
    }

    SDL_FreeSurface(surface);
}

TTF_Font* SDLRenderer::getFont(int size) {
    auto it = fontCache_.find(size);
    if (it != fontCache_.end()) {
        return it->second;
    }

    TTF_Font* font = TTF_OpenFont(defaultFontPath_.c_str(), size);
    if (font) {
        fontCache_[size] = font;
    }
    return font;
}

void SDLRenderer::clearFontCache() {
    for (auto& [size, font] : fontCache_) {
        if (font) {
            TTF_CloseFont(font);
        }
    }
    fontCache_.clear();
}

void SDLRenderer::clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer_, r, g, b, a);
    SDL_RenderClear(renderer_);
}

void SDLRenderer::drawRect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer_, r, g, b, a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer_, &rect);
}

void SDLRenderer::drawSprite(int srcX, int srcY, int srcW, int srcH,
                             int destX, int destY, int destW, int destH) {
    if (!spriteSheet_) return;

    SDL_Rect srcRect = {srcX, srcY, srcW, srcH};
    SDL_Rect destRect = {destX, destY, destW, destH};
    SDL_RenderCopy(renderer_, spriteSheet_, &srcRect, &destRect);
}

void SDLRenderer::setSpriteSheet(const std::string& path) {
    if (spriteSheet_) {
        SDL_DestroyTexture(spriteSheet_);
    }
    spriteSheet_ = loadTexture(path);
}

} // namespace tank
