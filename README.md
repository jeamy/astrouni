# AstroUniverse - Modern Astrology Software

Moderne C++20 Portierung der deutschen Astrologie-Software **AstroUnivers v0.04Beta**.

## Features

- ✅ **Swiss Ephemeris Integration** - Höchste Genauigkeit (NASA JPL DE431)
- ✅ **Alle Planeten** - Sonne, Mond, Merkur bis Pluto, Mondknoten, Chiron
- ✅ **Häusersysteme** - Placidus, Koch, Regiomontanus, Equal, Whole Sign, etc.
- ✅ **Aspekt-Berechnung** - Konjunktion, Opposition, Trigon, Quadrat, Sextil, etc.
- ✅ **Cross-Platform** - Linux, Windows, macOS
- ✅ **Modern C++20** - Type-safe, RAII, Smart Pointers
- ✅ **Unit Tests** - Google Test Framework

## Quick Start

### 1. Voraussetzungen

```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake git libgtest-dev

# macOS
brew install cmake googletest

# Windows
# Visual Studio 2019+ oder MinGW-w64
```

### 2. Build

**Linux/macOS:**
```bash
./build.sh          # Release Build
./build.sh Debug    # Debug Build
```

**Windows:**
```cmd
build.bat           # Release Build
build.bat Debug     # Debug Build
```

**Oder manuell:**
```bash
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
```

### 3. Tests

**Linux/macOS:**
```bash
./test.sh
```

**Windows:**
```cmd
test.bat
```

### 4. Ausführen

**Linux/macOS:**
```bash
cd build
./example_chart
```

**Windows:**
```cmd
cd build\Release
example_chart.exe
```

**Ausgabe:**
```
═══════════════════════════════════════════
  AstroUniverse - Chart Calculation Demo
═══════════════════════════════════════════

Planeten-Positionen:
═══════════════════════════════════════════════════════════════════
Sonne         280.23°  Lat:   0.00°  Dist:  0.98334 AU  Speed:   1.02°/day
Mond           98.45°  Lat:   4.12°  Dist:  0.00257 AU  Speed:  13.18°/day
Merkur        271.56°  Lat:  -2.34°  Dist:  0.46789 AU  Speed:   1.45°/day
...
```

## Projekt-Struktur

```
astrouni/
├── CMakeLists.txt           # Haupt-Build-Konfiguration
├── build.sh / build.bat     # Build-Scripts (Linux/macOS/Windows)
├── test.sh / test.bat       # Test-Scripts
├── clean.sh / clean.bat     # Clean-Scripts
│
├── core/                    # Core Library
│   ├── CMakeLists.txt
│   ├── include/astro/
│   │   ├── data_types.h     # Datenstrukturen
│   │   ├── swiss_ephemeris.h # Swiss Ephemeris Wrapper
│   │   ├── math_utils.h     # Mathematik-Funktionen
│   │   └── calculations.h   # Berechnungs-Engine
│   └── src/
│       ├── data_types.cpp
│       ├── swiss_ephemeris.cpp
│       ├── math_utils.cpp
│       └── calculations.cpp
│
├── tests/                   # Unit Tests
│   ├── CMakeLists.txt
│   ├── test_math_utils.cpp
│   ├── test_swiss_ephemeris.cpp
│   └── test_calculations.cpp
│
├── swisseph/               # Swiss Ephemeris
│   ├── README.md
│   ├── download_ephe.sh    # Download von GitHub
│   └── ephe/               # Ephemeriden-Files
│       ├── seas_18.se1     # Planeten (218 KB)
│       ├── semo_18.se1     # Mond (1.3 MB)
│       └── sepl_18.se1     # Äußere Planeten (473 KB)
│
├── legacy/                 # Original Legacy Code (Referenz)
├── example_chart.cpp       # Beispielprogramm
│
└── docs/                   # Dokumentation
    ├── README.md
    ├── QUICKSTART.md
    ├── BUILD.md
    ├── INSTALLATION.md
    ├── PORTIERUNGSPLAN.md
    └── STATUS.md
```

## API-Beispiel

