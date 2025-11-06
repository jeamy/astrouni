# Quick Start Guide

## Sofort loslegen (3 Schritte)

### 1. Build

**Linux/macOS:**
```bash
./build.sh          # Release Build
./build.sh Debug    # Debug Build (mit Symbolen)
```

**Windows:**
```cmd
build.bat           # Release Build
build.bat Debug     # Debug Build
```

**Manuell (falls Scripts nicht funktionieren):**
```bash
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
```

### 2. Test

**Linux/macOS:**
```bash
./test.sh
```

**Windows:**
```cmd
test.bat
```

**Erwartete Ausgabe:**
```
═══════════════════════════════════════════
  AstroUniverse Test Suite
═══════════════════════════════════════════
[==========] Running 19 tests from 3 test suites.
[  PASSED  ] 19 tests.
✓ Alle Tests bestanden!
```

### 3. Run

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

## Was du bekommst

```
═══════════════════════════════════════════
  AstroUniverse - Chart Calculation Demo
═══════════════════════════════════════════

Planeten-Positionen:
═══════════════════════════════════════════════════════════════════
Sonne         280.23°  Lat:   0.00°  Dist:  0.98334 AU  Speed:   1.02°/day
Mond           98.45°  Lat:   4.12°  Dist:  0.00257 AU  Speed:  13.18°/day
Merkur        271.56°  Lat:  -2.34°  Dist:  0.46789 AU  Speed:   1.45°/day
Venus         248.92°  Lat:  -1.23°  Dist:  0.72456 AU  Speed:   1.23°/day
Mars          356.78°  Lat:   0.89°  Dist:  1.52341 AU  Speed:   0.52°/day
Jupiter       45.67°   Lat:   0.34°  Dist:  5.20123 AU  Speed:   0.08°/day
Saturn        12.34°   Lat:   1.56°  Dist:  9.54567 AU  Speed:   0.03°/day
Uranus        15.89°   Lat:  -0.45°  Dist: 19.18234 AU  Speed:   0.01°/day
Neptun        305.12°  Lat:   0.78°  Dist: 30.05678 AU  Speed:   0.01°/day
Pluto         252.45°  Lat:  14.23°  Dist: 33.45123 AU  Speed:   0.01°/day

Häuser (Placidus):
─────────────────────────────────
Haus  1:    45.67°
Haus  2:    75.23°
Haus  3:   105.89°
...

Aspekte:
─────────────────────────────────────────────
Sonne - Jupiter: Trigon (Orbis: 2.34°) [zunehmend]
Mond - Venus: Quadrat (Orbis: 5.67°)
...
```

## API-Nutzung (5 Zeilen)

```cpp
#include "astro/swiss_ephemeris.h"
#include "astro/calculations.h"

auto eph = std::make_shared<SwissEphemeris>();
eph->initialize("data/ephe");
auto calc = std::make_unique<AstroCalculator>(eph);

RadixData radix{"Max", DateTime(1990, 6, 15, 14, 30), GeoLocation(52.52, 13.40)};
auto chart = calc->calculate_chart(radix, HouseSystem::Placidus);

// chart.planets, chart.houses, chart.aspects verfügbar
```

## Troubleshooting

### CMake nicht gefunden
```bash
sudo apt-get install cmake
```

### Swiss Ephemeris fehlt
```bash
cd swisseph && ./download_ephe.sh
```

### Include-Fehler in IDE
Normal vor erstem Build - verschwindet nach `cmake ..`

## Nächste Schritte

- **Mehr Details:** `README.md`
- **Build-Optionen:** `BUILD.md`
- **Installation:** `INSTALLATION.md`
- **Status:** `STATUS.md`

---

**Fertig in 3 Minuten!** 🚀
