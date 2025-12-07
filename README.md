# AstroUniverse 2026 - Astrology Software

Moderne C++20/Qt6 Portierung der deutschen Astrologie-Software **AstroUnivers v0.04Beta**.

## Features

- ✅ **Präzise Berechnungen** - Swiss Ephemeris (eingebettet, Cross-Platform)
- ✅ **Cross-Platform** - Linux, macOS, Windows
- ✅ **Moderne GUI** - Qt6
- ✅ **Legacy-Kompatibel** - Liest AstroUnivers .dat Dateien
- ✅ **Unit Tests** - Alle bestanden
- ✅ **Open Source** - GPL v2

## Voraussetzungen

**Ubuntu/Debian:**
```bash
sudo apt-get install build-essential cmake git qt6-base-dev qt6-tools-dev
```

**macOS:**
```bash
brew install cmake qt@6
```

**Windows:**
- Visual Studio 2022 mit C++ Desktop Development
- Qt6 von https://www.qt.io/download-qt-installer

## Build

```bash
cd astrouni2026
mkdir build && cd build
cmake ..
make -j$(nproc)      # Linux/macOS
# oder: cmake --build . --config Release  # Windows
```

## Tests

```bash
cd build
ctest --output-on-failure
```

## Ausführen

```bash
./astrouni2026       # Linux/macOS
astrouni2026.exe     # Windows
```

## Projekt-Struktur

```
astrouni2026/
├── CMakeLists.txt           # CMake Build-Konfiguration
├── src/
│   ├── main.cpp             # Entry Point
│   ├── core/                # Berechnungs-Engine
│   │   ├── constants.h      # Konstanten
│   │   ├── data_types.h     # RADIX, ORTE, AU_INIT
│   │   ├── calculations.h   # Mathematik
│   │   ├── chart_calc.h     # Horoskop-Berechnung
│   │   ├── transit_calc.h   # Transit-Berechnung
│   │   └── swiss_eph.h      # Swiss Ephemeris Wrapper
│   ├── data/                # Daten-I/O
│   │   ├── legacy_io.h      # .dat Dateien
│   │   ├── orte_db.h        # Orte-Datenbank
│   │   └── person_db.h      # Personen-Datenbank
│   └── gui/                 # Qt6 GUI
│       ├── main_window.h
│       ├── radix_window.h
│       ├── chart_widget.h
│       └── dialogs/         # Dialoge
├── swisseph/                # Swiss Ephemeris (eingebettet)
│   ├── src/                 # Quellcode (Cross-Platform)
│   └── ephe/                # Ephemeriden-Dateien
├── data/                    # Legacy .dat Dateien
├── tests/                   # Unit Tests
└── docs/                    # Dokumentation
```

## API-Beispiel (Qt6 Version)

```cpp
#include "core/swiss_eph.h"
#include "core/chart_calc.h"
#include "core/calculations.h"

using namespace astro;

// Swiss Ephemeris initialisieren
swissEph().setEphePath("swisseph/ephe");

// Radix-Daten vorbereiten
Radix radix;
radix.rFix.vorname = "Max";
radix.rFix.name = "Mustermann";
radix.rFix.tag = 15;
radix.rFix.monat = 6;
radix.rFix.jahr = 1990;
radix.rFix.zeit = 14.5;  // 14:30
radix.rFix.breite = 52.52;  // Berlin
radix.rFix.laenge = 13.40;
radix.rFix.zone = 1.0;  // MEZ
radix.hausSys = TYP_PLACIDUS;

// Chart berechnen
ChartCalc::calculate(radix);

// Planeten ausgeben
for (int i = 0; i < radix.anzahlPlanet; ++i) {
    QString pos = Calculations::degToString(radix.planet[i], true, true);
    qDebug() << "Planet" << i << ":" << pos;
}
```

## Dokumentation

- **[astrouni2026/docs/PORTIERUNGSPLAN_QT6.md](astrouni2026/docs/PORTIERUNGSPLAN_QT6.md)** - Qt6 Portierungsplan

## Technologie-Stack

- **C++20** - Modern C++ Standard
- **Qt6 ≥ 6.2** - GUI Framework (Cross-Platform)
- **CMake 3.16+** - Build System
- **Swiss Ephemeris 2.10** - Eingebettet (keine externe Abhängigkeit)

## Lizenz

**GNU General Public License v2.0**

AstroUniverse ist freie Software unter der GPL v2 Lizenz.

- **Lizenz:** [LICENSE](LICENSE) (GPL v2)
- **Copyright:** [COPYRIGHT](COPYRIGHT)
- **SPDX:** `GPL-2.0-or-later`

### Drittanbieter-Komponenten

- **Swiss Ephemeris** - GPL v2 / LGPL (Astrodienst AG)
- **Qt6** - LGPL v3 (The Qt Company)

### Swiss Ephemeris
- **GPL v2** oder **LGPL** für Open Source
- **Kommerzielle Lizenz** verfügbar (~CHF 750)
- Siehe: https://www.astro.com/swisseph/

## Roadmap

### Phase 1: Core (Abgeschlossen)
- [x] Projektstruktur erstellt
- [x] Swiss Ephemeris eingebettet (Cross-Platform)
- [x] Datenstrukturen portiert (RADIX, ORTE, AU_INIT)
- [x] Mathematik-Funktionen portiert
- [x] Berechnungs-Engine portiert
- [x] Unit Tests

### Phase 2: GUI (In Arbeit)
- [x] Qt6 Hauptfenster
- [x] Radix-Fenster
- [x] Chart-Widget (Radix-Zeichnung)
- [x] Personen-Dialog
- [x] Ort-Suche Dialog
- [ ] Weitere Dialoge vervollständigen
- [ ] Drucken implementieren
- [ ] Speichern/Laden

### Phase 3: Erweiterte Features (Geplant)
- [ ] Transit-Fenster
- [ ] Synastrie
- [ ] Composit
- [ ] Export (PNG, PDF)

## Entwicklung

### Anforderungen
- GCC 11+ oder Clang 15+ oder MSVC 2022 (C++20)
- CMake 3.16+
- Qt6 ≥ 6.2

### Build-Optionen

```bash
cd astrouni2026

# Debug-Build
cmake -DCMAKE_BUILD_TYPE=Debug -B build-debug
cmake --build build-debug

# Release-Build (optimiert)
cmake -DCMAKE_BUILD_TYPE=Release -B build
cmake --build build

# Ohne Tests
cmake -DBUILD_TESTS=OFF -B build
```

## Beitragen

1. Fork das Repository
2. Feature-Branch erstellen (`git checkout -b feature/amazing-feature`)
3. Änderungen committen (`git commit -m 'Add amazing feature'`)
4. Branch pushen (`git push origin feature/amazing-feature`)
5. Pull Request öffnen

## Kontakt

- **Projekt**: AstroUniverse
- **Status**: Qt6 GUI in Entwicklung
- **Version**: 2026.1.0

## Credits

- **Original AstroUnivers v0.04Beta**: Legacy Windows-Software (16-bit Borland C++)
- **Swiss Ephemeris**: Astrodienst Zürich (https://www.astro.com/swisseph/)
- **Portierung**: Modern C++20 mit Qt6

---

*Stand: Dezember 2025*
