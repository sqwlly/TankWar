#include "states/ScoreState.hpp"
#include "states/GameStateManager.hpp"
#include "graphics/SpriteSheet.hpp"
#include "level/LevelLoader.hpp"
#include "rendering/IRenderer.hpp"
#include "input/IInput.hpp"
#include "utils/Constants.hpp"

namespace tank {

ScoreState::ScoreState(GameStateManager& manager, int levelNumber, bool victory,
                       bool twoPlayer, bool useWaveGenerator)
    : stateManager_(manager)
    , levelNumber_(levelNumber)
    , victory_(victory)
    , twoPlayerMode_(twoPlayer)
    , useWaveGenerator_(useWaveGenerator)
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

    stateManager_.recordHighScore(totalScore_);
    highScore_ = stateManager_.getHighScore();
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

    // Header, centered
    std::string stageText = "STAGE " + std::to_string(levelNumber_);
    Vector2 stageSize = renderer.measureText(stageText, 20);
    renderer.drawText(stageText,
                     Vector2((screenWidth - stageSize.x) / 2.0f, 36.0f),
                     Constants::COLOR_WHITE, 20);

    const char* playerLabel = "I-PLAYER";
    Vector2 labelSize = renderer.measureText(playerLabel, 16);
    renderer.drawText(playerLabel,
                     Vector2((screenWidth - labelSize.x) / 2.0f, 68.0f),
                     Constants::Color(255, 0, 0), 16);

    std::string scoreText = std::to_string(totalScore_);
    Vector2 scoreSize = renderer.measureText(scoreText, 18);
    renderer.drawText(scoreText,
                     Vector2((screenWidth - scoreSize.x) / 2.0f, 92.0f),
                     Constants::Color(255, 200, 0), 18);

    const std::string highScoreText = "HI-SCORE " + std::to_string(highScore_);
    Vector2 highScoreSize = renderer.measureText(highScoreText, 12);
    renderer.drawText(highScoreText,
                     Vector2((screenWidth - highScoreSize.x) / 2.0f, 116.0f),
                     Constants::Color(180, 180, 180), 12);

    // Enemy type rows (real tank sprites, centered as a group)
    for (int i = 0; i < 4; ++i) {
        int rowY = 140 + i * 44;
        renderRow(renderer, i, rowY);
    }

    // Separator line
    int lineY = 140 + 4 * 44 + 4;
    renderer.drawRect((screenWidth - 280) / 2, lineY, 280, 3, 255, 255, 255, 255);

    // Total
    std::string totalText = "TOTAL  " + std::to_string(displayedTotal_);
    Vector2 totalSize = renderer.measureText(totalText, 18);
    renderer.drawText(totalText,
                     Vector2((screenWidth - totalSize.x) / 2.0f, static_cast<float>(lineY + 18)),
                     Constants::COLOR_WHITE, 18);

    // Continue prompt
    if (animationComplete_) {
        const char* prompt = "PRESS ENTER TO CONTINUE";
        Vector2 promptSize = renderer.measureText(prompt, 12);
        renderer.drawText(prompt,
                         Vector2((screenWidth - promptSize.x) / 2.0f, static_cast<float>(lineY + 52)),
                         Constants::COLOR_GRAY, 12);
    }
}

void ScoreState::renderRow(IRenderer& renderer, int row, int y) {
    int screenWidth = renderer.getWidth();

    int points = displayedKills_[row] * POINTS_PER_TYPE[row];
    std::string text = std::to_string(displayedKills_[row]) + " x " +
                       std::to_string(points) + " PTS";
    Vector2 textSize = renderer.measureText(text, 16);

    // Group: [tank sprite] + gap + text, centered on screen
    const int spriteSize = Sprites::ELEMENT_SIZE;  // 34
    const int gap = 12;
    float groupWidth = spriteSize + gap + textSize.x;
    int groupX = static_cast<int>((screenWidth - groupWidth) / 2.0f);

    // Same sprite the type uses in-game: row 2, 8 columns per type
    int srcX = (row * 8 + Sprites::Tank::DIR_RIGHT_COL) * Sprites::ELEMENT_SIZE;
    int srcY = Sprites::Tank::ENEMY_BASIC_Y;
    renderer.drawSprite(srcX, srcY, Sprites::ELEMENT_SIZE, Sprites::ELEMENT_SIZE,
                       groupX, y, spriteSize, spriteSize);

    // Text vertically centered on the sprite
    float textY = y + (spriteSize - textSize.y) / 2.0f;
    renderer.drawText(text,
                     Vector2(static_cast<float>(groupX + spriteSize + gap), textY),
                     Constants::COLOR_WHITE, 16);
}

void ScoreState::handleInput(const IInput& input) {
    if (animationComplete_) {
        if (input.isKeyPressed(SDL_SCANCODE_RETURN) || input.isKeyPressed(SDL_SCANCODE_SPACE)) {
            if (victory_) {
                // Go to next level (campaign loops back to stage 1 after the last)
                int nextLevel = levelNumber_ + 1;
                if (nextLevel > LevelLoader::getTotalLevels()) {
                    nextLevel = 1;
                }
                stateManager_.changeToStage(nextLevel, twoPlayerMode_, useWaveGenerator_);
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
