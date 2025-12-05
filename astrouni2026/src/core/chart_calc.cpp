/**
 * @file chart_calc.cpp
 * @brief 1:1 Port der Horoskop-Berechnungen aus legacy/auwurzel.c
 */

#include "chart_calc.h"
#include "calculations.h"
#include "swiss_eph.h"
#include <cmath>

namespace astro {

//==============================================================================
// Haupt-Berechnungsfunktionen
//==============================================================================

int ChartCalc::calculate(Radix& radix, Radix* transit, int typ) {
    // 1. Julianisches Datum berechnen
    radix.jd = Calculations::julianDay(
        radix.rFix.tag, 
        radix.rFix.monat, 
        radix.rFix.jahr,
        radix.rFix.zeit - radix.rFix.zone - radix.rFix.sommerzeit,
        true  // Gregorianisch
    );
    
    // 2. Variablen berechnen (MC, ASC, Siderische Zeit, etc.)
    calcVariables(radix);
    
    // 3. Häuser berechnen
    int result = calcHouses(radix);
    if (result != ERR_OK) {
        return result;
    }
    
    // 4. Planeten berechnen
    calcPlanets(radix);
    
    // 5. Planet-Typen setzen
    setPlanetType(radix, typ);
    
    // 6. Qualitäten berechnen
    calcQualities(radix);
    
    // 7. Fehlende Werte für Synastrie (STRICT LEGACY: kein Composit)
    if (transit != nullptr && typ == TYP_SYNASTRIE) {
        calcMissing(radix, transit, typ);
    }
    
    return ERR_OK;
}

void ChartCalc::calcVariables(Radix& radix) {
    // Breite für Berechnungen
    radix.breite = radix.rFix.breite;
    
    // Delta T
    radix.dT = swissEph().calcDeltaT(radix.jd);
    
    // Obliquität (Schiefe der Ekliptik)
    radix.ob = swissEph().calcObliquity(radix.jd);
    
    // Siderische Zeit
    radix.sid = swissEph().calcSiderealTime(radix.jd, radix.rFix.laenge);
    
    // RAMC (Rektaszension des MC)
    radix.ra = radix.sid;
    
    // MC (Medium Coeli)
    double tanRA = std::tan(radix.ra * PI / DEGHALB);
    double cosOB = std::cos(radix.ob * PI / DEGHALB);
    radix.mc = std::atan(tanRA / cosOB) * DEGHALB / PI;
    
    // MC in den richtigen Quadranten bringen
    if (radix.ra >= 180.0) {
        radix.mc += 180.0;
    }
    if (radix.mc < 0.0) {
        radix.mc += 360.0;
    }
    radix.mc = Calculations::mod360(radix.mc);
    
    // ASC (Aszendent)
    double sinRA = std::sin(radix.ra * PI / DEGHALB);
    double tanLat = std::tan(radix.breite * PI / DEGHALB);
    double sinOB = std::sin(radix.ob * PI / DEGHALB);
    
    double y = -std::cos(radix.ra * PI / DEGHALB);
    double x = sinRA * cosOB + tanLat * sinOB;
    
    radix.asc = std::atan2(y, x) * DEGHALB / PI;
    radix.asc = Calculations::mod360(radix.asc);
    
    // Vertex
    double coLat = 90.0 - radix.breite;
    double tanCoLat = std::tan(coLat * PI / DEGHALB);
    y = -std::cos((radix.ra + 180.0) * PI / DEGHALB);
    x = std::sin((radix.ra + 180.0) * PI / DEGHALB) * cosOB + tanCoLat * sinOB;
    radix.ver = std::atan2(y, x) * DEGHALB / PI;
    radix.ver = Calculations::mod360(radix.ver);
}

int ChartCalc::calcHouses(Radix& radix) {
    // Swiss Ephemeris für Häuserberechnung verwenden
    double cusps[13];
    double ascmc[10];
    
    bool success = swissEph().calcHouses(
        radix.jd,
        radix.breite,
        radix.rFix.laenge,
        radix.hausSys,
        cusps,
        ascmc
    );
    
    if (!success) {
        return ERR_HOUSE;
    }
    
    // Häuserspitzen übernehmen
    for (int i = 0; i < MAX_HAUS; ++i) {
        radix.haus[i] = cusps[i + 1];  // Swiss Eph: Index 1-12
        radix.hausRad[i] = radix.haus[i] * PI / DEGHALB;
        radix.stzHaus[i] = Calculations::getZeichen(radix.haus[i]);
    }
    
    // ASC und MC aus Swiss Ephemeris übernehmen
    radix.asc = ascmc[0];
    radix.mc = ascmc[1];
    
    return ERR_OK;
}

void ChartCalc::calcPlanets(Radix& radix) {
    for (int i = 0; i < radix.anzahlPlanet; ++i) {
        double lon, lat, dist, speed;
        
        if (swissEph().calcPlanet(i, radix.jd, lon, lat, dist, speed)) {
            radix.planet[i] = lon;
            radix.planetRad[i] = lon * PI / DEGHALB;
            radix.stzPlanet[i] = Calculations::getZeichen(lon);
            
            // Rückläufigkeit prüfen
            if (speed < 0.0) {
                radix.planetTyp[i] |= P_TYP_RUCK;
            }
            
            // In welchem Haus steht der Planet?
            radix.inHaus[i] = getHouseOfPlanet(lon, radix.haus);
        } else {
            // Berechnung fehlgeschlagen - Position auf 0 setzen
            radix.planet[i] = 0.0;
            radix.planetRad[i] = 0.0;
            radix.stzPlanet[i] = 0;
            radix.inHaus[i] = 0;
            // Debug: Fehler ausgeben
            qWarning("Planet %d konnte nicht berechnet werden: %s", 
                     i, qPrintable(swissEph().getLastError()));
        }
    }
}

void ChartCalc::calcMissing(Radix& radix, Radix* transit, int typ) {
    // STRICT LEGACY: Nur Radix, Synastrie, Transit - kein Composit
    // Synastrie: Beide Radixe bleiben unverändert
    // Transit: Transit-Planeten werden separat berechnet
    Q_UNUSED(radix);
    Q_UNUSED(transit);
    Q_UNUSED(typ);
}

//==============================================================================
// Aspekt-Berechnungen
//==============================================================================

void ChartCalc::calcAspects(Radix& radix, const QVector<float>& orben) {
    static const int aspekte[] = {
        KONJUNKTION, HALBSEX, SEXTIL, QUADRATUR, TRIGON, QUINCUNX, OPOSITION
    };
    
    int numPlanets = radix.anzahlPlanet;
    
    for (int i = 0; i < numPlanets; ++i) {
        for (int j = i + 1; j < numPlanets; ++j) {
            int idx = i * numPlanets + j;
            radix.aspPlanet[idx] = KEIN_ASP;
            radix.winkelPlanet[idx] = 0.0;
            
            double pos1 = radix.planet[i];
            double pos2 = radix.planet[j];
            
            for (int a = 0; a < ASPEKTE; ++a) {
                int aspekt = aspekte[a];
                float orb = (orben.size() > a * MAX_PLANET + i) ? 
                            orben[a * MAX_PLANET + i] : 8.0f;
                
                double exakterWinkel;
                if (Calculations::checkAspekt(pos1, pos2, aspekt, orb, exakterWinkel)) {
                    radix.aspPlanet[idx] = static_cast<int8_t>(aspekt);
                    radix.winkelPlanet[idx] = exakterWinkel;
                    break;  // Nur den engsten Aspekt speichern
                }
            }
            
            // Symmetrische Matrix
            radix.aspPlanet[j * numPlanets + i] = radix.aspPlanet[idx];
            radix.winkelPlanet[j * numPlanets + i] = radix.winkelPlanet[idx];
        }
    }
}

void ChartCalc::calcAngles(Radix& radix, Radix* transit, int typ) {
    int numPlanets = radix.anzahlPlanet;
    
    // Winkel zwischen Planeten
    for (int i = 0; i < numPlanets; ++i) {
        for (int j = 0; j < numPlanets; ++j) {
            int idx = i * numPlanets + j;
            
            double pos1 = radix.planet[i];
            double pos2 = (transit != nullptr && typ == TYP_TRANSIT) ? 
                          transit->planet[j] : radix.planet[j];
            
            radix.winkelPlanet[idx] = std::fabs(Calculations::minDist(pos1, pos2));
        }
    }
    
    // Winkel zwischen Planeten und Häusern
    for (int i = 0; i < numPlanets; ++i) {
        for (int h = 0; h < MAX_HAUS; ++h) {
            int idx = i * MAX_HAUS + h;
            radix.winkelHaus[idx] = std::fabs(Calculations::minDist(radix.planet[i], radix.haus[h]));
        }
    }
}

//==============================================================================
// Qualitäten-Berechnung
//==============================================================================

void ChartCalc::calcQualities(Radix& radix) {
    // Qualitäten zurücksetzen
    for (int i = 0; i < MAX_QUALITATEN; ++i) {
        radix.qualitaten[i] = 0;
    }
    
    // Element-Zuordnung der Zeichen
    // Feuer: Widder, Löwe, Schütze (0, 4, 8)
    // Erde: Stier, Jungfrau, Steinbock (1, 5, 9)
    // Luft: Zwillinge, Waage, Wassermann (2, 6, 10)
    // Wasser: Krebs, Skorpion, Fische (3, 7, 11)
    
    // Modus-Zuordnung
    // Cardinal: Widder, Krebs, Waage, Steinbock (0, 3, 6, 9)
    // Fix: Stier, Löwe, Skorpion, Wassermann (1, 4, 7, 10)
    // Beweglich: Zwillinge, Jungfrau, Schütze, Fische (2, 5, 8, 11)
    
    // Geschlecht
    // Männlich: Feuer, Luft
    // Weiblich: Erde, Wasser
    
    for (int i = 0; i < radix.anzahlPlanet; ++i) {
        int8_t zeichen = radix.stzPlanet[i];
        
        // Element
        switch (zeichen % 4) {
            case 0: radix.qualitaten[QU_FEUER]++; break;
            case 1: radix.qualitaten[QU_ERDE]++; break;
            case 2: radix.qualitaten[QU_LUFT]++; break;
            case 3: radix.qualitaten[QU_WASSER]++; break;
        }
        
        // Modus
        switch (zeichen % 3) {
            case 0: radix.qualitaten[QU_CARDINAL]++; break;
            case 1: radix.qualitaten[QU_FIX]++; break;
            case 2: radix.qualitaten[QU_BEWEGLICH]++; break;
        }
        
        // Geschlecht
        if (zeichen % 4 == 0 || zeichen % 4 == 2) {
            radix.qualitaten[QU_MANN]++;  // Feuer, Luft = männlich
        } else {
            radix.qualitaten[QU_FRAU]++;  // Erde, Wasser = weiblich
        }
    }
}

//==============================================================================
// Hilfsfunktionen
//==============================================================================

int8_t ChartCalc::getHouseOfPlanet(double planetPos, const QVector<double>& hausSpitzen) {
    planetPos = Calculations::mod360(planetPos);
    
    for (int i = 0; i < MAX_HAUS; ++i) {
        int nextHouse = (i + 1) % MAX_HAUS;
        double start = hausSpitzen[i];
        double end = hausSpitzen[nextHouse];
        
        // Behandlung des Übergangs über 0°
        if (end < start) {
            if (planetPos >= start || planetPos < end) {
                return static_cast<int8_t>(i);
            }
        } else {
            if (planetPos >= start && planetPos < end) {
                return static_cast<int8_t>(i);
            }
        }
    }
    
    return 0;  // Fallback
}

void ChartCalc::setPlanetType(Radix& radix, int typ) {
    for (int i = 0; i < radix.anzahlPlanet; ++i) {
        // Basis-Typ setzen
        radix.planetTyp[i] = P_TYP_NORM;
        
        // Rückläufigkeit prüfen (bereits in calcPlanets gesetzt)
        if (swissEph().isRetrograde(i, radix.jd)) {
            radix.planetTyp[i] |= P_TYP_RUCK;
        }
        
        // Horoskop-Typ (STRICT LEGACY: nur Radix, Synastrie, Transit)
        switch (typ) {
            case TYP_TRANSIT:
                radix.planetTyp[i] |= P_TYP_TRANSIT;
                break;
            case TYP_SYNASTRIE:
                radix.planetTyp[i] |= P_TYP_SYNASTRIE;
                break;
        }
    }
}

//==============================================================================
// Häusersystem-spezifische Berechnungen
// Diese werden nur als Fallback verwendet, wenn Swiss Ephemeris nicht verfügbar ist
//==============================================================================

void ChartCalc::housesKoch(Radix& radix) {
    // Koch-Häuser werden von Swiss Ephemeris berechnet
    // Diese Funktion ist nur für Kompatibilität vorhanden
    calcHouses(radix);
}

void ChartCalc::housesPlacidus(Radix& radix) {
    calcHouses(radix);
}

void ChartCalc::housesEqual(Radix& radix) {
    // Equal Houses: Alle Häuser 30° vom ASC
    for (int i = 0; i < MAX_HAUS; ++i) {
        radix.haus[i] = Calculations::mod360(radix.asc + i * 30.0);
        radix.hausRad[i] = radix.haus[i] * PI / DEGHALB;
        radix.stzHaus[i] = Calculations::getZeichen(radix.haus[i]);
    }
}

void ChartCalc::housesEqualMid(Radix& radix) {
    // Equal Houses vom MC
    for (int i = 0; i < MAX_HAUS; ++i) {
        radix.haus[i] = Calculations::mod360(radix.mc - 90.0 + i * 30.0);
        radix.hausRad[i] = radix.haus[i] * PI / DEGHALB;
        radix.stzHaus[i] = Calculations::getZeichen(radix.haus[i]);
    }
}

void ChartCalc::housesWhole(Radix& radix) {
    // Whole Sign Houses: Jedes Zeichen ist ein Haus
    int8_t ascZeichen = Calculations::getZeichen(radix.asc);
    for (int i = 0; i < MAX_HAUS; ++i) {
        radix.haus[i] = ((ascZeichen + i) % 12) * 30.0;
        radix.hausRad[i] = radix.haus[i] * PI / DEGHALB;
        radix.stzHaus[i] = (ascZeichen + i) % 12;
    }
}

void ChartCalc::housesTopocentric(Radix& radix) {
    calcHouses(radix);
}

void ChartCalc::housesCampanus(Radix& radix) {
    calcHouses(radix);
}

void ChartCalc::housesMeridian(Radix& radix) {
    calcHouses(radix);
}

void ChartCalc::housesRegiomontanus(Radix& radix) {
    calcHouses(radix);
}

void ChartCalc::housesPorphyry(Radix& radix) {
    // Porphyry: Quadranten gleichmäßig aufteilen
    double asc = radix.asc;
    double mc = radix.mc;
    double ic = Calculations::mod360(mc + 180.0);
    double dsc = Calculations::mod360(asc + 180.0);
    
    // Quadrant 1: ASC bis IC
    double q1 = Calculations::minDist(asc, ic);
    if (q1 < 0) q1 += 360.0;
    
    // Quadrant 2: IC bis DSC
    double q2 = Calculations::minDist(ic, dsc);
    if (q2 < 0) q2 += 360.0;
    
    // Quadrant 3: DSC bis MC
    double q3 = Calculations::minDist(dsc, mc);
    if (q3 < 0) q3 += 360.0;
    
    // Quadrant 4: MC bis ASC
    double q4 = Calculations::minDist(mc, asc);
    if (q4 < 0) q4 += 360.0;
    
    radix.haus[0] = asc;                                    // Haus 1 = ASC
    radix.haus[1] = Calculations::mod360(asc + q1 / 3.0);   // Haus 2
    radix.haus[2] = Calculations::mod360(asc + 2.0 * q1 / 3.0); // Haus 3
    radix.haus[3] = ic;                                     // Haus 4 = IC
    radix.haus[4] = Calculations::mod360(ic + q2 / 3.0);    // Haus 5
    radix.haus[5] = Calculations::mod360(ic + 2.0 * q2 / 3.0); // Haus 6
    radix.haus[6] = dsc;                                    // Haus 7 = DSC
    radix.haus[7] = Calculations::mod360(dsc + q3 / 3.0);   // Haus 8
    radix.haus[8] = Calculations::mod360(dsc + 2.0 * q3 / 3.0); // Haus 9
    radix.haus[9] = mc;                                     // Haus 10 = MC
    radix.haus[10] = Calculations::mod360(mc + q4 / 3.0);   // Haus 11
    radix.haus[11] = Calculations::mod360(mc + 2.0 * q4 / 3.0); // Haus 12
    
    for (int i = 0; i < MAX_HAUS; ++i) {
        radix.hausRad[i] = radix.haus[i] * PI / DEGHALB;
        radix.stzHaus[i] = Calculations::getZeichen(radix.haus[i]);
    }
}

void ChartCalc::housesPorphyryNeo(Radix& radix) {
    housesPorphyry(radix);  // Gleich wie Porphyry
}

void ChartCalc::housesMorinus(Radix& radix) {
    calcHouses(radix);
}

void ChartCalc::housesAlcabitius(Radix& radix) {
    calcHouses(radix);
}

void ChartCalc::housesNull(Radix& radix) {
    // Keine Häuser - alle auf 0
    for (int i = 0; i < MAX_HAUS; ++i) {
        radix.haus[i] = i * 30.0;
        radix.hausRad[i] = radix.haus[i] * PI / DEGHALB;
        radix.stzHaus[i] = i;
    }
}

} // namespace astro
