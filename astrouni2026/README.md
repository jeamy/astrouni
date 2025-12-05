# AstroUniverse 2026

**1:1 Portierung der AstroUniverse v0.04Beta Astrologie-Software**

Von 16-bit Borland C++ (Windows 3.1) nach modernem C++20 mit Qt6 für Mac, Windows und Linux.

## Übersicht

AstroUniverse ist eine vollständige Astrologie-Software mit:
- Radix-Horoskop Berechnung und Darstellung
- Transit-Berechnungen
- Synastrie und Composit
- 13 verschiedene Häusersysteme
- 17 Himmelskörper (Planeten, Mondknoten, Lilith, Chiron, Asteroiden)
- 7 Aspekte mit konfigurierbaren Orben
- Orte- und Personen-Datenbank

## Portierungs-Strategie

Diese Portierung folgt einem strikten **1:1 Mapping** des Legacy-Codes:
- Alle Funktionen werden exakt nachgebildet
- Die GUI verhält sich identisch zum Original
- Die .dat Daten-Dateien werden unverändert übernommen
- Nur die Ephemeriden-Berechnung wird durch Swiss Ephemeris ersetzt

## Projektstruktur

```
astrouni2026/
├── src/
│   ├── main.cpp              # Haupteinstiegspunkt
│   ├── core/                 # Kern-Logik (GUI-unabhängig)
│   │   ├── constants.h       # Konstanten aus astrouni.h
│   │   ├── data_types.h/cpp  # RADIX, ORTE, AU_INIT
│   │   ├── calculations.h/cpp # Mathematik (aucalc.c)
│   │   ├── chart_calc.h/cpp  # Horoskop-Berechnung (auwurzel.c)
│   │   ├── transit_calc.h/cpp # Transit-Berechnung (autransi.c)
│   │   └── swiss_eph.h/cpp   # Swiss Ephemeris Wrapper
│   ├── data/                 # Daten-Handling
│   │   ├── legacy_io.h/cpp   # .dat Dateien (io.c, astrofil.c)
│   │   ├── orte_db.h/cpp     # Orte-Datenbank
│   │   └── person_db.h/cpp   # Personen-Datenbank
│   └── gui/                  # Qt6 GUI
│       ├── main_window.h/cpp # Hauptfenster
│       ├── radix_window.h/cpp # Radix-Fenster
│       ├── chart_widget.h/cpp # Radix-Zeichnung
│       └── dialogs/          # Alle Dialoge
├── data/                     # Daten-Dateien aus Legacy
├── swisseph/                 # Swiss Ephemeris
├── tests/                    # Unit Tests
└── docs/                     # Dokumentation
```

## Build

### Voraussetzungen

- CMake ≥ 3.16
- C++20 Compiler (GCC 12+, Clang 15+, MSVC 2022)
- Qt6 ≥ 6.5

### Linux/macOS

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Windows

```cmd
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

## Verwendung

```bash
./astrouni2026
```

## Legacy-Code Mapping

| Legacy-Datei | Funktion | Qt6-Entsprechung |
|--------------|----------|------------------|
| astrouni.c | WinMain, MainWndProc | main.cpp, main_window.cpp |
| auwurzel.c | sRadix, sHaus, sPlanet | chart_widget.cpp |
| aucalc.c | dJulTag, dMod, vGrad2GMS | calculations.cpp |
| astrofil.c | Dialoge, Daten-I/O | dialogs/, legacy_io.cpp |
| autransi.c | Transit-Berechnung | transit_calc.cpp |
| aupla.c/aupla1.c | Ephemeriden | swiss_eph.cpp (ersetzt) |

## Daten-Dateien

Die folgenden .dat Dateien aus dem Legacy-Code werden unverändert verwendet:

- `astroger.dat` - Deutsche Orte-Datenbank
- `astroorg.dat` - Orte-Datenbank
- `europa.dat` - Europa-Orte
- `astronam.dat` - Namen-Datenbank
- `default.dat` - Standard-Einstellungen
- `astroini.dat` - Benutzer-Einstellungen

## Lizenz

Basierend auf AstroUniverse v0.04Beta by A.S. and J.L. ©1997

## Dokumentation

Siehe `docs/PORTIERUNGSPLAN_QT6.md` für detaillierte Informationen zur Portierung.
