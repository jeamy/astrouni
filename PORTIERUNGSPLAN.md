# Portierungsplan: AstroUnivers → Modern C++

## Übersicht

Migration der deutschen Astrologie-Software AstroUnivers v0.04Beta (16-bit Windows/Borland C++) zu modernem C++ mit wxWidgets GUI für Cross-Platform Support (Linux/Windows/macOS).

## Legacy Analyse

### ✅ Abgeschlossen

**Erkenntnisse:**
- **Datenstrukturen**: `RADIXFIX`, `ORTE` mit Koordinaten, Zeiten
- **Windows-API Dependencies**: `HWND`, `COLORREF`, `HINSTANCE`
- **Ephemeriden**: `astroeph.dat` (450KB) mit Planetendaten
- **Deutsche Texte**: Konstanten für Planeten, Zeichen, Häuser
- **Berechnungen**: Planetenpositionen, Häusersysteme, Aspekte
- **GUI**: 16-bit Windows mit nativen Dialogen

**Herausforderungen:**
- 16-bit → 32/64-bit Portierung
- Borland C++ → moderner C++ Compiler
- Windows API → Cross-Platform wxWidgets
- Hardcodierte Pfade (`c:\borlandc\include`)

## Architektur-Design

### Moderne C++ Struktur (Implementiert)

```
astrouni/
├── CMakeLists.txt           # ✅ Haupt-Build-Konfiguration
├── build.sh / build.bat     # ✅ Build-Scripts (Linux/macOS/Windows)
├── test.sh / test.bat       # ✅ Test-Scripts
├── clean.sh / clean.bat     # ✅ Clean-Scripts
│
├── core/                    # ✅ Platform-unabhängige Berechnungen
│   ├── CMakeLists.txt       # ✅ Core Build-Config
│   ├── include/astro/
│   │   ├── data_types.h     # ✅ Datenstrukturen (RADIXFIX, ORTE → C++)
│   │   ├── swiss_ephemeris.h # ✅ Swiss Ephemeris Wrapper
│   │   ├── math_utils.h     # ✅ Mathematik (Julianisches Datum, Winkel)
│   │   └── calculations.h   # ✅ Berechnungs-Engine (Charts, Aspekte)
│   └── src/
│       ├── data_types.cpp   # ✅ Implementierung
│       ├── swiss_ephemeris.cpp # ✅ Implementierung
│       ├── math_utils.cpp   # ✅ Implementierung
│       └── calculations.cpp # ✅ Implementierung
│
├── tests/                   # ✅ Unit Tests (Google Test)
│   ├── CMakeLists.txt       # ✅ Test Build-Config
│   ├── test_math_utils.cpp  # ✅ Mathematik Tests
│   ├── test_swiss_ephemeris.cpp # ✅ Swiss Ephemeris Tests
│   └── test_calculations.cpp # ✅ Berechnungs Tests
│
├── swisseph/               # ✅ Swiss Ephemeris
│   ├── README.md           # ✅ Swiss Ephemeris Dokumentation
│   ├── download_ephe.sh    # ✅ Ephemeriden-Download Script (GitHub)
│   └── ephe/               # ✅ Ephemeriden-Files
│       ├── seas_18.se1     # ✅ Planeten (218 KB)
│       ├── semo_18.se1     # ✅ Mond (1.3 MB)
│       └── sepl_18.se1     # ✅ Äußere Planeten (473 KB)
│
├── legacy/                 # ✅ Original Legacy Code (Referenz)
│   └── [41 Dateien]        # ✅ Für Vergleich und Analyse
│
├── example_chart.cpp       # ✅ Beispielprogramm
│
├── gui/                    # 📋 Geplant (Phase 3)
│   ├── main_frame.h/cpp    # 📋 wxWidgets Hauptfenster
│   ├── chart_panel.h/cpp   # 📋 Radix-Darstellung
│   └── dialogs/            # 📋 Dialoge
│
└── docs/                   # ✅ Dokumentation
    ├── README.md           # ✅ Projekt-Übersicht
    ├── QUICKSTART.md       # ✅ Quick Start Guide
    ├── BUILD.md            # ✅ Build-Anleitung
    ├── INSTALLATION.md     # ✅ Setup-Guide
    ├── PORTIERUNGSPLAN.md  # ✅ Dieser Plan
    ├── STATUS.md           # ✅ Projekt-Status
    ├── SWISS_EPHEMERIS_INTEGRATION.md # ✅ Swiss Ephemeris Details
    └── DATEI_ANALYSE.md    # ✅ Legacy Code Analyse
```

