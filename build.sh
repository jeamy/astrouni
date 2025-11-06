#!/bin/bash
# Build Script für Linux/macOS
# AstroUniverse - Modern Astrology Software

set -e  # Exit bei Fehler

# Farben für Output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}═══════════════════════════════════════════${NC}"
echo -e "${GREEN}  AstroUniverse Build Script${NC}"
echo -e "${GREEN}═══════════════════════════════════════════${NC}"
echo ""

# Prüfe Voraussetzungen
echo -e "${YELLOW}Prüfe Voraussetzungen...${NC}"

if ! command -v cmake &> /dev/null; then
    echo -e "${RED}Fehler: CMake nicht gefunden!${NC}"
    echo "Installation:"
    echo "  Ubuntu/Debian: sudo apt-get install cmake"
    echo "  macOS:         brew install cmake"
    exit 1
fi

if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo -e "${RED}Fehler: C++ Compiler nicht gefunden!${NC}"
    echo "Installation:"
    echo "  Ubuntu/Debian: sudo apt-get install build-essential"
    echo "  macOS:         xcode-select --install"
    exit 1
fi

echo -e "${GREEN}✓ CMake gefunden: $(cmake --version | head -1)${NC}"
if command -v g++ &> /dev/null; then
    echo -e "${GREEN}✓ GCC gefunden: $(g++ --version | head -1)${NC}"
elif command -v clang++ &> /dev/null; then
    echo -e "${GREEN}✓ Clang gefunden: $(clang++ --version | head -1)${NC}"
fi
echo ""

# Build-Typ
BUILD_TYPE="${1:-Release}"
echo -e "${YELLOW}Build-Typ: ${BUILD_TYPE}${NC}"
echo ""

# Build-Verzeichnis erstellen
echo -e "${YELLOW}Erstelle Build-Verzeichnis...${NC}"
mkdir -p build
cd build

# CMake konfigurieren
echo -e "${YELLOW}Konfiguriere CMake...${NC}"
cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..

if [ $? -ne 0 ]; then
    echo -e "${RED}CMake-Konfiguration fehlgeschlagen!${NC}"
    exit 1
fi
echo -e "${GREEN}✓ CMake-Konfiguration erfolgreich${NC}"
echo ""

# Kompilieren
echo -e "${YELLOW}Kompiliere Projekt...${NC}"
NPROC=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
cmake --build . -j${NPROC}

if [ $? -ne 0 ]; then
    echo -e "${RED}Kompilierung fehlgeschlagen!${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Kompilierung erfolgreich${NC}"
echo ""

# Zusammenfassung
echo -e "${GREEN}═══════════════════════════════════════════${NC}"
echo -e "${GREEN}  Build erfolgreich abgeschlossen!${NC}"
echo -e "${GREEN}═══════════════════════════════════════════${NC}"
echo ""
echo "Ausführbare Dateien:"
echo "  - ./example_chart       (Beispielprogramm)"
echo "  - ./tests/astro_tests   (Unit Tests)"
echo ""
echo "Nächste Schritte:"
echo "  ./test.sh               (Tests ausführen)"
echo "  ./example_chart         (Demo starten)"
echo ""
