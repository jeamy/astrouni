#!/bin/bash
# Swiss Ephemeris Ephemeriden-Dateien herunterladen

set -e

EPHE_DIR="./ephe"
BASE_URL="https://www.astro.com/ftp/swisseph/ephe"

echo "=== Swiss Ephemeris Ephemeriden Download ==="
echo "Zielverzeichnis: $EPHE_DIR"
echo ""

mkdir -p "$EPHE_DIR"
cd "$EPHE_DIR"

# Kompakt-Dateien (18 Jahrhunderte: 1800-3000 AD)
echo "Lade Kompakt-Ephemeriden (1800-3000 AD)..."

files=(
    "seas_18.se1"   # Hauptplaneten (Sonne, Merkur, Venus, Mars)
    "semo_18.se1"   # Mond
    "sepl_18.se1"   # Äußere Planeten (Jupiter, Saturn, Uranus, Neptun, Pluto)
)

for file in "${files[@]}"; do
    if [ -f "$file" ]; then
        echo "  ✓ $file bereits vorhanden"
    else
        echo "  → Lade $file..."
        curl -# -L -o "$file" "$BASE_URL/$file" || {
            echo "  ✗ Fehler beim Download von $file"
            exit 1
        }
        echo "  ✓ $file heruntergeladen"
    fi
done

echo ""
echo "=== Optional: Erweiterte Ephemeriden ==="
echo "Für größeren Zeitraum (600 BC - 5400 AD):"
echo ""
echo "  curl -L -o seplm54.se1 $BASE_URL/seplm54.se1  # ~50 MB"
echo "  curl -L -o seasm54.se1 $BASE_URL/seasm54.se1  # ~30 MB"
echo "  curl -L -o semom54.se1 $BASE_URL/semom54.se1  # ~20 MB"
echo ""

echo "=== Fixsterne (optional) ==="
echo "  curl -L -o sefstars.txt $BASE_URL/sefstars.txt"
echo ""

echo "=== Download abgeschlossen ==="
ls -lh *.se1 2>/dev/null || echo "Keine .se1 Dateien gefunden"
echo ""
echo "Verwendung im Code:"
echo "  swe_set_ephe_path(\"$(pwd)\");"
