#include "states/ConstructionState.hpp"
#include "states/GameStateManager.hpp"
#include "states/PlayingState.hpp"
#include "input/InputManager.hpp"
#include "utils/Vector2.hpp"
#include <algorithm>

namespace tank {

namespace {
constexpr int SIDEBAR_X = Constants::GAME_WIDTH + 6;
constexpr int SIDEBAR_Y = 8;

constexpr Constants::Color COLOR_UI_BG{16, 16, 16, 255};
constexpr Constants::Color COLOR_GRID{40, 40, 40, 255};
constexpr Constants::Color COLOR_CURSOR{255, 64, 64, 255};
constexpr Constants::Color COLOR_HOVER{255, 255, 255, 180};

Constants::Color terrainColor(TerrainType type) {
    switch (type) {
        case TerrainType::Steel: return {160, 160, 160, 255};
        case TerrainType::Brick: return {180, 90, 50, 255};
        case TerrainType::Water: return {0, 110, 255, 255};
        case TerrainType::Grass: return {0, 180, 0, 255};
        case TerrainType::Empty:
        case TerrainType::Base:
        default:
            return {0, 0, 0, 255};
    }
}
} // namespace

ConstructionState::ConstructionState(GameStateManager& manager, int levelNumber)
    : stateManager_(manager)
    , levelNumber_(std::max(1, levelNumber))
{
}

void ConstructionState::enter() {
    cursorX_ = 0;
    cursorY_ = 0;
    brushType_ = TerrainType::Brick;
    dirty_ = false;
    mode_ = EditorMode::Terrain;
    selectedEnemyIndex_ = 0;
    hoverX_ = -1;
    hoverY_ = -1;
    lastPaintX_ = -1;
    lastPaintY_ = -1;
    lastPaintButton_ = 0;
    pendingConfirm_ = ConfirmAction::None;
    confirmTimeLeft_ = 0.0f;
    statusText_.clear();
    statusTimeLeft_ = 0.0f;
    lastSavedPath_.clear();

    loadLevel(levelNumber_);
    setStatus("Construction: 方向键移动，数字键选材质，鼠标左绘制右擦除，Tab切模式");
}

void ConstructionState::exit() {
    level_.reset();
}

void ConstructionState::update(float deltaTime) {
    if (statusTimeLeft_ > 0.0f) {
        statusTimeLeft_ = std::max(0.0f, statusTimeLeft_ - deltaTime);
        if (statusTimeLeft_ <= 0.0f) {
            statusText_.clear();
        }
    }

    if (confirmTimeLeft_ > 0.0f) {
        confirmTimeLeft_ = std::max(0.0f, confirmTimeLeft_ - deltaTime);
        if (confirmTimeLeft_ <= 0.0f) {
            pendingConfirm_ = ConfirmAction::None;
        }
    }
}

void ConstructionState::render(IRenderer& renderer) {
    renderer.clear(0, 0, 0, 255);

    // Sidebar background
    renderer.drawRectangle(Rectangle(static_cast<float>(Constants::GAME_WIDTH), 0.0f,
                                     static_cast<float>(Constants::UI_PANEL_WIDTH),
                                     static_cast<float>(Constants::WINDOW_HEIGHT)),
                           COLOR_UI_BG,
                           true);

    renderGrid(renderer);
    renderHover(renderer);
    renderCursor(renderer);
    renderSidebar(renderer);
}

void ConstructionState::handleInput(const InputManager& input) {
    const bool largeBrush = input.isKeyDown(SDL_SCANCODE_LSHIFT) || input.isKeyDown(SDL_SCANCODE_RSHIFT);

    updateHoverFromMouse(input);
    handleMousePaint(input, largeBrush);

    if (input.isKeyPressed(SDL_SCANCODE_TAB)) {
        mode_ = (mode_ == EditorMode::Terrain) ? EditorMode::EnemyList : EditorMode::Terrain;
        setStatus(mode_ == EditorMode::Terrain ? "模式：地形编辑" : "模式：敌人序列编辑");
    }

    // Navigation
    if (input.isKeyPressed(SDL_SCANCODE_LEFT)) moveCursor(-1, 0);
    if (input.isKeyPressed(SDL_SCANCODE_RIGHT)) moveCursor(1, 0);
    if (input.isKeyPressed(SDL_SCANCODE_UP)) moveCursor(0, -1);
    if (input.isKeyPressed(SDL_SCANCODE_DOWN)) moveCursor(0, 1);

    if (mode_ == EditorMode::Terrain) {
        // Brush selection (0-4)
        if (input.isKeyPressed(SDL_SCANCODE_0)) brushType_ = TerrainType::Empty;
        if (input.isKeyPressed(SDL_SCANCODE_1)) brushType_ = TerrainType::Steel;
        if (input.isKeyPressed(SDL_SCANCODE_2)) brushType_ = TerrainType::Brick;
        if (input.isKeyPressed(SDL_SCANCODE_3)) brushType_ = TerrainType::Water;
        if (input.isKeyPressed(SDL_SCANCODE_4)) brushType_ = TerrainType::Grass;
    }

    if (mode_ == EditorMode::Terrain) {
        // Paint / erase
        if (input.isKeyPressed(SDL_SCANCODE_SPACE) || input.isKeyPressed(SDL_SCANCODE_RETURN)) {
            paintAtCursor(largeBrush);
        }
        if (input.isKeyPressed(SDL_SCANCODE_BACKSPACE) || input.isKeyPressed(SDL_SCANCODE_DELETE) ||
            input.isKeyPressed(SDL_SCANCODE_X)) {
            eraseAtCursor(largeBrush);
        }
    }

    if (mode_ == EditorMode::EnemyList && level_) {
        ensureDefaultEnemyList();
        const size_t count = level_->getEnemySpawnCount();
        if (count > 0) {
            selectedEnemyIndex_ = std::clamp(selectedEnemyIndex_, 0, static_cast<int>(count - 1));
        } else {
            selectedEnemyIndex_ = 0;
        }

        if (input.isKeyPressed(SDL_SCANCODE_LEFTBRACKET)) {
            selectedEnemyIndex_ = std::max(0, selectedEnemyIndex_ - 1);
        }
        if (input.isKeyPressed(SDL_SCANCODE_RIGHTBRACKET) && count > 0) {
            selectedEnemyIndex_ = std::min(static_cast<int>(count - 1), selectedEnemyIndex_ + 1);
        }

        const size_t idx = static_cast<size_t>(selectedEnemyIndex_);
        bool changed = false;
        if (input.isKeyPressed(SDL_SCANCODE_1)) changed = level_->setEnemySpawnType(idx, EnemyType::Basic) || changed;
        if (input.isKeyPressed(SDL_SCANCODE_2)) changed = level_->setEnemySpawnType(idx, EnemyType::Fast) || changed;
        if (input.isKeyPressed(SDL_SCANCODE_3)) changed = level_->setEnemySpawnType(idx, EnemyType::Power) || changed;
        if (input.isKeyPressed(SDL_SCANCODE_4)) changed = level_->setEnemySpawnType(idx, EnemyType::Heavy) || changed;
        if (input.isKeyPressed(SDL_SCANCODE_P)) changed = level_->toggleEnemySpawnPowerUp(idx) || changed;
        if (changed) dirty_ = true;
    }

    // Level navigation
    if (input.isKeyPressed(SDL_SCANCODE_PAGEUP)) {
        levelNumber_ = std::max(1, levelNumber_ - 1);
        loadLevel(levelNumber_);
        return;
    }
    if (input.isKeyPressed(SDL_SCANCODE_PAGEDOWN)) {
        levelNumber_ = std::min(LevelLoader::getTotalLevels(), levelNumber_ + 1);
        loadLevel(levelNumber_);
        return;
    }

    // New / reload
    if (input.isKeyPressed(SDL_SCANCODE_N)) {
        createNewLevel(levelNumber_);
        setStatus("已创建空白关卡");
        return;
    }
    if (input.isKeyPressed(SDL_SCANCODE_L)) {
        if (!dirty_) {
            loadLevel(levelNumber_);
        } else if (consumeConfirm(ConfirmAction::ReloadLevel)) {
            loadLevel(levelNumber_);
        } else {
            requestConfirm(ConfirmAction::ReloadLevel, "将丢弃未保存更改：再次按 L 重新加载");
        }
        return;
    }

    // Save (S) -> custom file; Ctrl+S -> overwrite Level_N (double confirm)
    const bool ctrlDown = input.isKeyDown(SDL_SCANCODE_LCTRL) || input.isKeyDown(SDL_SCANCODE_RCTRL);
    if (ctrlDown && input.isKeyPressed(SDL_SCANCODE_S)) {
        if (consumeConfirm(ConfirmAction::OverwriteLevelFile)) {
            overwriteCurrentLevelFile();
        } else {
            requestConfirm(ConfirmAction::OverwriteLevelFile, "将覆盖 assets/levels/Level_N：再次按 Ctrl+S 确认");
        }
        return;
    }
    if (!ctrlDown && input.isKeyPressed(SDL_SCANCODE_S)) {
        saveToCustomFile();
        return;
    }

    // Quick playtest (F5) - saves to Level_custom then starts PlayingState from that file
    if (input.isKeyPressed(SDL_SCANCODE_F5)) {
        if (level_) {
            if (saveToCustomFile()) {
                stateManager_.pushState(std::make_unique<PlayingState>(stateManager_, levelNumber_, false, "assets/levels/Level_custom"));
            }
        }
        return;
    }

    // Exit
    if (input.isKeyPressed(SDL_SCANCODE_ESCAPE)) {
        if (!dirty_) {
            stateManager_.changeToMenu();
            return;
        }

        if (consumeConfirm(ConfirmAction::ExitToMenu)) {
            stateManager_.changeToMenu();
            return;
        }

        requestConfirm(ConfirmAction::ExitToMenu, "未保存更改：再次按 Esc 退出，或按 S 保存");
        return;
    }
}

void ConstructionState::loadLevel(int levelNumber) {
    levelNumber_ = std::clamp(levelNumber, 1, LevelLoader::getTotalLevels());
    level_ = levelLoader_.loadLevel(levelNumber_);
    if (!level_) {
        createNewLevel(levelNumber_);
        setStatus("加载失败：已创建空白关卡");
        return;
    }

    ensureDefaultEnemyList();
    dirty_ = false;
    pendingConfirm_ = ConfirmAction::None;
    confirmTimeLeft_ = 0.0f;
    setStatus("已加载 Level_" + std::to_string(levelNumber_));
}

void ConstructionState::createNewLevel(int levelNumber) {
    levelNumber_ = std::clamp(levelNumber, 1, LevelLoader::getTotalLevels());
    level_ = std::make_unique<Level>(levelNumber_);
    level_->clear();
    ensureDefaultEnemyList();
    dirty_ = true;
}

void ConstructionState::ensureDefaultEnemyList() {
    if (!level_) return;
    if (!level_->getEnemySpawnList().empty()) return;

    for (int i = 0; i < Constants::TOTAL_ENEMIES_PER_LEVEL; ++i) {
        const bool hasPowerUp = (i % 4 == 3);
        level_->addEnemySpawn(EnemyType::Basic, hasPowerUp);
    }
}

void ConstructionState::moveCursor(int dx, int dy) {
    cursorX_ = std::clamp(cursorX_ + dx, 0, Constants::GRID_WIDTH - 1);
    cursorY_ = std::clamp(cursorY_ + dy, 0, Constants::GRID_HEIGHT - 1);
}

void ConstructionState::paintAtCursor(bool largeBrush) {
    if (!level_) return;

    const auto apply = [&](int x, int y) {
        if (x < 0 || x >= Constants::GRID_WIDTH || y < 0 || y >= Constants::GRID_HEIGHT) return;
        level_->setTerrainAt(x, y, brushType_);
    };

    apply(cursorX_, cursorY_);
    if (largeBrush) {
        apply(cursorX_ + 1, cursorY_);
        apply(cursorX_, cursorY_ + 1);
        apply(cursorX_ + 1, cursorY_ + 1);
    }

    dirty_ = true;
}

void ConstructionState::eraseAtCursor(bool largeBrush) {
    if (!level_) return;

    const auto apply = [&](int x, int y) {
        if (x < 0 || x >= Constants::GRID_WIDTH || y < 0 || y >= Constants::GRID_HEIGHT) return;
        level_->setTerrainAt(x, y, TerrainType::Empty);
    };

    apply(cursorX_, cursorY_);
    if (largeBrush) {
        apply(cursorX_ + 1, cursorY_);
        apply(cursorX_, cursorY_ + 1);
        apply(cursorX_ + 1, cursorY_ + 1);
    }

    dirty_ = true;
}

void ConstructionState::updateHoverFromMouse(const InputManager& input) {
    hoverX_ = -1;
    hoverY_ = -1;

    const int mx = input.getMouseX();
    const int my = input.getMouseY();
    if (mx < 0 || my < 0) return;
    if (mx >= Constants::GAME_WIDTH || my >= Constants::GAME_HEIGHT) return;

    hoverX_ = std::clamp(mx / Constants::CELL_SIZE, 0, Constants::GRID_WIDTH - 1);
    hoverY_ = std::clamp(my / Constants::CELL_SIZE, 0, Constants::GRID_HEIGHT - 1);
}

void ConstructionState::handleMousePaint(const InputManager& input, bool largeBrush) {
    if (mode_ != EditorMode::Terrain) return;
    if (!level_) return;
    if (hoverX_ < 0 || hoverY_ < 0) return;

    const bool lmbDown = input.isMouseButtonDown(SDL_BUTTON_LEFT);
    const bool rmbDown = input.isMouseButtonDown(SDL_BUTTON_RIGHT);
    if (!lmbDown && !rmbDown) {
        lastPaintX_ = -1;
        lastPaintY_ = -1;
        lastPaintButton_ = 0;
        return;
    }

    const uint8_t activeButton = lmbDown ? SDL_BUTTON_LEFT : SDL_BUTTON_RIGHT;
    if (hoverX_ == lastPaintX_ && hoverY_ == lastPaintY_ && activeButton == lastPaintButton_) return;

    cursorX_ = hoverX_;
    cursorY_ = hoverY_;

    if (lmbDown) {
        paintAtCursor(largeBrush);
    } else {
        eraseAtCursor(largeBrush);
    }

    lastPaintX_ = hoverX_;
    lastPaintY_ = hoverY_;
    lastPaintButton_ = activeButton;
}

bool ConstructionState::saveToCustomFile() {
    if (!level_) return false;

    const std::string filePath = "assets/levels/Level_custom";
    const bool ok = levelLoader_.saveToFile(*level_, filePath);
    if (ok) {
        lastSavedPath_ = filePath;
        dirty_ = false;
        setStatus("已保存到 " + filePath);
    } else {
        setStatus("保存失败：" + filePath);
    }
    return ok;
}

bool ConstructionState::overwriteCurrentLevelFile() {
    if (!level_) return false;

    const std::string filePath = "assets/levels/Level_" + std::to_string(levelNumber_);
    const bool ok = levelLoader_.saveToFile(*level_, filePath);
    if (ok) {
        lastSavedPath_ = filePath;
        dirty_ = false;
        setStatus("已覆盖保存到 " + filePath);
    } else {
        setStatus("保存失败：" + filePath);
    }
    return ok;
}

void ConstructionState::setStatus(const std::string& text, float seconds) {
    statusText_ = text;
    statusTimeLeft_ = seconds;
}

void ConstructionState::requestConfirm(ConfirmAction action, const std::string& hint, float seconds) {
    pendingConfirm_ = action;
    confirmTimeLeft_ = seconds;
    setStatus(hint, seconds);
}

bool ConstructionState::consumeConfirm(ConfirmAction action) {
    if (pendingConfirm_ != action) return false;
    if (confirmTimeLeft_ <= 0.0f) return false;
    pendingConfirm_ = ConfirmAction::None;
    confirmTimeLeft_ = 0.0f;
    return true;
}

void ConstructionState::renderGrid(IRenderer& renderer) {
    if (!level_) return;

    const auto& map = level_->getTerrainMap();
    for (int y = 0; y < Constants::GRID_HEIGHT; ++y) {
        for (int x = 0; x < Constants::GRID_WIDTH; ++x) {
            const TerrainType type = map[y][x];
            const int px = x * Constants::CELL_SIZE;
            const int py = y * Constants::CELL_SIZE;

            if (type != TerrainType::Empty) {
                renderer.drawRectangle(Rectangle(static_cast<float>(px), static_cast<float>(py),
                                                 static_cast<float>(Constants::CELL_SIZE),
                                                 static_cast<float>(Constants::CELL_SIZE)),
                                       terrainColor(type),
                                       true);
            }

            renderer.drawRectangle(Rectangle(static_cast<float>(px), static_cast<float>(py),
                                             static_cast<float>(Constants::CELL_SIZE),
                                             static_cast<float>(Constants::CELL_SIZE)),
                                   COLOR_GRID,
                                   false);
        }
    }

    // Base (fixed)
    Vector2 basePos = level_->getBasePosition();
    renderer.drawRectangle(Rectangle(basePos.x, basePos.y,
                                     static_cast<float>(Constants::ELEMENT_SIZE),
                                     static_cast<float>(Constants::ELEMENT_SIZE)),
                           Constants::COLOR_RED,
                           false);
}

void ConstructionState::renderHover(IRenderer& renderer) {
    if (hoverX_ < 0 || hoverY_ < 0) return;
    const int px = hoverX_ * Constants::CELL_SIZE;
    const int py = hoverY_ * Constants::CELL_SIZE;
    renderer.drawRectangle(Rectangle(static_cast<float>(px), static_cast<float>(py),
                                     static_cast<float>(Constants::CELL_SIZE),
                                     static_cast<float>(Constants::CELL_SIZE)),
                           COLOR_HOVER,
                           false);
}

void ConstructionState::renderCursor(IRenderer& renderer) {
    const int px = cursorX_ * Constants::CELL_SIZE;
    const int py = cursorY_ * Constants::CELL_SIZE;
    renderer.drawRectangle(Rectangle(static_cast<float>(px), static_cast<float>(py),
                                     static_cast<float>(Constants::CELL_SIZE),
                                     static_cast<float>(Constants::CELL_SIZE)),
                           COLOR_CURSOR,
                           false);
}

void ConstructionState::renderSidebar(IRenderer& renderer) {
    renderer.drawText("CONSTRUCTION", Vector2(SIDEBAR_X, SIDEBAR_Y), Constants::COLOR_WHITE, 16);

    int y = SIDEBAR_Y + 22;
    renderer.drawText("关卡: " + std::to_string(levelNumber_), Vector2(SIDEBAR_X, static_cast<float>(y)), Constants::COLOR_WHITE, 14);
    y += 18;
    renderer.drawText(std::string("模式: ") + (mode_ == EditorMode::Terrain ? "地形" : "敌人"),
                      Vector2(SIDEBAR_X, static_cast<float>(y)), Constants::COLOR_GRAY, 14);
    y += 18;
    renderer.drawText(std::string("光标: ") + std::to_string(cursorX_) + "," + std::to_string(cursorY_),
                      Vector2(SIDEBAR_X, static_cast<float>(y)), Constants::COLOR_GRAY, 14);
    y += 18;

    renderer.drawText(std::string("笔刷: ") + terrainName(brushType_), Vector2(SIDEBAR_X, static_cast<float>(y)), Constants::COLOR_WHITE, 14);
    y += 22;

    // Brush preview
    renderer.drawRectangle(Rectangle(static_cast<float>(SIDEBAR_X), static_cast<float>(y), 18.0f, 18.0f),
                           terrainColor(brushType_),
                           true);
    renderer.drawRectangle(Rectangle(static_cast<float>(SIDEBAR_X), static_cast<float>(y), 18.0f, 18.0f),
                           COLOR_GRID,
                           false);
    y += 28;

    renderer.drawText("0 空  1 钢", Vector2(SIDEBAR_X, static_cast<float>(y)), Constants::COLOR_GRAY, 12);
    y += 14;
    renderer.drawText("2 砖  3 水", Vector2(SIDEBAR_X, static_cast<float>(y)), Constants::COLOR_GRAY, 12);
    y += 14;
    renderer.drawText("4 草  Shift=2x2", Vector2(SIDEBAR_X, static_cast<float>(y)), Constants::COLOR_GRAY, 12);
    y += 22;

    renderer.drawText("鼠标左键绘制/右键擦除", Vector2(SIDEBAR_X, static_cast<float>(y)), Constants::COLOR_GRAY, 12);
    y += 14;
    renderer.drawText("Tab: 切换模式", Vector2(SIDEBAR_X, static_cast<float>(y)), Constants::COLOR_GRAY, 12);
    y += 22;

    if (mode_ == EditorMode::EnemyList && level_) {
        const auto& enemies = level_->getEnemySpawnList();

        renderer.drawText("敌人序列:", Vector2(SIDEBAR_X, static_cast<float>(y)), Constants::COLOR_WHITE, 12);
        y += 14;
        renderer.drawText("[] 选中  1-4 类型  P 奖励", Vector2(SIDEBAR_X, static_cast<float>(y)), Constants::COLOR_GRAY, 12);
        y += 16;

        std::string line;
        line.reserve(enemies.size() * 2);
        for (size_t i = 0; i < enemies.size(); ++i) {
            const int v = static_cast<int>(enemies[i].type) + 1;
            line += std::to_string(v);
            line += enemies[i].hasPowerUp ? "*" : " ";
        }
        renderer.drawText(line, Vector2(SIDEBAR_X, static_cast<float>(y)), Constants::COLOR_GRAY, 12);
        y += 16;

        if (!enemies.empty()) {
            const int selected = std::clamp(selectedEnemyIndex_, 0, static_cast<int>(enemies.size() - 1));
            const auto& sel = enemies[static_cast<size_t>(selected)];
            std::string selText = "选中 #" + std::to_string(selected) +
                                  " = " + std::to_string(static_cast<int>(sel.type) + 1) +
                                  (sel.hasPowerUp ? " (BONUS)" : "");
            renderer.drawText(selText, Vector2(SIDEBAR_X, static_cast<float>(y)), Constants::COLOR_WHITE, 12);
            y += 18;
        }
    }

    renderer.drawText("S: 保存到 Level_custom", Vector2(SIDEBAR_X, static_cast<float>(y)), Constants::COLOR_GRAY, 12);
    y += 14;
    renderer.drawText("Ctrl+S: 覆盖 Level_N", Vector2(SIDEBAR_X, static_cast<float>(y)), Constants::COLOR_GRAY, 12);
    y += 14;
    renderer.drawText("F5: 保存并试玩(可F6返回)", Vector2(SIDEBAR_X, static_cast<float>(y)), Constants::COLOR_GRAY, 12);
    y += 14;
    renderer.drawText("L: 重新加载  N: 新建", Vector2(SIDEBAR_X, static_cast<float>(y)), Constants::COLOR_GRAY, 12);
    y += 14;
    renderer.drawText("PgUp/PgDn: 切关", Vector2(SIDEBAR_X, static_cast<float>(y)), Constants::COLOR_GRAY, 12);
    y += 20;

    if (!lastSavedPath_.empty()) {
        renderer.drawText("Last: " + lastSavedPath_, Vector2(SIDEBAR_X, static_cast<float>(y)), Constants::COLOR_GRAY, 12);
        y += 16;
    }

    if (!statusText_.empty()) {
        renderer.drawText(statusText_, Vector2(static_cast<float>(Constants::GAME_WIDTH + 4),
                                              static_cast<float>(Constants::WINDOW_HEIGHT - 18)),
                          Constants::COLOR_WHITE,
                          12);
    }
}

const char* ConstructionState::terrainName(TerrainType type) {
    switch (type) {
        case TerrainType::Empty: return "空";
        case TerrainType::Steel: return "钢";
        case TerrainType::Brick: return "砖";
        case TerrainType::Water: return "水";
        case TerrainType::Grass: return "草";
        case TerrainType::Base: return "基地";
        default: return "未知";
    }
}

} // namespace tank
