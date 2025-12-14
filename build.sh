#!/bin/bash
#
# AstroUniverse Build Script
#

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/astrouni2026/build"

echo "=== AstroUniverse Build Script ==="
echo "Working directory: $SCRIPT_DIR"
echo ""

# Clean old build
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning old build directory..."
    rm -rf "$BUILD_DIR"
fi

# Create build directory
echo "Creating build directory..."
mkdir -p "$BUILD_DIR"

# Configure with CMake
echo ""
echo "=== Configuring with CMake ==="
cd "$BUILD_DIR"
cmake .. || {
    echo ""
    echo "ERROR: CMake configuration failed!"
    exit 1
}

# Build
echo ""
echo "=== Building ==="
make -j$(nproc) || {
    echo ""
    echo "ERROR: Build failed!"
    exit 1
}

# Success
echo ""
echo "=== Build Successful! ==="
echo "Executable: $BUILD_DIR/AstroUniverse"
echo ""
echo "Run with: cd $BUILD_DIR && ./AstroUniverse"
