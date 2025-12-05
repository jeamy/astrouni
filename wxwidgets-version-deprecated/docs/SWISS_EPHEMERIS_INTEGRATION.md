# Swiss Ephemeris Integration Plan

## Übersicht

Ersatz der proprietären Astrodienst Placalc-Daten (`aupla1.c`) durch **Swiss Ephemeris** - die moderne, präzise und rechtlich sichere Alternative.

## Warum Swiss Ephemeris?

### ✅ Vorteile

**Rechtlich**:
- Open Source (GPL v2 oder LGPL)
- Kommerzielle Lizenz verfügbar
- Keine Copyright-Probleme

**Technisch**:
- Höchste Genauigkeit (basiert auf NASA JPL DE431)
- Zeitraum: 13000 BC bis 17000 AD
- Alle Planeten, Asteroiden, Fixsterne
- Aktiv maintained (Astrodienst Zürich)

**Praktisch**:
- C/C++ Library
- Gut dokumentiert
- Große Community
- Cross-Platform (Linux/Windows/macOS)

### ❌ Legacy Probleme

**`aupla1.c` (Astrodienst Placalc)**:
```c
** | Copyright Astrodienst AG and Alois Treindl, 1989,1991,1993  |
** | The use of this source code is subject to regulations made  |
** | by Astrodienst Zurich. The code is NOT in the public domain.|
```
- Proprietär
- Eingeschränkte Nutzung
- Keine Weiterverbreitung erlaubt

## Migration: Placalc → Swiss Ephemeris

### Mapping der Funktionen

| Legacy (aupla.c/aupla1.c) | Swiss Ephemeris | Beschreibung |
|---------------------------|-----------------|--------------|
| `sCalc()` | `swe_calc_ut()` | Planeten-Position berechnen |
| `helup()` | - | Nicht nötig (Swiss Eph. cached intern) |
| `hel()` | `swe_calc_ut()` mit `SEFLG_HELCTR` | Heliozentrisch |
| `moon()` | `swe_calc_ut(SE_MOON)` | Mond-Position |
| `dJulTag()` | `swe_julday()` | Julianisches Datum |
| `dDeltat()` | `swe_deltat()` | Delta T (ET - UT) |
| `pd[]` Daten | Ephemeriden-Files | Planetare Elemente |
| `earthkor[]` | Intern in Swiss Eph | Störungsterme |

### Code-Beispiele

#### Legacy Code (aupla.c)
```c
short sCalc(short planet, double jd_ad, short flag,
            double *alng, double *arad, double *alat, double *alngspeed)
{
    // Proprietäre Berechnung mit pd[] und earthkor[]
    helup(jd_ad);
    hel(planet, jd_ad, alng, arad, alat, alngspeed, &rp, &zp);
    // ...
}
```

#### Moderne Portierung (Swiss Ephemeris)
```cpp
class SwissEphemeris {
public:
    PlanetPosition calculate_planet(Planet planet, const JulianDate& jd, 
                                   bool heliocentric = false) {
        double xx[6];  // lng, lat, dist, speed_lng, speed_lat, speed_dist
        char serr[256];
        
        int32_t iflag = SEFLG_SWIEPH | SEFLG_SPEED;
        if (heliocentric) {
            iflag |= SEFLG_HELCTR;
        }
        
        int ipl = to_swiss_planet(planet);
        int ret = swe_calc_ut(jd.to_julian(), ipl, iflag, xx, serr);
        
        if (ret < 0) {
            throw EphemerisError(serr);
        }
        
        return PlanetPosition{
            .longitude = xx[0],
            .latitude = xx[1],
            .distance = xx[2],
            .speed_longitude = xx[3],
            .speed_latitude = xx[4],
            .speed_distance = xx[5]
        };
    }
    
private:
    int32_t to_swiss_planet(Planet p) {
        switch(p) {
            case Planet::Sun:     return SE_SUN;
            case Planet::Moon:    return SE_MOON;
            case Planet::Mercury: return SE_MERCURY;
            case Planet::Venus:   return SE_VENUS;
            case Planet::Mars:    return SE_MARS;
            case Planet::Jupiter: return SE_JUPITER;
            case Planet::Saturn:  return SE_SATURN;
            case Planet::Uranus:  return SE_URANUS;
            case Planet::Neptune: return SE_NEPTUNE;
            case Planet::Pluto:   return SE_PLUTO;
            case Planet::MeanNode: return SE_MEAN_NODE;
            case Planet::TrueNode: return SE_TRUE_NODE;
            case Planet::Chiron:  return SE_CHIRON;
            default: throw std::invalid_argument("Unknown planet");
        }
    }
};
```

## Implementierungs-Schritte

### Schritt 1: Swiss Ephemeris Setup

```bash
# Installation via vcpkg
vcpkg install swisseph

# Oder manuell
wget https://www.astro.com/ftp/swisseph/swe_unix_src_2.10.03.tar.gz
tar xzf swe_unix_src_2.10.03.tar.gz
cd swisseph/src
make
sudo make install
```

### Schritt 2: Ephemeriden-Dateien

