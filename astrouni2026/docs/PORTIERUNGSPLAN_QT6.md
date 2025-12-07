# Portierungsplan: AstroUniverse Legacy → Qt6/C++20

**Ziel:** Strikte 1:1-Portierung der 16-bit Borland C++ Anwendung "AstroUniverse v0.04Beta" nach modernem C++20 mit Qt6 für Mac, Windows und Linux.

**Datum:** Dezember 2025

---

## 1. Analyse des Legacy-Codes

### 1.1 Quell-Dateien (20.500 Zeilen C)

| Datei | Größe | Funktion | Priorität |
|-------|-------|----------|-----------|
| `astrouni.h` | 1293 Zeilen | Hauptheader, alle Strukturen | ⭐⭐⭐ |
| `astrouni.c` | ~600 Zeilen | WinMain, Event-Handler | ⭐⭐⭐ |
| `auwurzel.c` | ~1400 Zeilen | Radix-Zeichnung, Kern-Berechnungen | ⭐⭐⭐ |
| `auwurzel.h` | ~120 Zeilen | Berechnungs-Konstanten | ⭐⭐⭐ |
| `aucalc.c` | ~650 Zeilen | Mathematische Funktionen | ⭐⭐⭐ |
| `aucalc.h` | ~330 Zeilen | Mathematik-Header | ⭐⭐⭐ |
| `astrofil.c` | ~2200 Zeilen | Datenbank-Verwaltung, Dialoge | ⭐⭐ |
| `auhelper.c` | ~1800 Zeilen | Hilfsfunktionen, Farben | ⭐⭐ |
| `auprint.c` | ~800 Zeilen | Druck-Funktionen | ⭐⭐ |
| `aupr1int.c` | ~600 Zeilen | Chart-Rendering | ⭐⭐ |
| `aulistbo.c` | ~350 Zeilen | Listbox-Handling | ⭐ |
| `autransi.c` | ~400 Zeilen | Transit-Berechnungen | ⭐⭐ |
| `aupla.c` | ~800 Zeilen | Ephemeriden (→ SwissEph) | ⚠️ Ersetzt |
| `aupla1.c` | ~900 Zeilen | Erweiterte Ephemeriden | ⚠️ Ersetzt |
| `io.c` | ~600 Zeilen | Datei-I/O | ⭐⭐ |

### 1.2 Daten-Dateien (Übernehmen)

| Datei | Größe | Inhalt |
|-------|-------|--------|
| `astroger.dat` | 271 KB | Deutsche Orte-Datenbank |
| `astroorg.dat` | 180 KB | Orte-Datenbank |
| `europa.dat` | 214 KB | Europa-Orte |
| `astronam.dat` | 1.8 KB | Namen-Datenbank |
| `default.dat` | 40 KB | Standard-Einstellungen |
| `astroini.dat` | 44 KB | Benutzer-Einstellungen |

**Hinweis:** `astroeph.dat` wird durch Swiss Ephemeris ersetzt.

### 1.3 GUI-Dialoge (aus astrouni.rc)

| Dialog | Funktion |
|--------|----------|
| `E_PERSON` | Personen erfassen |
| `E_ORT` | Orte erfassen |
| `ORT_AUSWAHL` | Ort suchen/auswählen |
| `HAUS_AUSWAHL` | Häusersystem wählen |
| `HORO_TYP` | Horoskop-Typ wählen |
| `TRANSIT` | Transit-Eingabe |
| `COMPOSIT` | Composit-Eingabe |
| `ORBEN_EIN` | Orben-Einstellungen |
| `FARBEN` | Farb-Einstellungen |
| `TRANS_EIN` | Transit-Selektion |
| `EINSTELL` | Allgemeine Einstellungen |
| `RADIXDLG` | Radix-Steuerung |

### 1.4 Menü-Struktur

```
MAINMENU
├── Datei
│   └── Beenden
├── Erfassen
│   ├── Personen Erfassen
│   └── Orte Erfassen
├── Horoskop
│   ├── Horoskoptyp
│   ├── Orben
│   ├── Farben
│   └── Einstellungen
└── Hilfe
    ├── Index (F1)
    ├── Tastatur
    ├── Befehle
    ├── Prozeduren
    └── Hilfe verwenden

RADIXMENU (Radix-Fenster)
├── Datei
│   ├── Speichern
│   ├── Drucken
│   └── Exit
├── Horoskop
└── Help

TRANSITMENU (Transit-Fenster)
├── Datei
│   ├── Speichern
│   ├── Drucken
│   └── Exit
├── Horoskop
└── Help
```

