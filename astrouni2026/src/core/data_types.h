#pragma once
/**
 * @file data_types.h
 * @brief 1:1 Port der Datenstrukturen aus legacy/astrouni.h
 * 
 * Strukturen: ORTE, RADIXFIX, RADIX, AU_INIT, SYS, EPHEM
 * Alle Felder werden exakt übernommen, nur die Typen werden
 * an C++20 angepasst (short → int16_t, long → int32_t).
 */

#include <cstdint>
#include <memory>
#include <QString>
#include <QVector>
#include <QColor>

#include "constants.h"

namespace astro {

//==============================================================================
// LBCOLOR - Listbox-Farben (aus astrouni.h Zeile 589-593)
//==============================================================================

struct LBColor {
    QColor pen;
    QColor brush;
    QColor text;
};

//==============================================================================
// ORTE - Orte-Datenbank-Record (aus astrouni.h Zeile 595-604)
//==============================================================================

#pragma pack(push, 1)
struct OrteLegacy {
    int16_t  cGultig;       // Record gültig? (short = 2 Bytes)
    int32_t  lIndex;        // Index im File
    int16_t  slRec;         // Record-Länge
    int16_t  slOrt;         // Ort-Länge
    char     szLand[4];     // Ländercode (z.B. "D", "A", "CH")
    float    fZone;         // Zeitzone (Stunden)
    double   dBreite;       // Geographische Breite
    double   dLange;        // Geographische Länge
};
#pragma pack(pop)

// Moderne Orte-Struktur für interne Verwendung
struct Orte {
    bool     gultig = false;
    int32_t  index = 0;
    QString  name;
    QString  land;
    float    zone = 0.0f;
    double   breite = 0.0;
    double   laenge = 0.0;
    
    // Konvertierung von/zu Legacy-Format
    static Orte fromLegacy(const OrteLegacy& legacy, const QString& ortName);
    OrteLegacy toLegacy() const;
};

//==============================================================================
// RADIXFIX - Personen-Stammdaten (aus astrouni.h Zeile 608-630)
//==============================================================================

// Legacy RADIXFIX Struktur aus astrouni.h (73 Bytes)
// Die Namen folgen direkt nach der Struktur mit variabler Länge
#pragma pack(push, 1)
struct RadixFixLegacy {
    int8_t   cGultig;           // Record gültig? (1 Byte)
    int32_t  lIndexRadix;       // Index Record im Radix-Datenfile
    int16_t  slRadix;           // Recordlänge RadixDaten
    int32_t  lIndexName;        // Index im Namen-Datenfile
    int16_t  slName;            // Recordlänge Namen
    int32_t  lIndexNotiz;       // Index Notiz-Datenfile
    int16_t  slNotiz;           // Länge Notiz
    int16_t  clVorName;         // Länge Vorname
    int16_t  clName;            // Länge Name
    int16_t  clBeruf;           // Länge Beruf
    int16_t  clOrt;             // Länge Ort
    double   fSommer;           // Sommerzeit (8 Bytes!)
    float    fZone;             // Zeitzone
    char     szLand[4];         // Land-Kürzel
    double   dLange;            // Geographische Länge
    double   dBreite;           // Geographische Breite
    double   dTime;             // Zeit (dezimal)
    int16_t  sTag;              // Tag
    int16_t  sMonat;            // Monat
    int16_t  sJahr;             // Jahr
    // Total: 73 Bytes
    // Namen folgen direkt danach: Vorname (clVorName), Name (clName), Beruf (clBeruf), Ort (clOrt)
};
#pragma pack(pop)

constexpr int RADIXFIX_SIZE = 73;  // sizeof(RadixFixLegacy) sollte 73 sein

// Moderne RadixFix-Struktur
struct RadixFix {
    bool     gultig = false;
    int32_t  indexRadix = 0;
    int32_t  indexName = 0;
    int32_t  indexNotiz = 0;
    
    QString  vorname;
    QString  name;
    QString  beruf;
    QString  ort;
    QString  land;
    QString  notiz;
    