### Design Prinzipien

1. **Separation of Concerns**: Core-Berechnungen getrennt von GUI
2. **Cross-Platform**: Nur wxWidgets für plattformspezifischen Code
3. **Modern C++**: C++17/20 Features, RAII, Smart Pointer
4. **Test-Driven**: Unit Tests für alle Core-Module

## Phasenplan (13 Wochen)

### Phase 1: Analyse & Design (Woche 1-2) ✅

- [x] Legacy Code-Struktur analysieren
- [x] Abhängigkeiten identifizieren
- [x] Moderne Architektur entwerfen
- [x] Technologiestack festlegen (C++20, wxWidgets, CMake)

### Phase 2: Core Library Portierung (Woche 3-5) 🚀 IN ARBEIT

#### 2.1 Datenstrukturen modernisieren

```cpp
// Legacy: RADIXFIX struct
class RadixData {
public:
    std::chrono::system_clock::time_point datetime;
    GeoLocation location;
    std::string name;
    bool is_valid() const;
};

// Legacy: ORTE struct  
class GeoLocation {
public:
    double latitude;    // dBreite
    double longitude;   // dLange
    std::string timezone;
    std::string country_code;
};
```

#### 2.2 Ephemeriden-Engine (Swiss Ephemeris) ✅ ENTSCHIEDEN

**Wichtig**: Statt proprietärer Astrodienst-Daten verwenden wir **Swiss Ephemeris** (GPL/LGPL).

```cpp
class Ephemeris {
public:
    Ephemeris();
    ~Ephemeris();
    
    // Swiss Ephemeris Integration
    bool initialize(const std::string& ephe_path);
    
    PlanetPosition calculate_planet(Planet planet, const JulianDate& jd, 
                                   bool heliocentric = false) const;
    std::vector<HouseCusp> calculate_houses(HouseSystem system, 
                                           const JulianDate& jd, 
                                           const GeoLocation& loc) const;
    std::vector<Aspect> find_aspects(const ChartData& chart, double orb = 8.0) const;
    
    // Zusätzliche Swiss Ephemeris Features
    double get_delta_t(const JulianDate& jd) const;
    bool is_retrograde(Planet planet, const JulianDate& jd) const;
    
private:
    struct SwissEphemerisImpl;
    std::unique_ptr<SwissEphemerisImpl> impl_;
};
```

**Swiss Ephemeris Vorteile**:
- ✅ Open Source (GPL/LGPL oder kommerziell)
- ✅ Höchste Genauigkeit (NASA JPL Ephemeriden)
- ✅ Zeitraum: 13000 BC bis 17000 AD
- ✅ Aktiv maintained
- ✅ C/C++ Library verfügbar

#### 2.3 API-Anpassungen 32/64-bit

- `long` → `int32_t`/`int64_t` wo passend
- `float` → `double` für bessere Präzision
- Windows-Typen entfernen (`HWND`, `COLORREF`)
- Pointer-Arithmetik durch moderne Container ersetzen

### Phase 3: Build System (Woche 6)

#### 3.1 CMake Konfiguration

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(AstroUniverse VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Swiss Ephemeris
find_library(SWISSEPH_LIB swe REQUIRED)
find_path(SWISSEPH_INCLUDE swephexp.h REQUIRED)

find_package(wxWidgets REQUIRED COMPONENTS core base)
find_package(GTest REQUIRED)

add_subdirectory(core)
add_subdirectory(gui) 
add_subdirectory(cli)
add_subdirectory(tests)
```

#### 3.2 Cross-Platform Dependencies

- **vcpkg** oder **Conan** für Package Management
- **Swiss Ephemeris** 2.10+ für Ephemeriden-Berechnungen
- **wxWidgets** 3.2+ für GUI
- **Google Test** für Unit Tests
- **Doxygen** für Dokumentation

**Swiss Ephemeris Installation**:
```bash
# vcpkg
vcpkg install swisseph

# Oder manuell von https://www.astro.com/swisseph/
# Lizenz: GPL oder kommerzielle Lizenz verfügbar
```

### Phase 4: wxWidgets GUI (Woche 7-9)

#### 4.1 Hauptfenster

```cpp
class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title);
    void OnNewChart(wxCommandEvent& event);
    void OnExportPNG(wxCommandEvent& event);
