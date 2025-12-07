#!/bin/bash
#
# AstroUniverse 2026 - Build Script
# Unterstützt: Linux, macOS
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$SCRIPT_DIR/astrouni2026"
BUILD_TYPE="${1:-Release}"

echo "========================================"
echo "  AstroUniverse 2026 Build"
echo "========================================"
echo "Build Type: $BUILD_TYPE"
echo ""

# Prüfen ob CMake installiert ist
if ! command -v cmake &> /dev/null; then
    echo "FEHLER: CMake nicht gefunden!"
    echo ""
    echo "Installation:"
    echo "  Ubuntu/Debian: sudo apt install cmake"
    echo "  macOS:         brew install cmake"
    exit 1
fi

# Prüfen ob Qt6 installiert ist
if ! cmake --find-package -DNAME=Qt6 -DCOMPILER_ID=GNU -DLANGUAGE=CXX -DMODE=EXIST &> /dev/null; then
    echo "WARNUNG: Qt6 möglicherweise nicht gefunden."
    echo ""
    echo "Installation:"
    echo "  Ubuntu/Debian: sudo apt install qt6-base-dev qt6-tools-dev"
    echo "  macOS:         brew install qt@6"
    echo ""
fi

# Build-Verzeichnis erstellen
BUILD_DIR="$PROJECT_DIR/build"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# CMake konfigurieren
echo "Konfiguriere CMake..."
cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" ..

# Bauen
echo ""
echo "Baue Projekt..."
NPROC=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
cmake --build . -j"$NPROC"

echo ""
echo "========================================"
echo "  Build erfolgreich!"
echo "========================================"
echo ""
echo "Ausführen:"
echo "  cd $BUILD_DIR"
echo "  ./astrouni2026"
echo ""
echo "Tests:"
echo "  cd $BUILD_DIR"
echo "  ctest --output-on-failure"
echo ""
