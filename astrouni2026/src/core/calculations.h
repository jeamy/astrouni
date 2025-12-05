#pragma once
/**
 * @file calculations.h
 * @brief 1:1 Port der mathematischen Funktionen aus legacy/aucalc.c
 * 
 * Funktionen: dJulTag, dMod, vGrad2GMS, dWinkel, vPolToRec, vRecToPol, etc.
 */

#include <cstdint>
#include <QString>

namespace astro {

/**
 * @brief Mathematische Berechnungsfunktionen für Astrologie
 * 
 * 1:1 Port von legacy/aucalc.c
 */
class Calculations {
public:
    //==========================================================================
    // Julianisches Datum (Port von dJulTag)
    //==========================================================================
    
    /**
     * @brief Berechnet das Julianische Datum
     * @param tag Tag (1-31)
     * @param monat Monat (1-12)
     * @param jahr Jahr
     * @param stunde Stunde als Dezimalzahl (z.B. 14.5 = 14:30)
     * @param gregorianisch true für gregorianischen Kalender
     * @return Julianisches Datum
     * 
     * Port von: dJulTag(short sTag, short sMonat, short sJahr, double dStunde, short sGreg)
     */
    static double julianDay(int16_t tag, int16_t monat, int16_t jahr, 
                           double stunde, bool gregorianisch = true);
    
    //==========================================================================
    // Modulo-Funktionen (Port von dMod, dModRad)
    //==========================================================================
    
    /**
     * @brief Normalisiert einen Winkel auf 0-360 Grad
     * @param grad Winkel in Grad
     * @return Normalisierter Winkel (0 <= result < 360)
     * 
     * Port von: dMod(double dD)
     */
    static double mod360(double grad);
    
    /**
     * @brief Normalisiert einen Winkel auf 0-2π Radiant
     * @param rad Winkel in Radiant
     * @return Normalisierter Winkel (0 <= result < 2π)
     * 
     * Port von: dModRad(double dD)
     */
    static double modRad(double rad);
    
    //==========================================================================
    // Winkel-Funktionen (Port von dWinkel, dMinDist)
    //==========================================================================
    
    /**
     * @brief Berechnet den Winkel aus kartesischen Koordinaten
     * @param x X-Koordinate
     * @param y Y-Koordinate
     * @return Winkel in Radiant (0 bis 2π)
     * 
     * Port von: dWinkel(double dX, double dY)
     */
    static double winkel(double x, double y);
    
    /**
     * @brief Berechnet die minimale Distanz zwischen zwei Winkeln
     * @param winkel1 Erster Winkel in Grad
     * @param winkel2 Zweiter Winkel in Grad
     * @return Minimale Distanz (-180 bis +180)
     * 
     * Port von: dMinDist(double, double)
     */
    static double minDist(double winkel1, double winkel2);
    
    /**
     * @brief Gibt das Vorzeichen einer Zahl zurück
     * @param d Zahl
     * @return -1, 0 oder 1
     * 
     * Port von: dVorz(double dD)
     */
    static double vorz(double d);
    
    //==========================================================================
    // Grad-Konvertierung (Port von vGrad2GMS, vGrad2Str)
    //==========================================================================
    
    /**
     * @brief Konvertiert Dezimalgrad in Grad/Minuten/Sekunden
     * @param dezGrad Winkel in Dezimalgrad
     * @param grad [out] Grad (0-29 innerhalb eines Zeichens)
     * @param minuten [out] Minuten (0-59)
     * @param sekunden [out] Sekunden (0-59)
     * 
     * Port von: vGrad2GMS(double dDezGrad, short* sGrad, short* sMinuten, short* sSekunden)
     */
    static void degToGMS(double dezGrad, int16_t& grad, int16_t& minuten, int16_t& sekunden);
    
    /**
     * @brief Konvertiert Dezimalgrad in einen formatierten String
     * @param grad Winkel in Dezimalgrad
     * @param mitSekunden true für "GG°MM'SS"", false für "GG°MM'"
     * @param mitZeichen true für Sternzeichen-Kürzel
     * @return Formatierter String
     * 
     * Port von: vGrad2Str(float, char*, short, short)
     */
    static QString degToString(double grad, bool mitSekunden = true, bool mitZeichen = false);
    
    //==========================================================================
    // Dezimal-Konvertierung (Port von dDez2Deg, dDeg2Dez)
    //==========================================================================
    
