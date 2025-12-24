#pragma once

#include "states/IGameState.hpp"
#include <array>

namespace tank {

class GameStateManager;

/**
 * @brief Score state - shows level completion scores
 */
class ScoreState : public IGameState {
public:
    ScoreState(GameStateManager& manager, int levelNumber, bool victory);
    ~ScoreState() override = default;

    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render(IRenderer& renderer) override;
    void handleInput(const IInput& input) override;

    StateType getType() const override { return StateType::Score; }

    // Set kill counts for each enemy type
    void setKillCounts(int type1, int type2, int type3, int type4);
    void setTotalScore(int score) { totalScore_ = score; }

private:
    GameStateManager& stateManager_;
    int levelNumber_;
    bool victory_;

    // Kill counts by enemy type (0-3)
    std::array<int, 4> killCounts_;
    std::array<int, 4> displayedKills_;  // For animation
    int totalKills_;
    int displayedTotal_;

    int totalScore_;
    int highScore_;

    // Animation state
    float animationTimer_;
    int currentRow_;       // Which row is animating
    bool animationComplete_;

    static constexpr float TICK_INTERVAL = 0.05f;  // 50ms per tick
    static constexpr int POINTS_PER_TYPE[] = {100, 200, 300, 400};

    void renderRow(IRenderer& renderer, int row, int y);
};

} // namespace tank
