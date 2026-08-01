# TankWar 玩法扩展开发计划

> 状态：已实现并验证 · 制定日期：2026-08-01 · 完成日期：2026-08-01
> 验收修复（2026-08-01）：Spade 地形同步与几何偏移、音效接线、AI/波次行为级测试、HI-SCORE 位置、结算界面接入过关流程、补齐 Level_19/Level_20。
> 依据：当前代码库实际结构（引用均标注文件路径），与 `docs/DESIGN_BASELINE.md` 的产品边界一致（不做玩法大改、KISS、可回归）。

## 1. 背景与现状

代码具备完整框架（战役/生存/双人/关卡编辑器），本计划识别的功能缺口已完成：

- 7 种道具均已实现，效果由 `PlayingState` 分派以支持全场规则
- 闪光敌人按可配置权重掉落 7 种道具；Bomb 走正常击杀计数与计分，但不触发连锁掉落
- `ProgressStore` 已兼容读写 Java 遗留 `progress.properties`，并保留未知键
- 4 类敌人使用不同策略，生存模式使用速度/装甲/混合三波次
- 道具图标继续采用程序化渲染，避免引入未经确认授权的外部素材

## 2. 路线图总览

| 阶段 | 内容 | 优先级 | 工作量 | 依赖 |
|------|------|--------|--------|------|
| P1 | 补全 6 种道具 + 掉落随机化 | 最高 | 中 | 已完成 |
| P2 | 最高分/关卡进度持久化 | 高 | 小 | 已完成 |
| P3 | 敌人 AI 差异化 + 生存波次特色 | 中 | 中 | 已完成 |
| P4 | 连杀/多杀奖励计分 | 中 | 小 | 已完成 |
| P5 | 设置菜单（音量/难度/按键说明） | 低 | 中 | 已完成 |

建议执行顺序：P1 → P2 先行（乐趣收益最大），P3/P4 其次，P5 收尾。

## 3. P1 — 道具补全

### 3.1 行为定义与默认数值

| 道具 | 行为 | 默认值 | 现成挂钩 |
|------|------|--------|----------|
| Star | 坦克升一级 | 已有 | `PlayerTank::upgrade()` |
| Gun | 直接升至满级 | level=3 | `PlayerTank::upgrade()` 循环 |
| IronCap | 无敌（含护盾特效） | 8 秒 | `PlayerTank::makeInvincible(duration)` |
| StopWatch | 冻结所有敌人（移动+开火停摆，出生计时同步暂停） | 8 秒 | `PlayingState::enemies_` 更新门控 |
| Bomb | 销毁当前场上全部敌人 | 计入击杀数与分数；不触发连锁掉落 | `PlayingState::enemies_` 遍历 |
| Tank | 拾取玩家 +1 命 | 上限 9 | `PlayingState::player1Lives_/player2Lives_` |
| Spade | 基地周围 8 格变为钢墙，超时还原为砖墙 | 20 秒 | `Level::setTerrainAt` + `Base::getPosition` |

数值常量统一放 `Constants.hpp`（如 `POWERUP_FREEZE_DURATION` 等），便于调整与测试引用。

### 3.2 掉落类型随机化

- 位置：`PlayingState.cpp:495` 处改为按权重随机
- 默认权重：Star 30 / IronCap 15 / StopWatch 15 / Gun 10 / Bomb 10 / Tank 10 / Spade 10
- 权重表放 `Constants.hpp`；随机源复用项目现有 `std::mt19937` 用法（参考 `EnemyTank::randomFire`）

### 3.3 实现步骤（文件级）

1. `include/utils/Constants.hpp`：新增道具时长、生命上限、掉落权重常量
2. `PowerUpManager::tryCollect`：返回 `std::optional<PowerUpType>` 替代 bool，效果分派上移到 `PlayingState`（全场效果需要访问 enemies_/lives_/level_，玩家自身效果仍可直接调用 PlayerTank）
3. `include/states/PlayingState.hpp` + `src/states/PlayingState.cpp`：
   - 新增 `applyPowerUp(PlayerTank&, PowerUpType)` 分派
   - 新增 `freezeTimer_`：敌人更新与开火循环的门控
   - 新增 `baseFortifyTimer_` + `fortifiedCells_`：Spade 改格记录与还原
   - Bomb：遍历 `enemies_` 置死（走正常死亡流程产生爆炸特效，置 `carriesPowerUp=false` 防连锁）
