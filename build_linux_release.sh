#!/bin/bash
# AstroUniverse 2026 - Linux Release Build
# Prüft/installiert Abhängigkeiten, baut Release, erstellt Minimal-Dist

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$SCRIPT_DIR/astrouni2026"
BUILD_DIR="$PROJECT_DIR/build-linux-release"
DIST_DIR="$PROJECT_DIR/dist/linux"
BUILD_TYPE="Release"

echo "=== AstroUniverse 2026 - Linux Release Build ==="
echo ""

#==============================================================================
# [0] Abhängigkeiten prüfen / installieren
#==============================================================================

MISSING_DEPS=""

# CMake
if ! command -v cmake &>/dev/null; then
  MISSING_DEPS="$MISSING_DEPS cmake"
fi

# C++ Compiler (g++ oder clang++)
if ! command -v g++ &>/dev/null && ! command -v clang++ &>/dev/null; then
  MISSING_DEPS="$MISSING_DEPS g++"
fi

# Qt6 (prüfen über qmake6 oder pkg-config)
QT6_OK=0
if command -v qmake6 &>/dev/null; then
  QT6_OK=1
elif pkg-config --exists Qt6Core 2>/dev/null; then
  QT6_OK=1
fi
if [ "$QT6_OK" -eq 0 ]; then
  MISSING_DEPS="$MISSING_DEPS qt6-base-dev"
fi

# Falls Abhängigkeiten fehlen: versuche automatische Installation (apt)
if [ -n "$MISSING_DEPS" ]; then
  echo "Fehlende Abhängigkeiten:$MISSING_DEPS"
  echo ""
  
  # Prüfen ob apt verfügbar
  if command -v apt-get &>/dev/null; then
    echo "Versuche automatische Installation via apt..."
    # Mapping auf Paketnamen
    APT_PKGS=""
    for dep in $MISSING_DEPS; do
      case "$dep" in
        cmake)        APT_PKGS="$APT_PKGS cmake" ;;
        g++)          APT_PKGS="$APT_PKGS g++" ;;
        qt6-base-dev) APT_PKGS="$APT_PKGS qt6-base-dev qt6-tools-dev libgl1-mesa-dev" ;;
      esac
    done
    
    echo "sudo apt-get update && sudo apt-get install -y$APT_PKGS"
    sudo apt-get update && sudo apt-get install -y $APT_PKGS || {
      echo ""
      echo "FEHLER: Automatische Installation fehlgeschlagen."
      echo "Bitte installiere manuell:"
      echo "  sudo apt-get install$APT_PKGS"
      exit 1
    }
    echo ""
  else
    # Kein apt - Hinweise ausgeben
    echo "Automatische Installation nicht möglich (kein apt gefunden)."
    echo ""
    echo "Bitte installiere die folgenden Pakete manuell:"
    echo ""
    for dep in $MISSING_DEPS; do
      case "$dep" in
        cmake)        echo "  - cmake" ;;
        g++)          echo "  - g++ (oder clang++)" ;;
        qt6-base-dev) echo "  - Qt6 Entwicklungspakete (qt6-base-dev, qt6-tools-dev)" ;;
      esac
    done
    echo ""
    echo "Fedora/RHEL:  sudo dnf install cmake gcc-c++ qt6-qtbase-devel"
    echo "Arch Linux:   sudo pacman -S cmake gcc qt6-base"
    echo "openSUSE:     sudo zypper install cmake gcc-c++ qt6-base-devel"
    exit 1
  fi
fi

echo "Alle Abhängigkeiten vorhanden."
echo ""

#==============================================================================
# [1] CMake konfigurieren
#==============================================================================
echo "[1/3] CMake konfigurieren..."
cmake -S "$PROJECT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

#==============================================================================
# [2] Bauen
#==============================================================================
echo ""
echo "[2/3] Bauen..."
NPROC=$(nproc 2>/dev/null || echo 4)
cmake --build "$BUILD_DIR" -j"$NPROC"

#==============================================================================
# [3] Dist-Verzeichnis erstellen (Minimalversion)
#==============================================================================
echo ""
echo "[3/3] Dist-Verzeichnis erstellen..."
rm -rf "$DIST_DIR"
mkdir -p "$DIST_DIR"

# Binärdatei kopieren
cp "$BUILD_DIR/astrouni2026" "$DIST_DIR/" || {
  echo "FEHLER: Binärdatei astrouni2026 wurde nicht gefunden." >&2
  exit 1
}

# MINIMAL erforderliche Daten-Dateien:
#  - astroorg.dat, astroger.dat, europa.dat  (Orte - mind. eine muss existieren)
#  - default.dat                             (Standard-Orben)
# NICHT nötig für Start: astronam.dat, astroini.dat, astrorad.dat, astronot.dat
mkdir -p "$DIST_DIR/data"
for f in astroorg.dat astroger.dat europa.dat default.dat; do
  if [ -f "$PROJECT_DIR/data/$f" ]; then
    cp "$PROJECT_DIR/data/$f" "$DIST_DIR/data/"
  fi
done