private:
    ChartPanel* chart_panel_;
    wxMenu* file_menu_;
};
```

#### 4.2 Chart-Darstellung

```cpp
class ChartPanel : public wxPanel {
public:
    void SetChartData(const std::shared_ptr<ChartData>& data);
    void RenderChart(wxDC& dc);
private:
    void DrawWheel(wxDC& dc, const wxRect& rect);
    void DrawPlanets(wxDC& dc, const wxPoint& center, int radius);
    void DrawAspects(wxDC& dc, const wxPoint& center, int radius);
};
```

#### 4.3 GUI Features

- **Radix Chart**: Kreisdiagramm mit Planeten, Häusern, Aspekten
- **Dialoge**: Neue Horoskope, Einstellungen, Export
- **Menüs**: Datei, Bearbeiten, Ansicht, Hilfe
- **Export**: PNG, SVG, PDF

### Phase 5: Daten & Lokalisierung (Woche 10)

#### 5.1 .dat Files Integration

```cpp
class DataFileManager {
public:
    bool LoadEphemeris(const std::string& filepath);
    bool LoadGermanTexts(const std::string& filepath);
    const GermanTexts& GetTexts() const { return texts_; }
private:
    std::unique_ptr<EphemerisData> ephemeris_;
    GermanTexts texts_;
};
```

#### 5.2 Deutsche Lokalisierung

```cpp
namespace Localization {
    constexpr const char* PLANET_NAMES[] = {
        "Sonne", "Mond", "Merkur", "Venus", "Mars",
        "Jupiter", "Saturn", "Uranus", "Neptun", "Pluto"
    };
    
    constexpr const char* ZODIAC_SIGNS[] = {
        "Widder", "Stier", "Zwilling", "Krebs", "Löwe",
        "Jungfrau", "Waage", "Skorpion", "Schütze", 
        "Steinbock", "Wassermann", "Fische"
    };
}
```

#### 5.3 Daten-Migration

- `astroeph.dat` → Binary Parser mit Endianness-Handling
- `astroger.dat` → Deutsche Texte und Bezeichnungen
- `default.dat` → Standard-Einstellungen und Farben

### Phase 6: Testing & Validierung (Woche 11-12)

#### 6.1 Unit Tests

```cpp
TEST(EphemerisTest, PlanetPositionAccuracy) {
    Ephemeris eph;
    JulianDate jd(2024, 1, 1, 12, 0, 0);
    
    auto sun_pos = eph.calculate_planet(Planet::Sun, jd);
    EXPECT_NEAR(sun_pos.longitude, 280.0, 0.01); // vs Legacy
}

