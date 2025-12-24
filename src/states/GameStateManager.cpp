#include "states/GameStateManager.hpp"
#include "states/MenuState.hpp"
#include "states/StageState.hpp"
#include "states/PlayingState.hpp"
#include "states/ScoreState.hpp"
#include "states/ConstructionState.hpp"
#include "input/IInput.hpp"
#include <iostream>

namespace tank {

void GameStateManager::pushState(std::unique_ptr<IGameState> state) {
    pendingPush_ = std::move(state);
}

void GameStateManager::popState() {
    pendingPop_ = true;
}

void GameStateManager::changeState(std::unique_ptr<IGameState> state) {
    pendingChange_ = std::move(state);
}

void GameStateManager::changeToMenu() {
    changeState(std::make_unique<MenuState>(*this));
}

void GameStateManager::changeToStage(int levelNumber, bool twoPlayer) {
    changeState(std::make_unique<StageState>(*this, levelNumber, twoPlayer));
}

void GameStateManager::changeToPlaying(int levelNumber, bool twoPlayer) {
    changeState(std::make_unique<PlayingState>(*this, levelNumber, twoPlayer, /*useWaveGenerator=*/true));
}

void GameStateManager::changeToScore(int levelNumber, bool victory) {
    changeState(std::make_unique<ScoreState>(*this, levelNumber, victory));
}

void GameStateManager::changeToConstruction(int levelNumber) {
    changeState(std::make_unique<ConstructionState>(*this, levelNumber));
}

void GameStateManager::processPendingOperations() {
    // Handle change state (pop current, push new)
    if (pendingChange_) {
        if (!states_.empty()) {
            states_.top()->exit();
            states_.pop();
        }
        pendingChange_->enter();
        states_.push(std::move(pendingChange_));
        pendingChange_ = nullptr;
        return;
    }

    // Handle pop
    if (pendingPop_) {
        if (!states_.empty()) {
            states_.top()->exit();
            states_.pop();
        }
        pendingPop_ = false;
    }

    // Handle push
    if (pendingPush_) {
        pendingPush_->enter();
        states_.push(std::move(pendingPush_));
        pendingPush_ = nullptr;
    }
}

void GameStateManager::update(float deltaTime) {
    processPendingOperations();

    if (!states_.empty()) {
        states_.top()->update(deltaTime);
    }
}

void GameStateManager::render(IRenderer& renderer) {
    if (!states_.empty()) {
        states_.top()->render(renderer);
    }
}

void GameStateManager::handleInput(const IInput& input) {
    if (!states_.empty()) {
        states_.top()->handleInput(input);
    }
}

IGameState* GameStateManager::getCurrentState() {
    if (states_.empty()) return nullptr;
    return states_.top().get();
}

const IGameState* GameStateManager::getCurrentState() const {
    if (states_.empty()) return nullptr;
    return states_.top().get();
}

} // namespace tank
