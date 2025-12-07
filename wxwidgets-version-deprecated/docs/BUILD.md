# Build-Anleitung: AstroUniverse

## Voraussetzungen

### System-Pakete

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    libgtest-dev
```

**Fedora/RHEL:**
```bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    git \
    gtest-devel
```

**macOS:**
```bash
brew install cmake googletest
```

### Swiss Ephemeris

Die Swiss Ephemeris Library sollte bereits im `swisseph/` Verzeichnis vorhanden sein.

Falls nicht, siehe `SETUP_SWISSEPH.md` für Installationsanleitung.

## Build-Prozess

### 1. Repository klonen (falls noch nicht geschehen)

```bash
git clone <repository-url>
cd astrouni
```

### 2. Build-Verzeichnis erstellen

```bash
mkdir build
cd build
```

### 3. CMake konfigurieren

```bash
cmake ..
```

**Optionen:**
- `-DBUILD_TESTS=OFF` - Tests deaktivieren
- `-DCMAKE_BUILD_TYPE=Release` - Release-Build (optimiert)
- `-DCMAKE_BUILD_TYPE=Debug` - Debug-Build (mit Symbolen)

**Beispiel:**
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

### 4. Kompilieren

```bash
cmake --build . -j$(nproc)
```

Oder mit Make:
```bash
make -j$(nproc)
```

### 5. Tests ausführen (optional)

```bash
ctest --output-on-failure
```

Oder direkt:
```bash
./astro_tests
```

## Beispielprogramm ausführen

```bash
./example_chart
```

Mit eigenem Ephemeriden-Pfad:
```bash
./example_chart /pfad/zu/ephe
```

## Ausgabe-Dateien

Nach erfolgreichem Build:

```
build/
├── libastro_core.a          # Core Library (statisch)
├── example_chart            # Beispielprogramm
└── astro_tests              # Unit Tests
```

## Installation (optional)

```bash
sudo cmake --install .
```

Installiert nach:
- `/usr/local/bin/example_chart`
- `/usr/local/share/astro_universe/ephe/` (Ephemeriden)

## Troubleshooting

### Swiss Ephemeris nicht gefunden

```
CMake Error: Could not find SWISSEPH_LIB
```

**Lösung:**
1. Prüfe ob `swisseph/lib/libswe.a` existiert
2. Falls nicht, siehe `SETUP_SWISSEPH.md`
3. Oder setze manuell:
   ```bash
   cmake -DSWISSEPH_LIB=/pfad/zu/libswe.a \
         -DSWISSEPH_INCLUDE=/pfad/zu/include ..
   ```

### Google Test nicht gefunden

```
CMake Warning: Google Test not found
```

**Lösung:**
- CMake lädt automatisch Google Test herunter (FetchContent)
- Oder installiere systemweit: `sudo apt-get install libgtest-dev`

### Ephemeriden-Files fehlen

```
Swiss Ephemeris error: file not found
```

**Lösung:**
1. Prüfe ob `data/ephe/*.se1` existiert
2. Falls nicht:
   ```bash
   cd swisseph
   ./download_ephe.sh
   ```

### Compiler-Fehler C++20

```
error: 'concept' does not name a type
```

**Lösung:**
- Stelle sicher, dass GCC 10+ oder Clang 10+ verwendet wird
- Oder reduziere auf C++17:
  ```bash
  cmake -DCMAKE_CXX_STANDARD=17 ..
  ```

## Clean Build

```bash
cd build
rm -rf *
cmake ..
cmake --build .
```

## Cross-Compilation (fortgeschritten)

### Für Windows (mit MinGW)

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/mingw-w64.cmake ..
```

### Für ARM (Raspberry Pi)

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-linux.cmake ..
```

## Performance-Optimierung

### Release-Build mit LTO

```bash
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON ..
```

### Mit Profiling

```bash
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
```

## Entwickler-Modus

### Mit AddressSanitizer (Memory-Leaks finden)

```bash
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_CXX_FLAGS="-fsanitize=address -g" ..
```

### Mit Clang-Tidy (Code-Analyse)

```bash
cmake -DCMAKE_CXX_CLANG_TIDY="clang-tidy;-checks=*" ..
```

## IDE-Integration

### CLion

1. Öffne Projekt-Root in CLion
2. CMake wird automatisch konfiguriert
3. Build-Konfiguration auswählen
4. Run/Debug wie gewohnt

### VSCode

1. Installiere CMake Tools Extension
2. Öffne Projekt-Root
3. `Ctrl+Shift+P` → "CMake: Configure"
4. `Ctrl+Shift+P` → "CMake: Build"

---

**Bei Problemen:** Siehe `PORTIERUNGSPLAN.md` oder erstelle ein Issue.
