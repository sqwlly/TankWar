# TankWar 音频素材

`music/` 中是可循环播放的背景音乐，`sfx/` 中是一次性音效。所有文件均为 44.1 kHz、16-bit、立体声 WAV，使用仓库内的确定性程序生成脚本创建：

```text
python scripts/generate_audio_assets.py
```

## 背景音乐

- `music/menu_theme.wav`：菜单主题，约 8.6 秒，可循环。
- `music/battle_theme.wav`：战斗主题，约 12 秒，可循环。

## 音效

- `sfx/menu_move.wav`、`sfx/menu_confirm.wav`：菜单导航和确认。
- `sfx/tank_shoot.wav`、`sfx/tank_hit.wav`：坦克射击和受击。
- `sfx/player_damage.wav`：玩家坦克受击。
- `sfx/tank_explosion.wav`：坦克爆炸。
- `sfx/powerup_collect.wav`：拾取道具。
- `sfx/brick_break.wav`：砖墙破坏。
- `sfx/player_damage.wav`：玩家受伤提示。

这些素材未使用第三方采样或网络下载内容；运行时会由 CMake 的资源复制步骤自动带到可执行文件旁的 `assets/audio/` 目录。
