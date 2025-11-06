#!/bin/bash
# Swiss Ephemeris Ephemeriden Download Script
# Lädt die benötigten Ephemeriden-Files von GitHub herunter

set -e

EPHE_DIR="./ephe"
# GitHub Raw URL (funktioniert zuverlässig)
BASE_URL="https://raw.githubusercontent.com/aloistr/swisseph/master/ephe"

echo "=== Swiss Ephemeris Ephemeriden Download ==="
echo "Quelle: GitHub (aloistr/swisseph)"
echo "Zielverzeichnis: $EPHE_DIR"
echo ""

# Erstelle Verzeichnis falls nicht vorhanden
mkdir -p "$EPHE_DIR"

# Kompakt-Ephemeriden (1800-3000 AD, ~2 MB total)
echo "Lade Kompakt-Ephemeriden (1800-3000 AD)..."

FILES=(
    "seas_18.se1"  # Planeten (~218 KB)
    "semo_18.se1"  # Mond (~1.3 MB)
    "sepl_18.se1"  # Äußere Planeten (~473 KB)
)

for file in "${FILES[@]}"; do
    if [ -f "$EPHE_DIR/$file" ]; then
        # Prüfe ob Datei gültig ist (nicht HTML)
        if file "$EPHE_DIR/$file" | grep -q "data"; then
            echo "  ✓ $file bereits vorhanden und gültig"
        else
            echo "  ⚠ $file ist korrupt, lade neu..."
            rm -f "$EPHE_DIR/$file"
            curl -L -o "$EPHE_DIR/$file" "$BASE_URL/$file"
        fi
    else
        echo "  → Lade $file..."
        curl -L -o "$EPHE_DIR/$file" "$BASE_URL/$file"
    fi
done

echo ""
echo "=== Optional: Erweiterte Ephemeriden ==="
echo "Für größeren Zeitraum (600 BC - 5400 AD):"
echo ""
echo "  cd $EPHE_DIR"
echo "  curl -L -O $BASE_URL/seplm54.se1  # ~50 MB"
echo "  curl -L -O $BASE_URL/seasm54.se1  # ~30 MB"
echo "  curl -L -O $BASE_URL/semom54.se1  # ~20 MB"
echo ""
echo "=== Fixsterne (optional) ==="
echo "  curl -L -O $BASE_URL/sefstars.txt"
echo ""

echo "=== Download abgeschlossen ==="
ls -lh *.se1 2>/dev/null || echo "Keine .se1 Dateien gefunden"
echo ""
echo "Verwendung im Code:"
echo "  swe_set_ephe_path(\"$(pwd)\");"
