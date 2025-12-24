#include "states/ScoreState.hpp"
#include "states/GameStateManager.hpp"
#include "rendering/IRenderer.hpp"
#include "input/IInput.hpp"
#include "utils/Constants.hpp"

namespace tank {

ScoreState::ScoreState(GameStateManager& manager, int levelNumber, bool victory)
    : stateManager_(manager)
    , levelNumber_(levelNumber)
    , victory_(victory)
    , killCounts_({0, 0, 0, 0})
    , displayedKills_({0, 0, 0, 0})
    , totalKills_(0)
    , displayedTotal_(0)
    , totalScore_(0)
    , highScore_(0)
    , animationTimer_(0.0f)
    , currentRow_(0)
    , animationComplete_(false)
{
}

void ScoreState::enter() {
    animationTimer_ = 0.0f;
    currentRow_ = 0;
    animationComplete_ = false;
    displayedKills_ = {0, 0, 0, 0};
    displayedTotal_ = 0;

    // Calculate total kills
    totalKills_ = 0;
    for (int i = 0; i < 4; ++i) {
        totalKills_ += killCounts_[i];
    }
}

void ScoreState::exit() {
    // Nothing to clean up
}

void ScoreState::setKillCounts(int type1, int type2, int type3, int type4) {
    killCounts_[0] = type1;
    killCounts_[1] = type2;
    killCounts_[2] = type3;
    killCounts_[3] = type4;
}

void ScoreState::update(float deltaTime) {
    if (animationComplete_) return;

    animationTimer_ += deltaTime;

    if (animationTimer_ >= TICK_INTERVAL) {
        animationTimer_ = 0.0f;

        // Animate current row
        if (currentRow_ < 4) {
            if (displayedKills_[currentRow_] < killCounts_[currentRow_]) {
                displayedKills_[currentRow_]++;
                // Play tick sound here if audio implemented
            } else {
                // Move to next row
                ++currentRow_;
            }
        } else {
            // Animate total
            if (displayedTotal_ < totalKills_) {
                displayedTotal_++;
            } else {
                animationComplete_ = true;
            }
        }
    }
}

void ScoreState::render(IRenderer& renderer) {
    // Black background
    renderer.clear(0, 0, 0, 255);

    int screenWidth = renderer.getWidth();
    int baseX = 60;
    int baseY = 40;

    // Draw "STAGE X" header
    std::string stageText = "STAGE " + std::to_string(levelNumber_);
    renderer.drawText(stageText, Vector2(static_cast<float>(screenWidth / 2 - 60), static_cast<float>(baseY)),
                     Constants::COLOR_WHITE, 20);

    // Draw "I-PLAYER" label
    renderer.drawText("I-PLAYER", Vector2(static_cast<float>(baseX), static_cast<float>(baseY + 50)),
                     Constants::Color(255, 0, 0), 20);

    // Draw total score
    std::string scoreText = std::to_string(totalScore_);
    renderer.drawText(scoreText, Vector2(static_cast<float>(baseX + 50), static_cast<float>(baseY + 80)),
                     Constants::Color(255, 200, 0), 20);

    // Draw each enemy type row
    for (int i = 0; i < 4; ++i) {
        int rowY = baseY + 110 + i * 45;
        renderRow(renderer, i, rowY);
    }

    // Draw separator line
    int lineY = baseY + 290;
    renderer.drawRect(baseX, lineY, screenWidth - baseX * 2, 3, 255, 255, 255, 255);

    // Draw total
    std::string totalText = "TOTAL  " + std::to_string(displayedTotal_);
    renderer.drawText(totalText, Vector2(static_cast<float>(baseX + 100), static_cast<float>(lineY + 20)),
                     Constants::COLOR_WHITE, 18);

    // Draw continue prompt
    if (animationComplete_) {
        renderer.drawText("Press ENTER to continue",
                         Vector2(static_cast<float>(baseX + 50), static_cast<float>(lineY + 60)),
                         Constants::COLOR_GRAY, 12);
    }
}

void ScoreState::renderRow(IRenderer& renderer, int row, int y) {
    int baseX = 60;

    // Points earned
    int points = displayedKills_[row] * POINTS_PER_TYPE[row];
    std::string pointsText = std::to_string(points);
    renderer.drawText(pointsText, Vector2(static_cast<float>(baseX + 50), static_cast<float>(y)),
                     Constants::COLOR_WHITE, 16);

    // "PTS" label
    renderer.drawText("PTS", Vector2(static_cast<float>(baseX + 130), static_cast<float>(y)),
                     Constants::COLOR_WHITE, 16);

    // Kill count
    std::string killText = std::to_string(displayedKills_[row]);
    renderer.drawText(killText, Vector2(static_cast<float>(baseX + 200), static_cast<float>(y)),
                     Constants::COLOR_WHITE, 16);

    // Arrow symbol
    renderer.drawText("<", Vector2(static_cast<float>(baseX + 230), static_cast<float>(y)),
                     Constants::COLOR_WHITE, 16);

    // Tank type indicator (colored rectangle as placeholder)
    int tankX = baseX + 260;
    Constants::Color tankColor(150, 150, 150);  // Default gray
    switch (row) {
        case 0: tankColor = Constants::Color(150, 150, 150); break;  // Basic - gray
        case 1: tankColor = Constants::Color(100, 200, 100); break;  // Fast - green
        case 2: tankColor = Constants::Color(200, 100, 100); break;  // Power - red
        case 3: tankColor = Constants::Color(200, 200, 100); break;  // Heavy - yellow
    }
    renderer.drawRect(tankX, y - 10, 24, 24,
                     tankColor.r, tankColor.g, tankColor.b, 255);
}

void ScoreState::handleInput(const IInput& input) {
    if (animationComplete_) {
        if (input.isKeyPressed(SDL_SCANCODE_RETURN) || input.isKeyPressed(SDL_SCANCODE_SPACE)) {
            if (victory_) {
                // Go to next level
                stateManager_.changeToStage(levelNumber_ + 1);
            } else {
                // Game over - return to menu
                stateManager_.changeToMenu();
            }
        }
    } else {
        // Skip animation
        if (input.isKeyPressed(SDL_SCANCODE_RETURN) || input.isKeyPressed(SDL_SCANCODE_SPACE)) {
            // Complete animation instantly
            for (int i = 0; i < 4; ++i) {
                displayedKills_[i] = killCounts_[i];
            }
            displayedTotal_ = totalKills_;
            animationComplete_ = true;
        }
    }
}

} // namespace tank
