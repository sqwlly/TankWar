#!/bin/bash
# Download SDL2 Windows libraries for cross-compilation
# Usage: ./scripts/download-sdl2-win64.sh [--mirror]

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
DEPS_DIR="$PROJECT_ROOT/deps/win64"

# SDL2 versions
SDL2_VERSION="2.30.10"
SDL2_IMAGE_VERSION="2.8.4"
SDL2_MIXER_VERSION="2.8.0"
SDL2_TTF_VERSION="2.22.0"

# Mirror prefix (for China mainland acceleration)
MIRROR_PREFIX=""

# GitHub raw URLs
GITHUB_SDL2="https://github.com/libsdl-org/SDL/releases/download/release-${SDL2_VERSION}/SDL2-devel-${SDL2_VERSION}-mingw.tar.gz"
GITHUB_SDL2_IMAGE="https://github.com/libsdl-org/SDL_image/releases/download/release-${SDL2_IMAGE_VERSION}/SDL2_image-devel-${SDL2_IMAGE_VERSION}-mingw.tar.gz"
GITHUB_SDL2_MIXER="https://github.com/libsdl-org/SDL_mixer/releases/download/release-${SDL2_MIXER_VERSION}/SDL2_mixer-devel-${SDL2_MIXER_VERSION}-mingw.tar.gz"
GITHUB_SDL2_TTF="https://github.com/libsdl-org/SDL_ttf/releases/download/release-${SDL2_TTF_VERSION}/SDL2_ttf-devel-${SDL2_TTF_VERSION}-mingw.tar.gz"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

log_info() { echo -e "${GREEN}[INFO]${NC} $1"; }
log_warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

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

    # Apply mirror prefix if set
    local final_url="$url"
    if [ -n "$MIRROR_PREFIX" ]; then
        final_url="${MIRROR_PREFIX}${url}"
    fi

    log_info "Downloading $name $version..."
    log_info "URL: $final_url"

    if command -v curl &> /dev/null; then
        curl -L --progress-bar -o "/tmp/$filename" "$final_url"
    elif command -v wget &> /dev/null; then
        wget --show-progress -O "/tmp/$filename" "$final_url"
    else
        log_error "Neither curl nor wget found. Please install one of them."
        exit 1
    fi

    log_info "Extracting $name..."
    tar -xzf "/tmp/$filename" -C "$DEPS_DIR"
    rm -f "/tmp/$filename"

    log_info "$name $version downloaded and extracted successfully."
}

show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --mirror, -m     Use GitHub mirror for faster download (China mainland)"
    echo "  --mirror=URL     Use custom mirror prefix"
    echo "  --help, -h       Show this help message"
    echo ""
    echo "Available mirrors:"
    echo "  ghproxy:    https://ghproxy.net/ (default)"
    echo "  ghproxy2:   https://mirror.ghproxy.com/"
    echo ""
    echo "Examples:"
    echo "  $0                           # Direct download from GitHub"
    echo "  $0 --mirror                  # Use default mirror (ghproxy)"
    echo "  $0 --mirror=https://xxx/     # Use custom mirror"
}

main() {
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --mirror|-m)
                MIRROR_PREFIX="https://ghproxy.net/"
                shift
                ;;
            --mirror=*)
                MIRROR_PREFIX="${1#*=}"
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
    echo "  SDL2 Windows Libraries Downloader"
    echo "=========================================="
    echo ""

    if [ -n "$MIRROR_PREFIX" ]; then
        log_info "Using mirror: ${CYAN}${MIRROR_PREFIX}${NC}"
    else
        log_info "Direct download from GitHub"
        log_warn "If download is slow, try: $0 --mirror"
    fi
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
    download_and_extract "SDL2" "$GITHUB_SDL2" "$SDL2_VERSION"
    download_and_extract "SDL2_image" "$GITHUB_SDL2_IMAGE" "$SDL2_IMAGE_VERSION"
    download_and_extract "SDL2_mixer" "$GITHUB_SDL2_MIXER" "$SDL2_MIXER_VERSION"
    download_and_extract "SDL2_ttf" "$GITHUB_SDL2_TTF" "$SDL2_TTF_VERSION"

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