4. `src/entities/powerups/PowerUp.cpp`：为 6 种类型补程序化图标渲染（图集无贴图，延续程序化策略：简单几何+字母区分），并做闪烁基座统一风格
5. 新增 `tests/unit/PowerUpEffectsTest.cpp`：SDL-free 验证每种道具的应用逻辑（冻结门控、Bomb 清场、Spade 改格与还原、Tank 加命上限、Gun 满级）

### 3.4 验收标准

- 新增单测全绿，`ctest` 全量通过（当前基线 90/90）
- 手动冒烟：每种道具拾取后效果可见、时长结束后状态正确恢复

## 4. P2 — 持久化

1. 新增 `include/utils/ProgressStore.hpp` / `src/utils/ProgressStore.cpp`：读写 `assets/progress.properties`（键沿用 `highScore`、`levelToPlay`，与 Java 遗留格式兼容）
2. `MenuState`：标题下方显示 `HI-SCORE`
3. `GameStateManager::changeToStage` / 战役胜利流程：通关解锁 `levelToPlay`；Game Over 时更新 `highScore`
4. （可选）主菜单战役项变为"继续第 N 关"
5. 新增 `tests/unit/ProgressStoreTest.cpp`：临时文件读写往返、缺文件默认值

## 5. P3 — 敌人 AI 差异化 + 生存波次

- `src/ai/`：按 `EnemyType` 分派策略——Basic 维持随机游走；Fast 以基地为目标寻路；Power 保持中距离停射；Heavy 直线压向基地
- `EnemyWaveGenerator`（已有 `Difficulty` 枚举）：波次特色化（纯快速波/装甲波/混合波），每波强度曲线写入常量
- 测试：给定简化地图断言决策方向/目标，SDL-free

## 6. P4 — 连杀/多杀奖励

- 单次伤害事件击杀 ≥2 敌人时按倍率加分（x2/x3）
- HUD 浮字反馈（复用 Effect/动画系统），`ScoreState` 结算展示
- 依赖 P1 完成后击杀入口统一（Bomb 清场不计连杀倍率，避免刷分）

## 7. P5 — 设置菜单

- 主菜单新增 `SETTINGS` 项（菜单框架刚重构，加项成本低）：音量调节（`IAudioPlayer`）、难度（敌人速度/血量倍率，作用于 `EnemyWaveGenerator` 与关卡加载）、按键说明页
- 设置项经 `ProgressStore` 落盘

## 8. 工程规范（每阶段通用）

- 流程：实现 → 单测（优先 SDL-free，参考 `tests/unit/SpawnTerrainCollisionTest.cpp` 模式）→ `ctest --test-dir build-tests` 全绿 → 主程序构建 + 冒烟
- 命名/风格遵循 `AGENTS.md`；提交信息 `type: summary`
- 贴图缺失一律程序化渲染，不引外部素材；如确需新素材，用脚本生成（参考 `scripts/generate_logo.py`）
- 每阶段完成后更新本文件状态标记

## 9. 风险与开放问题

- **Bomb 击杀计分**：计入击杀数会影响关卡结束条件（杀满 20 过关），需在 P1 实现时明确走同一计数入口
- **Spade 还原**：若还原时某格已被炸毁/被编辑器改动，默认直接写回砖墙；与 ConstructionState 的关卡数据保持一致格式
- **冻结范围**：出生中/无敌中的敌人是否冻结——默认全部冻结（简单一致）
- **双人归属**：Tank 道具给拾取者 +1 命；IronCap 仅作用拾取者
- **存档兼容**：`progress.properties` 被旧 Java 版使用时注意键冲突，写入前备份非我方键值