    double   sommerzeit = 0.0;
    float    zone = 0.0f;
    double   laenge = 0.0;
    double   breite = 0.0;
    double   zeit = 0.0;        // Zeit als Dezimalzahl (z.B. 14.5 = 14:30)
    int16_t  tag = 1;
    int16_t  monat = 1;
    int16_t  jahr = 2000;
    
    // Konvertierung von/zu Legacy-Format
    static RadixFix fromLegacy(const RadixFixLegacy& legacy);
    RadixFixLegacy toLegacy() const;
};

//==============================================================================
// RADIX - Vollständige Horoskop-Daten (aus astrouni.h Zeile 634-695)
//==============================================================================

struct Radix {
    RadixFix rFix;              // Stammdaten
    
    // Berechnete Daten - Arrays
    QVector<int8_t>  stzPlanet;      // Sternzeichen der Planeten (0-11)
    QVector<int8_t>  stzHaus;        // Sternzeichen der Häuser (0-11)
    QVector<double>  planet;         // Planeten-Positionen (Grad)
    QVector<double>  planetRad;      // Planeten-Positionen (Radiant)
    QVector<double>  haus;           // Häuser-Positionen (Grad)
    QVector<double>  hausRad;        // Häuser-Positionen (Radiant)
    QVector<int8_t>  aspPlanet;      // Aspekte zwischen Planeten
    QVector<double>  winkelPlanet;   // Winkel zwischen Planeten
    QVector<int8_t>  aspHaus;        // Aspekte Planet-Haus
    QVector<double>  winkelHaus;     // Winkel Planet-Haus
    QVector<int16_t> planetTyp;      // Planet-Typ (normal/rückläufig/transit)
    QVector<int8_t>  inHaus;         // Planet in welchem Haus
    int16_t          qualitaten[MAX_QUALITATEN] = {0};  // Qualitäten-Zähler
    
    // Datum/Zeit als String
    QString  datum;             // Format: "TT.MM.JJJJ"
    QString  zeit;              // Format: "HH:MM"
    
    // Berechnungs-Variablen
    double   breite = 0.0;      // Geographische Breite (für Berechnung)
    double   dT = 0.0;          // Delta T
    double   jd = 0.0;          // Julianisches Datum
    double   sid = 0.0;         // Siderische Zeit
    double   ver = 0.0;         // Vertex
    double   mc = 0.0;          // Medium Coeli
    double   asc = 0.0;         // Aszendent
    double   ra = 0.0;          // Rektaszension
    double   ob = 0.0;          // Obliquität (Schiefe der Ekliptik)
    
    // Einstellungen
    int8_t   hausSys = TYP_PLACIDUS;  // Häusersystem
    int8_t   horoTyp = TYP_RADIX;     // Horoskop-Typ
    int16_t  anzahlPlanet = MAX_PLANET;
    
    // Zeichnungs-Flags
    int32_t  paintFlag = 0;
    int16_t  rotate = 0;
    double   stzGrad = 0.0;
    int8_t   stzIndex = 0;
    
    // Synastrie/Transit: Zweiter Radix (STRICT LEGACY: plRadix[0])
    std::shared_ptr<Radix> synastrie;  // Zweiter Radix für Synastrie/Transit
    
    // Konstruktor - initialisiert Arrays
    Radix();
    
    // Hilfsfunktionen
    void clear();
    void allocate(int16_t numPlanets = MAX_PLANET);
};

//==============================================================================
// SYS - System-Einstellungen (aus astrouni.h Zeile 699-717)
//==============================================================================

struct Sys {
    int16_t  xScr = 0;          // Bildschirm X
    int16_t  yScr = 0;          // Bildschirm Y
    int16_t  xScrDpi = 96;      // Bildschirm DPI X
    int16_t  yScrDpi = 96;      // Bildschirm DPI Y
    int16_t  scrCol = 0;        // Bildschirm Farben
    double   scrFactor = 1.0;   // Bildschirm Faktor
    double   scrMul = 1.0;      // Bildschirm Multiplikator
    
