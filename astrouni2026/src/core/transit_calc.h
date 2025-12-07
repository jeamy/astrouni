#pragma once
/**
 * @file transit_calc.h
 * @brief 1:1 Port der Transit-Berechnungen aus legacy/autransi.c
 */

#include <QDateTime>
#include <functional>
#include "data_types.h"
#include "chart_calc.h"

namespace astro {

// Forward declaration
struct TransitAspekt;

/**
 * @brief Transit-Berechnungen
 * 
 * 1:1 Port von legacy/autransi.c
 */
class TransitCalc {
public:
    //==========================================================================
    // Transit-Berechnung
    //==========================================================================
    
    /**
     * @brief Berechnet Transit-Positionen für ein Datum
     * @param radix Basis-Radix
     * @param transit [out] Transit-Radix
     * @param datum Transit-Datum
     * @param zeit Transit-Zeit
     * @return ERR_OK bei Erfolg
     */
    static int calcTransit(const Radix& radix, Radix& transit,
                          const QDate& datum, const QTime& zeit);
    
    /**
     * @brief Berechnet Multi-Transit (Suche nach Aspekten)
     * @param radix Basis-Radix
     * @param transit Transit-Radix
     * @param inkrement Inkrement-Typ (INC_TAGE, INC_MONATE, etc.)
     * @return Anzahl gefundener Aspekte
     * 
     * Port von: sCalcMultiTransit(RADIX*, short)
     */
    static int calcMultiTransit(const Radix& radix,
                                const QDate& startDatum,
                                const QTime& startZeit,
                                const QDate& endDatum,
                                const QTime& endZeit,
                                int inkrement,
                                QVector<Radix>* transits = nullptr,
                                QVector<TransitAspekt>* aspekte = nullptr,
                                const QVector<float>& orbenPlanet = {},
                                const QVector<float>& orbenHaus = {},
                                bool* abortFlag = nullptr,
                                const std::function<void(int,const QDate&,const QTime&)>& progressCb = nullptr);
    
    //==========================================================================
    // Rückläufigkeit
    //==========================================================================
    
    /**
     * @brief Berechnet Rückläufigkeits-Perioden
     * @param planet Planet-Index
     * @param startDatum Start-Datum
     * @param endDatum End-Datum
     * @param perioden [out] Liste der Rückläufigkeits-Perioden
     * @return Anzahl der Perioden
     * 
     * Port von: sRucklauf(HWND)
     */
    static int calcRetrograde(int planet, const QDate& startDatum, 
                             const QDate& endDatum,
                             QVector<QPair<QDate, QDate>>& perioden);
    
    //==========================================================================
    // Datum-Inkrement
    //==========================================================================
    
    /**
     * @brief Inkrementiert ein Datum
     * @param datum [in/out] Datum
     * @param zeit [in/out] Zeit
     * @param inkrement Inkrement-Typ
     * @param anzahl Anzahl der Inkremente
     * @return ERR_OK bei Erfolg
     * 
     * Port von: sIncDate(char*, char*, short, short, short)
     */
    static int incDate(QDate& datum, QTime& zeit, int inkrement, int anzahl);
    
    //==========================================================================
    // Aspekt-Suche
    //==========================================================================
    
    /**
     * @brief Sucht nach dem nächsten exakten Aspekt
     * @param radixPlanet Radix-Planet-Position
     * @param transitPlanet Transit-Planet-Index
     * @param aspekt Gesuchter Aspekt
     * @param startJD Start-JD
     * @param richtung 1 = vorwärts, -1 = rückwärts
     * @return JD des exakten Aspekts, oder -1 wenn nicht gefunden
     */
    static double findExactAspect(double radixPlanet, int transitPlanet,
                                  int aspekt, double startJD, int richtung);
    
    /**
     * @brief Sucht alle Aspekte in einem Zeitraum
     * @param radix Basis-Radix
     * @param startDatum Start-Datum
     * @param endDatum End-Datum
     * @param orben Orben für Aspekte
     * @param aspekte [out] Gefundene Aspekte
     * @return Anzahl gefundener Aspekte
     */
    static int findAspectsInRange(const Radix& radix,
                                  const QDate& startDatum,
                                  const QDate& endDatum,
                                  const QVector<float>& orben,
                                  QVector<TransitAspekt>& aspekte);
    
private:
    // Hilfsfunktion für Aspekt-Suche
    static bool isAspectApplying(double pos1, double pos2, double speed, int aspekt);
};

/**
 * @brief Struktur für gefundene Transit-Aspekte
 */
struct TransitAspekt {
    QDateTime zeitpunkt;        // Zeitpunkt des Aspekts
    int transitPlanet;          // Transit-Planet
    int radixPlanet;            // Radix-Planet oder Haus
    int aspekt;                 // Aspekt-Typ
    double orb;                 // Orbis
    bool applying;              // true = zunehmend, false = abnehmend
    bool retrograde;            // Transit-Planet rückläufig?
    bool isHaus = false;        // true = Aspekt zu Haus (radixPlanet = Hausindex)
    int transitIndex = 0;       // Index im Multi-Transit-Lauf
    int startIndex = 0;         // Beginn-Index für Sequenz
    int endIndex = 0;           // Ende-Index für Sequenz
};

} // namespace astro
