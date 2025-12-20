# Windows 编译指南

## 方案 A：Windows 本地编译（推荐）

### 1. 安装依赖

1. **安装 MSYS2**：https://www.msys2.org/

2. **打开 MSYS2 UCRT64 终端**，运行：
   ```bash
   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake
   pacman -S mingw-w64-ucrt-x86_64-SDL2 mingw-w64-ucrt-x86_64-SDL2_image
   pacman -S mingw-w64-ucrt-x86_64-SDL2_mixer mingw-w64-ucrt-x86_64-SDL2_ttf
   ```

### 2. 编译项目

```bash
cd /path/to/TanksCpp
mkdir build-win && cd build-win
cmake .. -G "MinGW Makefiles"
cmake --build . --config Release
```

### 3. 运行

编译完成后，`TankGame.exe` 在 `build-win/` 目录下。

---

## 方案 B：Visual Studio 编译

### 1. 安装依赖

1. **安装 Visual Studio 2022**（选择 C++ 桌面开发工作负载）
2. **安装 CMake**：https://cmake.org/download/
3. **下载 SDL2 开发库**（VC 版本）：
   - SDL2: https://github.com/libsdl-org/SDL/releases
   - SDL2_image: https://github.com/libsdl-org/SDL_image/releases
   - SDL2_mixer: https://github.com/libsdl-org/SDL_mixer/releases
   - SDL2_ttf: https://github.com/libsdl-org/SDL_ttf/releases

### 2. 配置环境变量

设置 `SDL2_DIR` 环境变量指向 SDL2 解压目录。

### 3. 编译

```cmd
cd TanksCpp
mkdir build-vs && cd build-vs
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

---

## 方案 C：Linux 交叉编译

### 1. 安装工具链

```bash
# Ubuntu/Debian
sudo apt install mingw-w64

# CentOS/RHEL/Alinux
sudo yum install mingw64-gcc mingw64-gcc-c++
```

### 2. 下载 SDL2 mingw 开发包

```bash
mkdir -p deps/win64 && cd deps/win64
curl -LO https://github.com/libsdl-org/SDL/releases/download/release-2.30.10/SDL2-devel-2.30.10-mingw.tar.gz
curl -LO https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.4/SDL2_image-devel-2.8.4-mingw.tar.gz
curl -LO https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.8.0/SDL2_mixer-devel-2.8.0-mingw.tar.gz
curl -LO https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.22.0/SDL2_ttf-devel-2.22.0-mingw.tar.gz

# 解压
for f in *.tar.gz; do tar xzf "$f"; done
```

### 3. 交叉编译

```bash
cd TanksCpp
mkdir build-mingw && cd build-mingw
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-mingw64.cmake
make -j4
```

---

## 运行时 DLL

Windows 可执行文件需要以下 DLL（从 SDL2 bin 目录复制）：
- SDL2.dll
- SDL2_image.dll
- SDL2_mixer.dll
- SDL2_ttf.dll
- libpng16-16.dll（如有）
- zlib1.dll（如有）
