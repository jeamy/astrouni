#pragma once
/**
 * @file swiss_eph.h
 * @brief Wrapper für Swiss Ephemeris
 * 
 * Ersetzt die Legacy-Ephemeriden-Berechnung aus aupla.c und aupla1.c
 * durch Swiss Ephemeris.
 */

#include <QString>
#include <QVector>
#include "constants.h"

namespace astro {

/**
 * @brief Swiss Ephemeris Wrapper
 * 
 * Kapselt alle Aufrufe an die Swiss Ephemeris Bibliothek.
 */
class SwissEph {
public:
    SwissEph();
    ~SwissEph();
    
    // Nicht kopierbar
    SwissEph(const SwissEph&) = delete;
    SwissEph& operator=(const SwissEph&) = delete;
    
    //==========================================================================
    // Initialisierung
    //==========================================================================
    
    /**
     * @brief Setzt den Pfad zu den Ephemeriden-Dateien
     * @param path Pfad zum Verzeichnis mit den .se1 Dateien
     */
    void setEphePath(const QString& path);
    
    /**
     * @brief Gibt den aktuellen Ephemeriden-Pfad zurück
     */
    QString getEphePath() const;
    
    /**
     * @brief Prüft ob die Ephemeriden-Dateien vorhanden sind
     * @return true wenn alle benötigten Dateien gefunden wurden
     */
    bool checkEpheFiles() const;
    
    //==========================================================================
    // Planeten-Berechnung
    //==========================================================================
    
    /**
     * @brief Berechnet die Position eines Planeten
     * @param planet Planet-Index (P_SONNE bis P_VESTA)
     * @param jd Julianisches Datum
     * @param longitude [out] Ekliptikale Länge in Grad
     * @param latitude [out] Ekliptikale Breite in Grad
     * @param distance [out] Entfernung in AU
     * @param speed [out] Geschwindigkeit in Grad/Tag
     * @return true bei Erfolg
     */
    bool calcPlanet(int planet, double jd, 
                    double& longitude, double& latitude,
                    double& distance, double& speed);
    
    /**
     * @brief Berechnet nur die Länge eines Planeten (schneller)
     * @param planet Planet-Index
     * @param jd Julianisches Datum
     * @return Ekliptikale Länge in Grad, oder -1 bei Fehler
     */
    double calcPlanetLongitude(int planet, double jd);
    
    /**
     * @brief Prüft ob ein Planet rückläufig ist
     * @param planet Planet-Index
     * @param jd Julianisches Datum
     * @return true wenn rückläufig
     */
    bool isRetrograde(int planet, double jd);
    
    //==========================================================================
    // Häuser-Berechnung
    //==========================================================================
    
    /**
     * @brief Berechnet die Häuserspitzen
     * @param jd Julianisches Datum
     * @param lat Geographische Breite
     * @param lon Geographische Länge
     * @param hsys Häusersystem (TYP_PLACIDUS, TYP_KOCH, etc.)
     * @param cusps [out] Array mit 13 Häuserspitzen (Index 1-12)
     * @param ascmc [out] Array mit ASC, MC, ARMC, Vertex, etc.
     * @return true bei Erfolg
     */
    bool calcHouses(double jd, double lat, double lon, int hsys,
                    double* cusps, double* ascmc);
    
    /**
     * @brief Konvertiert internes Häusersystem zu Swiss Ephemeris Zeichen
     * @param hsys Internes Häusersystem (TYP_PLACIDUS, etc.)
     * @return Swiss Ephemeris Häusersystem-Zeichen ('P', 'K', etc.)
     */
    static char houseSysToChar(int hsys);
    
    //==========================================================================
    // Siderische Zeit
    //==========================================================================
    
    /**
     * @brief Berechnet die siderische Zeit
     * @param jd Julianisches Datum (UT)
     * @param longitude Geographische Länge
     * @return Lokale siderische Zeit in Grad
     */
    double calcSiderealTime(double jd, double longitude);
    
    //==========================================================================
    // Obliquität
    //==========================================================================
    
    /**
     * @brief Berechnet die Schiefe der Ekliptik
     * @param jd Julianisches Datum
     * @return Obliquität in Grad
     */
    double calcObliquity(double jd);
    
    //==========================================================================
    // Delta T
    //==========================================================================
    
    /**
     * @brief Berechnet Delta T (TT - UT)
     * @param jd Julianisches Datum (UT)
     * @return Delta T in Tagen
     */
    double calcDeltaT(double jd);
    
    //==========================================================================
    // Fehlerbehandlung
    //==========================================================================
    
    /**
     * @brief Gibt die letzte Fehlermeldung zurück
     */
    QString getLastError() const;
    
    /**
     * @brief Prüft ob ein Fehler aufgetreten ist
     */
    bool hasError() const;
    
    /**
     * @brief Löscht den Fehlerstatus
     */
    void clearError();
    
private:
    QString m_ephePath;
    QString m_lastError;
    bool m_initialized;
    
    // Konvertiert internen Planet-Index zu Swiss Ephemeris Planet-ID
    int toSwissEphPlanet(int planet) const;
    
    // Setzt Fehlermeldung
    void setError(const QString& error);
};

/**
 * @brief Globale Swiss Ephemeris Instanz
 * 
 * Singleton für einfachen Zugriff auf Swiss Ephemeris.
 */
SwissEph& swissEph();

} // namespace astro
