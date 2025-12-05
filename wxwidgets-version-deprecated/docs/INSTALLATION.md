# Installation & Setup

## System-Anforderungen

- **Compiler**: GCC 10+ oder Clang 10+ (C++20 Support)
- **Build-System**: CMake 3.20+ oder Make
- **Betriebssystem**: Linux, macOS, Windows (MinGW/MSVC)

## Schritt-für-Schritt Installation

### 1. Abhängigkeiten installieren

#### Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    libgtest-dev
```

#### Fedora/RHEL/CentOS

```bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    git \
    gtest-devel
```

#### macOS

```bash
# Xcode Command Line Tools
xcode-select --install

# Homebrew Pakete
brew install cmake googletest
```

#### Windows

**Option 1: MSYS2/MinGW**
```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake
```

**Option 2: Visual Studio**
- Visual Studio 2019+ mit C++ Desktop Development
- CMake über Visual Studio Installer

### 2. Swiss Ephemeris prüfen

```bash
cd astrouni
ls -la swisseph/lib/libswe.a
ls -la data/ephe/*.se1
```

Falls Dateien fehlen:
```bash
cd swisseph
./download_ephe.sh
```

Siehe auch: `SETUP_SWISSEPH.md`

### 3. Projekt bauen

#### Mit CMake (empfohlen)

```bash
mkdir build
cd build
cmake ..
cmake --build . -j$(nproc)
```

#### Ohne CMake (Makefile)

Falls CMake nicht verfügbar, erstelle ein einfaches Makefile:

```bash
# Siehe unten für Makefile-Beispiel
make -j$(nproc)
```

### 4. Tests ausführen

```bash
cd build
./astro_tests
```

### 5. Beispielprogramm ausführen

```bash
./example_chart
```

## Alternative: Makefile-basierter Build (ohne CMake)

Falls CMake nicht installiert werden kann, hier ein einfaches Makefile:

```makefile
# Makefile für AstroUniverse (ohne CMake)

CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2 -Icore/include -Iswisseph/include
LDFLAGS = -Lswisseph/lib -lswe

SOURCES = core/src/swiss_ephemeris.cpp \
          core/src/data_types.cpp \
          core/src/math_utils.cpp \
          core/src/calculations.cpp \
          example_chart.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all: example_chart

example_chart: $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o example_chart

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) example_chart

.PHONY: all clean
```

Speichere als `Makefile` im Projekt-Root und führe aus:
```bash
make
./example_chart
```

## Troubleshooting

### Problem: CMake nicht gefunden

**Lösung 1: CMake installieren**
```bash
sudo apt-get install cmake  # Ubuntu/Debian
brew install cmake          # macOS
```

**Lösung 2: Makefile verwenden** (siehe oben)

### Problem: Swiss Ephemeris Library nicht gefunden

```
CMake Error: Could not find SWISSEPH_LIB
```

**Lösung:**
```bash
# Prüfe ob Library existiert
ls -la swisseph/lib/libswe.a

# Falls nicht, baue Swiss Ephemeris
cd swisseph
make
```

Siehe `SETUP_SWISSEPH.md` für Details.

### Problem: C++20 nicht unterstützt

```
error: 'concept' does not name a type
```

**Lösung:**
- Update GCC: `sudo apt-get install gcc-10 g++-10`
- Oder reduziere auf C++17 in `CMakeLists.txt`:
  ```cmake
  set(CMAKE_CXX_STANDARD 17)
  ```

### Problem: Google Test nicht gefunden

**Lösung 1: System-Installation**
```bash
sudo apt-get install libgtest-dev
```

**Lösung 2: CMake lädt automatisch herunter** (FetchContent)
- Nichts tun, CMake regelt das

**Lösung 3: Tests deaktivieren**
```bash
cmake -DBUILD_TESTS=OFF ..
```

### Problem: Ephemeriden-Files fehlen

```
Swiss Ephemeris error: file not found
```

**Lösung:**
```bash
cd swisseph
./download_ephe.sh
```

Oder manuell:
```bash
mkdir -p data/ephe
cd data/ephe
wget https://www.astro.com/ftp/swisseph/ephe/seas_18.se1
wget https://www.astro.com/ftp/swisseph/ephe/semo_18.se1
wget https://www.astro.com/ftp/swisseph/ephe/sepl_18.se1
```

## Verifikation

Nach erfolgreicher Installation:

```bash
# 1. Prüfe ob Binary existiert
ls -la build/example_chart

# 2. Führe aus
./build/example_chart

# 3. Erwartete Ausgabe
# ═══════════════════════════════════════════
#   AstroUniverse - Chart Calculation Demo
# ═══════════════════════════════════════════
# ...
# Planeten-Positionen:
# Sonne         280.23°  ...
```

## Nächste Schritte

- Siehe `README.md` für API-Beispiele
- Siehe `BUILD.md` für erweiterte Build-Optionen
- Siehe `PORTIERUNGSPLAN.md` für Roadmap

---

**Bei Problemen:** Erstelle ein Issue mit:
- Betriebssystem und Version
- Compiler-Version (`gcc --version`)
- CMake-Version (`cmake --version`)
- Fehlermeldung
