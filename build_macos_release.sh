#!/bin/bash
# AstroUniverse 2026 - macOS Release Build
# Prüft/installiert Abhängigkeiten, baut Release, erstellt Minimal-Dist

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$SCRIPT_DIR/astrouni2026"
BUILD_DIR="$PROJECT_DIR/build-macos-release"
DIST_DIR="$PROJECT_DIR/dist/macos"
BUILD_TYPE="Release"

echo "=== AstroUniverse 2026 - macOS Release Build ==="
echo ""

#==============================================================================
# [0] Abhängigkeiten prüfen / installieren
#==============================================================================

MISSING_DEPS=""

# Xcode Command Line Tools (liefert clang++)
if ! command -v clang++ &>/dev/null; then
  MISSING_DEPS="$MISSING_DEPS xcode-cli"
fi

# CMake
if ! command -v cmake &>/dev/null; then
  MISSING_DEPS="$MISSING_DEPS cmake"
fi

# Qt6 (prüfen über qmake6 oder brew --prefix qt@6)
QT6_OK=0
if command -v qmake6 &>/dev/null; then
  QT6_OK=1
elif [ -d "$(brew --prefix qt@6 2>/dev/null)/lib/cmake/Qt6" ]; then
  QT6_OK=1
  # Qt6 Pfad für CMake setzen
  export CMAKE_PREFIX_PATH="$(brew --prefix qt@6)"
fi
if [ "$QT6_OK" -eq 0 ]; then
  MISSING_DEPS="$MISSING_DEPS qt6"
fi

# Falls Abhängigkeiten fehlen
if [ -n "$MISSING_DEPS" ]; then
  echo "Fehlende Abhängigkeiten:$MISSING_DEPS"
  echo ""
  
  # Prüfen ob Homebrew verfügbar
  if command -v brew &>/dev/null; then
    echo "Versuche automatische Installation via Homebrew..."
    for dep in $MISSING_DEPS; do
      case "$dep" in
        xcode-cli)
          echo "Installiere Xcode Command Line Tools..."
          xcode-select --install 2>/dev/null || echo "(bereits installiert oder manuell bestätigen)"
          ;;
        cmake)
          echo "brew install cmake"
          brew install cmake
          ;;
        qt6)
          echo "brew install qt@6"
          brew install qt@6
          export CMAKE_PREFIX_PATH="$(brew --prefix qt@6)"
          ;;
      esac
    done
    echo ""
  else
    # Kein Homebrew - Hinweise ausgeben
    echo "Automatische Installation nicht möglich (Homebrew nicht gefunden)."
    echo ""
    echo "Bitte installiere Homebrew und dann die Abhängigkeiten:"
    echo ""
    echo "  /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
    echo ""
    echo "Danach:"
    echo "  xcode-select --install   # Xcode Command Line Tools"
    echo "  brew install cmake qt@6"
    exit 1
  fi
fi

# Qt6 Pfad für CMake (falls über Homebrew installiert)
if [ -z "$CMAKE_PREFIX_PATH" ] && [ -d "$(brew --prefix qt@6 2>/dev/null)/lib/cmake/Qt6" ]; then
  export CMAKE_PREFIX_PATH="$(brew --prefix qt@6)"
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
NPROC=$(sysctl -n hw.ncpu 2>/dev/null || echo 4)
cmake --build "$BUILD_DIR" -j"$NPROC" --config "$BUILD_TYPE"

#==============================================================================
# [3] Dist-Verzeichnis erstellen (Minimalversion)
#==============================================================================
echo ""
echo "[3/3] Dist-Verzeichnis erstellen..."
rm -rf "$DIST_DIR"
mkdir -p "$DIST_DIR"

# Binärdatei kopieren (Single-Config-Generator: direkt im Build-Dir)
if [ -f "$BUILD_DIR/astrouni2026" ]; then
  cp "$BUILD_DIR/astrouni2026" "$DIST_DIR/"
else
  # Multi-Config (z.B. Xcode): im Unterordner Release/
  if [ -f "$BUILD_DIR/$BUILD_TYPE/astrouni2026" ]; then
    cp "$BUILD_DIR/$BUILD_TYPE/astrouni2026" "$DIST_DIR/"
  else
    echo "FEHLER: Binärdatei astrouni2026 wurde nicht gefunden." >&2
    exit 1
  fi
