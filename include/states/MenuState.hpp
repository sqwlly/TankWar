#pragma once

#include "states/IGameState.hpp"
#include "rendering/IRenderer.hpp"

namespace tank {

class GameStateManager;

/**
 * @brief Menu state - main menu
 *
 * Layout:
 * - Brick-wall logo (assets/images/logo.png) centered at the top
 * - Menu items in a centered panel with a tank-sprite cursor
 * - Two decorative player tanks flanking the panel
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
        Survival = 1,
        Settings = 2
    };

    static constexpr int MENU_ITEM_COUNT = 3;

    enum class SettingsItem {
        Volume = 0,
        Difficulty = 1,
        Controls = 2
    };

    MenuItem selectedItem_ = MenuItem::Campaign;
    bool twoPlayerMode_ = false;
    bool settingsOpen_ = false;
    SettingsItem selectedSettingsItem_ = SettingsItem::Volume;

    // Animation state
    float animTimer_ = 0.0f;
    float fadeAlpha_ = 0.0f;
    float cursorPulse_ = 0.0f;

    // Logo (owned by the renderer's texture cache)
    SDL_Texture* logoTexture_ = nullptr;

    // Layout
    static constexpr int LOGO_WIDTH = 440;     // scaled from 522x55 source
    static constexpr int LOGO_HEIGHT = 46;
    static constexpr int LOGO_Y = 42;
    static constexpr int MENU_START_Y = 196;
    static constexpr int MENU_ITEM_HEIGHT = 44;
    static constexpr int MENU_PANEL_WIDTH = 280;

    void selectNextItem();
    void selectPreviousItem();
    void confirmSelection();
    void handleSettingsInput(const IInput& input);

    void renderBackground(IRenderer& renderer);
    void renderTitle(IRenderer& renderer);
    void renderMenuItems(IRenderer& renderer);
    void renderCursor(IRenderer& renderer, int x, int y);
    void renderDecorations(IRenderer& renderer);
    void renderFooter(IRenderer& renderer);
    void renderSettings(IRenderer& renderer);
};

} // namespace tank
