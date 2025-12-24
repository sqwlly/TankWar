#pragma once

#include "states/IGameState.hpp"
#include "level/LevelLoader.hpp"
#include <memory>
#include <string>

namespace tank {

class GameStateManager;

/**
 * @brief Construction mode - a simple level editor
 */
class ConstructionState : public IGameState {
public:
    explicit ConstructionState(GameStateManager& manager, int levelNumber = 1);
    ~ConstructionState() override = default;

    void enter() override;
    void exit() override;

    void update(float deltaTime) override;
    void render(IRenderer& renderer) override;
    void handleInput(const IInput& input) override;

    StateType getType() const override { return StateType::Construction; }

private:
    enum class EditorMode {
        Terrain,
        EnemyList
    };

    enum class ConfirmAction {
        None,
        ExitToMenu,
        OverwriteLevelFile,
        ReloadLevel
    };

    GameStateManager& stateManager_;
    LevelLoader levelLoader_;

    std::unique_ptr<Level> level_;
    int levelNumber_ = 1;

    int cursorX_ = 0;
    int cursorY_ = 0;
    TerrainType brushType_ = TerrainType::Brick;
    bool dirty_ = false;

    EditorMode mode_ = EditorMode::Terrain;
    int selectedEnemyIndex_ = 0;

    int hoverX_ = -1;
    int hoverY_ = -1;
    int lastPaintX_ = -1;
    int lastPaintY_ = -1;
    uint8_t lastPaintButton_ = 0;

    // UI / status
    std::string statusText_;
    float statusTimeLeft_ = 0.0f;
    std::string lastSavedPath_;

    // Confirmation (double-press within a time window)
    ConfirmAction pendingConfirm_ = ConfirmAction::None;
    float confirmTimeLeft_ = 0.0f;

    void loadLevel(int levelNumber);
    void createNewLevel(int levelNumber);
    void ensureDefaultEnemyList();

    void moveCursor(int dx, int dy);
    void paintAtCursor(bool largeBrush);
    void eraseAtCursor(bool largeBrush);
    void updateHoverFromMouse(const IInput& input);
    void handleMousePaint(const IInput& input, bool largeBrush);

    bool saveToCustomFile();
    bool overwriteCurrentLevelFile();

    void setStatus(const std::string& text, float seconds = 2.0f);
    void requestConfirm(ConfirmAction action, const std::string& hint, float seconds = 2.0f);
    bool consumeConfirm(ConfirmAction action);

    void renderGrid(IRenderer& renderer);
    void renderSidebar(IRenderer& renderer);
    void renderCursor(IRenderer& renderer);
    void renderHover(IRenderer& renderer);

    static const char* terrainName(TerrainType type);
};

} // namespace tank
