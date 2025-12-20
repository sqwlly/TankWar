#include "core/Game.hpp"
#include "core/ServiceLocator.hpp"
#include "states/MenuState.hpp"
#include <SDL2/SDL.h>
#include <iostream>

namespace tank {

Game::Game() = default;

Game::~Game() {
    shutdown();
}

bool Game::initialize() {
    std::cout << "Initializing Tank Game..." << std::endl;

    if (!initializeRenderer()) {
        return false;
    }

    if (!initializeInput()) {
        return false;
    }

    initializeServices();
    loadInitialState();

    running_ = true;
    previousTime_ = SDL_GetTicks();

    std::cout << "Game initialized successfully!" << std::endl;
    return true;
}

bool Game::initializeRenderer() {
    renderer_ = std::make_shared<SDLRenderer>();
    if (!renderer_->initialize(
            Constants::WINDOW_TITLE,
            Constants::WINDOW_WIDTH,
            Constants::WINDOW_HEIGHT)) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }

    // Load sprite sheet
    renderer_->setSpriteSheet(Constants::Paths::SPRITE_SHEET);
    std::cout << "Sprite sheet loaded: " << Constants::Paths::SPRITE_SHEET << std::endl;

    return true;
}

bool Game::initializeInput() {
    inputManager_ = std::make_shared<InputManager>();

    // Set up input callback for quit events only
    inputManager_->setEventCallback([this](const InputEvent& event) {
        if (event.type == InputEvent::Type::Quit) {
            quit();
        }
    });

    return true;
}

bool Game::initializeAudio() {
    // Audio initialization will be added in Phase 8
    return true;
}

void Game::initializeServices() {
    ServiceLocator::provide(renderer_);
    ServiceLocator::provide(inputManager_);
}

void Game::loadInitialState() {
    // Start with menu state
    stateManager_.pushState(std::make_unique<MenuState>(stateManager_));
}

void Game::shutdown() {
    running_ = false;

    // Clear state manager
    while (!stateManager_.isEmpty()) {
        stateManager_.popState();
        stateManager_.update(0); // Process pending pop
    }

    // Reset services
    ServiceLocator::reset();

    // Shutdown renderer
    if (renderer_) {
        renderer_->shutdown();
        renderer_.reset();
    }

    inputManager_.reset();

    std::cout << "Game shutdown complete" << std::endl;
}

void Game::run() {
    constexpr float FIXED_DELTA = Constants::FIXED_DELTA_TIME;
    constexpr float MAX_FRAME_TIME = 0.25f; // Prevent spiral of death

    while (running_) {
        uint32_t currentTime = SDL_GetTicks();
        float frameTime = (currentTime - previousTime_) / 1000.0f;
        previousTime_ = currentTime;

        // Clamp frame time to prevent spiral of death
        if (frameTime > MAX_FRAME_TIME) {
            frameTime = MAX_FRAME_TIME;
        }

        accumulator_ += frameTime;

        // Process input
        processInput();

        // Fixed time step updates
        while (accumulator_ >= FIXED_DELTA) {
            update(FIXED_DELTA);
            accumulator_ -= FIXED_DELTA;
        }

        // Render
        render();

        // Simple frame limiting (VSync should handle this, but just in case)
        uint32_t frameMs = SDL_GetTicks() - currentTime;
        if (frameMs < static_cast<uint32_t>(Constants::FRAME_TIME)) {
            SDL_Delay(static_cast<uint32_t>(Constants::FRAME_TIME) - frameMs);
        }
    }
}

void Game::processInput() {
    inputManager_->processEvents();
    stateManager_.handleInput(*inputManager_);
}

void Game::update(float deltaTime) {
    stateManager_.update(deltaTime);
    inputManager_->update();
}

void Game::render() {
    renderer_->clear();
    stateManager_.render(*renderer_);
    renderer_->present();
}

} // namespace tank