---

## 2. Kern-Datenstrukturen (1:1 Port)

### 2.1 ORTE (Orte-Datenbank)
```cpp
struct Orte {
    int16_t  cGultig;      // Record gültig?
    int32_t  lIndex;       // Index im File
    int16_t  slRec;        // Record-Länge
    int16_t  slOrt;        // Ort-Länge
    char     szLand[4];    // Ländercode
    float    fZone;        // Zeitzone
    double   dBreite;      // Geographische Breite
    double   dLange;       // Geographische Länge
};
```

### 2.2 RADIXFIX (Personen-Stammdaten)
```cpp
struct RadixFix {
    char     cGultig;           // Record gültig?
    int32_t  lIndexRadix;       // Index Radix-Daten
    int16_t  slRadix;           // Recordlänge Radix
    int32_t  lIndexName;        // Index Namen-File
    int16_t  slName;            // Recordlänge Name
    int32_t  lIndexNotiz;       // Index Notiz-File
    int16_t  slNotiz;           // Länge Notiz
    int16_t  clVorName;         // Länge Vorname
    int16_t  clName;            // Länge Name
    int16_t  clBeruf;           // Länge Beruf
    int16_t  clOrt;             // Länge Ort
    double   fSommer;           // Sommerzeit
    float    fZone;             // Zeitzone
    char     szLand[4];         // Land
    double   dLange;            // Länge
    double   dBreite;           // Breite
    double   dTime;             // Zeit
    int16_t  sTag;              // Tag
    int16_t  sMonat;            // Monat
    int16_t  sJahr;             // Jahr
};
```

### 2.3 RADIX (Vollständige Horoskop-Daten)
```cpp
struct Radix {
    RadixFix rFix;              // Stammdaten
    QString  pszName;           // Name
    QString  pszVorName;        // Vorname
    QString  pszBeruf;          // Beruf
    QString  pszOrt;            // Ort
    QString  pszNotiz;          // Notiz
    
    // Berechnete Daten
    QVector<char>   pszStzPlanet;    // Sternzeichen Planeten
    QVector<char>   pszStzHaus;      // Sternzeichen Häuser
    QVector<double> pdPlanet;        // Planeten-Positionen
    QVector<double> pdPlanetRad;     // Planeten in Radiant
    QVector<double> pdHaus;          // Häuser-Positionen
    QVector<double> pdHausRad;       // Häuser in Radiant
    QVector<char>   pszAspPlanet;    // Aspekte Planeten
    QVector<double> pdWinkelPlanet;  // Winkel Planeten
    QVector<char>   pszAspHaus;      // Aspekte Häuser
    QVector<double> pdWinkelHaus;    // Winkel Häuser
    QVector<int16_t> psPlanetTyp;    // Planet-Typ (normal/rückläufig)
    QVector<char>   pszInHaus;       // Planet in Haus
    int16_t  sQualitaten[9];         // Qualitäten
    
    // Berechnungs-Variablen
    double   dBreite;           // Geographische Breite
    double   dT;                // Zeit-Variable
    double   dJD;               // Julianisches Datum
    double   dSid;              // Siderische Zeit
    double   VER;               // Vertex
    double   MC;                // Medium Coeli
    double   ASC;               // Aszendent
    double   RA;                // Rektaszension
    double   OB;                // Obliquität
    
    char     cHausSys;          // Häusersystem
    char     cHoroTyp;          // Horoskop-Typ
    int16_t  sAnzahlPlanet;     // Anzahl Planeten
};
```

### 2.4 AU_INIT (Globale Einstellungen)
```cpp
struct AuInit {
    int16_t  sSelHaus;          // Gewähltes Häusersystem
    int16_t  sSelHoro;          // Gewählter Horoskop-Typ
    int16_t  sRotRadix;         // Radix-Rotation
    int16_t  sAspekte;          // Aspekt-Auswahl
    int16_t  sTeilung;          // Teilung (3er/9er)
    
    QVector<float> pfOrbenPlanet;   // Orben Planeten
    QVector<float> pfOrbenHaus;     // Orben Häuser
    QVector<float> pfOrbenTPlanet;  // Orben Transit-Planeten
    QVector<float> pfOrbenTHaus;    // Orben Transit-Häuser
    QVector<float> pfOrbenSPlanet;  // Orben Synastrie-Planeten
    QVector<float> pfOrbenSHaus;    // Orben Synastrie-Häuser
    
    Radix    Radix;             // Aktuelle Radix-Daten
};
```

---

