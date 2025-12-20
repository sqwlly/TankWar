#include "states/StageState.hpp"
#include "states/GameStateManager.hpp"
#include "rendering/IRenderer.hpp"
#include "input/InputManager.hpp"
#include "utils/Constants.hpp"

namespace tank {

StageState::StageState(GameStateManager& manager, int levelNumber)
    : stateManager_(manager)
    , levelNumber_(levelNumber)
    , displayTime_(0.0f)
    , waitingForInput_(false)
{
}

void StageState::enter() {
    displayTime_ = 0.0f;
    waitingForInput_ = false;
}

void StageState::exit() {
    // Nothing to clean up
}

void StageState::update(float deltaTime) {
    displayTime_ += deltaTime;

    // After display duration, transition to playing state
    if (displayTime_ >= DISPLAY_DURATION && !waitingForInput_) {
        stateManager_.changeToPlaying(levelNumber_, false);
    }
}

void StageState::render(IRenderer& renderer) {
    // Gray background (like original #535353)
    renderer.clear(83, 83, 83, 255);

    // Draw "STAGE X" text centered
    int screenWidth = renderer.getWidth();
    int screenHeight = renderer.getHeight();

    // Black text background box
    int boxWidth = 200;
    int boxHeight = 40;
    int boxX = (screenWidth - boxWidth) / 2;
    int boxY = (screenHeight - boxHeight) / 2;

    // Draw stage number
    std::string stageText = "STAGE " + std::to_string(levelNumber_);

    renderer.drawText(
        stageText,
        Vector2(static_cast<float>(boxX + 20), static_cast<float>(boxY + 10)),
        Constants::COLOR_BLACK,
        24
    );
}

void StageState::handleInput(const InputManager& input) {
    // Allow skipping with Enter or Space
    if (input.isKeyPressed(SDL_SCANCODE_RETURN) || input.isKeyPressed(SDL_SCANCODE_SPACE)) {
        stateManager_.changeToPlaying(levelNumber_, false);
    }
}

} // namespace tank