TEST(HouseSystemTest, PlacidusConsistency) {
    // Test gegen Legacy-Ausgaben
}
```

#### 6.2 Integration Tests

- **GUI-Rendering Tests**: Chart-Darstellung validieren
- **Datei-I/O Tests**: .dat Files korrekt geladen
- **Cross-Platform Tests**: Linux/Windows/macOS Konsistenz

#### 6.3 Regression Tests

- Legacy-Ausgaben als Referenz
- Numerische Genauigkeit sicherstellen
- Performance-Benchmarks

### Phase 7: Deployment (Woche 13)

#### 7.1 Paketierung

- **Windows**: NSIS Installer mit wxWidgets DLLs
- **Linux**: AppImage für Distribution-übergreifende Kompatibilität
- **macOS**: .dmg Bundle mit Code Signing

#### 7.2 Dokumentation

- **API-Dokumentation**: Doxygen für Core Library
- **Benutzerhandbuch**: Deutsch mit Screenshots
- **Migration Guide**: Anleitung für Legacy-Nutzer
- **Developer Guide**: Setup und Contributing

## Technologiestack

### Core Dependencies
- **C++20**: Modern C++ mit Concepts, Modules, Coroutines
- **CMake 3.20+**: Build System für Cross-Platform Support
- **Google Test**: Unit Testing Framework

### GUI Framework
- **wxWidgets 3.2+**: Native Widgets, Cross-Platform
- **Chart Rendering**: Custom wxDC Implementation
- **Export**: wxWidgets File Dialogs

### Development Tools
- **vcpkg**: C++ Package Manager
- **Doxygen**: API Dokumentation
- **Git**: Version Control
- **CLion/VSCode**: IDE Support

## Risiken & Mitigation

### Technische Risiken

1. **Ephemeriden-Genauigkeit**: Legacy-Algorithmus muss exakt portiert werden
   - *Mitigation*: Extensive Regression Tests
   
2. **Cross-Platform GUI**: wxWidgets Learning Curve
   - *Mitigation*: Prototypen früh erstellen, wxWidgets Erfahrung aufbauen

3. **Performance**: C++ vs Legacy C Performance
   - *Mitigation*: Profiling, Optimierungen wo nötig

### Projekt-Risiken

1. **Scope Creep**: Zu viele Features hinzufügen
   - *Mitigation*: Fokus auf Core-Features, MVP-Ansatz

2. **Daten-Kompatibilität**: .dat Files nicht lesbar
   - *Mitigation*: Early Integration, Reverse Engineering

## Erfolgs-Kriterien

### Funktionalität
- [ ] Alle Legacy-Berechnungen exakt reproduziert
- [ ] Deutsche Lokalisierung vollständig
- [ ] Cross-Platform Compatibility erreicht
- [ ] GUI entspricht modernen Standards

### Qualität
- [ ] 90%+ Code Coverage in Unit Tests
- [ ] Memory Leaks = 0 (Valgrind/AddressSanitizer)
- [ ] Performance innerhalb 10% von Legacy

### Usability
- [ ] Intuitive Bedienung für Legacy-Nutzer
- [ ] Stabile Ausführung auf allen Plattformen
- [ ] Gute Dokumentation und Hilfe

## Zeitplan

| Woche | Phase | Meilenstein |
|-------|-------|-------------|
| 1-2   | Analyse | Legacy-Code verstanden, Architektur entworfen |
| 3-5   | Core | Ephemeriden-Engine portiert |
| 6     | Build | CMake Setup funktioniert cross-platform |
| 7-9   | GUI | wxCharts implementiert |
| 10    | Daten | .dat Files integriert |
| 11-12 | Tests | Alle Tests grün, Regression validiert |
| 13    | Release | Paketierung und Dokumentation abgeschlossen |

## Aktuelle Implementierung (November 2025)

### ✅ Phase 2 Abgeschlossen (6. November 2025)

**Core Library:**
- [x] CMake Projekt-Struktur erstellt
- [x] Swiss Ephemeris C++ Wrapper implementiert
- [x] Core Datenstrukturen portiert (RADIXFIX, ORTE → C++20)
- [x] Mathematik-Module portiert (aucalc.c → math_utils.cpp)
- [x] Berechnungs-Engine portiert (auwurzel.c → calculations.cpp)

**Build & Test:**
- [x] Build-Scripts für Linux/macOS/Windows (build.sh, build.bat)
- [x] Test-Scripts für alle Plattformen (test.sh, test.bat)
- [x] Clean-Scripts (clean.sh, clean.bat)
- [x] Unit Tests implementiert (19 Tests, alle bestanden)
- [x] Beispielprogramm funktioniert

**Dokumentation:**
- [x] README.md mit Quick Start
- [x] QUICKSTART.md (3-Schritte-Anleitung)
- [x] BUILD.md (Detaillierte Build-Anleitung)
- [x] INSTALLATION.md (Setup mit Troubleshooting)
- [x] STATUS.md (Projekt-Status)
- [x] Projekt-Struktur aktualisiert

### 🚀 Phase 3: GUI (Geplant)
- [ ] wxWidgets Integration
- [ ] Hauptfenster implementieren
- [ ] Chart-Panel (Radix-Darstellung)
- [ ] Dialoge und Menüs
- [ ] Export-Funktionen (PNG, SVG, PDF)

### 📋 Nächste Schritte (Priorität)

1. **Projekt-Struktur anlegen**
   ```
   astro_universe/
   ├── CMakeLists.txt
   ├── core/
   │   ├── include/
   │   └── src/
   ├── tests/
   └── data/ephe/
   ```

2. **Swiss Ephemeris Wrapper** (swiss_ephemeris.h/cpp)
   - Initialisierung mit Ephemeriden-Pfad
   - Planeten-Berechnungen
   - Häusersysteme
   - Delta T

3. **Core Datenstrukturen** (data_types.h)
   - RadixData (RADIXFIX)
   - GeoLocation (ORTE)
   - PlanetPosition
   - HouseCusp

4. **Mathematik-Basis** (math_utils.h/cpp)
   - Julianisches Datum
   - Winkel-Berechnungen
   - Koordinaten-Konvertierung

5. **Unit Tests** (Google Test)
   - Swiss Ephemeris Integration testen
   - Mathematik-Funktionen validieren
   - Gegen Legacy-Ausgaben vergleichen

---

*Stand: 6. November 2025, 20:07 UTC+1*  
*Version: 1.1*  
*Status: Phase 2 - Core Library Portierung gestartet*
