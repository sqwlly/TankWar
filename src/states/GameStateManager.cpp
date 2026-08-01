#include "states/GameStateManager.hpp"
#include "states/MenuState.hpp"
#include "states/StageState.hpp"
#include "states/PlayingState.hpp"
#include "states/ScoreState.hpp"
#include "states/ConstructionState.hpp"
#include "core/ServiceLocator.hpp"
#include "input/IInput.hpp"
#include <algorithm>
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
    saveProgress();  // Run ended: flush deferred high-score writes
    changeState(std::make_unique<MenuState>(*this));
}

void GameStateManager::changeToStage(int levelNumber, bool twoPlayer, bool useWaveGenerator) {
    saveProgress();  // Stage boundary: flush deferred high-score writes
    changeState(std::make_unique<StageState>(*this, levelNumber, twoPlayer, useWaveGenerator));
}

void GameStateManager::changeToPlaying(int levelNumber, bool twoPlayer, bool useWaveGenerator) {
    changeState(std::make_unique<PlayingState>(*this, levelNumber, twoPlayer, useWaveGenerator));
}

void GameStateManager::changeToScore(int levelNumber, bool victory, bool twoPlayer, bool useWaveGenerator) {
    auto scoreState = std::make_unique<ScoreState>(*this, levelNumber, victory, twoPlayer, useWaveGenerator);
    scoreState->setKillCounts(enemyKillCounts_[0], enemyKillCounts_[1],
                              enemyKillCounts_[2], enemyKillCounts_[3]);
    scoreState->setTotalScore(player1Score_ + player2Score_);
    // Counts are per stage; start the next stage with a clean tally.
    enemyKillCounts_.fill(0);
    changeState(std::move(scoreState));
}

void GameStateManager::changeToConstruction(int levelNumber) {
    changeState(std::make_unique<ConstructionState>(*this, levelNumber));
}

void GameStateManager::beginRun() {
    resetPlayerLevels();
    player1Score_ = 0;
    player2Score_ = 0;
    enemyKillCounts_.fill(0);
}

void GameStateManager::addPlayerScore(int playerId, int points) {
    if (points <= 0) {
        return;
    }

    int& score = playerId == 2 ? player2Score_ : player1Score_;
    score += points;
    recordHighScore(player1Score_ + player2Score_);
}

int GameStateManager::getPlayerScore(int playerId) const {
    return playerId == 2 ? player2Score_ : player1Score_;
}

void GameStateManager::recordEnemyKill(EnemyType type) {
    const int index = static_cast<int>(type);
    if (index >= 0 && index < static_cast<int>(enemyKillCounts_.size())) {
        ++enemyKillCounts_[index];
    }
}

int GameStateManager::getEnemyKillCount(EnemyType type) const {
    const int index = static_cast<int>(type);
    if (index < 0 || index >= static_cast<int>(enemyKillCounts_.size())) {
        return 0;
    }
    return enemyKillCounts_[index];
}

bool GameStateManager::loadProgress(const std::string& filePath) {
    progressStore_ = ProgressStore(filePath);
    progress_ = progressStore_.load();
    progressLoaded_ = true;
    if (ServiceLocator::hasAudio()) {
        ServiceLocator::getAudio().setMasterVolume(progress_.masterVolume);
    }
    return true;
}

void GameStateManager::recordHighScore(int score) {
    if (score <= progress_.highScore) {
        return;
    }
    // In-memory only; persisted at stage/run boundaries to avoid a file
    // write on every kill during a record run.
    progress_.highScore = score;
}

void GameStateManager::unlockCampaignLevel(int completedLevel) {
    const int nextLevel = std::clamp(completedLevel + 1, 1, Constants::MAX_LEVEL);
    if (nextLevel <= progress_.levelToPlay) {
        return;
    }
    progress_.levelToPlay = nextLevel;
    saveProgress();
}

void GameStateManager::setDifficulty(GameDifficulty difficulty) {
    progress_.difficulty = static_cast<GameDifficulty>(std::clamp(static_cast<int>(difficulty), 0, 2));
    saveProgress();
}

void GameStateManager::setMasterVolume(float volume) {
    progress_.masterVolume = std::clamp(volume, 0.0f, 1.0f);
    if (ServiceLocator::hasAudio()) {
        ServiceLocator::getAudio().setMasterVolume(progress_.masterVolume);
    }
    saveProgress();
}

void GameStateManager::saveProgress() {
    if (progressLoaded_) {
        progressStore_.save(progress_);
    }
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
