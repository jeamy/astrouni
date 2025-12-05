/**
 * @file calculations.cpp
 * @brief 1:1 Port der mathematischen Funktionen aus legacy/aucalc.c
 */

#include "calculations.h"
#include "constants.h"
#include <cmath>

namespace astro {

//==============================================================================
// Julianisches Datum
// Port von: dJulTag(short sTag, short sMonat, short sJahr, double dStunde, short sGreg)
// Quelle: legacy/aucalc.c Zeile 6-27
//==============================================================================

double Calculations::julianDay(int16_t tag, int16_t monat, int16_t jahr, 
                               double stunde, bool gregorianisch) {
    double dJd = 0.0;
    double dU, dU0, dU1, dU2;
    
    dU = static_cast<double>(jahr);
    if (monat < 3) {
        dU -= 1.0;
    }
    dU0 = dU + 4712.0;
    dU1 = static_cast<double>(monat) + 1.0;
    if (dU1 < 4.0) {
        dU1 += 12.0;
    }
    dJd = std::floor(dU0 * 365.25) + std::floor(30.6 * dU1 + 0.000001) +
          static_cast<double>(tag) + stunde / 24.0 - 63.5;
    
    if (gregorianisch) {
        dU2 = std::floor(std::fabs(dU) / 100.0) - std::floor(std::fabs(dU) / 400.0);
        if (dU < 0.0) {
            dU2 = -dU2;
        }
        dJd = dJd - dU2 + 2.0;
        if ((dU < 0.0) && (dU / 100.0 == std::floor(dU / 100.0)) &&
            (dU / 400.0 != std::floor(dU / 400.0))) {
            dJd -= 1.0;
        }
    }
    return dJd;
}

//==============================================================================
// Modulo-Funktionen
// Port von: dMod(double dD), dModRad(double dD)
// Quelle: legacy/aucalc.c Zeile 55-73
//==============================================================================

double Calculations::mod360(double grad) {
    // Schnelle Prüfung für häufige Fälle
    if (grad >= DEGMAX) {
        grad -= DEGMAX;
    } else if (grad < 0.0) {
        grad += DEGMAX;
    }
    
    // Falls immer noch außerhalb des Bereichs
    if (grad >= 0.0 && grad < DEGMAX) {
        return grad;
    }
    return grad - std::floor(grad / DEGMAX) * DEGMAX;
}

double Calculations::modRad(double rad) {
    while (rad >= PI2) {
        rad -= PI2;
    }
    while (rad < 0.0) {
        rad += PI2;
    }
    return rad;
}

//==============================================================================
// Winkel-Funktionen
// Port von: dWinkel(double dX, double dY), dVorz(double dD)
// Quelle: legacy/aucalc.c Zeile 75-95
//==============================================================================

double Calculations::winkel(double x, double y) {
    double a;
    
    if (x != 0.0) {
        if (y != 0.0) {
            a = std::atan(y / x);
        } else {
            a = (x < 0.0) ? PI : 0.0;
        }
    } else {
        a = (y < 0.0) ? -PIHALB : PIHALB;
    }
    
    if (a < 0.0) {
        a += PI;
    }
    if (y < 0.0) {
        a += PI;
    }
    return a;
}

double Calculations::minDist(double winkel1, double winkel2) {
    double diff = winkel2 - winkel1;
    
    // Normalisieren auf -180 bis +180
    while (diff > DEGHALB) {
        diff -= DEGMAX;
    }
    while (diff < -DEGHALB) {
        diff += DEGMAX;
    }
    return diff;
}

double Calculations::vorz(double d) {
    if (d == 0.0) return 0.0;
    return (d < 0.0) ? -1.0 : 1.0;
}

//==============================================================================
// Grad-Konvertierung
// Port von: vGrad2GMS(double dDezGrad, short* sGrad, short* sMinuten, short* sSekunden)
// Quelle: legacy/aucalc.c Zeile 29-53
//==============================================================================

void Calculations::degToGMS(double dezGrad, int16_t& grad, int16_t& minuten, int16_t& sekunden) {
    grad = static_cast<int16_t>(std::floor(dezGrad));
    dezGrad = (dezGrad - static_cast<double>(grad)) * 100.0;
    
    // Grad auf 0-29 normalisieren (innerhalb eines Zeichens)
    while (grad >= 30) {
        grad -= 30;
    }
    
    minuten = static_cast<int16_t>(std::floor(dezGrad));
    dezGrad = (dezGrad - static_cast<double>(minuten)) * 100.0;
    
    // Minuten normalisieren
    while (minuten >= 60) {
        grad++;
        minuten -= 60;
    }
    
    sekunden = static_cast<int16_t>(std::floor(dezGrad + RUNDEN));
    
    // Sekunden normalisieren
    while (sekunden >= 60) {
        minuten++;
        if (minuten >= 60) {
            grad++;
            minuten -= 60;
        }
        sekunden -= 60;
    }
}

QString Calculations::degToString(double grad, bool mitSekunden, bool mitZeichen) {
    // Position normalisieren
    grad = mod360(grad);
    
    int16_t g, m, s;
    degToGMS(grad, g, m, s);
    
    QString result;
    
    if (mitZeichen) {
        int8_t zeichen = getZeichen(grad);
        result = QString("%1°%2'").arg(g, 2, 10, QChar('0')).arg(m, 2, 10, QChar('0'));
        if (mitSekunden) {
            result += QString("%1\"").arg(s, 2, 10, QChar('0'));
        }
        result += " " + getZeichenKuerzel(zeichen);
    } else {
        result = QString("%1°%2'").arg(g, 2, 10, QChar('0')).arg(m, 2, 10, QChar('0'));
        if (mitSekunden) {
            result += QString("%1\"").arg(s, 2, 10, QChar('0'));
        }
    }
    
    return result;
}

QString Calculations::degToZeichenString(double grad, bool mitSekunden) {
    // STRICT LEGACY: Position im Sternzeichen (0-30°)
    grad = mod360(grad);
    
    // Grad innerhalb des Zeichens (0-30)
    double gradImZeichen = std::fmod(grad, 30.0);
    
    int16_t g = static_cast<int16_t>(gradImZeichen);
    double rest = (gradImZeichen - g) * 60.0;
    int16_t m = static_cast<int16_t>(rest);
    int16_t s = static_cast<int16_t>((rest - m) * 60.0);
    
    // Legacy-Format: "14°20'32""
    if (mitSekunden) {
        return QString("%1°%2'%3\"")
            .arg(g, 2, 10, QChar('0'))
            .arg(m, 2, 10, QChar('0'))
            .arg(s, 2, 10, QChar('0'));
    } else {
        return QString("%1°%2'")
            .arg(g, 2, 10, QChar('0'))
            .arg(m, 2, 10, QChar('0'));
    }
}

//==============================================================================
// Dezimal-Konvertierung
// Port von: dDez2Deg(double), dDeg2Dez(double)
//==============================================================================

double Calculations::dezToDeg(double dez) {
    // Format: GG.MMSS -> echte Dezimalgrad
    int grad = static_cast<int>(dez);
    double rest = (dez - grad) * 100.0;
    int minuten = static_cast<int>(rest);
    double sekunden = (rest - minuten) * 100.0;
    
    return static_cast<double>(grad) + 
           static_cast<double>(minuten) / 60.0 + 
           sekunden / 3600.0;
}

double Calculations::degToDez(double deg) {
    // Echte Dezimalgrad -> Format GG.MMSS
    int grad = static_cast<int>(deg);
    double rest = (deg - grad) * 60.0;
    int minuten = static_cast<int>(rest);
    double sekunden = (rest - minuten) * 60.0;
    
    return static_cast<double>(grad) + 
           static_cast<double>(minuten) / 100.0 + 
           sekunden / 10000.0;
}

//==============================================================================
// Koordinaten-Konvertierung
// Port von: vPolToRec, vRecToPol, dRecToSph
//==============================================================================

void Calculations::polToRec(double radius, double winkel, double& x, double& y) {
    x = radius * std::cos(winkel);
    y = radius * std::sin(winkel);
}

void Calculations::recToPol(double x, double y, double& radius, double& winkel) {
    radius = std::sqrt(x * x + y * y);
    winkel = Calculations::winkel(x, y);
}

double Calculations::recToSph(double x, double y, double z) {
    double r = std::sqrt(x * x + y * y + z * z);
    if (r == 0.0) return 0.0;
    return std::asin(z / r);
}

//==============================================================================
// Zeit-Funktionen
//==============================================================================

double Calculations::timeToDecimal(int16_t stunden, int16_t minuten) {
    return static_cast<double>(stunden) + static_cast<double>(minuten) / 60.0;
}

void Calculations::decimalToTime(double dezimal, int16_t& stunden, int16_t& minuten) {
    stunden = static_cast<int16_t>(dezimal);
    minuten = static_cast<int16_t>((dezimal - stunden) * 60.0 + 0.5);
    if (minuten >= 60) {
        minuten -= 60;
        stunden++;
    }
}

double Calculations::deltaT(double jd) {
    // Vereinfachte Delta-T Berechnung
    // Für genauere Werte sollte Swiss Ephemeris verwendet werden
    double t = (jd - 2451545.0) / 36525.0;  // Jahrhunderte seit J2000.0
    
    // Polynomiale Approximation für 1900-2100
    if (t >= -1.0 && t <= 1.0) {
        return 62.92 + 0.32217 * t + 0.005589 * t * t;
    }
    
    // Außerhalb des Bereichs: grobe Schätzung
    return 60.0 + 30.0 * t;
}

//==============================================================================
// Sternzeichen-Funktionen
//==============================================================================

int8_t Calculations::getZeichen(double grad) {
    grad = mod360(grad);
    return static_cast<int8_t>(grad / 30.0);
}

QString Calculations::getZeichenName(int8_t index) {
    static const char* namen[] = {
        WIDDER_TXT, STIER_TXT, ZWILLING_TXT, KREBS_TXT,
        LOWE_TXT, JUNGFRAU_TXT, WAAGE_TXT, SKORPION_TXT,
        SCHUTZE_TXT, STEINBOCK_TXT, WASSERMANN_TXT, FISCHE_TXT
    };
    
    if (index >= 0 && index < 12) {
        return QString::fromUtf8(namen[index]);
    }
    return QString();
}

QString Calculations::getZeichenKuerzel(int8_t index) {
    static const char* kuerzel[] = {
        "Wid", "Sti", "Zwi", "Kre", "Löw", "Jun",
        "Waa", "Sko", "Sch", "Ste", "Was", "Fis"
    };
    
    if (index >= 0 && index < 12) {
        return QString::fromUtf8(kuerzel[index]);
    }
    return QString();
}

//==============================================================================
// Aspekt-Funktionen
//==============================================================================

bool Calculations::checkAspekt(double pos1, double pos2, int aspekt, 
                               double orb, double& exakterWinkel) {
    double diff = std::fabs(minDist(pos1, pos2));
    exakterWinkel = diff;
    
    double abweichung = std::fabs(diff - static_cast<double>(aspekt));
    return abweichung <= orb;
}

QString Calculations::getAspektName(int aspekt) {
    switch (aspekt) {
        case KONJUNKTION: return QString::fromUtf8("Konjunktion");
        case HALBSEX:     return QString::fromUtf8("Halbsextil");
        case SEXTIL:      return QString::fromUtf8("Sextil");
        case QUADRATUR:   return QString::fromUtf8("Quadratur");
        case TRIGON:      return QString::fromUtf8("Trigon");
        case QUINCUNX:    return QString::fromUtf8("Quincunx");
        case OPOSITION:   return QString::fromUtf8("Opposition");
        default:          return QString();
    }
}

} // namespace astro
