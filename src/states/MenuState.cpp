#include "states/MenuState.hpp"
#include "states/GameStateManager.hpp"
#include "core/ServiceLocator.hpp"
#include "graphics/SpriteSheet.hpp"
#include "input/IInput.hpp"
#include <iostream>
#include <cmath>

namespace tank {

namespace {

void playMenuSfx(SoundId id) {
    if (ServiceLocator::hasAudio()) {
        ServiceLocator::getAudio().playSound(id);
    }
}

} // namespace

MenuState::MenuState(GameStateManager& manager)
    : stateManager_(manager)
{
}

void MenuState::enter() {
    selectedItem_ = MenuItem::Campaign;
    twoPlayerMode_ = false;
    animTimer_ = 0.0f;
    fadeAlpha_ = 0.0f;
    cursorPulse_ = 0.0f;

    if (ServiceLocator::hasAudio()) {
        ServiceLocator::getAudio().playMusic("assets/audio/music/menu_theme.wav", true);
    }
}

void MenuState::exit() {
}

void MenuState::update(float deltaTime) {
    // Animation timers
    animTimer_ += deltaTime;
    cursorPulse_ += deltaTime * 5.0f;

    // Fade in effect
    if (fadeAlpha_ < 1.0f) {
        fadeAlpha_ += deltaTime * 2.0f;
        if (fadeAlpha_ > 1.0f) fadeAlpha_ = 1.0f;
    }
}

void MenuState::render(IRenderer& renderer) {
    renderBackground(renderer);
    renderDecorations(renderer);
    renderTitle(renderer);
    renderMenuItems(renderer);
    renderFooter(renderer);
    if (settingsOpen_) {
        renderSettings(renderer);
    }
}

void MenuState::renderBackground(IRenderer& renderer) {
    renderer.clear(
        Constants::UIColors::BG_DARK.r,
        Constants::UIColors::BG_DARK.g,
        Constants::UIColors::BG_DARK.b, 255);
}

void MenuState::renderDecorations(IRenderer& renderer) {
    // Animated tanks flanking the menu panel, facing each other
    const int tankY = MENU_START_Y + MENU_ITEM_HEIGHT - 17;
    float bob = std::sin(animTimer_ * 1.5f) * 6.0f;
    int animFrame = static_cast<int>(animTimer_ * 4.0f) % 2;

    const int panelX = (Constants::WINDOW_WIDTH - MENU_PANEL_WIDTH) / 2;

    // Left tank (Player 1, facing right)
    Rectangle p1Sprite = Sprites::Tank::getFrame(
        Sprites::Tank::P1_BASE_Y,
        Sprites::Tank::DIR_RIGHT_COL,
        animFrame, 0);
    renderer.drawSprite(
        static_cast<int>(p1Sprite.x), static_cast<int>(p1Sprite.y),
        static_cast<int>(p1Sprite.width), static_cast<int>(p1Sprite.height),
        panelX - 70, tankY + static_cast<int>(bob), 40, 40);

    // Right tank (Player 2, facing left)
    Rectangle p2Sprite = Sprites::Tank::getFrame(
        Sprites::Tank::P2_BASE_Y,
        Sprites::Tank::DIR_LEFT_COL,
        animFrame, 0);
    renderer.drawSprite(
        static_cast<int>(p2Sprite.x), static_cast<int>(p2Sprite.y),
        static_cast<int>(p2Sprite.width), static_cast<int>(p2Sprite.height),
        panelX + MENU_PANEL_WIDTH + 30, tankY - static_cast<int>(bob), 40, 40);
}

void MenuState::renderTitle(IRenderer& renderer) {
    if (!logoTexture_) {
        logoTexture_ = renderer.loadTexture(Constants::Paths::LOGO_IMAGE);
    }

    const int logoX = (Constants::WINDOW_WIDTH - LOGO_WIDTH) / 2;
    if (logoTexture_) {
        renderer.drawTexture(logoTexture_,
                            Rectangle(static_cast<float>(logoX), static_cast<float>(LOGO_Y),
                                      static_cast<float>(LOGO_WIDTH), static_cast<float>(LOGO_HEIGHT)));
    }

    // Subtitle
    const char* subtitle = "C++ EDITION";
    Vector2 size = renderer.measureText(subtitle, 14);
    uint8_t alpha = static_cast<uint8_t>(180 * fadeAlpha_);
    renderer.drawText(subtitle,
                     Vector2((Constants::WINDOW_WIDTH - size.x) / 2.0f,
                             static_cast<float>(LOGO_Y + LOGO_HEIGHT + 10)),
                     Constants::Color(150, 150, 150, alpha), 14);

    // High score under the title
    const std::string highScore = "HI-SCORE " + std::to_string(stateManager_.getHighScore());
    Vector2 hsSize = renderer.measureText(highScore, 12);
    renderer.drawText(highScore,
                     Vector2((Constants::WINDOW_WIDTH - hsSize.x) / 2.0f,
                             static_cast<float>(LOGO_Y + LOGO_HEIGHT + 30)),
                     Constants::Color(255, 200, 0, alpha), 12);
}

void MenuState::renderMenuItems(IRenderer& renderer) {
    const std::string campaignLabel = "CONTINUE STAGE " + std::to_string(stateManager_.getCampaignStartLevel());
    const char* items[] = { campaignLabel.c_str(), "SURVIVAL", "SETTINGS" };

    const char* descriptions[] = {
        "Play through 20 stages",
        "Endless wave mode",
        "Volume, difficulty and controls"
    };

    const int panelX = (Constants::WINDOW_WIDTH - MENU_PANEL_WIDTH) / 2;

    for (int i = 0; i < MENU_ITEM_COUNT; ++i) {
        int y = MENU_START_Y + i * MENU_ITEM_HEIGHT;
        bool selected = (static_cast<int>(selectedItem_) == i);

        if (selected) {
            // Panel highlight + left accent bar
            renderer.drawRect(panelX, y - 6, MENU_PANEL_WIDTH, 36,
                             Constants::UIColors::BG_PANEL.r,
                             Constants::UIColors::BG_PANEL.g,
                             Constants::UIColors::BG_PANEL.b, 255);
            renderer.drawRect(panelX, y - 6, 4, 36,
                             Constants::UIColors::PRIMARY.r,
                             Constants::UIColors::PRIMARY.g,
                             Constants::UIColors::PRIMARY.b, 255);
            renderCursor(renderer, panelX + 10, y + 1);
        }

        Constants::Color textColor = selected
            ? Constants::UIColors::MENU_HIGHLIGHT
            : Constants::UIColors::MENU_NORMAL;
        renderer.drawText(items[i],
                         Vector2(static_cast<float>(panelX + 52), static_cast<float>(y)),
                         textColor, 20);
    }

    // Description of the selected item, centered below the panel
    const char* description = descriptions[static_cast<int>(selectedItem_)];
    Vector2 descSize = renderer.measureText(description, 11);
    renderer.drawText(description,
                     Vector2((Constants::WINDOW_WIDTH - descSize.x) / 2.0f,
                             static_cast<float>(MENU_START_Y + MENU_ITEM_COUNT * MENU_ITEM_HEIGHT + 6)),
                     Constants::Color(120, 120, 120), 11);

    // Player mode toggle, centered as "MODE: " + value
    int modeY = MENU_START_Y + MENU_ITEM_COUNT * MENU_ITEM_HEIGHT + 34;
    const char* modeLabel = "MODE: ";
    const char* modeValue = twoPlayerMode_ ? "2 PLAYERS" : "1 PLAYER";
    Constants::Color modeColor = twoPlayerMode_
        ? Constants::UIColors::PLAYER2
        : Constants::UIColors::PLAYER1;

    Vector2 labelSize = renderer.measureText(modeLabel, 14);
    Vector2 valueSize = renderer.measureText(modeValue, 14);
    float modeX = (Constants::WINDOW_WIDTH - labelSize.x - valueSize.x) / 2.0f;
    renderer.drawText(modeLabel, Vector2(modeX, static_cast<float>(modeY)),
                     Constants::Color(100, 100, 100), 14);
    renderer.drawText(modeValue, Vector2(modeX + labelSize.x, static_cast<float>(modeY)),
                     modeColor, 14);

    // Construction mode hint
    const char* hint = "C - CONSTRUCTION";
    Vector2 hintSize = renderer.measureText(hint, 12);
    renderer.drawText(hint,
                     Vector2((Constants::WINDOW_WIDTH - hintSize.x) / 2.0f,
                             static_cast<float>(modeY + 26)),
                     Constants::Color(80, 80, 80), 12);
}

void MenuState::renderCursor(IRenderer& renderer, int x, int y) {
    // Tank sprite cursor with a gentle horizontal bob
    float bounce = std::sin(cursorPulse_) * 3.0f;
    int cursorX = x + static_cast<int>(bounce);
    int animFrame = static_cast<int>(animTimer_ * 4.0f) % 2;

    Rectangle sprite = Sprites::Tank::getFrame(
        Sprites::Tank::P1_BASE_Y,
        Sprites::Tank::DIR_RIGHT_COL,
        animFrame, 0);
    renderer.drawSprite(
        static_cast<int>(sprite.x), static_cast<int>(sprite.y),
        static_cast<int>(sprite.width), static_cast<int>(sprite.height),
        cursorX, y - 4, 32, 32);
}

void MenuState::renderFooter(IRenderer& renderer) {
    int footerY = Constants::WINDOW_HEIGHT - 28;

    // Controls hint, centered
    const char* controls = "UP/DOWN: SELECT   ENTER: CONFIRM   P: PLAYERS";
    Vector2 size = renderer.measureText(controls, 11);
    renderer.drawText(controls,
                     Vector2((Constants::WINDOW_WIDTH - size.x) / 2.0f,
                             static_cast<float>(footerY)),
                     Constants::Color(80, 80, 80), 11);

    // Version/credits
    renderer.drawText("v1.0",
                     Vector2(static_cast<float>(Constants::WINDOW_WIDTH - 40),
                             static_cast<float>(footerY)),
                     Constants::Color(60, 60, 60), 10);
}

void MenuState::handleInput(const IInput& input) {
    if (settingsOpen_) {
        handleSettingsInput(input);
        return;
    }

    if (input.isKeyPressed(SDL_SCANCODE_C)) {
        stateManager_.changeToConstruction(1);
        return;
    }

    if (input.isKeyPressed(SDL_SCANCODE_P)) {
        twoPlayerMode_ = !twoPlayerMode_;
        return;
    }

    // Direct mode selection via number keys
    if (input.isKeyPressed(SDL_SCANCODE_1)) {
        selectedItem_ = MenuItem::Campaign;
        playMenuSfx(SoundId::MenuConfirm);
        confirmSelection();
        return;
    }
    if (input.isKeyPressed(SDL_SCANCODE_2)) {
        selectedItem_ = MenuItem::Survival;
        playMenuSfx(SoundId::MenuConfirm);
        confirmSelection();
        return;
    }
    if (input.isKeyPressed(SDL_SCANCODE_3)) {
        selectedItem_ = MenuItem::Settings;
        confirmSelection();
        return;
    }

    // Navigate
    if (input.isKeyPressed(SDL_SCANCODE_UP) || input.isKeyPressed(SDL_SCANCODE_W)) {
        selectPreviousItem();
        playMenuSfx(SoundId::MenuMove);
    } else if (input.isKeyPressed(SDL_SCANCODE_DOWN) || input.isKeyPressed(SDL_SCANCODE_S)) {
        selectNextItem();
        playMenuSfx(SoundId::MenuMove);
    }

    // Confirm
    if (input.isKeyPressed(SDL_SCANCODE_RETURN) || input.isKeyPressed(SDL_SCANCODE_SPACE)) {
        playMenuSfx(SoundId::MenuConfirm);
        confirmSelection();
    }
}

void MenuState::selectNextItem() {
    int current = static_cast<int>(selectedItem_);
    current = (current + 1) % MENU_ITEM_COUNT;
    selectedItem_ = static_cast<MenuItem>(current);
}

void MenuState::selectPreviousItem() {
    int current = static_cast<int>(selectedItem_);
    current = (current - 1 + MENU_ITEM_COUNT) % MENU_ITEM_COUNT;
    selectedItem_ = static_cast<MenuItem>(current);
}

void MenuState::confirmSelection() {
    std::cout << "Selected menu item: " << static_cast<int>(selectedItem_) << std::endl;

    switch (selectedItem_) {
        case MenuItem::Campaign:
            stateManager_.beginRun();
            stateManager_.changeToStage(stateManager_.getCampaignStartLevel(), twoPlayerMode_);
            break;

        case MenuItem::Survival:
            stateManager_.beginRun();
            stateManager_.changeToPlaying(1, /*twoPlayer=*/twoPlayerMode_, /*useWaveGenerator=*/true);
            break;

        case MenuItem::Settings:
            settingsOpen_ = true;
            selectedSettingsItem_ = SettingsItem::Volume;
            break;
    }
}

void MenuState::handleSettingsInput(const IInput& input) {
    if (input.isKeyPressed(SDL_SCANCODE_ESCAPE)) {
        settingsOpen_ = false;
        return;
    }

    int selected = static_cast<int>(selectedSettingsItem_);
    if (input.isKeyPressed(SDL_SCANCODE_UP) || input.isKeyPressed(SDL_SCANCODE_W)) {
        selected = (selected + 2) % 3;
    } else if (input.isKeyPressed(SDL_SCANCODE_DOWN) || input.isKeyPressed(SDL_SCANCODE_S)) {
        selected = (selected + 1) % 3;
    }
    selectedSettingsItem_ = static_cast<SettingsItem>(selected);

    const bool decrease = input.isKeyPressed(SDL_SCANCODE_LEFT) || input.isKeyPressed(SDL_SCANCODE_A);
    const bool increase = input.isKeyPressed(SDL_SCANCODE_RIGHT) || input.isKeyPressed(SDL_SCANCODE_D);
    if (selectedSettingsItem_ == SettingsItem::Volume && (decrease || increase)) {
        const float delta = decrease ? -0.1f : 0.1f;
        stateManager_.setMasterVolume(stateManager_.getMasterVolume() + delta);
    } else if (selectedSettingsItem_ == SettingsItem::Difficulty && (decrease || increase)) {
        int value = static_cast<int>(stateManager_.getDifficulty());
        value = (value + (decrease ? 2 : 1)) % 3;
        stateManager_.setDifficulty(static_cast<GameDifficulty>(value));
    }
}

void MenuState::renderSettings(IRenderer& renderer) {
    const int panelWidth = 360;
    const int panelHeight = 250;
    const int panelX = (Constants::WINDOW_WIDTH - panelWidth) / 2;
    const int panelY = 112;
    renderer.drawRect(panelX, panelY, panelWidth, panelHeight,
                     8, 10, 15, 240);
    renderer.drawRect(panelX, panelY, panelWidth, 2,
                     Constants::UIColors::PRIMARY.r, Constants::UIColors::PRIMARY.g,
                     Constants::UIColors::PRIMARY.b, 255);

    const char* title = "SETTINGS";
    Vector2 titleSize = renderer.measureText(title, 22);
    renderer.drawText(title, Vector2((Constants::WINDOW_WIDTH - titleSize.x) / 2.0f,
                                     static_cast<float>(panelY + 20)),
                     Constants::UIColors::MENU_HIGHLIGHT, 22);

    const int volumePercent = static_cast<int>(stateManager_.getMasterVolume() * 100.0f + 0.5f);
    const char* difficultyNames[] = {"EASY", "NORMAL", "HARD"};
    const std::string values[] = {
        "VOLUME: " + std::to_string(volumePercent) + "%",
        std::string("DIFFICULTY: ") + difficultyNames[static_cast<int>(stateManager_.getDifficulty())],
        "CONTROLS: P1 WASD + SPACE / P2 ARROWS + ENTER"
    };

    for (int i = 0; i < 3; ++i) {
        const bool selected = static_cast<int>(selectedSettingsItem_) == i;
        const int y = panelY + 68 + i * 36;
        if (selected) {
            renderer.drawText(">", Vector2(static_cast<float>(panelX + 22), static_cast<float>(y)),
                             Constants::UIColors::MENU_HIGHLIGHT, 16);
        }
        renderer.drawText(values[i], Vector2(static_cast<float>(panelX + 44), static_cast<float>(y)),
                         selected ? Constants::COLOR_WHITE : Constants::UIColors::MENU_NORMAL, 13);
    }

    const char* hint = "LEFT/RIGHT: CHANGE   ESC: BACK";
    Vector2 hintSize = renderer.measureText(hint, 11);
    renderer.drawText(hint, Vector2((Constants::WINDOW_WIDTH - hintSize.x) / 2.0f,
                                    static_cast<float>(panelY + panelHeight - 28)),
                     Constants::Color(120, 120, 120), 11);
}

} // namespace tank