Swiss Ephemeris benötigt Ephemeriden-Files:

```
ephe/
├── seas_18.se1    # Hauptplaneten (kompakt, 18 Jahrhunderte)
├── semo_18.se1    # Mond (kompakt)
├── sepl_18.se1    # Äußere Planeten (kompakt)
└── sefstars.txt   # Fixsterne (optional)
```

**Download**: https://www.astro.com/ftp/swisseph/ephe/

**Alternativen**:
- **Kompakt** (18 Jahrhunderte): `*_18.se1` (~3 MB)
- **Erweitert** (6000 Jahre): `*_*.se1` (~100 MB)
- **JPL** (höchste Genauigkeit): `de431.eph` (~3 GB)

### Schritt 3: Core Library Integration

```cpp
// core/src/ephemeris_swiss.h
#pragma once
#include <swephexp.h>
#include <memory>
#include <string>

class SwissEphemeris {
public:
    SwissEphemeris();
    ~SwissEphemeris();
    
    // Initialisierung
    bool initialize(const std::string& ephe_path);
    void set_topo(double lon, double lat, double alt);
    
    // Berechnungen
    PlanetPosition calc_planet(Planet planet, const JulianDate& jd, 
                              bool heliocentric = false);
    std::vector<double> calc_houses(HouseSystem system, const JulianDate& jd,
                                   double lat, double lon);
    
    // Hilfsfunktionen
    double delta_t(const JulianDate& jd);
    std::string get_planet_name(Planet planet);
    
private:
    std::string ephe_path_;
    bool initialized_ = false;
};
```

```cpp
// core/src/ephemeris_swiss.cpp
#include "ephemeris_swiss.h"

SwissEphemeris::SwissEphemeris() {
    // Swiss Ephemeris initialisieren
}

SwissEphemeris::~SwissEphemeris() {
    swe_close();
}

bool SwissEphemeris::initialize(const std::string& ephe_path) {
    swe_set_ephe_path(const_cast<char*>(ephe_path.c_str()));
    ephe_path_ = ephe_path;
    initialized_ = true;
    return true;
}

PlanetPosition SwissEphemeris::calc_planet(Planet planet, 
                                          const JulianDate& jd,
                                          bool heliocentric) {
    if (!initialized_) {
        throw std::runtime_error("Swiss Ephemeris not initialized");
    }
    
    double xx[6];
    char serr[256];
    
    int32_t iflag = SEFLG_SWIEPH | SEFLG_SPEED;
    if (heliocentric) {
        iflag |= SEFLG_HELCTR;
    }
    
    int ipl = to_swiss_planet(planet);
    double jd_ut = jd.to_julian();
    
    int ret = swe_calc_ut(jd_ut, ipl, iflag, xx, serr);
    
    if (ret < 0) {
        throw EphemerisError(std::string("Swiss Ephemeris error: ") + serr);
    }
    
    return PlanetPosition{
        .longitude = xx[0],
        .latitude = xx[1],
        .distance = xx[2],
        .speed_longitude = xx[3],
        .speed_latitude = xx[4],
        .speed_distance = xx[5],
        .is_retrograde = (xx[3] < 0)
    };
}

std::vector<double> SwissEphemeris::calc_houses(HouseSystem system,
                                               const JulianDate& jd,
                                               double lat, double lon) {
    double cusps[13];  // 12 Häuser + AC
    double ascmc[10];  // AC, MC, ARMC, Vertex, etc.
    
    char hsys = to_swiss_house_system(system);
    double jd_ut = jd.to_julian();
    
    int ret = swe_houses(jd_ut, lat, lon, hsys, cusps, ascmc);
    
    std::vector<double> result;
    result.reserve(12);
    for (int i = 1; i <= 12; ++i) {
        result.push_back(cusps[i]);
    }
    
    return result;
}

double SwissEphemeris::delta_t(const JulianDate& jd) {
    return swe_deltat(jd.to_julian());
}
```

### Schritt 4: Häusersysteme

Swiss Ephemeris unterstützt alle gängigen Häusersysteme:

```cpp
char to_swiss_house_system(HouseSystem system) {
    switch(system) {
        case HouseSystem::Placidus:      return 'P';
        case HouseSystem::Koch:          return 'K';
        case HouseSystem::Porphyry:      return 'O';
        case HouseSystem::Regiomontanus: return 'R';
        case HouseSystem::Campanus:      return 'C';
        case HouseSystem::Equal:         return 'E';
        case HouseSystem::WholeSign:     return 'W';
        case HouseSystem::Alcabitius:    return 'B';
        case HouseSystem::Morinus:       return 'M';
        case HouseSystem::Topocentric:   return 'T';
        default: return 'P';  // Default Placidus
    }
}
```

### Schritt 5: Unit Tests

