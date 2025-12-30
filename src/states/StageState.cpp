#include "states/StageState.hpp"
#include "states/GameStateManager.hpp"
#include "rendering/IRenderer.hpp"
#include "input/IInput.hpp"
#include "utils/Constants.hpp"
#include <cmath>

namespace tank {

StageState::StageState(GameStateManager& manager, int levelNumber, bool twoPlayer, bool useWaveGenerator)
    : stateManager_(manager)
    , levelNumber_(levelNumber)
    , twoPlayerMode_(twoPlayer)
    , useWaveGenerator_(useWaveGenerator)
    , displayTime_(0.0f)
    , curtainProgress_(0.0f)
    , textScale_(0.0f)
    , glowPulse_(0.0f)
    , readyToTransition_(false)
{
}

void StageState::enter() {
    displayTime_ = 0.0f;
    curtainProgress_ = 0.0f;
    textScale_ = 0.0f;
    glowPulse_ = 0.0f;
    readyToTransition_ = false;
}

void StageState::exit() {
    // Nothing to clean up
}

void StageState::update(float deltaTime) {
    displayTime_ += deltaTime;
    glowPulse_ += deltaTime * 4.0f;

    // Animate curtain opening
    if (curtainProgress_ < 1.0f) {
        curtainProgress_ += deltaTime * CURTAIN_SPEED;
        if (curtainProgress_ > 1.0f) curtainProgress_ = 1.0f;
    }

    // Animate text scale (pop in effect)
    if (curtainProgress_ > 0.3f && textScale_ < 1.0f) {
        textScale_ += deltaTime * TEXT_SCALE_SPEED;
        if (textScale_ > 1.0f) textScale_ = 1.0f;
    }

    // After display duration, transition to playing state
    if (displayTime_ >= DISPLAY_DURATION && !readyToTransition_) {
        readyToTransition_ = true;
        stateManager_.changeToPlaying(levelNumber_, twoPlayerMode_, useWaveGenerator_);
    }
}

void StageState::render(IRenderer& renderer) {
    // Dark background
    renderer.clear(
        Constants::UIColors::BG_DARK.r,
        Constants::UIColors::BG_DARK.g,
        Constants::UIColors::BG_DARK.b, 255);

    // Render stage info in center
    renderStageInfo(renderer);

    // Render player indicators at bottom
    renderPlayerIndicators(renderer);

    // Render curtain animation (on top)
    renderCurtains(renderer);
}

void StageState::renderCurtains(IRenderer& renderer) {
    int screenWidth = Constants::WINDOW_WIDTH;
    int screenHeight = Constants::WINDOW_HEIGHT;

    // Calculate curtain positions (open from center)
    float openAmount = curtainProgress_;
    int halfWidth = screenWidth / 2;

    // Easing function for smooth animation
    float easedOpen = 1.0f - std::pow(1.0f - openAmount, 3.0f);

    int leftCurtainX = static_cast<int>(-halfWidth * easedOpen);
    int rightCurtainX = static_cast<int>(halfWidth + halfWidth * easedOpen);

    // Left curtain
    renderer.drawRect(leftCurtainX, 0, halfWidth, screenHeight,
                     Constants::UIColors::STAGE_BG.r,
                     Constants::UIColors::STAGE_BG.g,
                     Constants::UIColors::STAGE_BG.b, 255);

    // Left curtain edge highlight
    renderer.drawRect(leftCurtainX + halfWidth - 4, 0, 4, screenHeight,
                     80, 80, 80, 255);

    // Right curtain
    renderer.drawRect(rightCurtainX, 0, halfWidth, screenHeight,
                     Constants::UIColors::STAGE_BG.r,
                     Constants::UIColors::STAGE_BG.g,
                     Constants::UIColors::STAGE_BG.b, 255);

    // Right curtain edge highlight
    renderer.drawRect(rightCurtainX, 0, 4, screenHeight,
                     80, 80, 80, 255);
}

void StageState::renderStageInfo(IRenderer& renderer) {
    int screenWidth = Constants::WINDOW_WIDTH;
    int screenHeight = Constants::WINDOW_HEIGHT;

    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;

    // Only render if curtain is partially open
    if (curtainProgress_ < 0.2f) return;

    // Stage text with scale animation
    float scale = textScale_;
    int textSize = static_cast<int>(28 * scale);
    if (textSize < 8) textSize = 8;

    // Glow effect behind text
    if (scale > 0.5f) {
        uint8_t glowAlpha = static_cast<uint8_t>((50 + 30 * std::sin(glowPulse_)) * scale);
        int glowSize = static_cast<int>(200 * scale);
        renderer.drawRect(centerX - glowSize / 2, centerY - 40, glowSize, 80,
                         Constants::UIColors::STAGE_ACCENT.r,
                         Constants::UIColors::STAGE_ACCENT.g,
                         Constants::UIColors::STAGE_ACCENT.b, glowAlpha);
    }

    // "STAGE" text
    std::string stageLabel = "STAGE";
    int labelWidth = static_cast<int>(80 * scale);
    renderer.drawText(stageLabel,
                     Vector2(static_cast<float>(centerX - labelWidth / 2), static_cast<float>(centerY - 30)),
                     Constants::UIColors::STAGE_TEXT, textSize);

    // Stage number (larger)
    std::string stageNum = std::to_string(levelNumber_);
    int numSize = static_cast<int>(48 * scale);
    if (numSize < 12) numSize = 12;

    // Number with accent color
    int numWidth = static_cast<int>(stageNum.length() * 20 * scale);
    renderer.drawText(stageNum,
                     Vector2(static_cast<float>(centerX - numWidth / 2), static_cast<float>(centerY + 5)),
                     Constants::UIColors::STAGE_ACCENT, numSize);

    // Decorative lines
    if (scale > 0.8f) {
        uint8_t lineAlpha = static_cast<uint8_t>(180 * (scale - 0.8f) * 5.0f);
        int lineWidth = static_cast<int>(120 * scale);

        // Top line
        renderer.drawRect(centerX - lineWidth / 2, centerY - 45, lineWidth, 2,
                         Constants::UIColors::STAGE_ACCENT.r,
                         Constants::UIColors::STAGE_ACCENT.g,
                         Constants::UIColors::STAGE_ACCENT.b, lineAlpha);

        // Bottom line
        renderer.drawRect(centerX - lineWidth / 2, centerY + 55, lineWidth, 2,
                         Constants::UIColors::STAGE_ACCENT.r,
                         Constants::UIColors::STAGE_ACCENT.g,
                         Constants::UIColors::STAGE_ACCENT.b, lineAlpha);
    }

    // Mode indicator (survival or campaign)
    if (scale > 0.9f) {
        const char* modeText = useWaveGenerator_ ? "SURVIVAL MODE" : "CAMPAIGN";
        uint8_t modeAlpha = static_cast<uint8_t>(150 * (scale - 0.9f) * 10.0f);
        renderer.drawText(modeText,
                         Vector2(static_cast<float>(centerX - 60), static_cast<float>(centerY + 70)),
                         Constants::Color(150, 150, 150, modeAlpha), 12);
    }
}

void StageState::renderPlayerIndicators(IRenderer& renderer) {
    if (textScale_ < 0.8f) return;

    int screenWidth = Constants::WINDOW_WIDTH;
    int screenHeight = Constants::WINDOW_HEIGHT;
    int y = screenHeight - 60;

    uint8_t alpha = static_cast<uint8_t>(255 * (textScale_ - 0.8f) * 5.0f);

    if (twoPlayerMode_) {
        // Player 1 indicator (left)
        renderer.drawRect(screenWidth / 4 - 40, y, 80, 30,
                         Constants::UIColors::PLAYER1.r,
                         Constants::UIColors::PLAYER1.g,
                         Constants::UIColors::PLAYER1.b, static_cast<uint8_t>(alpha / 4));
        renderer.drawText("P1 READY",
                         Vector2(static_cast<float>(screenWidth / 4 - 35), static_cast<float>(y + 8)),
                         Constants::Color(Constants::UIColors::PLAYER1.r,
                                         Constants::UIColors::PLAYER1.g,
                                         Constants::UIColors::PLAYER1.b, alpha), 14);

        // Player 2 indicator (right)
        renderer.drawRect(3 * screenWidth / 4 - 40, y, 80, 30,
                         Constants::UIColors::PLAYER2.r,
                         Constants::UIColors::PLAYER2.g,
                         Constants::UIColors::PLAYER2.b, static_cast<uint8_t>(alpha / 4));
        renderer.drawText("P2 READY",
                         Vector2(static_cast<float>(3 * screenWidth / 4 - 35), static_cast<float>(y + 8)),
                         Constants::Color(Constants::UIColors::PLAYER2.r,
                                         Constants::UIColors::PLAYER2.g,
                                         Constants::UIColors::PLAYER2.b, alpha), 14);
    } else {
        // Single player indicator (center)
        renderer.drawRect(screenWidth / 2 - 50, y, 100, 30,
                         Constants::UIColors::PLAYER1.r,
                         Constants::UIColors::PLAYER1.g,
                         Constants::UIColors::PLAYER1.b, static_cast<uint8_t>(alpha / 4));
        renderer.drawText("PLAYER 1",
                         Vector2(static_cast<float>(screenWidth / 2 - 40), static_cast<float>(y + 8)),
                         Constants::Color(Constants::UIColors::PLAYER1.r,
                                         Constants::UIColors::PLAYER1.g,
                                         Constants::UIColors::PLAYER1.b, alpha), 14);
    }

    // Skip hint
    if (displayTime_ > 0.5f) {
        uint8_t hintAlpha = static_cast<uint8_t>(80 + 40 * std::sin(glowPulse_ * 2.0f));
        renderer.drawText("PRESS ENTER TO START",
                         Vector2(static_cast<float>(screenWidth / 2 - 80), static_cast<float>(screenHeight - 25)),
                         Constants::Color(100, 100, 100, hintAlpha), 11);
    }
}

void StageState::handleInput(const IInput& input) {
    // Allow skipping with Enter or Space
    if (input.isKeyPressed(SDL_SCANCODE_RETURN) || input.isKeyPressed(SDL_SCANCODE_SPACE)) {
        if (!readyToTransition_) {
            readyToTransition_ = true;
            stateManager_.changeToPlaying(levelNumber_, twoPlayerMode_, useWaveGenerator_);
        }
    }
}

} // namespace tank