## 3. Projektstruktur

```
astrouni2026/
├── CMakeLists.txt              # Haupt-Build
├── README.md
│
├── src/
│   ├── main.cpp                # Qt Application Entry
│   │
│   ├── core/                   # Kern-Logik (GUI-unabhängig)
│   │   ├── CMakeLists.txt
│   │   ├── constants.h         # Konstanten aus astrouni.h
│   │   ├── data_types.h        # RADIX, ORTE, AU_INIT
│   │   ├── data_types.cpp
│   │   ├── calculations.h      # Port von aucalc.c
│   │   ├── calculations.cpp
│   │   ├── chart_calc.h        # Port von auwurzel.c (Berechnungen)
│   │   ├── chart_calc.cpp
│   │   ├── transit_calc.h      # Port von autransi.c
│   │   ├── transit_calc.cpp
│   │   └── swiss_eph.h/cpp     # Swiss Ephemeris Wrapper
│   │
│   ├── data/                   # Daten-Handling
│   │   ├── CMakeLists.txt
│   │   ├── legacy_io.h         # Port von io.c, astrofil.c
│   │   ├── legacy_io.cpp
│   │   ├── orte_db.h           # Orte-Datenbank
│   │   ├── orte_db.cpp
│   │   ├── person_db.h         # Personen-Datenbank
│   │   └── person_db.cpp
│   │
│   └── gui/                    # Qt6 GUI
│       ├── CMakeLists.txt
│       ├── main_window.h       # Port von MainWndProc
│       ├── main_window.cpp
│       ├── main_window.ui
│       │
│       ├── radix_window.h      # Port von RadixWndProc
│       ├── radix_window.cpp
│       ├── radix_window.ui
│       │
│       ├── transit_window.h    # Port von TransitWndProc
│       ├── transit_window.cpp
│       │
│       ├── chart_widget.h      # Radix-Zeichnung (auwurzel.c)
│       ├── chart_widget.cpp
│       │
│       └── dialogs/
│           ├── person_dialog.h/cpp/ui      # E_PERSON
│           ├── ort_dialog.h/cpp/ui         # E_ORT
│           ├── ort_search_dialog.h/cpp/ui  # ORT_AUSWAHL
│           ├── haus_dialog.h/cpp/ui        # HAUS_AUSWAHL
│           ├── horo_typ_dialog.h/cpp/ui    # HORO_TYP
│           ├── transit_dialog.h/cpp/ui     # TRANSIT
│           ├── composit_dialog.h/cpp/ui    # COMPOSIT
│           ├── orben_dialog.h/cpp/ui       # ORBEN_EIN
│           ├── farben_dialog.h/cpp/ui      # FARBEN
│           ├── trans_sel_dialog.h/cpp/ui   # TRANS_EIN
│           └── settings_dialog.h/cpp/ui    # EINSTELL
│
├── data/                       # Daten-Dateien
│   ├── astroger.dat            # Kopie aus legacy/
│   ├── astroorg.dat
│   ├── europa.dat
│   ├── astronam.dat
│   ├── default.dat
│   └── astroini.dat
│
├── resources/
│   ├── fonts/
│   │   └── asu_____.ttf        # Astrologie-Font
│   ├── icons/
│   │   ├── main.ico
│   │   └── radix.ico
│   └── astrouni.qrc            # Qt Resource File
│
├── swisseph/                   # Swiss Ephemeris
│   ├── src/                    # SwissEph Quellcode
│   └── ephe/                   # Ephemeriden-Dateien
│
├── tests/
│   ├── CMakeLists.txt
│   ├── test_calculations.cpp
│   ├── test_chart_calc.cpp
│   └── test_legacy_io.cpp
│
└── docs/
    ├── PORTIERUNGSPLAN_QT6.md  # Diese Datei
    └── MAPPING.md              # Funktions-Mapping
```

---

## 4. Funktions-Mapping (Legacy → Qt6)

### 4.1 Haupt-Funktionen

| Legacy | Qt6 | Datei |
|--------|-----|-------|
| `WinMain()` | `main()` | `main.cpp` |
| `InitApplication()` | `MainWindow::MainWindow()` | `main_window.cpp` |
| `MainWndProc()` | `MainWindow` Slots | `main_window.cpp` |
| `RadixWndProc()` | `RadixWindow` Slots | `radix_window.cpp` |
| `TransitWndProc()` | `TransitWindow` Slots | `transit_window.cpp` |

### 4.2 Berechnungs-Funktionen