```cpp
#include "astro/swiss_ephemeris.h"
#include "astro/calculations.h"

using namespace astro;

// Swiss Ephemeris initialisieren
auto ephemeris = std::make_shared<SwissEphemeris>();
ephemeris->initialize("data/ephe");

// Calculator erstellen
auto calculator = std::make_unique<AstroCalculator>(ephemeris);

// Radix-Daten
RadixData radix;
radix.name = "Max Mustermann";
radix.birth_datetime = DateTime(1990, 6, 15, 14, 30, 0.0);
radix.birth_location = GeoLocation(52.52, 13.40);  // Berlin
radix.julian_date = JulianDate(radix.birth_datetime);

// Chart berechnen
auto chart = calculator->calculate_chart(radix, HouseSystem::Placidus);

// Planeten ausgeben
for (const auto& planet : chart.planets) {
    std::cout << "Longitude: " << planet.longitude << "°\n";
}
```

## Dokumentation

- **[BUILD.md](BUILD.md)** - Detaillierte Build-Anleitung
- **[PORTIERUNGSPLAN.md](PORTIERUNGSPLAN.md)** - Migrations-Strategie
- **[SWISS_EPHEMERIS_INTEGRATION.md](SWISS_EPHEMERIS_INTEGRATION.md)** - Swiss Ephemeris Details
- **[DATEI_ANALYSE.md](DATEI_ANALYSE.md)** - Legacy Code Analyse

## Tests

```bash
cd build
ctest --output-on-failure
```

Oder direkt:
```bash
./astro_tests
```

**Test-Coverage:**
- ✅ Mathematik-Funktionen (Julianisches Datum, Winkel, Koordinaten)
- ✅ Swiss Ephemeris Integration (Planeten, Häuser, Delta T)
- ✅ Berechnungs-Engine (Charts, Aspekte)

## Technologie-Stack

- **C++20** - Modern C++ mit Concepts, Modules
- **CMake 3.20+** - Build System
- **Swiss Ephemeris 2.10+** - Ephemeriden-Berechnungen
- **Google Test** - Unit Testing
- **Git** - Version Control

## Lizenz

### Code
- **AstroUniverse**: GPL v3 (siehe LICENSE)
- **Legacy Code**: Original Copyright (nur für Analyse)

### Swiss Ephemeris
- **GPL v2** oder **LGPL** für Open Source
- **Kommerzielle Lizenz** verfügbar (~CHF 750)
- Siehe: https://www.astro.com/swisseph/

## Roadmap

### Phase 1: Core Library ✅ (Woche 3-5)
- [x] Swiss Ephemeris Integration
- [x] Datenstrukturen (RadixData, GeoLocation, etc.)
- [x] Mathematik-Module (Julianisches Datum, Winkel)
- [x] Berechnungs-Engine (Planeten, Häuser, Aspekte)
- [x] Unit Tests

### Phase 2: GUI (Woche 7-9) 🚧
- [ ] wxWidgets Integration
- [ ] Chart-Darstellung (Radix-Kreis)
- [ ] Dialoge (Neue Charts, Einstellungen)
- [ ] Export (PNG, SVG, PDF)

### Phase 3: Features (Woche 10-12) 📋
- [ ] Deutsche Lokalisierung
- [ ] Orte-Datenbank
- [ ] Transite und Progressionen
- [ ] Synastrie (Partner-Vergleich)

## Entwicklung

### Anforderungen
- GCC 10+ oder Clang 10+ (C++20 Support)
- CMake 3.20+
- Swiss Ephemeris Library

### Build-Optionen

```bash
# Debug-Build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release-Build (optimiert)
cmake -DCMAKE_BUILD_TYPE=Release ..

# Ohne Tests
cmake -DBUILD_TESTS=OFF ..

# Mit AddressSanitizer
cmake -DCMAKE_CXX_FLAGS="-fsanitize=address" ..
```

## Beitragen

1. Fork das Repository
2. Feature-Branch erstellen (`git checkout -b feature/amazing-feature`)
3. Änderungen committen (`git commit -m 'Add amazing feature'`)
4. Branch pushen (`git push origin feature/amazing-feature`)
5. Pull Request öffnen

## Kontakt

- **Projekt**: AstroUniverse
- **Status**: Phase 2 - Core Library abgeschlossen
- **Version**: 1.0.0-alpha

## Credits

- **Original AstroUnivers**: Legacy Windows-Software (16-bit Borland C++)
- **Swiss Ephemeris**: Astrodienst Zürich (https://www.astro.com/swisseph/)
- **Portierung**: Modern C++20 Implementation

---

*Stand: November 2025*
