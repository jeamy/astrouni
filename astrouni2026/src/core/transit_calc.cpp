/**
 * @file transit_calc.cpp
 * @brief 1:1 Port der Transit-Berechnungen aus legacy/autransi.c
 */

#include "transit_calc.h"
#include "chart_calc.h"
#include "calculations.h"
#include "swiss_eph.h"

namespace astro {

//==============================================================================
// Transit-Berechnung
//==============================================================================

int TransitCalc::calcTransit(const Radix& radix, Radix& transit,
                             const QDate& datum, const QTime& zeit) {
    // Transit-Radix initialisieren
    transit.clear();
    transit.allocate(radix.anzahlPlanet);
    
    // Koordinaten vom Radix übernehmen
    transit.rFix.laenge = radix.rFix.laenge;
    transit.rFix.breite = radix.rFix.breite;
    transit.rFix.zone = radix.rFix.zone;
    transit.rFix.sommerzeit = 0.0;  // Transit ohne Sommerzeit
    
    // Datum und Zeit setzen
    transit.rFix.tag = datum.day();
    transit.rFix.monat = datum.month();
    transit.rFix.jahr = datum.year();
    transit.rFix.zeit = Calculations::timeToDecimal(zeit.hour(), zeit.minute());
    
    // Häusersystem übernehmen
    transit.hausSys = radix.hausSys;
    transit.horoTyp = TYP_TRANSIT;
    
    // Berechnen
    return ChartCalc::calculate(transit, nullptr, TYP_TRANSIT);
}

int TransitCalc::calcMultiTransit(const Radix& radix, Radix& transit, int inkrement) {
    // TODO: Implementierung der Multi-Transit-Suche
    // Dies ist eine komplexe Funktion, die über einen Zeitraum iteriert
    // und alle Aspekte zwischen Transit- und Radix-Planeten findet
    
    return 0;
}

//==============================================================================
// Rückläufigkeit
//==============================================================================

int TransitCalc::calcRetrograde(int planet, const QDate& startDatum, 
                                const QDate& endDatum,
                                QVector<QPair<QDate, QDate>>& perioden) {
    perioden.clear();
    
    // Sonne und Mond sind nie rückläufig
    if (planet == P_SONNE || planet == P_MOND) {
        return 0;
    }
    
    // Start-JD berechnen
    double startJD = Calculations::julianDay(
        startDatum.day(), startDatum.month(), startDatum.year(), 12.0, true);
    double endJD = Calculations::julianDay(
        endDatum.day(), endDatum.month(), endDatum.year(), 12.0, true);
    
    bool wasRetro = swissEph().isRetrograde(planet, startJD);
    QDate retroStart;
    
    if (wasRetro) {
        retroStart = startDatum;
    }
    
    // Tagesweise durchgehen
    for (double jd = startJD; jd <= endJD; jd += 1.0) {
        bool isRetro = swissEph().isRetrograde(planet, jd);
        
        if (isRetro && !wasRetro) {
            // Beginn einer Rückläufigkeit
            // JD in Datum umrechnen
            int y, m, d;
            double h;
            // Vereinfachte Umrechnung
            double z = jd + 0.5;
            int a = static_cast<int>(z);
            int b = a + 1524;
            int c = static_cast<int>((b - 122.1) / 365.25);
            int dd = static_cast<int>(365.25 * c);
            int e = static_cast<int>((b - dd) / 30.6001);
            d = b - dd - static_cast<int>(30.6001 * e);
            m = (e < 14) ? e - 1 : e - 13;
            y = (m > 2) ? c - 4716 : c - 4715;
            
            retroStart = QDate(y, m, d);
        } else if (!isRetro && wasRetro) {
            // Ende einer Rückläufigkeit
            int y, m, d;
            double z = jd + 0.5;
            int a = static_cast<int>(z);
            int b = a + 1524;
            int c = static_cast<int>((b - 122.1) / 365.25);
            int dd = static_cast<int>(365.25 * c);
            int e = static_cast<int>((b - dd) / 30.6001);
            d = b - dd - static_cast<int>(30.6001 * e);
            m = (e < 14) ? e - 1 : e - 13;
            y = (m > 2) ? c - 4716 : c - 4715;
            
            QDate retroEnd(y, m, d);
            perioden.append(qMakePair(retroStart, retroEnd));
        }
        
        wasRetro = isRetro;
    }
    
    // Falls am Ende noch rückläufig
    if (wasRetro) {
        perioden.append(qMakePair(retroStart, endDatum));
    }
    
    return perioden.size();
}

//==============================================================================
// Datum-Inkrement
//==============================================================================

int TransitCalc::incDate(QDate& datum, QTime& zeit, int inkrement, int anzahl) {
    switch (inkrement) {
        case INC_MINUTEN:
            zeit = zeit.addSecs(anzahl * 60);
            if (zeit.hour() < 0) {
                datum = datum.addDays(-1);
            }
            break;
            
        case INC_STUNDEN:
            zeit = zeit.addSecs(anzahl * 3600);
            break;
            
        case INC_TAGE:
            datum = datum.addDays(anzahl);
            break;
            
        case INC_WOCHEN:
            datum = datum.addDays(anzahl * 7);
            break;
            
        case INC_MONATE:
            datum = datum.addMonths(anzahl);
            break;
            
        case INC_JAHRE:
            datum = datum.addYears(anzahl);
            break;
            
        default:
            return ERR_INC;
    }
    
    return ERR_OK;
}

//==============================================================================
// Aspekt-Suche
//==============================================================================

double TransitCalc::findExactAspect(double radixPlanet, int transitPlanet,
                                    int aspekt, double startJD, int richtung) {
    const double PRECISION = 0.0001;  // ~8 Sekunden
    const double MAX_DAYS = 365.0;    // Maximal 1 Jahr suchen
    
    double jd = startJD;
    double step = richtung * 1.0;  // 1 Tag
    
    // Grobe Suche
    double lastDiff = 999.0;
    for (int i = 0; i < MAX_DAYS; ++i) {
        double transitPos = swissEph().calcPlanetLongitude(transitPlanet, jd);
        if (transitPos < 0) return -1;
        
        double diff = std::fabs(Calculations::minDist(transitPos, radixPlanet));
        double aspDiff = std::fabs(diff - aspekt);
        
        if (aspDiff < 1.0) {
            // Feine Suche
            step = richtung * 0.01;  // ~15 Minuten
            
            while (std::fabs(step) > PRECISION) {
                double newJD = jd + step;
                double newPos = swissEph().calcPlanetLongitude(transitPlanet, newJD);
                double newDiff = std::fabs(Calculations::minDist(newPos, radixPlanet));
                double newAspDiff = std::fabs(newDiff - aspekt);
                
                if (newAspDiff < aspDiff) {
                    jd = newJD;
                    aspDiff = newAspDiff;
                } else {
                    step /= -2.0;  // Richtung wechseln und halbieren
                }
                
                if (aspDiff < PRECISION) {
                    return jd;
                }
            }
            return jd;
        }
        
        lastDiff = aspDiff;
        jd += step;
    }
    
    return -1;  // Nicht gefunden
}

bool TransitCalc::isAspectApplying(double pos1, double pos2, double speed, int aspekt) {
    double diff = Calculations::minDist(pos1, pos2);
    double aspDiff = diff - aspekt;
    
    // Wenn der Aspekt noch nicht exakt ist und der Planet sich darauf zubewegt
    if (aspDiff > 0 && speed > 0) return true;
    if (aspDiff < 0 && speed < 0) return true;
    
    return false;
}

int TransitCalc::findAspectsInRange(const Radix& radix,
                                    const QDate& startDatum,
                                    const QDate& endDatum,
                                    const QVector<float>& orben,
                                    QVector<TransitAspekt>& aspekte) {
    aspekte.clear();
    
    static const int aspektWinkel[] = {
        KONJUNKTION, HALBSEX, SEXTIL, QUADRATUR, TRIGON, QUINCUNX, OPOSITION
    };
    
    double startJD = Calculations::julianDay(
        startDatum.day(), startDatum.month(), startDatum.year(), 0.0, true);
    double endJD = Calculations::julianDay(
        endDatum.day(), endDatum.month(), endDatum.year(), 24.0, true);
    
    // Für jeden Transit-Planeten
    for (int tp = 0; tp < radix.anzahlPlanet; ++tp) {
        // Für jeden Radix-Planeten
        for (int rp = 0; rp < radix.anzahlPlanet; ++rp) {
            double radixPos = radix.planet[rp];
            
            // Für jeden Aspekt
            for (int a = 0; a < ASPEKTE; ++a) {
                int asp = aspektWinkel[a];
                float orb = (orben.size() > a * MAX_PLANET + tp) ?
                            orben[a * MAX_PLANET + tp] : 8.0f;
                
                // Suche exakten Aspekt
                double exactJD = findExactAspect(radixPos, tp, asp, startJD, 1);
                
                while (exactJD > 0 && exactJD <= endJD) {
                    TransitAspekt ta;
                    
                    // JD in DateTime umrechnen (vereinfacht)
                    double z = exactJD + 0.5;
                    int aa = static_cast<int>(z);
                    int b = aa + 1524;
                    int c = static_cast<int>((b - 122.1) / 365.25);
                    int dd = static_cast<int>(365.25 * c);
                    int e = static_cast<int>((b - dd) / 30.6001);
                    int day = b - dd - static_cast<int>(30.6001 * e);
                    int month = (e < 14) ? e - 1 : e - 13;
                    int year = (month > 2) ? c - 4716 : c - 4715;
                    double frac = exactJD - static_cast<int>(exactJD) + 0.5;
                    if (frac >= 1.0) frac -= 1.0;
                    int hour = static_cast<int>(frac * 24.0);
                    int minute = static_cast<int>((frac * 24.0 - hour) * 60.0);
                    
                    ta.zeitpunkt = QDateTime(QDate(year, month, day), QTime(hour, minute));
                    ta.transitPlanet = tp;
                    ta.radixPlanet = rp;
                    ta.aspekt = asp;
                    ta.orb = 0.0;  // Exakt
                    ta.retrograde = swissEph().isRetrograde(tp, exactJD);
                    
                    // Applying/Separating
                    double lon, lat, dist, speed;
                    swissEph().calcPlanet(tp, exactJD - 0.01, lon, lat, dist, speed);
                    ta.applying = isAspectApplying(lon, radixPos, speed, asp);
                    
                    aspekte.append(ta);
                    
                    // Nächsten Aspekt suchen
                    exactJD = findExactAspect(radixPos, tp, asp, exactJD + 1.0, 1);
                }
            }
        }
    }
    
    // Nach Datum sortieren
    std::sort(aspekte.begin(), aspekte.end(), 
              [](const TransitAspekt& a, const TransitAspekt& b) {
                  return a.zeitpunkt < b.zeitpunkt;
              });
    
    return aspekte.size();
}

} // namespace astro
