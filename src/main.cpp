/**
 * @file main.cpp
 * @brief Tank Battle - C++ Edition
 *
 * A classic tank battle game implemented in C++ with SDL2
 * following SOLID principles.
 *
 * Original Java version by: [Original Author]
 * C++ rewrite following SOLID principles
 */

#include "core/Game.hpp"
#include <SDL.h>  // Required for SDL_main handling on Windows
#include <exception>
#include <filesystem>
#include <iostream>
#include <system_error>

namespace {

bool hasRuntimeAssets(const std::filesystem::path& root) {
    std::error_code error;
    const bool hasFont = std::filesystem::is_regular_file(root / "assets" / "joystix.ttf", error);
    if (error || !hasFont) {
        return false;
    }

    error.clear();
    return std::filesystem::is_regular_file(
        root / "assets" / "images" / "tank_sprite.png", error) && !error;
}

void configureRuntimeWorkingDirectory() {
    std::error_code error;
    const std::filesystem::path currentDirectory = std::filesystem::current_path(error);
    if (!error && hasRuntimeAssets(currentDirectory)) {
        return;
    }

    char* basePath = SDL_GetBasePath();
    if (!basePath) {
        std::cerr << "Unable to locate the executable directory: " << SDL_GetError() << std::endl;
        return;
    }

    const std::filesystem::path executableDirectory = std::filesystem::u8path(basePath);
    SDL_free(basePath);

    if (!hasRuntimeAssets(executableDirectory)) {
        std::cerr << "Runtime assets were not found beside the executable: "
                  << executableDirectory << std::endl;
        return;
    }

    std::filesystem::current_path(executableDirectory, error);
    if (error) {
        std::cerr << "Unable to use the executable directory for runtime assets: "
                  << error.message() << std::endl;
    }
}

} // namespace

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    configureRuntimeWorkingDirectory();

    std::cout << "==================================" << std::endl;
    std::cout << "  Tank Battle - C++ Edition" << std::endl;
    std::cout << "  Built with SDL2 + SOLID" << std::endl;
    std::cout << "==================================" << std::endl;

    try {
        tank::Game game;

        if (!game.initialize()) {
            std::cerr << "Failed to initialize game" << std::endl;
            return 1;
        }

        game.run();

        std::cout << "Game exited normally" << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 1;
    }
}