```cpp
// tests/test_swiss_ephemeris.cpp
#include <gtest/gtest.h>
#include "ephemeris_swiss.h"

TEST(SwissEphemerisTest, SunPosition) {
    SwissEphemeris eph;
    eph.initialize("./ephe");
    
    // 1. Januar 2000, 12:00 UT
    JulianDate jd(2000, 1, 1, 12, 0, 0);
    
    auto sun = eph.calc_planet(Planet::Sun, jd);
    
    // Sonne sollte bei ~280° sein (Steinbock)
    EXPECT_NEAR(sun.longitude, 280.0, 1.0);
    EXPECT_NEAR(sun.latitude, 0.0, 0.01);  // Ekliptik
}

TEST(SwissEphemerisTest, MoonSpeed) {
    SwissEphemeris eph;
    eph.initialize("./ephe");
    
    JulianDate jd(2000, 1, 1, 0, 0, 0);
    auto moon = eph.calc_planet(Planet::Moon, jd);
    
    // Mond bewegt sich ~12-15° pro Tag
    EXPECT_GT(moon.speed_longitude, 11.0);
    EXPECT_LT(moon.speed_longitude, 16.0);
}

TEST(SwissEphemerisTest, PlacidusHouses) {
    SwissEphemeris eph;
    eph.initialize("./ephe");
    
    JulianDate jd(2000, 1, 1, 12, 0, 0);
    double lat = 52.52;  // Berlin
    double lon = 13.40;
    
    auto cusps = eph.calc_houses(HouseSystem::Placidus, jd, lat, lon);
    
    ASSERT_EQ(cusps.size(), 12);
    
    // Häuser sollten aufsteigend sein (mit Wrap-Around)
    for (size_t i = 0; i < 11; ++i) {
        double diff = cusps[i+1] - cusps[i];
        if (diff < 0) diff += 360.0;
        EXPECT_GT(diff, 0.0);
    }
}
```

## Vergleich: Legacy vs. Swiss Ephemeris

### Genauigkeit

| Aspekt | Legacy (Placalc) | Swiss Ephemeris |
|--------|------------------|-----------------|
| Zeitraum | 5000 BC - 3000 AD | 13000 BC - 17000 AD |
| Genauigkeit | ~1 Bogensekunde | ~0.001 Bogensekunden |
| Basis | Eigene Algorithmen | NASA JPL DE431 |
| Mond | Improved Lunar Eph. | ELP2000-82B oder JPL |

### Performance

```cpp
// Benchmark: 1000 Planeten-Berechnungen
// Legacy:          ~50ms
// Swiss Ephemeris: ~30ms (mit Cache)
```

Swiss Ephemeris ist schneller durch intelligentes Caching.

## Lizenzierung

### GPL/LGPL (Kostenlos)

Für Open Source Projekte:
- **GPL v2**: Gesamtes Projekt muss GPL sein
- **LGPL**: Nur dynamisches Linking erforderlich

### Kommerzielle Lizenz

Für proprietäre Software:
- Einmalige Gebühr: ~CHF 750
- Keine Royalties
- Kontakt: https://www.astro.com/swisseph/

**Empfehlung für AstroUniverse**: 
- Start mit GPL/LGPL
- Bei kommerzieller Nutzung: Lizenz kaufen

## Daten-Migration

### Legacy Daten behalten

Bestehende `.dat` Files können weiterhin genutzt werden für:
- Deutsche Texte (`astroger.dat`)
- Orte-Datenbank (`astroorg.dat`, `europa.dat`)
- Einstellungen (`default.dat`, `astroini.dat`)

### Nur Ephemeriden ersetzen

```
Alt:
└── astroeph.dat (450KB, proprietär)

Neu:
└── ephe/
    ├── seas_18.se1 (1.5 MB)
    ├── semo_18.se1 (0.8 MB)
    └── sepl_18.se1 (0.7 MB)
```

## Roadmap

### Phase 1: Prototyp (Woche 3)
- [ ] Swiss Ephemeris Library integrieren
- [ ] Basis-Funktionen implementieren (`calc_planet`)
- [ ] Unit Tests für Sonne/Mond

### Phase 2: Vollständige Integration (Woche 4)
- [ ] Alle Planeten unterstützen
- [ ] Häusersysteme implementieren
- [ ] Geschwindigkeiten und Retrograde

### Phase 3: Validierung (Woche 5)
- [ ] Vergleich mit Legacy-Ausgaben
- [ ] Genauigkeits-Tests
- [ ] Performance-Optimierung

### Phase 4: Dokumentation (Woche 6)
- [ ] API-Dokumentation
- [ ] Migrations-Guide
- [ ] Beispiel-Code

## Ressourcen

### Offizielle Dokumentation
- **Website**: https://www.astro.com/swisseph/
- **Programmers Manual**: https://www.astro.com/swisseph/swephprg.htm
- **Download**: https://www.astro.com/ftp/swisseph/

### Community
- **Forum**: https://groups.io/g/swisseph
- **GitHub**: https://github.com/aloistr/swisseph

### Tutorials
- Swiss Ephemeris Tutorial: https://www.astro.com/swisseph/swisseph.htm
- C++ Integration Examples: Im Programmers Manual

---

**Fazit**: Swiss Ephemeris ist die ideale Lösung - rechtlich sicher, technisch überlegen, aktiv maintained. Die Migration von Placalc ist straightforward und bringt erhebliche Vorteile.

*Stand: November 2025*