    /**
     * @brief Konvertiert Dezimalformat (z.B. 12.30 = 12°30') in echte Dezimalgrad
     * @param dez Wert im Format GG.MMSS
     * @return Dezimalgrad
     * 
     * Port von: dDez2Deg(double)
     */
    static double dezToDeg(double dez);
    
    /**
     * @brief Konvertiert Dezimalgrad in Dezimalformat (z.B. 12.5° = 12.30)
     * @param deg Dezimalgrad
     * @return Wert im Format GG.MMSS
     * 
     * Port von: dDeg2Dez(double)
     */
    static double degToDez(double deg);
    
    //==========================================================================
    // Koordinaten-Konvertierung (Port von vPolToRec, vRecToPol)
    //==========================================================================
    
    /**
     * @brief Konvertiert Polarkoordinaten in kartesische Koordinaten
     * @param radius Radius
     * @param winkel Winkel in Radiant
     * @param x [out] X-Koordinate
     * @param y [out] Y-Koordinate
     * 
     * Port von: vPolToRec(double, double, double*, double*)
     */
    static void polToRec(double radius, double winkel, double& x, double& y);
    
    /**
     * @brief Konvertiert kartesische Koordinaten in Polarkoordinaten
     * @param x X-Koordinate
     * @param y Y-Koordinate
     * @param radius [out] Radius
     * @param winkel [out] Winkel in Radiant
     * 
     * Port von: vRecToPol(double, double, double*, double*)
     */
    static void recToPol(double x, double y, double& radius, double& winkel);
    
    /**
     * @brief Konvertiert rechteckige in sphärische Koordinaten
     * @param x X-Koordinate
     * @param y Y-Koordinate
     * @param z Z-Koordinate
     * @return Winkel
     * 
     * Port von: dRecToSph(double, double, double)
     */
    static double recToSph(double x, double y, double z);
    
    //==========================================================================
    // Zeit-Funktionen
    //==========================================================================
    
    /**
     * @brief Konvertiert Stunden:Minuten in Dezimalstunden
     * @param stunden Stunden
     * @param minuten Minuten
     * @return Dezimalstunden (z.B. 14:30 = 14.5)
     */
    static double timeToDecimal(int16_t stunden, int16_t minuten);
    
    /**
     * @brief Konvertiert Dezimalstunden in Stunden:Minuten
     * @param dezimal Dezimalstunden
     * @param stunden [out] Stunden
     * @param minuten [out] Minuten
     */
    static void decimalToTime(double dezimal, int16_t& stunden, int16_t& minuten);
    
    /**
     * @brief Berechnet Delta T (Differenz zwischen TT und UT)
     * @param jd Julianisches Datum
     * @return Delta T in Sekunden
     * 
     * Port von: dDeltat(double)
     */
    static double deltaT(double jd);
    
    //==========================================================================
    // Sternzeichen-Funktionen
    //==========================================================================
    
    /**
     * @brief Ermittelt das Sternzeichen für eine Position
     * @param grad Position in Grad (0-360)
     * @return Sternzeichen-Index (0-11)
     */
    static int8_t getZeichen(double grad);
    
    /**
     * @brief Gibt den Namen eines Sternzeichens zurück
     * @param index Sternzeichen-Index (0-11)
     * @return Name des Sternzeichens
     */
    static QString getZeichenName(int8_t index);
    
    /**
     * @brief Gibt das Kürzel eines Sternzeichens zurück
     * @param index Sternzeichen-Index (0-11)
     * @return 3-Buchstaben-Kürzel (z.B. "Wid", "Sti")
     */
    static QString getZeichenKuerzel(int8_t index);
    
    //==========================================================================
    // Aspekt-Funktionen
    //==========================================================================
    
    /**
     * @brief Prüft ob ein Aspekt zwischen zwei Positionen besteht
     * @param pos1 Erste Position in Grad
     * @param pos2 Zweite Position in Grad
     * @param aspekt Aspekt-Winkel (0, 30, 60, 90, 120, 150, 180)
     * @param orb Erlaubter Orbis
     * @param exakterWinkel [out] Tatsächlicher Winkel
     * @return true wenn Aspekt innerhalb des Orbis
     */
    static bool checkAspekt(double pos1, double pos2, int aspekt, 
                           double orb, double& exakterWinkel);
    
    /**
     * @brief Gibt den Namen eines Aspekts zurück
     * @param aspekt Aspekt-Winkel
     * @return Name des Aspekts
     */
    static QString getAspektName(int aspekt);
};

} // namespace astro
