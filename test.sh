#!/bin/bash
# Test Script für Linux/macOS
# AstroUniverse - Modern Astrology Software

set -e

# Farben
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${GREEN}═══════════════════════════════════════════${NC}"
echo -e "${GREEN}  AstroUniverse Test Suite${NC}"
echo -e "${GREEN}═══════════════════════════════════════════${NC}"
echo ""

# Prüfe ob Build existiert
if [ ! -d "build" ]; then
    echo -e "${RED}Fehler: Build-Verzeichnis nicht gefunden!${NC}"
    echo "Bitte zuerst ./build.sh ausführen"
    exit 1
fi

cd build

# Prüfe ob Tests existieren
if [ ! -f "tests/astro_tests" ]; then
    echo -e "${RED}Fehler: Test-Binary nicht gefunden!${NC}"
    echo "Bitte zuerst ./build.sh ausführen"
    exit 1
fi

# Tests ausführen
echo -e "${YELLOW}Führe Unit Tests aus...${NC}"
echo ""

./tests/astro_tests

TEST_RESULT=$?

echo ""
if [ $TEST_RESULT -eq 0 ]; then
    echo -e "${GREEN}═══════════════════════════════════════════${NC}"
    echo -e "${GREEN}  ✓ Alle Tests bestanden!${NC}"
    echo -e "${GREEN}═══════════════════════════════════════════${NC}"
else
    echo -e "${RED}═══════════════════════════════════════════${NC}"
    echo -e "${RED}  ✗ Tests fehlgeschlagen!${NC}"
    echo -e "${RED}═══════════════════════════════════════════${NC}"
    exit 1
fi

# Optional: CTest ausführen
if command -v ctest &> /dev/null; then
    echo ""
    echo -e "${YELLOW}Führe CTest aus...${NC}"
    ctest --output-on-failure
fi

echo ""
echo -e "${GREEN}Test-Suite erfolgreich abgeschlossen!${NC}"
