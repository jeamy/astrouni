/**
 * @file swiss_eph.cpp
 * @brief Implementierung des Swiss Ephemeris Wrappers
 */

#include "swiss_eph.h"
#include <QDir>
#include <QFile>

// Swiss Ephemeris Header
extern "C" {
#include "swephexp.h"
}

namespace astro {

//==============================================================================
// Konstruktor / Destruktor
//==============================================================================

SwissEph::SwissEph() 
    : m_initialized(false) {
}

SwissEph::~SwissEph() {
    swe_close();
}

//==============================================================================
// Initialisierung
//==============================================================================

void SwissEph::setEphePath(const QString& path) {
    m_ephePath = path;
    QByteArray pathBytes = path.toLocal8Bit();
    swe_set_ephe_path(const_cast<char*>(pathBytes.constData()));
    m_initialized = true;
    clearError();
}

QString SwissEph::getEphePath() const {
    return m_ephePath;
}

bool SwissEph::checkEpheFiles() const {
    if (m_ephePath.isEmpty()) {
        return false;
    }
    
    QDir dir(m_ephePath);
    if (!dir.exists()) {
        return false;
    }
    
    // Prüfe auf mindestens eine Ephemeriden-Datei
    QStringList filters;
    filters << "*.se1" << "*.SE1";
    QStringList files = dir.entryList(filters, QDir::Files);
    
    return !files.isEmpty();
}

//==============================================================================
// Planeten-Berechnung
//==============================================================================

int SwissEph::toSwissEphPlanet(int planet) const {
    // Mapping von internen Planet-Indizes zu Swiss Ephemeris
    switch (planet) {
        case P_SONNE:   return SE_SUN;
        case P_MOND:    return SE_MOON;
        case P_MERKUR:  return SE_MERCURY;
        case P_VENUS:   return SE_VENUS;
        case P_MARS:    return SE_MARS;
        case P_JUPITER: return SE_JUPITER;
        case P_SATURN:  return SE_SATURN;
        case P_URANUS:  return SE_URANUS;
        case P_NEPTUN:  return SE_NEPTUNE;
        case P_PLUTO:   return SE_PLUTO;
        case P_NKNOTEN: return SE_TRUE_NODE;
        case P_LILITH:  return SE_MEAN_APOG;  // Mittlere Lilith
        case P_CHIRON:  return SE_CHIRON;
        case P_CERES:   return SE_CERES;
        case P_PALLAS:  return SE_PALLAS;
        case P_JUNO:    return SE_JUNO;
        case P_VESTA:   return SE_VESTA;
        default:        return -1;
    }
}

bool SwissEph::calcPlanet(int planet, double jd, 
                          double& longitude, double& latitude,
                          double& distance, double& speed) {
    int sePlanet = toSwissEphPlanet(planet);
    if (sePlanet < 0) {
        setError(QString("Unbekannter Planet: %1").arg(planet));
        return false;
    }
    
    double xx[6];
    char serr[256] = {0};
    
    int32_t iflag = SEFLG_SPEED | SEFLG_SWIEPH;
    int32_t iret = swe_calc_ut(jd, sePlanet, iflag, xx, serr);
    
    if (iret < 0) {
        setError(QString::fromLatin1(serr));
        return false;
    }
    
    longitude = xx[0];  // Ekliptikale Länge
    latitude = xx[1];   // Ekliptikale Breite
    distance = xx[2];   // Entfernung in AU
    speed = xx[3];      // Geschwindigkeit in Grad/Tag
    
    clearError();
    return true;
}

double SwissEph::calcPlanetLongitude(int planet, double jd) {
    double lon, lat, dist, speed;
    if (calcPlanet(planet, jd, lon, lat, dist, speed)) {
        return lon;
    }
    return -1.0;
}

bool SwissEph::isRetrograde(int planet, double jd) {
    double lon, lat, dist, speed;
    if (calcPlanet(planet, jd, lon, lat, dist, speed)) {
        return speed < 0.0;
    }
    return false;
}

//==============================================================================
// Häuser-Berechnung
//==============================================================================

char SwissEph::houseSysToChar(int hsys) {
    // Mapping von internen Häusersystem-Konstanten zu Swiss Ephemeris Zeichen
    switch (hsys) {
        case TYP_PLACIDUS:  return 'P';
        case TYP_KOCH:      return 'K';
        case TYP_EQUAL:     return 'E';  // Equal (ASC)
        case TYP_EQUALMID:  return 'X';  // Equal (MC)
        case TYP_WHOLE:     return 'W';
        case TYP_TOPOZEN:   return 'T';  // Topocentric (Polich/Page)
        case TYP_CAMPANUS:  return 'C';
        case TYP_MERIDIAN:  return 'X';  // Meridian/Axial
        case TYP_REGIOMON:  return 'R';
        case TYP_PORPHYRY:  return 'O';
        case TYP_PORPHYRYN: return 'O';  // Neo-Porphyry = Porphyry
        case TYP_MORINUS:   return 'M';
        case TYP_ALCABIT:   return 'B';
        case TYP_NULL:      return 'E';  // Fallback zu Equal
        default:            return 'P';  // Default: Placidus
    }
}

bool SwissEph::calcHouses(double jd, double lat, double lon, int hsys,
                          double* cusps, double* ascmc) {
    char hsysChar = houseSysToChar(hsys);
    char serr[256] = {0};
    
    int iret = swe_houses_ex(jd, SEFLG_SWIEPH, lat, lon, hsysChar, cusps, ascmc);
    
    if (iret < 0) {
        setError(QString("Fehler bei Häuserberechnung: %1").arg(serr));
        return false;
    }
    
    clearError();
    return true;
}

//==============================================================================
// Siderische Zeit
//==============================================================================

double SwissEph::calcSiderealTime(double jd, double longitude) {
    double armc = swe_sidtime(jd);  // ARMC in Stunden
    
    // Lokale siderische Zeit = ARMC + Länge/15
    double lst = armc + longitude / 15.0;
    
    // Normalisieren auf 0-24 Stunden
    while (lst >= 24.0) lst -= 24.0;
    while (lst < 0.0) lst += 24.0;
    
    // Konvertieren in Grad (0-360)
    return lst * 15.0;
}

//==============================================================================
// Obliquität
//==============================================================================

double SwissEph::calcObliquity(double jd) {
    double xx[6];
    char serr[256] = {0};
    
    // Berechne Nutation und Obliquität
    int32_t iret = swe_calc_ut(jd, SE_ECL_NUT, 0, xx, serr);
    
    if (iret < 0) {
        // Fallback: Standardwert
        return AXE;
    }
    
    return xx[0];  // Wahre Obliquität
}

//==============================================================================
// Delta T
//==============================================================================

double SwissEph::calcDeltaT(double jd) {
    return swe_deltat(jd);
}

//==============================================================================
// Fehlerbehandlung
//==============================================================================

QString SwissEph::getLastError() const {
    return m_lastError;
}

bool SwissEph::hasError() const {
    return !m_lastError.isEmpty();
}

void SwissEph::clearError() {
    m_lastError.clear();
}

void SwissEph::setError(const QString& error) {
    m_lastError = error;
}

//==============================================================================
// Globale Instanz
//==============================================================================

SwissEph& swissEph() {
    static SwissEph instance;
    return instance;
}

} // namespace astro
