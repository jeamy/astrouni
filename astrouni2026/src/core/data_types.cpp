/**
 * @file data_types.cpp
 * @brief Implementierung der Datenstrukturen
 */

#include "data_types.h"
#include <cstring>

namespace astro {

//==============================================================================
// Farb-Arrays - Initialisierung
//==============================================================================

// Standard-Farben für Bildschirm (aus astrouni.h Zeile 522-551)
QColor sColorDefault[COLORS] = {
    QColor(192, 192, 192),  // COL_HINTER - WHITE (Grau)
    QColor(255, 0, 0),      // COL_FEUER - LRED
    QColor(128, 64, 0),     // COL_ERDE - BROWN
    QColor(255, 255, 0),    // COL_LUFT - YELLOW
    QColor(0, 0, 128),      // COL_WASSER - BLUE
    QColor(0, 0, 0),        // COL_TXT - BLACK
    QColor(128, 128, 128),  // COL_RADIX - GRAY
    QColor(255, 255, 255),  // COL_ASZ - LWHITE
    QColor(128, 0, 128),    // COL_PLANET_TIC - MAGENTA
    QColor(255, 0, 255),    // COL_PLANET_TICT - LMAGENTA
    QColor(0, 0, 255),      // COL_PLAN - LBLUE
    QColor(0, 128, 128),    // COL_PLAN_T - CYAN
    QColor(128, 128, 128),  // COL_HAUSER - GRAY
    QColor(128, 0, 0),      // CKONJUNKTION - RED
    QColor(0, 128, 0),      // CSEXTIL - GREEN
    QColor(128, 0, 0),      // CQUADRATUR - RED
    QColor(0, 128, 0),      // CTRIGON - GREEN
    QColor(128, 0, 128),    // COPOSITION - MAGENTA
    QColor(0, 255, 0),      // CHALBSEX - LGREEN
    QColor(255, 0, 0),      // CQUINCUNX - LRED
    QColor(0, 0, 128),      // COL_LBTXT_NORM - BLUE
    QColor(0, 0, 128),      // COL_LBTXT_SEL - BLUE
    QColor(0, 0, 128),      // COL_LBTXT_FOCUS - BLUE
    QColor(255, 255, 255),  // COL_LBTXT_BK - LWHITE
    QColor(192, 192, 192),  // COL_LBREC_NORM - WHITE
    QColor(128, 128, 128),  // COL_LBREC_SEL - GRAY
    QColor(128, 128, 128),  // COL_LBREC_FOCUS - GRAY
    QColor(0, 0, 128)       // COL_LBREC_BK - BLUE
};

// Standard-Farben für Druck (Schwarz/Weiß)
QColor pColorDefault[COLORS] = {
    QColor(255, 255, 255),  // COL_HINTER - LWHITE
    QColor(0, 0, 0),        // COL_FEUER - BLACK
    QColor(0, 0, 0),        // COL_ERDE - BLACK
    QColor(0, 0, 0),        // COL_LUFT - BLACK
    QColor(0, 0, 0),        // COL_WASSER - BLACK
    QColor(0, 0, 0),        // COL_TXT - BLACK
    QColor(0, 0, 0),        // COL_RADIX - BLACK
    QColor(0, 0, 0),        // COL_ASZ - BLACK
    QColor(0, 0, 0),        // COL_PLANET_TIC - BLACK
    QColor(0, 0, 0),        // COL_PLANET_TICT - BLACK
    QColor(0, 0, 0),        // COL_PLAN - BLACK
    QColor(0, 0, 0),        // COL_PLAN_T - BLACK
    QColor(0, 0, 0),        // COL_HAUSER - BLACK
    QColor(0, 0, 0),        // CKONJUNKTION - BLACK
    QColor(0, 0, 0),        // CSEXTIL - BLACK
    QColor(0, 0, 0),        // CQUADRATUR - BLACK
    QColor(0, 0, 0),        // CTRIGON - BLACK
    QColor(0, 0, 0),        // COPOSITION - BLACK
    QColor(0, 0, 0),        // CHALBSEX - BLACK
    QColor(0, 0, 0),        // CQUINCUNX - BLACK
    QColor(0, 0, 0),        // COL_LBTXT_NORM - BLACK
    QColor(0, 0, 0),        // COL_LBTXT_SEL - BLACK
    QColor(0, 0, 0),        // COL_LBTXT_FOCUS - BLACK
    QColor(0, 0, 0),        // COL_LBTXT_BK - BLACK
    QColor(255, 255, 255),  // COL_LBREC_NORM - LWHITE
    QColor(255, 255, 255),  // COL_LBREC_SEL - LWHITE
    QColor(255, 255, 255),  // COL_LBREC_FOCUS - LWHITE
    QColor(255, 255, 255)   // COL_LBREC_BK - LWHITE
};

// Aktuelle Farben (werden bei Programmstart initialisiert)
QColor sColor[COLORS];
QColor pColor[COLORS];

void initDefaultColors() {
    for (int i = 0; i < COLORS; ++i) {
        sColor[i] = sColorDefault[i];
        pColor[i] = pColorDefault[i];
    }
}

//==============================================================================
// Orte - Konvertierung
//==============================================================================

Orte Orte::fromLegacy(const OrteLegacy& legacy, const QString& ortName) {
    Orte o;
    o.gultig = (legacy.cGultig != 0);
    o.index = legacy.lIndex;
    o.name = ortName;
    o.land = QString::fromLatin1(legacy.szLand, 3).trimmed();
    o.zone = legacy.fZone;
    o.breite = legacy.dBreite;
    o.laenge = legacy.dLange;
    return o;
}

OrteLegacy Orte::toLegacy() const {
    OrteLegacy legacy;
    std::memset(&legacy, 0, sizeof(legacy));
    legacy.cGultig = gultig ? 1 : 0;
    legacy.lIndex = index;
    legacy.slRec = sizeof(OrteLegacy);
    legacy.slOrt = static_cast<int16_t>(name.toLatin1().size());
    
    QByteArray landBytes = land.toLatin1();
    std::strncpy(legacy.szLand, landBytes.constData(), 3);
    
    legacy.fZone = zone;
    legacy.dBreite = breite;
    legacy.dLange = laenge;
    return legacy;
}

//==============================================================================
// RadixFix - Konvertierung
//==============================================================================

RadixFix RadixFix::fromLegacy(const RadixFixLegacy& legacy) {
    RadixFix rf;
    rf.gultig = (legacy.cGultig != 0);
    rf.indexRadix = legacy.lIndexRadix;
    rf.indexName = legacy.lIndexName;
    rf.indexNotiz = legacy.lIndexNotiz;
    rf.land = QString::fromLatin1(legacy.szLand, 3).trimmed();
    rf.sommerzeit = legacy.fSommer;
    rf.zone = legacy.fZone;
    rf.laenge = legacy.dLange;
    rf.breite = legacy.dBreite;
    rf.zeit = legacy.dTime;
    rf.tag = legacy.sTag;
    rf.monat = legacy.sMonat;
    rf.jahr = legacy.sJahr;
    return rf;
}

RadixFixLegacy RadixFix::toLegacy() const {
    RadixFixLegacy legacy;
    std::memset(&legacy, 0, sizeof(legacy));
    
    legacy.cGultig = gultig ? 1 : 0;
    legacy.lIndexRadix = indexRadix;
    legacy.lIndexName = indexName;
    legacy.lIndexNotiz = indexNotiz;
    legacy.clVorName = static_cast<int16_t>(vorname.toLatin1().size());
    legacy.clName = static_cast<int16_t>(name.toLatin1().size());
    legacy.clBeruf = static_cast<int16_t>(beruf.toLatin1().size());
    legacy.clOrt = static_cast<int16_t>(ort.toLatin1().size());
    legacy.fSommer = sommerzeit;
    legacy.fZone = zone;
    
    QByteArray landBytes = land.toLatin1();
    std::strncpy(legacy.szLand, landBytes.constData(), 3);
    
    legacy.dLange = laenge;
    legacy.dBreite = breite;
    legacy.dTime = zeit;
    legacy.sTag = tag;
    legacy.sMonat = monat;
    legacy.sJahr = jahr;
    
    return legacy;
}

//==============================================================================
// Radix - Konstruktor und Methoden
//==============================================================================

Radix::Radix() {
    allocate(MAX_PLANET);
}

void Radix::clear() {
    rFix = RadixFix();
    
    stzPlanet.fill(0);
    stzHaus.fill(0);
    planet.fill(0.0);
    planetRad.fill(0.0);
    haus.fill(0.0);
    hausRad.fill(0.0);
    aspPlanet.fill(KEIN_ASP);
    winkelPlanet.fill(0.0);
    aspHaus.fill(KEIN_ASP);
    winkelHaus.fill(0.0);
    planetTyp.fill(P_TYP_NORM);
    inHaus.fill(0);
    
    for (int i = 0; i < MAX_QUALITATEN; ++i) {
        qualitaten[i] = 0;
    }
    
    datum.clear();
    zeit.clear();
    
    breite = 0.0;
    dT = 0.0;
    jd = 0.0;
    sid = 0.0;
    ver = 0.0;
    mc = 0.0;
    asc = 0.0;
    ra = 0.0;
    ob = 0.0;
    
    hausSys = TYP_PLACIDUS;
    horoTyp = TYP_RADIX;
    anzahlPlanet = MAX_PLANET;
    
    paintFlag = 0;
    rotate = 0;
    stzGrad = 0.0;
    stzIndex = 0;
}

void Radix::allocate(int16_t numPlanets) {
    anzahlPlanet = numPlanets;
    
    stzPlanet.resize(numPlanets);
    stzHaus.resize(MAX_HAUS);
    planet.resize(numPlanets);
    planetRad.resize(numPlanets);
    haus.resize(MAX_HAUS + 1);  // 13 Häuser (0-12, wobei 0 = ASC)
    hausRad.resize(MAX_HAUS + 1);
    
    // Aspekt-Matrix: Planet x Planet
    aspPlanet.resize(numPlanets * numPlanets);
    winkelPlanet.resize(numPlanets * numPlanets);
    
    // Aspekte Planet-Haus
    aspHaus.resize(numPlanets * MAX_HAUS);
    winkelHaus.resize(numPlanets * MAX_HAUS);
    
    planetTyp.resize(numPlanets);
    inHaus.resize(numPlanets);
    
    clear();
}

//==============================================================================
// AuInit - Konstruktor und Methoden
//==============================================================================

AuInit::AuInit() {
    initOrben();
    initColors();
}

void AuInit::initColors() {
    // Planeten-Farben initialisieren
    planetColor.resize(MAX_PLANET);
    planetColor[P_SONNE]   = QColor(255, 200, 0);    // Gold
    planetColor[P_MOND]    = QColor(192, 192, 192);  // Silber
    planetColor[P_MERKUR]  = QColor(255, 255, 0);    // Gelb
    planetColor[P_VENUS]   = QColor(0, 255, 0);      // Grün
    planetColor[P_MARS]    = QColor(255, 0, 0);      // Rot
    planetColor[P_JUPITER] = QColor(128, 0, 255);    // Violett
    planetColor[P_SATURN]  = QColor(128, 64, 0);     // Braun
    planetColor[P_URANUS]  = QColor(0, 255, 255);    // Cyan
    planetColor[P_NEPTUN]  = QColor(0, 128, 255);    // Blau
    planetColor[P_PLUTO]   = QColor(128, 0, 0);      // Dunkelrot
    for (int i = 10; i < MAX_PLANET; ++i) {
        planetColor[i] = QColor(128, 128, 128);      // Grau für Rest
    }
    
    // Aspekt-Farben initialisieren
    aspektColor.resize(MAX_ASPEKT);
    aspektColor[0] = QColor(128, 0, 0);    // Konjunktion - Rot
    aspektColor[1] = QColor(0, 128, 0);    // Halbsextil - Grün
    aspektColor[2] = QColor(0, 128, 0);    // Sextil - Grün
    aspektColor[3] = QColor(255, 0, 0);    // Quadrat - Rot
    aspektColor[4] = QColor(0, 255, 0);    // Trigon - Grün
    aspektColor[5] = QColor(128, 128, 0);  // Quincunx - Olive
    aspektColor[6] = QColor(128, 0, 128);  // Opposition - Magenta
    for (int i = 7; i < MAX_ASPEKT; ++i) {
        aspektColor[i] = QColor(128, 128, 128);
    }
}

void AuInit::initOrben() {
    // Standard-Orben initialisieren
    // Format: [Planet1 * MAX_PLANET + Planet2] * ASPEKTE + Aspekt
    // Wie im Legacy: MAX_PLANET * MAX_PLANET * ASPEKTE
    
    int size = MAX_PLANET * MAX_PLANET * ASPEKTE;
    
    orbenPlanet.resize(size);
    orbenHaus.resize(MAX_PLANET * MAX_HAUS * ASPEKTE);
    orbenTPlanet.resize(size);
    orbenTHaus.resize(MAX_PLANET * MAX_HAUS * ASPEKTE);
    orbenSPlanet.resize(size);
    orbenSHaus.resize(MAX_PLANET * MAX_HAUS * ASPEKTE);
    
    // Standard-Orben (aus Legacy-Code)
    // Konjunktion: 10°, Sextil: 6°, Quadrat: 8°, Trigon: 8°, Opposition: 10°
    // Halbsextil: 2°, Quincunx: 3°
    
    float defaultOrben[ASPEKTE] = {
        10.0f,  // Konjunktion
        2.0f,   // Halbsextil
        6.0f,   // Sextil
        8.0f,   // Quadratur
        8.0f,   // Trigon
        3.0f,   // Quincunx
        10.0f   // Opposition
    };
    
    // Planet-zu-Planet Orben
    for (int pl1 = 0; pl1 < MAX_PLANET; ++pl1) {
        for (int pl2 = 0; pl2 < MAX_PLANET; ++pl2) {
            for (int asp = 0; asp < ASPEKTE; ++asp) {
                int idx = (pl1 * MAX_PLANET + pl2) * ASPEKTE + asp;
                float orb = defaultOrben[asp];
                
                // Sonne und Mond haben größere Orben
                if (pl1 == P_SONNE || pl1 == P_MOND || pl2 == P_SONNE || pl2 == P_MOND) {
                    orb += 2.0f;
                }
                // Asteroiden haben kleinere Orben
                if (pl1 >= P_CHIRON || pl2 >= P_CHIRON) {
                    orb -= 2.0f;
                    if (orb < 1.0f) orb = 1.0f;
                }
                
                orbenPlanet[idx] = orb;
                orbenTPlanet[idx] = orb - 1.0f;  // Transit etwas enger
                orbenSPlanet[idx] = orb;
            }
        }
    }
    
    // Planet-zu-Haus Orben
    for (int pl = 0; pl < MAX_PLANET; ++pl) {
        for (int haus = 0; haus < MAX_HAUS; ++haus) {
            for (int asp = 0; asp < ASPEKTE; ++asp) {
                int idx = (pl * MAX_HAUS + haus) * ASPEKTE + asp;
                float orb = defaultOrben[asp];
                
                if (pl == P_SONNE || pl == P_MOND) {
                    orb += 2.0f;
                }
                if (pl >= P_CHIRON) {
                    orb -= 2.0f;
                    if (orb < 1.0f) orb = 1.0f;
                }
                
                orbenHaus[idx] = orb;
                orbenTHaus[idx] = orb - 1.0f;
                orbenSHaus[idx] = orb;
            }
        }
    }
}

} // namespace astro
