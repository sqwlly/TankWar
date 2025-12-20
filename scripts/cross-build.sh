#!/bin/bash
# Cross-compile TankWar for Windows x64
# Usage: ./scripts/cross-build.sh [--package]

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build-win64"
RELEASE_DIR="$PROJECT_ROOT/release"
DEPS_DIR="$PROJECT_ROOT/deps/win64"

# SDL2 versions (must match download script)
SDL2_VERSION="2.30.10"
SDL2_IMAGE_VERSION="2.8.4"
SDL2_MIXER_VERSION="2.8.0"
SDL2_TTF_VERSION="2.22.0"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

log_info() { echo -e "${GREEN}[INFO]${NC} $1"; }
log_warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }
log_step() { echo -e "${CYAN}[STEP]${NC} $1"; }

check_prerequisites() {
    log_step "Checking prerequisites..."

    # Check MinGW
    if ! command -v x86_64-w64-mingw32-g++ &> /dev/null; then
        log_error "MinGW-w64 not found. Please install:"
        echo "  Ubuntu/Debian: sudo apt install mingw-w64"
        echo "  Fedora:        sudo dnf install mingw64-gcc-c++"
        echo "  Arch:          sudo pacman -S mingw-w64-gcc"
        exit 1
    fi
    log_info "MinGW-w64: $(x86_64-w64-mingw32-g++ --version | head -n1)"

    # Check CMake
    if ! command -v cmake &> /dev/null; then
        log_error "CMake not found. Please install cmake."
        exit 1
    fi
    log_info "CMake: $(cmake --version | head -n1)"

    # Check SDL2 dependencies
    if [ ! -d "$DEPS_DIR/SDL2-$SDL2_VERSION" ]; then
        log_error "SDL2 Windows libraries not found."
        echo "Please run: ./scripts/download-sdl2-win64.sh"
        exit 1
    fi
    log_info "SDL2 libraries found in deps/win64/"
}

configure_build() {
    log_step "Configuring CMake..."

    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"

    cmake "$PROJECT_ROOT" \
        -DCMAKE_TOOLCHAIN_FILE="$PROJECT_ROOT/cmake/Toolchain-mingw64.cmake" \
        -DCMAKE_BUILD_TYPE=Release

    cd "$PROJECT_ROOT"
}

build_project() {
    log_step "Building project..."

    cd "$BUILD_DIR"
    make -j$(nproc)
    cd "$PROJECT_ROOT"

    if [ -f "$BUILD_DIR/TankGame.exe" ]; then
        log_info "Build successful: $BUILD_DIR/TankGame.exe"
    else
        log_error "Build failed: TankGame.exe not found"
        exit 1
    fi
}

package_release() {
    log_step "Packaging release..."

    local PACKAGE_DIR="$RELEASE_DIR/TankWar-win64"

    # Clean and create release directory
    rm -rf "$PACKAGE_DIR"
    mkdir -p "$PACKAGE_DIR"

    # Copy executable
    cp "$BUILD_DIR/TankGame.exe" "$PACKAGE_DIR/"

    # Copy assets
    cp -r "$PROJECT_ROOT/assets" "$PACKAGE_DIR/"

    # Copy SDL2 DLLs
    local SDL2_BIN="$DEPS_DIR/SDL2-$SDL2_VERSION/x86_64-w64-mingw32/bin"
    local SDL2_IMAGE_BIN="$DEPS_DIR/SDL2_image-$SDL2_IMAGE_VERSION/x86_64-w64-mingw32/bin"
    local SDL2_MIXER_BIN="$DEPS_DIR/SDL2_mixer-$SDL2_MIXER_VERSION/x86_64-w64-mingw32/bin"
    local SDL2_TTF_BIN="$DEPS_DIR/SDL2_ttf-$SDL2_TTF_VERSION/x86_64-w64-mingw32/bin"

    cp "$SDL2_BIN/"*.dll "$PACKAGE_DIR/" 2>/dev/null || true
    cp "$SDL2_IMAGE_BIN/"*.dll "$PACKAGE_DIR/" 2>/dev/null || true
    cp "$SDL2_MIXER_BIN/"*.dll "$PACKAGE_DIR/" 2>/dev/null || true
    cp "$SDL2_TTF_BIN/"*.dll "$PACKAGE_DIR/" 2>/dev/null || true

    # Copy MinGW runtime DLLs
    local MINGW_BIN="/usr/x86_64-w64-mingw32/sys-root/mingw/bin"
    if [ -d "$MINGW_BIN" ]; then
        cp "$MINGW_BIN/libwinpthread-1.dll" "$PACKAGE_DIR/" 2>/dev/null || true
        cp "$MINGW_BIN/libgcc_s_seh-1.dll" "$PACKAGE_DIR/" 2>/dev/null || true
        cp "$MINGW_BIN/libstdc++-6.dll" "$PACKAGE_DIR/" 2>/dev/null || true
    fi

    # List DLLs
    log_info "Copied DLLs:"
    ls -1 "$PACKAGE_DIR/"*.dll 2>/dev/null | xargs -n1 basename | sed 's/^/  - /'

    # Create ZIP
    cd "$RELEASE_DIR"
    rm -f TankWar-win64.zip
    zip -r TankWar-win64.zip TankWar-win64/
    cd "$PROJECT_ROOT"

    log_info "Package created: $RELEASE_DIR/TankWar-win64.zip"

    # Show package contents
    echo ""
    echo "Package contents:"
    unzip -l "$RELEASE_DIR/TankWar-win64.zip" | tail -n +4 | head -n -2
}

show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --package    Build and create release package"
    echo "  --clean      Clean build directory before building"
    echo "  --help       Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0              # Just build"
    echo "  $0 --package    # Build and package"
    echo "  $0 --clean      # Clean build"
}

main() {
    local do_package=false
    local do_clean=false

    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --package)
                do_package=true
                shift
                ;;
            --clean)
                do_clean=true
                shift
                ;;
            --help|-h)
                show_usage
                exit 0
                ;;
            *)
                log_error "Unknown option: $1"
                show_usage
                exit 1
                ;;
        esac
    done

    echo "=========================================="
    echo "  TankWar Cross-Compiler (Linux → Win64)"
    echo "=========================================="
    echo ""

    # Clean if requested
    if [ "$do_clean" = true ]; then
        log_step "Cleaning build directory..."
        rm -rf "$BUILD_DIR"
    fi

    check_prerequisites
    configure_build
    build_project

    if [ "$do_package" = true ]; then
        package_release
    fi

    echo ""
    echo "=========================================="
    log_info "Build completed successfully!"
    echo ""
    echo "Output: $BUILD_DIR/TankGame.exe"
    if [ "$do_package" = true ]; then
        echo "Package: $RELEASE_DIR/TankWar-win64.zip"
    fi
    echo "=========================================="
}

main "$@"
