# Swiss Ephemeris Setup

## Verzeichnisstruktur

```
swisseph/
├── ephe/              # Ephemeriden-Dateien
│   ├── seas_18.se1    # Hauptplaneten (kompakt, 1800-3000 AD)
│   ├── semo_18.se1    # Mond (kompakt)
│   └── sepl_18.se1    # Äußere Planeten (kompakt)
├── src/               # Swiss Ephemeris Source (optional)
└── README.md          # Diese Datei
```

## Ephemeriden-Dateien

### Kompakt-Version (empfohlen für Start)

Die heruntergeladenen `*_18.se1` Dateien decken ab:
- **Zeitraum**: 1800 - 3000 AD (1200 Jahre)
- **Größe**: ~3 MB total
- **Genauigkeit**: Ausreichend für die meisten Anwendungen

### Erweiterte Versionen (optional)

Falls größerer Zeitraum benötigt:

```bash
# 6000 Jahre (600 BC - 5400 AD)
cd ephe
wget https://www.astro.com/ftp/swisseph/ephe/seplm54.se1  # ~50 MB
wget https://www.astro.com/ftp/swisseph/ephe/seasm54.se1  # ~30 MB
wget https://www.astro.com/ftp/swisseph/ephe/semom54.se1  # ~20 MB
```

## Swiss Ephemeris Library Installation

### Linux (Ubuntu/Debian)

```bash
# Option 1: System-Package (falls verfügbar)
sudo apt-get install libswe-dev

# Option 2: Manuell kompilieren
cd /tmp
wget https://www.astro.com/ftp/swisseph/swe_unix_src_2.10.03.tar.gz
tar xzf swe_unix_src_2.10.03.tar.gz
cd swisseph/src
make
sudo make install
sudo ldconfig
```

### macOS

```bash
# Homebrew (falls verfügbar)
brew install swisseph

# Oder manuell wie Linux
```

### Windows

```bash
# vcpkg (empfohlen)
vcpkg install swisseph:x64-windows

# Oder manuell:
# 1. Download: https://www.astro.com/ftp/swisseph/swe_win32_2.10.03.zip
# 2. Extrahieren nach C:\swisseph
# 3. In CMake: set(SWISSEPH_ROOT "C:/swisseph")
```

## Cross-Platform CMake Setup

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(AstroUniverse)

# Swiss Ephemeris finden
if(WIN32)
    # Windows: vcpkg oder manuell
    find_package(swisseph CONFIG)
    if(NOT swisseph_FOUND)
        set(SWISSEPH_ROOT "C:/swisseph" CACHE PATH "Swiss Ephemeris root")
        find_library(SWISSEPH_LIB NAMES swe PATHS ${SWISSEPH_ROOT}/lib)
        find_path(SWISSEPH_INCLUDE swephexp.h PATHS ${SWISSEPH_ROOT}/include)
    endif()
elseif(APPLE)
    # macOS: Homebrew oder manuell
    find_library(SWISSEPH_LIB swe PATHS /usr/local/lib /opt/homebrew/lib)
    find_path(SWISSEPH_INCLUDE swephexp.h PATHS /usr/local/include /opt/homebrew/include)
else()
    # Linux: System-Installation
    find_library(SWISSEPH_LIB swe PATHS /usr/lib /usr/local/lib)
    find_path(SWISSEPH_INCLUDE swephexp.h PATHS /usr/include /usr/local/include)
endif()

if(NOT SWISSEPH_LIB OR NOT SWISSEPH_INCLUDE)
    message(FATAL_ERROR "Swiss Ephemeris not found. Please install it.")
endif()

message(STATUS "Swiss Ephemeris Library: ${SWISSEPH_LIB}")
message(STATUS "Swiss Ephemeris Include: ${SWISSEPH_INCLUDE}")

# Ephemeriden-Pfad setzen
set(EPHE_PATH "${CMAKE_SOURCE_DIR}/swisseph/ephe" CACHE PATH "Ephemeris data path")
add_definitions(-DEPHE_PATH="${EPHE_PATH}")

# Target mit Swiss Ephemeris linken
add_executable(astro_core src/main.cpp)
target_include_directories(astro_core PRIVATE ${SWISSEPH_INCLUDE})
target_link_libraries(astro_core PRIVATE ${SWISSEPH_LIB})
```

## Verwendung im Code

```cpp
#include <swephexp.h>

// Initialisierung
swe_set_ephe_path("./swisseph/ephe");

// Planeten-Position berechnen
double jd = 2451545.0;  // J2000.0
double xx[6];
char serr[256];
int ret = swe_calc_ut(jd, SE_SUN, SEFLG_SWIEPH, xx, serr);

if (ret >= 0) {
    printf("Sun longitude: %.6f°\n", xx[0]);
}

// Aufräumen
swe_close();
```

## Test

```bash
# Kompilieren und testen
mkdir build && cd build
cmake ..
make
./astro_core
```

## Lizenz

Swiss Ephemeris ist verfügbar unter:
- **GPL v2** oder **LGPL** (kostenlos für Open Source)
- **Kommerzielle Lizenz** (~CHF 750, für proprietäre Software)

Mehr Info: https://www.astro.com/swisseph/

## Ressourcen

- **Website**: https://www.astro.com/swisseph/
- **Dokumentation**: https://www.astro.com/swisseph/swephprg.htm
- **Download**: https://www.astro.com/ftp/swisseph/
- **Forum**: https://groups.io/g/swisseph
