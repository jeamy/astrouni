#pragma once
/**
 * @file chart_calc.h
 * @brief 1:1 Port der Horoskop-Berechnungen aus legacy/auwurzel.c
 * 
 * Funktionen: sCalcChart, sCalcHauser, vCalcPlanets, vKoch, vHausPlacidus, etc.
 */

#include "data_types.h"

namespace astro {

/**
 * @brief Horoskop-Berechnungen
 * 
 * 1:1 Port von legacy/auwurzel.c (Berechnungsteil)
 */
class ChartCalc {
public:
    //==========================================================================
    // Haupt-Berechnungsfunktionen
    //==========================================================================
    
    /**
     * @brief Berechnet ein vollständiges Horoskop
     * @param radix [in/out] Radix-Daten mit Eingabe (Datum, Zeit, Ort) und Ausgabe
     * @param transit Optional: Transit-Radix für Synastrie/Composit
     * @param typ Horoskop-Typ (TYP_RADIX, TYP_TRANSIT, etc.)
     * @return ERR_OK bei Erfolg, Fehlercode sonst
     * 
     * Port von: sCalcChart(RADIX*, RADIX*, short)
     */
    static int calculate(Radix& radix, Radix* transit = nullptr, int typ = TYP_RADIX);
    
    /**
     * @brief Berechnet die Häuserspitzen
     * @param radix [in/out] Radix mit JD, Breite, Länge und Häusersystem
     * @return ERR_OK bei Erfolg
     * 
     * Port von: sCalcHauser(RADIX*)
     */
    static int calcHouses(Radix& radix);
    
    /**
     * @brief Berechnet die Planetenpositionen
     * @param radix [in/out] Radix mit JD
     * 
     * Port von: vCalcPlanets(RADIX*)
     */
    static void calcPlanets(Radix& radix);
    
    /**
     * @brief Berechnet Variablen (MC, ASC, Siderische Zeit, etc.)
     * @param radix [in/out] Radix mit JD und Koordinaten
     * 
     * Port von: vCalcVar(RADIX*)
     */
    static void calcVariables(Radix& radix);
    
    /**
     * @brief Berechnet fehlende Werte (Composit, Synastrie)
     * @param radix Haupt-Radix
     * @param transit Transit/Partner-Radix
     * @param typ Horoskop-Typ
     * 
     * Port von: vCalcMissing(RADIX*, RADIX*, short)
     */
    static void calcMissing(Radix& radix, Radix* transit, int typ);
    
    //==========================================================================
    // Häusersystem-Berechnungen
    //==========================================================================
    
    /**
     * @brief Koch-Häusersystem
     * Port von: vKoch(RADIX*)
     */
    static void housesKoch(Radix& radix);
    
    /**
     * @brief Placidus-Häusersystem
     * Port von: vHausPlacidus(RADIX*)
     */
    static void housesPlacidus(Radix& radix);
    
    /**
     * @brief Equal-Häusersystem (vom ASC)
     * Port von: vEqual(RADIX*)
     */
    static void housesEqual(Radix& radix);
    
    /**
     * @brief Equal-Häusersystem (vom MC)
     * Port von: vEqualMid(RADIX*)
     */
    static void housesEqualMid(Radix& radix);
    
    /**
     * @brief Whole Sign Häusersystem
     * Port von: vWhole(RADIX*)
     */
    static void housesWhole(Radix& radix);
    
    /**
     * @brief Topocentric (Polich/Page) Häusersystem
     * Port von: vTopocentric(RADIX*)
     */
    static void housesTopocentric(Radix& radix);
    
    /**
     * @brief Campanus-Häusersystem
     * Port von: vCampanus(RADIX*)
     */
    static void housesCampanus(Radix& radix);
    
    /**
     * @brief Meridian-Häusersystem
     * Port von: vMeridian(RADIX*)
     */
    static void housesMeridian(Radix& radix);
    
    /**
     * @brief Regiomontanus-Häusersystem
     * Port von: vRegiomontanus(RADIX*)
     */
    static void housesRegiomontanus(Radix& radix);
    
    /**
     * @brief Porphyry-Häusersystem
     * Port von: vPorphyry(RADIX*)
     */
    static void housesPorphyry(Radix& radix);
    
    /**
     * @brief Neo-Porphyry-Häusersystem
     * Port von: vPorphyryNeo(RADIX*)
     */
    static void housesPorphyryNeo(Radix& radix);
    
    /**
     * @brief Morinus-Häusersystem
     * Port von: vMorinus(RADIX*)
     */
    static void housesMorinus(Radix& radix);
    
    /**
     * @brief Alcabitius-Häusersystem
     * Port von: vAlcabitius(RADIX*)
     */
    static void housesAlcabitius(Radix& radix);
    
    /**
     * @brief Null-Häusersystem (keine Häuser)
     * Port von: vNull(RADIX*)
     */
    static void housesNull(Radix& radix);
    
    //==========================================================================
    // Aspekt-Berechnungen
    //==========================================================================
    
    /**
     * @brief Berechnet alle Aspekte zwischen Planeten
     * @param radix Radix-Daten
     * @param orben Orben-Array
     * 
     * Port von: vCalcAsp(RADIX*, short)
     */
    static void calcAspects(Radix& radix, const QVector<float>& orben);
    
    /**
     * @brief Berechnet Winkel zwischen allen Planeten
     * @param radix Haupt-Radix
     * @param transit Optional: Transit-Radix
     * @param typ Berechnungstyp
     * 
     * Port von: vCalcWinkel(RADIX*, RADIX*, short)
     */
    static void calcAngles(Radix& radix, Radix* transit, int typ);
    
    //==========================================================================
    // Qualitäten-Berechnung
    //==========================================================================
    
    /**
     * @brief Berechnet die Qualitäten (Elemente, Modi, Geschlecht)
     * @param radix [in/out] Radix-Daten
     */
    static void calcQualities(Radix& radix);
    
    //==========================================================================
    // Hilfsfunktionen
    //==========================================================================
    
    /**
     * @brief Berechnet in welchem Haus ein Planet steht
     * @param planetPos Position des Planeten in Grad
     * @param hausSpitzen Array mit Häuserspitzen
     * @return Haus-Index (0-11)
     */
    static int8_t getHouseOfPlanet(double planetPos, const QVector<double>& hausSpitzen);
    
    /**
     * @brief Setzt den Planet-Typ (normal, rückläufig, transit, etc.)
     * @param radix Radix-Daten
     * @param typ Horoskop-Typ
     * 
     * Port von: sSetPlanetTyp(RADIX*, short)
     */
    static void setPlanetType(Radix& radix, int typ);
    
private:
    // Hilfsfunktionen für Häuserberechnung
    static double spitzePlacidus(const Radix& radix, double ra, double ob, int haus);
    static double spitzeTopocentric(const Radix& radix, double ra);
    static double spitzeMid(const Radix& radix);
    static double spitzeAsc(const Radix& radix);
    static double spitzeEP(const Radix& radix);
};

} // namespace astro
