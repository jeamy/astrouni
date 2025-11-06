#!/bin/bash
# GUI Start Script für Linux/macOS
# AstroUniverse - Modern Astrology Software

set -e

# Farben
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${GREEN}═══════════════════════════════════════════${NC}"
echo -e "${GREEN}  AstroUniverse GUI${NC}"
echo -e "${GREEN}═══════════════════════════════════════════${NC}"
echo ""

# Prüfe ob GUI existiert
if [ ! -f "build/gui/astro_gui" ]; then
    echo -e "${RED}Fehler: GUI nicht gefunden!${NC}"
    echo "Bitte zuerst ./build.sh ausführen"
    exit 1
fi

# Prüfe Display
if [ -z "$DISPLAY" ]; then
    echo -e "${YELLOW}Warnung: Keine Display-Variable gesetzt${NC}"
    echo "GUI kann in Docker-Umgebung ohne X11 nicht gestartet werden"
    echo ""
    echo "Für lokalen Test:"
    echo "  1. Projekt auf lokale Maschine kopieren"
    echo "  2. ./build.sh ausführen"
    echo "  3. ./run_gui.sh ausführen"
    exit 1
fi

# Starte GUI
echo -e "${YELLOW}Starte wxWidgets GUI...${NC}"
cd build/gui
./astro_gui
