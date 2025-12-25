#pragma once

#include "states/IGameState.hpp"
#include "rendering/IRenderer.hpp"

namespace tank {

class GameStateManager;

/**
 * @brief Menu state - main menu of the game
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
    float cursorBlinkTimer_ = 0.0f;
    bool cursorVisible_ = true;

    // Menu positions
    static constexpr int MENU_START_Y = 280;
    static constexpr int MENU_ITEM_HEIGHT = 30;
    static constexpr int MENU_CURSOR_X = 150;

    void selectNextItem();
    void selectPreviousItem();
    void confirmSelection();

    void renderTitle(IRenderer& renderer);
    void renderMenuItems(IRenderer& renderer);
    void renderCursor(IRenderer& renderer);
};

} // namespace tank
