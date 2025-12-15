#!/bin/bash
#
# AstroUniverse Build Script
#

set -euo pipefail  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="$SCRIPT_DIR/astrouni2026"
BUILD_DIR="$SOURCE_DIR/build"

CLEAN=0
BUILD_TYPE="Release"
JOBS="$(nproc)"

usage() {
    echo "Usage: $0 [--clean] [--debug|--release] [--jobs N]"
}

while [ $# -gt 0 ]; do
    case "$1" in
        --clean|-c)
            CLEAN=1
            shift
            ;;
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --release)
            BUILD_TYPE="Release"
            shift
            ;;
        --jobs|-j)
            if [ $# -lt 2 ]; then
                usage
                exit 1
            fi
            JOBS="$2"
            shift 2
            ;;
        --help|-h)
            usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

echo "=== AstroUniverse Build Script ==="
echo "Working directory: $SCRIPT_DIR"
echo "Source directory:  $SOURCE_DIR"
echo "Build directory:   $BUILD_DIR"
echo "Build type:        $BUILD_TYPE"
echo "Jobs:              $JOBS"
echo ""

if [ "$CLEAN" -eq 1 ] && [ -d "$BUILD_DIR" ]; then
    echo "Cleaning old build directory..."
    rm -rf "$BUILD_DIR"
fi

echo "Creating build directory..."
mkdir -p "$BUILD_DIR"

echo ""
echo "=== Configuring with CMake ==="
cmake -S "$SOURCE_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" || {
    echo ""
    echo "ERROR: CMake configuration failed!"
    exit 1
}

echo ""
echo "=== Building ==="
cmake --build "$BUILD_DIR" --parallel "$JOBS" || {
    echo ""
    echo "ERROR: Build failed!"
    exit 1
}

echo ""
echo "=== Build Successful! ==="

if [ -x "$BUILD_DIR/astrouni2026" ]; then
    echo "Executable: $BUILD_DIR/astrouni2026"
    echo ""
    echo "Run with: cd $BUILD_DIR && ./astrouni2026"
elif [ -x "$BUILD_DIR/AstroUniverse" ]; then
    echo "Executable: $BUILD_DIR/AstroUniverse"
    echo ""
    echo "Run with: cd $BUILD_DIR && ./AstroUniverse"
else
    echo "Executable: (not found in $BUILD_DIR)"
fi
