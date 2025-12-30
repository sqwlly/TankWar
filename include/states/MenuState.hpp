#pragma once

#include "states/IGameState.hpp"
#include "rendering/IRenderer.hpp"

namespace tank {

class GameStateManager;

/**
 * @brief Menu state - main menu with improved visual design
 *
 * Features:
 * - Animated title with wave effect
 * - Smooth cursor animation
 * - Tank decorations
 * - Fade-in entrance animation
 */
class MenuState : public IGameState {
public:
    explicit MenuState(GameStateManager& manager);
    ~MenuState() override = default;

    void enter() override;
    void exit() override;

    void update(float deltaTime) override;
    void render(IRenderer& renderer) override;
    void handleInput(const IInput& input) override;

    StateType getType() const override { return StateType::Menu; }

private:
    GameStateManager& stateManager_;

    enum class MenuItem {
        Campaign = 0,
        Survival = 1
    };

    static constexpr int MENU_ITEM_COUNT = 2;

    MenuItem selectedItem_ = MenuItem::Campaign;
    bool twoPlayerMode_ = false;

    // Animation state
    float animTimer_ = 0.0f;
    float fadeAlpha_ = 0.0f;
    float titleBounce_ = 0.0f;
    float cursorPulse_ = 0.0f;

    // Menu positions
    static constexpr int TITLE_Y = 80;
    static constexpr int MENU_START_Y = 240;
    static constexpr int MENU_ITEM_HEIGHT = 36;
    static constexpr int MENU_X = 180;

    void selectNextItem();
    void selectPreviousItem();
    void confirmSelection();

    void renderBackground(IRenderer& renderer);
    void renderTitle(IRenderer& renderer);
    void renderMenuItems(IRenderer& renderer);
    void renderCursor(IRenderer& renderer, int x, int y);
    void renderDecorations(IRenderer& renderer);
    void renderFooter(IRenderer& renderer);
};

} // namespace tank
