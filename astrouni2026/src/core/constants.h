#pragma once
/**
 * @file constants.h
 * @brief 1:1 Port der Konstanten aus legacy/astrouni.h
 * 
 * Alle Konstanten werden exakt übernommen, nur die Syntax
 * wird an C++20 angepasst.
 */

#include <cstdint>
#include <cmath>

namespace astro {

//==============================================================================
// Mathematische Konstanten (aus astrouni.h Zeile 28-41)
//==============================================================================

inline constexpr double PI         = 3.14159265358979323846;
inline constexpr double PI2        = PI * 2.0;
inline constexpr double PIHALB     = PI / 2.0;
inline constexpr double DEGMAX     = 360.0;
inline constexpr double DEGHALB    = 180.0;
inline constexpr double DEGVIER    = 90.0;
inline constexpr double DEG2RAD    = DEGHALB / PI;
inline constexpr double E2000      = -24.736467;
inline constexpr double AXE        = 23.44578889;
inline constexpr double KLEIN      = 1.7453E-09;
inline constexpr double GROSS      = 10000.0;
inline constexpr double RUNDEN     = 0.5;
inline constexpr double AR         = 1.33333333;

// TAN Makro als inline Funktion
inline double TAN(double d) { return std::sin(d) / std::cos(d); }

//==============================================================================
// Kreis-Konstanten für Radix-Zeichnung (aus astrouni.h Zeile 58-78)
//==============================================================================

// Normal
inline constexpr double KREIS_STZ      = 0.882;
inline constexpr double KREIS_10_GRAD  = 0.824;
inline constexpr double KREIS_3_GRAD   = 0.765;
inline constexpr double KREIS_5_GRAD   = 0.735;
inline constexpr double KREIS_PLANET   = 0.677;
inline constexpr double KREIS_ASP      = 0.3;
inline constexpr double KREIS_KLEIN    = 0.02;

// Ohne 3er oder 9er
inline constexpr double KREIS3_STZ      = 0.882;
inline constexpr double KREIS3_10_GRAD  = 0.824;
inline constexpr double KREIS3_3_GRAD   = 0.824;
inline constexpr double KREIS3_5_GRAD   = 0.794;
inline constexpr double KREIS3_PLANET   = 0.734;

// Ohne 3er und 9er
inline constexpr double KREIS9_STZ      = 0.872;
inline constexpr double KREIS9_10_GRAD  = 0.872;
inline constexpr double KREIS9_3_GRAD   = 0.872;
inline constexpr double KREIS9_5_GRAD   = 0.830;
inline constexpr double KREIS9_PLANET   = 0.765;

//==============================================================================
// Planeten-Indizes (aus astrouni.h Zeile 248-268)
//==============================================================================

inline constexpr int P_ERDE     = 0;
inline constexpr int P_SONNE    = 0;
inline constexpr int P_MOND     = 1;
inline constexpr int P_MERKUR   = 2;
inline constexpr int P_VENUS    = 3;
inline constexpr int P_MARS     = 4;
inline constexpr int P_JUPITER  = 5;
inline constexpr int P_SATURN   = 6;
inline constexpr int P_URANUS   = 7;
inline constexpr int P_NEPTUN   = 8;
inline constexpr int P_PLUTO    = 9;
inline constexpr int P_NKNOTEN  = 10;
inline constexpr int P_LILITH   = 11;
inline constexpr int P_CHIRON   = 12;
inline constexpr int P_CERES    = 13;
inline constexpr int P_PALLAS   = 14;
inline constexpr int P_JUNO     = 15;
inline constexpr int P_VESTA    = 16;

inline constexpr int MAX_PLANET     = 17;
inline constexpr int MAX_HAUS       = 12;
inline constexpr int MAX_QUALITATEN = 9;
inline constexpr int MAX_ASPEKT     = 12;

//==============================================================================
// Sternzeichen-Indizes (aus astrouni.h Zeile 201-225)
//==============================================================================

inline constexpr int iWIDDER      = 0;
inline constexpr int iSTIER       = 1;
inline constexpr int iZWILLINGE   = 2;
inline constexpr int iKREBS       = 3;
inline constexpr int iLOWE        = 4;
inline constexpr int iJUNGFRAU    = 5;
inline constexpr int iWAAGE       = 6;
inline constexpr int iSKORPION    = 7;
inline constexpr int iSCHUTZE     = 8;
inline constexpr int iSTEINBOCK   = 9;
inline constexpr int iWASSERMANN  = 10;
inline constexpr int iFISCHE      = 11;

inline constexpr int HAUS_INDEX   = 12;
inline constexpr int iASZ         = 12;
inline constexpr int iHA2         = 13;
inline constexpr int iHA3         = 14;
inline constexpr int iHA4         = 15;
inline constexpr int iHA5         = 16;
inline constexpr int iHA6         = 17;
inline constexpr int iHA7         = 18;
inline constexpr int iHA8         = 19;
inline constexpr int iHA9         = 20;
inline constexpr int iHA10        = 21;
inline constexpr int iHA11        = 22;
inline constexpr int iHA12        = 23;
inline constexpr int PLANET_INDEX = 24;
inline constexpr int ASP_INDEX    = 41;

//==============================================================================
// Häusersystem-Typen (aus astrouni.h Zeile 287-303)
//==============================================================================

inline constexpr int TYP_KOCH       = 0;
inline constexpr int TYP_PLACIDUS   = 1;
inline constexpr int TYP_EQUAL      = 2;
inline constexpr int TYP_EQUALMID   = 3;
inline constexpr int TYP_WHOLE      = 4;
inline constexpr int TYP_TOPOZEN    = 5;
inline constexpr int TYP_CAMPANUS   = 6;
inline constexpr int TYP_MERIDIAN   = 7;
inline constexpr int TYP_REGIOMON      = 8;
inline constexpr int TYP_REGIOMONTANUS = 8;  // Alias
inline constexpr int TYP_PORPHYRY   = 9;
inline constexpr int TYP_PORPHYRYN  = 10;
inline constexpr int TYP_MORINUS    = 11;
inline constexpr int TYP_ALCABIT    = 12;
inline constexpr int TYP_NULL       = 13;

//==============================================================================
// Horoskop-Typen (aus astrouni.h Zeile 306-312)
//==============================================================================

inline constexpr int TYP_RADIX      = 0;
inline constexpr int TYP_SOLAR      = 1;
inline constexpr int TYP_LUNAR      = 2;
inline constexpr int TYP_TRANSIT    = 3;
inline constexpr int TYP_SYNASTRIE  = 4;
inline constexpr int TYP_COMPOSIT   = 5;

//==============================================================================
// Planet-Typen (aus astrouni.h Zeile 315-320)
//==============================================================================

inline constexpr int P_TYP_NORM      = 0x1;
inline constexpr int P_TYP_RUCK      = 0x2;
inline constexpr int P_TYP_TRANSIT   = 0x4;
inline constexpr int P_TYP_SYNASTRIE = 0x8;
inline constexpr int P_TYP_COMPOSIT  = 0x10;
inline constexpr int P_TYP_IGNORE    = -1;

//==============================================================================
// Qualitäten (aus astrouni.h Zeile 323-331)
//==============================================================================

inline constexpr int QU_ERDE      = 0;
inline constexpr int QU_WASSER    = 1;
inline constexpr int QU_FEUER     = 2;
inline constexpr int QU_LUFT      = 3;
inline constexpr int QU_CARDINAL  = 4;
inline constexpr int QU_FIX       = 5;
inline constexpr int QU_BEWEGLICH = 6;
inline constexpr int QU_FRAU      = 7;
inline constexpr int QU_MANN      = 8;

//==============================================================================
// Aspekte (aus astrouni.h Zeile 334-354)
//==============================================================================

inline constexpr int ASPEKTE      = 7;
inline constexpr int KEIN_ASP     = -1;
inline constexpr int KONJUNKTION  = 0;
inline constexpr int HALBSEX      = 30;
inline constexpr int SEXTIL       = 60;
inline constexpr int QUADRATUR    = 90;
inline constexpr int TRIGON       = 120;
inline constexpr int QUINCUNX     = 150;
inline constexpr int OPOSITION    = 180;

// Aspekt-Flags
inline constexpr int S_KON   = 0x1;
inline constexpr int S_SEX   = 0x2;
inline constexpr int S_QUA   = 0x4;
inline constexpr int S_TRI   = 0x8;
inline constexpr int S_OPO   = 0x10;
inline constexpr int S_HAL   = 0x20;
inline constexpr int S_QUI   = 0x40;
inline constexpr int S_PLAN  = 0x1000;
inline constexpr int S_ASTER = 0x2000;
inline constexpr int S_HAUS  = 0x4000;

inline constexpr int S_3er   = 0x1;
inline constexpr int S_9er   = 0x2;

//==============================================================================
// Inkrement-Typen (aus astrouni.h Zeile 279-284)
//==============================================================================

inline constexpr int INC_MINUTEN = 1;
inline constexpr int INC_STUNDEN = 2;
inline constexpr int INC_TAGE    = 3;
inline constexpr int INC_WOCHEN  = 5;
inline constexpr int INC_MONATE  = 6;
inline constexpr int INC_JAHRE   = 7;

//==============================================================================
// Fehler-Codes (aus astrouni.h Zeile 440-464)
//==============================================================================

inline constexpr int ERR_STRANSIT   = 3;
inline constexpr int ERR_CANCEL     = 2;
inline constexpr int ERR_OK         = 1;
inline constexpr int ERR_NOITEM     = 0;
inline constexpr int ERR_NAME       = -1;
inline constexpr int ERR_DATE       = -2;
inline constexpr int ERR_TIME       = -3;
inline constexpr int ERR_ZONE       = -4;
inline constexpr int ERR_STIME      = -5;
inline constexpr int ERR_ORT        = -6;
inline constexpr int ERR_HOUSE      = -7;
inline constexpr int ERR_CHART      = -8;
inline constexpr int ERR_MEM        = -10;
inline constexpr int ERR_FON        = -11;
inline constexpr int ERR_FILE       = -12;
inline constexpr int ERR_PRINT      = -13;
inline constexpr int ERR_ORBEN      = -14;
inline constexpr int ERR_IGNORE     = -15;
inline constexpr int ERR_EPHEM      = -16;
inline constexpr int ERR_RADIX      = -17;
inline constexpr int ERR_TRANSIT    = -18;
inline constexpr int ERR_SYNASTRIE  = -19;
inline constexpr int ERR_COMPOSIT   = -20;
inline constexpr int ERR_INC        = -21;
inline constexpr int ERR_SEEK       = -22;

//==============================================================================
// Farb-Indizes (aus astrouni.h Zeile 485-520)
//==============================================================================

inline constexpr int COLORS          = 28;
inline constexpr int SCR_COLOR_INDEX = 0;
inline constexpr int COL_ELEMENT     = 4;
inline constexpr int COL_PRN         = 0;
inline constexpr int COL_SCR         = 1;
inline constexpr int COL_EL          = 1;

inline constexpr int COL_HINTER      = 0;
inline constexpr int COL_FEUER       = 1;
inline constexpr int COL_ERDE        = 2;
inline constexpr int COL_LUFT        = 3;
inline constexpr int COL_WASSER      = 4;
inline constexpr int COL_TXT         = 5;
inline constexpr int COL_RADIX       = 6;
inline constexpr int COL_ASZ         = 7;
inline constexpr int COL_PLANET_TIC  = 8;
inline constexpr int COL_PLANET_TICT = 9;
inline constexpr int COL_PLAN        = 10;
inline constexpr int COL_PLAN_T      = 11;
inline constexpr int COL_HAUSER      = 12;
inline constexpr int CKONJUNKTION    = 13;
inline constexpr int CSEXTIL         = 14;
inline constexpr int CQUADRATUR      = 15;
inline constexpr int CTRIGON         = 16;
inline constexpr int COPOSITION      = 17;
inline constexpr int CHALBSEX        = 18;
inline constexpr int CQUINCUNX       = 19;
inline constexpr int COL_LBTXT_NORM  = 20;
inline constexpr int COL_LBTXT_SEL   = 21;
inline constexpr int COL_LBTXT_FOCUS = 22;
inline constexpr int COL_LBTXT_BK    = 23;
inline constexpr int COL_LBREC_NORM  = 24;
inline constexpr int COL_LBREC_SEL   = 25;
inline constexpr int COL_LBREC_FOCUS = 26;
inline constexpr int COL_LBREC_BK    = 27;

//==============================================================================
// Datei-Namen (aus astrouni.h Zeile 132-139)
//==============================================================================

inline constexpr const char* ORTDAT = "astroger.dat";
inline constexpr const char* NAMDAT = "astronam.dat";
inline constexpr const char* RADDAT = "astrorad.dat";
inline constexpr const char* NOTDAT = "astronot.dat";
inline constexpr const char* ORBDAT = "default.dat";
inline constexpr const char* INIDAT = "astroini.dat";

// Version
inline constexpr int16_t MAIN_VERSION = 4;  // v0.04Beta

// Rotation
inline constexpr int ROT_ASC = 1;
inline constexpr int ROT_SONNE = 2;

//==============================================================================
// Text-Konstanten (aus astrouni.h Zeile 147-198)
// Deutsche Texte für Planeten, Zeichen, Häuser
//==============================================================================

inline constexpr const char* PLANETPOS_TXT   = "Planetenpositionen";
inline constexpr const char* HAUSPOS_TXT     = "Häuserpositionen";
inline constexpr const char* HAUSSYS_TXT     = "Häuser";
inline constexpr const char* TRANSITPOS_TXT  = "Transit Planeten";
inline constexpr const char* QUALITAT_TXT    = "Qualitäten";

// Planeten-Namen
inline constexpr const char* SONNE_TXT    = "Sonne";
inline constexpr const char* MOND_TXT     = "Mond";
inline constexpr const char* MERKUR_TXT   = "Merkur";
inline constexpr const char* VENUS_TXT    = "Venus";
inline constexpr const char* MARS_TXT     = "Mars";
inline constexpr const char* JUPITER_TXT  = "Jupiter";
inline constexpr const char* SATURN_TXT   = "Saturn";
inline constexpr const char* URANUS_TXT   = "Uranus";
inline constexpr const char* NEPTUN_TXT   = "Neptun";
inline constexpr const char* PLUTO_TXT    = "Pluto";
inline constexpr const char* NMKL_TXT     = "N.Mndknt.";
inline constexpr const char* CHIRON_TXT   = "Chiron";
inline constexpr const char* CERES_TXT    = "Ceres";
inline constexpr const char* PALLAS_TXT   = "Pallas";
inline constexpr const char* JUNO_TXT     = "Juno";
inline constexpr const char* VESTA_TXT    = "Vesta";
inline constexpr const char* LILITH_TXT   = "Lilith";

// Sternzeichen-Namen
inline constexpr const char* WIDDER_TXT     = "Widder";
inline constexpr const char* STIER_TXT      = "Stier";
inline constexpr const char* ZWILLING_TXT   = "Zwilling";
inline constexpr const char* KREBS_TXT      = "Krebs";
inline constexpr const char* LOWE_TXT       = "Löwe";
inline constexpr const char* JUNGFRAU_TXT   = "Jungfrau";
inline constexpr const char* WAAGE_TXT      = "Waage";
inline constexpr const char* SKORPION_TXT   = "Skorpion";
inline constexpr const char* SCHUTZE_TXT    = "Schütze";
inline constexpr const char* STEINBOCK_TXT  = "Steinbock";
inline constexpr const char* WASSERMANN_TXT = "Wassermann";
inline constexpr const char* FISCHE_TXT     = "Fische";

// Häuser-Namen
inline constexpr const char* ASZENDENT_TXT = "Aszendent";
inline constexpr const char* HAUS2_TXT     = "Haus  2";
inline constexpr const char* HAUS3_TXT     = "Haus  3";
inline constexpr const char* HAUS4_TXT     = "Haus  4";
inline constexpr const char* HAUS5_TXT     = "Haus  5";
inline constexpr const char* HAUS6_TXT     = "Haus  6";
inline constexpr const char* HAUS7_TXT     = "Haus  7";
inline constexpr const char* HAUS8_TXT     = "Haus  8";
inline constexpr const char* HAUS9_TXT     = "Haus  9";
inline constexpr const char* HAUS10_TXT    = "Haus 10";
inline constexpr const char* HAUS11_TXT    = "Haus 11";
inline constexpr const char* HAUS12_TXT    = "Haus 12";

// Qualitäten-Namen
inline constexpr const char* QU_ERDE_TXT      = "Erde";
inline constexpr const char* QU_WASSER_TXT    = "Wasser";
inline constexpr const char* QU_FEUER_TXT     = "Feuer";
inline constexpr const char* QU_LUFT_TXT      = "Luft";
inline constexpr const char* QU_CARDINAL_TXT  = "Cardinal";
inline constexpr const char* QU_FIX_TXT       = "Fix";
inline constexpr const char* QU_BEWEGLICH_TXT = "Beweglich";
inline constexpr const char* QU_FRAU_TXT      = "Weiblich";
inline constexpr const char* QU_MANN_TXT      = "Männlich";

} // namespace astro
