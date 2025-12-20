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
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

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