| Legacy | Qt6 | Datei |
|--------|-----|-------|
| `dJulTag()` | `Calculations::julianDay()` | `calculations.cpp` |
| `dMod()` | `Calculations::mod360()` | `calculations.cpp` |
| `vGrad2GMS()` | `Calculations::degToGMS()` | `calculations.cpp` |
| `sCalcChart()` | `ChartCalc::calculate()` | `chart_calc.cpp` |
| `sCalcHauser()` | `ChartCalc::calcHouses()` | `chart_calc.cpp` |
| `vCalcPlanets()` | `ChartCalc::calcPlanets()` | `chart_calc.cpp` |
| `vKoch()` | `ChartCalc::housesKoch()` | `chart_calc.cpp` |
| `vHausPlacidus()` | `ChartCalc::housesPlacidus()` | `chart_calc.cpp` |
| `vEqual()` | `ChartCalc::housesEqual()` | `chart_calc.cpp` |
| `vTopocentric()` | `ChartCalc::housesTopocentric()` | `chart_calc.cpp` |
| `vCampanus()` | `ChartCalc::housesCampanus()` | `chart_calc.cpp` |
| `vRegiomontanus()` | `ChartCalc::housesRegiomontanus()` | `chart_calc.cpp` |
| `vPorphyry()` | `ChartCalc::housesPorphyry()` | `chart_calc.cpp` |
| `vMorinus()` | `ChartCalc::housesMorinus()` | `chart_calc.cpp` |
| `vAlcabitius()` | `ChartCalc::housesAlcabitius()` | `chart_calc.cpp` |
| `vWhole()` | `ChartCalc::housesWhole()` | `chart_calc.cpp` |

### 4.3 Zeichnungs-Funktionen

| Legacy | Qt6 | Datei |
|--------|-----|-------|
| `sRadix()` | `ChartWidget::paintEvent()` | `chart_widget.cpp` |
| `sHaus()` | `ChartWidget::drawHouses()` | `chart_widget.cpp` |
| `sPlanet()` | `ChartWidget::drawPlanets()` | `chart_widget.cpp` |
| `sDrawAspekte()` | `ChartWidget::drawAspects()` | `chart_widget.cpp` |
| `vPlanetDraw()` | `ChartWidget::drawPlanetSymbol()` | `chart_widget.cpp` |
| `vPlanetTicDraw()` | `ChartWidget::drawPlanetTic()` | `chart_widget.cpp` |

### 4.4 Dialog-Funktionen

| Legacy | Qt6 | Datei |
|--------|-----|-------|
| `DlgPErfassen()` | `PersonDialog` | `person_dialog.cpp` |
| `DlgOErfassen()` | `OrtDialog` | `ort_dialog.cpp` |
| `DlgOrtAuswahl()` | `OrtSearchDialog` | `ort_search_dialog.cpp` |
| `DlgHausAuswahl()` | `HausDialog` | `haus_dialog.cpp` |
| `DlgHoroAuswahl()` | `HoroTypDialog` | `horo_typ_dialog.cpp` |
| `DlgTransit()` | `TransitDialog` | `transit_dialog.cpp` |
| `DlgOrbenEin()` | `OrbenDialog` | `orben_dialog.cpp` |
| `DlgFarben()` | `FarbenDialog` | `farben_dialog.cpp` |
| `DlgTransEin()` | `TransSelDialog` | `trans_sel_dialog.cpp` |

### 4.5 I/O-Funktionen

| Legacy | Qt6 | Datei |
|--------|-----|-------|
| `sReadIni()` | `LegacyIO::readIni()` | `legacy_io.cpp` |
| `sWriteini()` | `LegacyIO::writeIni()` | `legacy_io.cpp` |
| `sReadDat()` | `LegacyIO::readDat()` | `legacy_io.cpp` |
| `sWriteDat()` | `LegacyIO::writeDat()` | `legacy_io.cpp` |
| `sOrben()` | `LegacyIO::readOrben()` | `legacy_io.cpp` |
| `sSearchOrt()` | `OrteDB::search()` | `orte_db.cpp` |

---

## 5. Ephemeriden-Ersetzung

### Legacy (aupla.c, aupla1.c)
- Eigene Ephemeriden-Berechnung
- `astroeph.dat` Binärdatei
- Begrenzte Genauigkeit

### Neu (Swiss Ephemeris)
```cpp
class SwissEph {
public:
    void setEphePath(const QString& path);
    double calcPlanet(int planet, double jd, int flags);
    void calcHouses(double jd, double lat, double lon, 
                    int hsys, double* cusps, double* ascmc);
    QString getLastError();
};
```

