# Linux 交叉编译到 Windows 指南

本文档介绍如何在 Linux 系统上交叉编译 TankWar 到 Windows 平台。

## 目录

- [前置条件](#前置条件)
- [安装工具链](#安装工具链)
- [下载 SDL2 库](#下载-sdl2-库)
- [编译项目](#编译项目)
- [打包发布](#打包发布)
- [常见问题](#常见问题)

---

## 前置条件

- Linux 系统 (Ubuntu/Debian/Fedora/Arch)
- CMake 3.16+
- 网络连接 (下载 SDL2 库)

## 安装工具链

### Ubuntu/Debian

```bash
sudo apt update
sudo apt install mingw-w64 mingw-w64-tools
```

### Fedora

```bash
sudo dnf install mingw64-gcc mingw64-gcc-c++
```

### Arch Linux

```bash
sudo pacman -S mingw-w64-gcc
```

### 验证安装

```bash
x86_64-w64-mingw32-g++ --version
# 应输出类似: x86_64-w64-mingw32-g++ (GCC) 12.x.x
```

## 下载 SDL2 库

需要下载 Windows 版本的 SDL2 开发库 (MinGW 版)。

### 自动下载脚本

```bash
# 在项目根目录执行
./scripts/download-sdl2-win64.sh

# 国内用户使用镜像加速 (推荐)
./scripts/download-sdl2-win64.sh --mirror
```

### 手动下载

从 [GitHub SDL Releases](https://github.com/libsdl-org/SDL/releases) 下载以下库的 `mingw` 版本：

| 库 | 版本 | 下载链接 |
|----|------|---------|
| SDL2 | 2.30.10 | [SDL2-devel-2.30.10-mingw.tar.gz](https://github.com/libsdl-org/SDL/releases/download/release-2.30.10/SDL2-devel-2.30.10-mingw.tar.gz) |
| SDL2_image | 2.8.4 | [SDL2_image-devel-2.8.4-mingw.tar.gz](https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.4/SDL2_image-devel-2.8.4-mingw.tar.gz) |
| SDL2_mixer | 2.8.0 | [SDL2_mixer-devel-2.8.0-mingw.tar.gz](https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.8.0/SDL2_mixer-devel-2.8.0-mingw.tar.gz) |
| SDL2_ttf | 2.22.0 | [SDL2_ttf-devel-2.22.0-mingw.tar.gz](https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.22.0/SDL2_ttf-devel-2.22.0-mingw.tar.gz) |

下载后解压到 `deps/win64/` 目录：

```bash
mkdir -p deps/win64
cd deps/win64

# 解压 (以 SDL2 为例)
tar -xzf SDL2-devel-2.30.10-mingw.tar.gz
tar -xzf SDL2_image-devel-2.8.4-mingw.tar.gz
tar -xzf SDL2_mixer-devel-2.8.0-mingw.tar.gz
tar -xzf SDL2_ttf-devel-2.22.0-mingw.tar.gz
```

解压后目录结构：

```
deps/win64/
├── SDL2-2.30.10/
│   └── x86_64-w64-mingw32/
│       ├── bin/          # DLL 文件
│       ├── include/      # 头文件
│       └── lib/          # 静态库
├── SDL2_image-2.8.4/
├── SDL2_mixer-2.8.0/
└── SDL2_ttf-2.22.0/
```

## 编译项目

### 使用编译脚本 (推荐)

```bash
./scripts/cross-build.sh
```

### 手动编译

```bash
# 创建构建目录
mkdir -p build-win64
cd build-win64

# 配置 CMake (使用 toolchain 文件)
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-mingw64.cmake \
    -DCMAKE_BUILD_TYPE=Release

# 编译
make -j$(nproc)
```

编译成功后，`TankGame.exe` 将生成在 `build-win64/` 目录。

## 打包发布

### 自动打包

```bash
./scripts/cross-build.sh --package
```

### 手动打包

将以下文件复制到发布目录：

```bash
mkdir -p release/TankWar-win64

# 复制可执行文件
cp build-win64/TankGame.exe release/TankWar-win64/

# 复制资源文件
cp -r assets release/TankWar-win64/

# 复制 SDL2 DLL 文件
cp deps/win64/SDL2-2.30.10/x86_64-w64-mingw32/bin/*.dll release/TankWar-win64/
cp deps/win64/SDL2_image-2.8.4/x86_64-w64-mingw32/bin/*.dll release/TankWar-win64/
cp deps/win64/SDL2_mixer-2.8.0/x86_64-w64-mingw32/bin/*.dll release/TankWar-win64/
cp deps/win64/SDL2_ttf-2.22.0/x86_64-w64-mingw32/bin/*.dll release/TankWar-win64/

# 打包
cd release
zip -r TankWar-win64.zip TankWar-win64/
```

### 发布目录结构

```
TankWar-win64/
├── TankGame.exe
├── assets/
│   ├── audio/
│   ├── images/
│   ├── levels/
│   └── joystix.ttf
├── SDL2.dll
├── SDL2_image.dll
├── SDL2_mixer.dll
├── SDL2_ttf.dll
└── (其他依赖 DLL)
```

## 常见问题

### Q: 找不到 `x86_64-w64-mingw32-g++`

确保已安装 MinGW-w64 工具链：

```bash
# Ubuntu/Debian
sudo apt install mingw-w64
```

### Q: CMake 找不到 SDL2

检查 `deps/win64/` 目录结构是否正确，确保版本号与 `Toolchain-mingw64.cmake` 中的路径匹配。

如需修改版本，编辑 `cmake/Toolchain-mingw64.cmake`：

```cmake
set(SDL2_DIR "${CMAKE_SOURCE_DIR}/deps/win64/SDL2-<版本号>/x86_64-w64-mingw32")
```

### Q: 运行时提示缺少 DLL

确保所有必需的 DLL 与 `TankGame.exe` 在同一目录。必需的 DLL：

- `SDL2.dll`
- `SDL2_image.dll`
- `SDL2_mixer.dll`
- `SDL2_ttf.dll`
- `libpng16-16.dll` (SDL2_image 依赖)
- `zlib1.dll` (SDL2_image 依赖)
- `libogg-0.dll` (SDL2_mixer 依赖)
- `libvorbis-0.dll` (SDL2_mixer 依赖)
- `libfreetype-6.dll` (SDL2_ttf 依赖)

### Q: 32 位 Windows 支持

修改 toolchain 文件使用 `i686-w64-mingw32` 编译器，并下载对应的 32 位 SDL2 库。

### Q: Windows 控制台窗口

编译时添加 `-mwindows` 链接选项可隐藏控制台：

```cmake
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
```

---

## 参考资料

- [MinGW-w64 官网](https://www.mingw-w64.org/)
- [SDL2 官方文档](https://wiki.libsdl.org/)
- [CMake 交叉编译文档](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html)