# Swiss Ephemeris Dateien (PFLICHT für Planetenberechnung)
#   - sepl_18.se1  (Planeten 1800-2400)
#   - semo_18.se1  (Mond 1800-2400)
#   - seas_18.se1  (Asteroiden 1800-2400)
mkdir -p "$DIST_DIR/swisseph/ephe"
if [ -d "$PROJECT_DIR/swisseph/ephe" ]; then
  echo "Kopiere Swiss Ephemeris Dateien..."
  cp "$PROJECT_DIR/swisseph/ephe/"*.se1 "$DIST_DIR/swisseph/ephe/" 2>/dev/null || true
else
  echo "WARNUNG: Swiss Ephemeris Dateien nicht gefunden unter $PROJECT_DIR/swisseph/ephe"
fi

 # Ressourcen (Icons etc.) - optional aber empfohlen
 if [ -d "$PROJECT_DIR/resources" ]; then
   mkdir -p "$DIST_DIR/resources"
   cp -r "$PROJECT_DIR/resources/." "$DIST_DIR/resources/"
 fi

# Qt-Libs für portable App bündeln (optional, wenn linuxdeployqt vorhanden)
if command -v linuxdeployqt &>/dev/null; then
  echo "Bündele Qt-Libs mit linuxdeployqt..."
  DESKTOP_FILE="$DIST_DIR/astrouni2026.desktop"
  cat > "$DESKTOP_FILE" <<EOF
[Desktop Entry]
Type=Application
Name=AstroUniverse 2026
Exec=astrouni2026
Icon=astrouni
Categories=Education;
EOF
  ICON_SRC="$PROJECT_DIR/resources/icons/astrouni.png"
  if [ -f "$ICON_SRC" ]; then
    cp "$ICON_SRC" "$DIST_DIR/astrouni.png"
  fi
  linuxdeployqt "$DESKTOP_FILE" -bundle-non-qt-libs -executable="$DIST_DIR/astrouni2026" || {
    echo "WARNUNG: linuxdeployqt konnte die Qt-Libs nicht bündeln." >&2
  }
elif command -v qtpaths6 &>/dev/null && command -v patchelf &>/dev/null; then
  echo "Bündele Qt-Libs manuell (Fallback ohne linuxdeployqt)..."
  QT_PLUGIN_DIR="$(qtpaths6 --plugin-dir 2>/dev/null || true)"
  QT_PREFIX="$(qtpaths6 --install-prefix 2>/dev/null || true)"
  QT_LIB_DIR="$QT_PREFIX/lib"
  
  mkdir -p "$DIST_DIR/lib" "$DIST_DIR/plugins"
  
  # Qt-abhängige libs des Binaries kopieren
  for lib in $(ldd "$DIST_DIR/astrouni2026" | awk '/Qt6/ {print $3}'); do
    [ -f "$lib" ] && cp -L "$lib" "$DIST_DIR/lib/"
  done
  
  # Wichtige Plugin-Ordner kopieren
  for sub in platforms styles imageformats xcbglintegrations iconengines; do
    if [ -d "$QT_PLUGIN_DIR/$sub" ]; then
      mkdir -p "$DIST_DIR/plugins/$sub"
      cp -L "$QT_PLUGIN_DIR/$sub"/*.so "$DIST_DIR/plugins/$sub"/ 2>/dev/null || true
    fi
  done
  
  # qt.conf damit Qt im lokalen plugins-Verzeichnis sucht
  cat > "$DIST_DIR/qt.conf" <<EOF
[Paths]
Prefix=.
Plugins=plugins
Imports=imports
Qml2Imports=qml
EOF
  
  # RPATH auf lokales lib-Verzeichnis setzen
  patchelf --set-rpath '$ORIGIN/lib' "$DIST_DIR/astrouni2026" || true
  for lib in "$DIST_DIR/lib/"*.so*; do
    patchelf --set-rpath '$ORIGIN' "$lib" || true
  done
else
  echo "Hinweis: Weder linuxdeployqt noch (qtpaths6+patchelf) gefunden – Qt-Libs werden nicht mitgeliefert."
  echo "Installiere entweder linuxdeployqt oder qtpaths6 + patchelf, um eine portable App zu erzeugen."
fi

# Release-Zip im dist-Verzeichnis erstellen (optional)
ZIP_NAME="astrouni2026-linux-release.zip"
if command -v zip &>/dev/null; then
  echo ""
  echo "Erzeuge Release-Zip: $ZIP_NAME"
  (
    cd "$PROJECT_DIR/dist" || exit 1
    rm -f "$ZIP_NAME"
    zip -r "$ZIP_NAME" "linux" >/dev/null
  )
  echo "Release-Zip erstellt: $PROJECT_DIR/dist/$ZIP_NAME"
else
  echo ""
  echo "Hinweis: 'zip' nicht gefunden, Release-Zip wird nicht erstellt."
  echo "Bitte installiere das Paket 'zip' (z.B. sudo apt install zip)."
fi

 echo ""
 echo "========================================"
 echo "  Release-Build fertig!"
 echo "========================================"
echo ""
echo "Start:"
echo "  cd $DIST_DIR"
echo "  ./astrouni2026"
echo ""
