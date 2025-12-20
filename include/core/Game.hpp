#pragma once

#include "states/GameStateManager.hpp"
#include "rendering/SDLRenderer.hpp"
#include "input/InputManager.hpp"
#include "utils/Constants.hpp"
#include <memory>
#include <string>

namespace tank {

/**
 * @brief Main game class - orchestrates all subsystems
 * Single Responsibility: Game lifecycle management
 */
class Game {
public:
    Game();
    ~Game();

    // Prevent copying
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    // Initialization
    bool initialize();
    void shutdown();

    // Main game loop
    void run();

    // State management
    GameStateManager& getStateManager() { return stateManager_; }

    // Quit game
    void quit() { running_ = false; }
    bool isRunning() const { return running_; }

private:
    bool running_ = false;

    // Core systems
    std::shared_ptr<SDLRenderer> renderer_;
    std::shared_ptr<InputManager> inputManager_;
    GameStateManager stateManager_;

    // Timing
    uint32_t previousTime_ = 0;
    float accumulator_ = 0.0f;

    // Game loop methods
    void processInput();
    void update(float deltaTime);
    void render();

    // Initialize subsystems
    bool initializeRenderer();
    bool initializeInput();
    bool initializeAudio();
    void initializeServices();

    // Load initial state
    void loadInitialState();
};

} // namespace tank