    int16_t  xPrn = 0;          // Drucker X
    int16_t  yPrn = 0;          // Drucker Y
    int16_t  xPrnDpi = 300;     // Drucker DPI X
    int16_t  yPrnDpi = 300;     // Drucker DPI Y
    int16_t  prnCol = 0;        // Drucker Farben
    double   prnFactor = 1.0;   // Drucker Faktor
    double   prnMul = 1.0;      // Drucker Multiplikator
    
    int16_t  status = 0;        // Status-Flags
};

//==============================================================================
// EPHEM - Ephemeriden-Einstellungen (aus astrouni.h Zeile 719-732)
// Hinweis: Wird durch Swiss Ephemeris ersetzt, aber für Kompatibilität behalten
//==============================================================================

struct Ephem {
    int32_t  lDefault = 0;
    int32_t  lPlanetI = 0;
    int8_t   cPlanetS = 0;
    int8_t   cPlanetE = 0;
    int32_t  lAsterI = 0;
    int8_t   cAsterS = 0;
    int8_t   cAsterE = 0;
    int32_t  lChironI = 0;
    int8_t   cChironS = 0;
    int8_t   cChironE = 0;
};

//==============================================================================
// AU_INIT - Globale Einstellungen (aus astrouni.h Zeile 734-755)
//==============================================================================

struct AuInit {
    int16_t  sSelHaus = TYP_PLACIDUS;   // Gewähltes Häusersystem
    int16_t  sSelHoro = TYP_RADIX;      // Gewählter Horoskop-Typ
    int16_t  sRotRadix = ROT_ASC;       // Radix-Rotation (ASC links = Standard)
    int16_t  sRotation = ROT_ASC;       // Rotation (ASC links oder Sonne oben)
    int16_t  sAspekte = S_KON | S_SEX | S_QUA | S_TRI | S_OPO;  // Aspekt-Auswahl
    int16_t  sTeilung = 0;              // Teilung (3er/9er)
    int16_t  sAnzahlPlanet = 10;        // Anzahl der angezeigten Planeten
    
    // Anzeige-Optionen
    bool     bShowMinutes = true;       // Minuten anzeigen
    bool     bShowSeconds = false;      // Sekunden anzeigen
    
    // Orben-Arrays (MAX_PLANET x MAX_PLANET x ASPEKTE) - wie im Legacy
    // Index: [planet1 * MAX_PLANET + planet2] * ASPEKTE + aspekt
    // oder vereinfacht: [planet1][planet2][aspekt]
    QVector<float> orbenPlanet;         // Orben Planeten (Radix)
    QVector<float> orbenHaus;           // Orben Häuser (Radix)
    QVector<float> orbenTPlanet;        // Orben Transit-Planeten
    QVector<float> orbenTHaus;          // Orben Transit-Häuser
    QVector<float> orbenSPlanet;        // Orben Synastrie-Planeten
    QVector<float> orbenSHaus;          // Orben Synastrie-Häuser
    
    // Farb-Arrays
    QVector<QColor> planetColor;        // Planeten-Farben
    QVector<QColor> aspektColor;        // Aspekt-Farben
    
    int16_t  sRet = 0;                  // Return-Wert
    QString  bDatum;                    // Basis-Datum
    QString  bZeit;                     // Basis-Zeit
    int16_t  sModus = 0;                // Modus
    
    Radix    radix;                     // Aktuelle Radix-Daten
    Ephem    ep;                        // Ephemeriden-Einstellungen
    
    // Konstruktor - initialisiert Orben mit Standardwerten
    AuInit();
    
    // Orben initialisieren
    void initOrben();
    
    // Farben initialisieren
    void initColors();
};

//==============================================================================
// Farb-Arrays (aus astrouni.h Zeile 522-587)
//==============================================================================

// Standard-Farben für Bildschirm
extern QColor sColorDefault[COLORS];

// Standard-Farben für Druck
extern QColor pColorDefault[COLORS];

// Aktuelle Farben
extern QColor sColor[COLORS];
extern QColor pColor[COLORS];

// Farben initialisieren
void initDefaultColors();

} // namespace astro
