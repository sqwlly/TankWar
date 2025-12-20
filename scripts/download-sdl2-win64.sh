#!/bin/bash
# Download SDL2 Windows libraries for cross-compilation
# Usage: ./scripts/download-sdl2-win64.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
DEPS_DIR="$PROJECT_ROOT/deps/win64"

# SDL2 versions
SDL2_VERSION="2.30.10"
SDL2_IMAGE_VERSION="2.8.4"
SDL2_MIXER_VERSION="2.8.0"
SDL2_TTF_VERSION="2.22.0"

# Download URLs
SDL2_URL="https://github.com/libsdl-org/SDL/releases/download/release-${SDL2_VERSION}/SDL2-devel-${SDL2_VERSION}-mingw.tar.gz"
SDL2_IMAGE_URL="https://github.com/libsdl-org/SDL_image/releases/download/release-${SDL2_IMAGE_VERSION}/SDL2_image-devel-${SDL2_IMAGE_VERSION}-mingw.tar.gz"
SDL2_MIXER_URL="https://github.com/libsdl-org/SDL_mixer/releases/download/release-${SDL2_MIXER_VERSION}/SDL2_mixer-devel-${SDL2_MIXER_VERSION}-mingw.tar.gz"
SDL2_TTF_URL="https://github.com/libsdl-org/SDL_ttf/releases/download/release-${SDL2_TTF_VERSION}/SDL2_ttf-devel-${SDL2_TTF_VERSION}-mingw.tar.gz"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

download_and_extract() {
    local name="$1"
    local url="$2"
    local version="$3"
    local filename="${name}-devel-${version}-mingw.tar.gz"
    local extract_dir="${name}-${version}"

    if [ -d "$DEPS_DIR/$extract_dir" ]; then
        log_warn "$name-$version already exists, skipping..."
        return 0
    fi

    log_info "Downloading $name $version..."

    if command -v curl &> /dev/null; then
        curl -L -o "/tmp/$filename" "$url"
    elif command -v wget &> /dev/null; then
        wget -O "/tmp/$filename" "$url"
    else
        log_error "Neither curl nor wget found. Please install one of them."
        exit 1
    fi

    log_info "Extracting $name..."
    tar -xzf "/tmp/$filename" -C "$DEPS_DIR"
    rm -f "/tmp/$filename"

    log_info "$name $version downloaded and extracted successfully."
}

main() {
    echo "=========================================="
    echo "  SDL2 Windows Libraries Downloader"
    echo "=========================================="
    echo ""

    # Check for required tools
    if ! command -v tar &> /dev/null; then
        log_error "tar is required but not installed."
        exit 1
    fi

    # Create deps directory
    mkdir -p "$DEPS_DIR"
    log_info "Dependencies directory: $DEPS_DIR"
    echo ""

    # Download libraries
    download_and_extract "SDL2" "$SDL2_URL" "$SDL2_VERSION"
    download_and_extract "SDL2_image" "$SDL2_IMAGE_URL" "$SDL2_IMAGE_VERSION"
    download_and_extract "SDL2_mixer" "$SDL2_MIXER_URL" "$SDL2_MIXER_VERSION"
    download_and_extract "SDL2_ttf" "$SDL2_TTF_URL" "$SDL2_TTF_VERSION"

    echo ""
    echo "=========================================="
    log_info "All SDL2 libraries downloaded successfully!"
    echo ""
    echo "Directory structure:"
    ls -la "$DEPS_DIR"
    echo ""
    echo "You can now run: ./scripts/cross-build.sh"
    echo "=========================================="
}

main "$@"