**Mapping:**
| Legacy Planet | SwissEph |
|---------------|----------|
| P_SONNE (0) | SE_SUN |
| P_MOND (1) | SE_MOON |
| P_MERKUR (2) | SE_MERCURY |
| P_VENUS (3) | SE_VENUS |
| P_MARS (4) | SE_MARS |
| P_JUPITER (5) | SE_JUPITER |
| P_SATURN (6) | SE_SATURN |
| P_URANUS (7) | SE_URANUS |
| P_NEPTUN (8) | SE_NEPTUNE |
| P_PLUTO (9) | SE_PLUTO |
| P_NKNOTEN (10) | SE_TRUE_NODE |
| P_LILITH (11) | SE_MEAN_APOG |
| P_CHIRON (12) | SE_CHIRON |
| P_CERES (13) | SE_CERES |
| P_PALLAS (14) | SE_PALLAS |
| P_JUNO (15) | SE_JUNO |
| P_VESTA (16) | SE_VESTA |

---

## 6. Implementierungs-Reihenfolge

### Phase 1: Grundstruktur (Woche 1)
1. [ ] Projektstruktur erstellen
2. [ ] CMakeLists.txt konfigurieren
3. [ ] Qt6 Setup verifizieren
4. [ ] Swiss Ephemeris integrieren

### Phase 2: Core-Logik (Woche 2-3)
1. [ ] `constants.h` - Alle Konstanten portieren
2. [ ] `data_types.h/cpp` - Strukturen portieren
3. [ ] `calculations.h/cpp` - Mathematik portieren
4. [ ] `swiss_eph.h/cpp` - SwissEph Wrapper
5. [ ] `chart_calc.h/cpp` - Häuser-Berechnungen
6. [ ] Unit Tests für Core

### Phase 3: Daten-Handling (Woche 4)
1. [ ] `legacy_io.h/cpp` - .dat Dateien lesen/schreiben
2. [ ] `orte_db.h/cpp` - Orte-Datenbank
3. [ ] `person_db.h/cpp` - Personen-Datenbank
4. [ ] Unit Tests für I/O

### Phase 4: GUI-Grundgerüst (Woche 5-6)
1. [ ] `main_window.h/cpp/ui` - Hauptfenster
2. [ ] `radix_window.h/cpp/ui` - Radix-Fenster
3. [ ] `chart_widget.h/cpp` - Chart-Zeichnung
4. [ ] Menü-Struktur

### Phase 5: Dialoge (Woche 7-8)
1. [ ] `person_dialog` - Personen erfassen
2. [ ] `ort_dialog` - Orte erfassen
3. [ ] `ort_search_dialog` - Ort suchen
4. [ ] `haus_dialog` - Häusersystem
5. [ ] `horo_typ_dialog` - Horoskop-Typ
6. [ ] `orben_dialog` - Orben
7. [ ] `farben_dialog` - Farben
8. [ ] `settings_dialog` - Einstellungen

### Phase 6: Erweiterte Features (Woche 9-10)
1. [ ] `transit_window` - Transit-Fenster
2. [ ] `transit_dialog` - Transit-Eingabe
3. [ ] `transit_calc` - Transit-Berechnung
4. [ ] Druck-Funktionen
5. [ ] Export (PNG, PDF)

### Phase 7: Feinschliff (Woche 11-12)
1. [ ] Vollständige Tests
2. [ ] Dokumentation
3. [ ] Cross-Platform Build (Mac, Windows, Linux)
4. [ ] Release-Vorbereitung

---

## 7. Technische Anforderungen

### Build-System
- **CMake** ≥ 3.16
- **C++20** Standard
- **Qt6** ≥ 6.5

### Abhängigkeiten
- Qt6::Core
- Qt6::Gui
- Qt6::Widgets
- Qt6::PrintSupport
- Swiss Ephemeris (statisch gelinkt)

### Plattformen
- **Linux:** GCC 12+ / Clang 15+
- **macOS:** Xcode 14+ / Apple Clang
- **Windows:** MSVC 2022 / MinGW-w64

---

## 8. Qualitätssicherung

### Tests
- Unit Tests für alle Berechnungen
- Vergleich mit Legacy-Ergebnissen
- GUI-Tests mit Qt Test

### Validierung
- Vergleich der Horoskop-Ergebnisse mit Legacy
- Überprüfung der .dat Datei-Kompatibilität
- Cross-Platform Tests

---

*Erstellt: Dezember 2025*
*Basierend auf Legacy-Code-Analyse*