fi

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

# Qt-Frameworks für portable App bündeln
APP_BUNDLE="$DIST_DIR/AstroUniverse2026.app"
mkdir -p "$APP_BUNDLE/Contents/MacOS"
mkdir -p "$APP_BUNDLE/Contents/Resources"
mkdir -p "$APP_BUNDLE/Contents/Frameworks"
mkdir -p "$APP_BUNDLE/Contents/PlugIns"

# Binary ins Bundle verschieben (falls noch nicht dort)
if [ -f "$DIST_DIR/astrouni2026" ]; then
  mv "$DIST_DIR/astrouni2026" "$APP_BUNDLE/Contents/MacOS/astrouni2026"
fi

# Icon optional kopieren
ICON_SRC="$PROJECT_DIR/resources/icons/astrouni.icns"
if [ -f "$ICON_SRC" ]; then
  cp "$ICON_SRC" "$APP_BUNDLE/Contents/Resources/astrouni.icns"
fi

# Minimal Info.plist
cat > "$APP_BUNDLE/Contents/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>CFBundleName</key><string>AstroUniverse 2026</string>
  <key>CFBundleDisplayName</key><string>AstroUniverse 2026</string>
  <key>CFBundleExecutable</key><string>astrouni2026</string>
  <key>CFBundleIdentifier</key><string>com.astro.universe2026</string>
  <key>CFBundleVersion</key><string>1.0</string>
  <key>CFBundleShortVersionString</key><string>1.0</string>
  <key>LSMinimumSystemVersion</key><string>10.15.0</string>
  <key>CFBundleIconFile</key><string>astrouni</string>
</dict>
</plist>
EOF

if command -v macdeployqt &>/dev/null; then
  echo "Bündele Qt-Frameworks mit macdeployqt..."
  macdeployqt "$APP_BUNDLE" -verbose=1 || {
    echo "WARNUNG: macdeployqt konnte die Qt-Frameworks nicht bündeln." >&2
  }
elif command -v qtpaths6 &>/dev/null; then
  echo "Bündele Qt-Frameworks manuell (Fallback ohne macdeployqt)..."
  QT_PREFIX="$(qtpaths6 --install-prefix 2>/dev/null || true)"
  QT_PLUGIN_DIR="$(qtpaths6 --plugin-dir 2>/dev/null || true)"
  QT_QML_DIR="$(qtpaths6 --qml-dir 2>/dev/null || true)"
  
  # Frameworks kopieren
  if [ -d "$QT_PREFIX/Frameworks" ]; then
    rsync -a "$QT_PREFIX/Frameworks/Qt6"*.framework "$APP_BUNDLE/Contents/Frameworks/" 2>/dev/null || true
  fi
  
  # Plugins kopieren
  for sub in platforms styles imageformats iconengines; do
    if [ -d "$QT_PLUGIN_DIR/$sub" ]; then
      mkdir -p "$APP_BUNDLE/Contents/PlugIns/$sub"
      cp -R "$QT_PLUGIN_DIR/$sub"/Qt6*.dylib "$APP_BUNDLE/Contents/PlugIns/$sub"/ 2>/dev/null || true
    fi
  done
  
  # qt.conf damit Qt lokale Pfade nutzt
  cat > "$APP_BUNDLE/Contents/Resources/qt.conf" <<EOF
[Paths]
Prefix=..
Plugins=PlugIns
Imports=.
Qml2Imports=.
EOF
  
  # RPATH für Binary setzen
  if command -v install_name_tool &>/dev/null; then
    install_name_tool -add_rpath "@executable_path/../Frameworks" "$APP_BUNDLE/Contents/MacOS/astrouni2026" || true
  fi
else
  echo "Hinweis: Weder macdeployqt noch qtpaths6 gefunden – Qt-Frameworks werden nicht mitgeliefert."
  echo "Installiere entweder macdeployqt (Qt@6) oder qtpaths6, um eine portable App zu erzeugen."
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
echo "(ggf. per Rechtsklick → Öffnen, falls Gatekeeper anschlägt)"
echo ""
