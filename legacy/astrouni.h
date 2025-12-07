/*----------------------------------------------------------------------------*/
/* Autor:               JeaMY  Lee                                            */
/* Datum:               Samstag 27.01.1996                                   */
/*----------------------------------------------------------------------------*/

#define STRICT

#include <windows.h>
#include <stdio.h>
#include <mem.h>
#include <string.h>
#include <commdlg.h>
#include <dos.h>
#include <math.h>
#include <stdlib.h>
#include <malloc.h>
#include <fcntl.h>
#include <conio.h>
#include <io.h>
#include <ctype.h>

/*
*/

//include "astrounr.h"

//konstanten
#define PI         3.14159265358979323846
#define PI2        (PI*2.0)
#define PIHALB     (PI/2.0)
#define DEGMAX     360.0
#define DEGHALB    180.0
#define DEGVIER    90.0
#define DEG2RAD    (DEGHALB/PI)
#define E2000      (-24.736467)
#define AXE        23.44578889
#define KLEIN      (1.7453E-09)
#define GROSS      10000.0
#define RUNDEN     0.5
#define TAN(d)     (sin(d)/cos(d))
#define AR         1.33333333
#define A_KEY      0xAD9638E7
#define BUF_SIZE 256

#define E_STEP 80              /* days step in LRZ ephe */
#define E_DAYS_PER_FILE 100000 /* days per ephe file */
#define E_OUTER         1
#define E_OUTER_BSIZE  60      /* blocksize */
#define E_OUTER_FSIZE   E_DAYS_PER_FILE/E_STEP*E_OUTER_BSIZE
#define E_CHIRON	    2
#define E_CHIRON_BSIZE 12      /* blocksize */
#define E_CHIRON_FSIZE  E_DAYS_PER_FILE/E_STEP*E_CHIRON_BSIZE
#define E_ASTER 	    3
#define E_ASTER_BSIZE  48      /* blocksize */
#define E_ASTER_FSIZE   E_DAYS_PER_FILE/E_STEP*E_ASTER_BSIZE

//normal
#define KREIS_STZ      0.882
#define KREIS_10_GRAD  0.824
#define KREIS_3_GRAD   0.765
#define KREIS_5_GRAD   0.735
#define KREIS_PLANET   0.677
#define KREIS_ASP      0.3
#define KREIS_KLEIN    0.02

//ohne 3er oder 9er
#define KREIS3_STZ      0.882
#define KREIS3_10_GRAD  0.824
#define KREIS3_3_GRAD   0.824
#define KREIS3_5_GRAD   0.794
#define KREIS3_PLANET   0.734

//ohne 3er und 9er
#define KREIS9_STZ      0.872
#define KREIS9_10_GRAD  0.872
#define KREIS9_3_GRAD   0.872
#define KREIS9_5_GRAD   0.830
#define KREIS9_PLANET   0.765

#define WM_INITRADIX   WM_USER+1
#define WM_INITDLG     WM_USER+2
#define MAUS_ZOOM	   0x1
#define MAUS_L		   0x2
#define MAUS_R		   0x4
#define MAUS_LD		   0x8
#define MAUS_RD		   0x10
#define MAUS_ASPEKT	   0x12

#define DRUBERFACTOR   32
#define DRW_LBA_PLANETA  0x10000000
#define DRW_LBA_PLANETT  0x20000000
#define DRW_LBA_HAUSA    0x01000000
#define DRW_LBA_HAUST    0x02000000

#define PB_DATEN	   0x100
#define PB_ASPEKTE	   0x101
#define PB_POSITION	   0x102
#define LB_DATEN	   0x103
#define LB_ASPEKTE	   0x104
#define LB_POSITION	   0x105

#define MAIN_VERSION     4
#define szAbout          " AstroUniverse\nVersion 0.04Beta\nby A.S. and J.L.\n     ©1997"
#define AboutText        "About AstroUniverse"
#define PERSON_ERFASST_TXT "Person gespeichert."
#define PERSON_NEU_TXT   "Neue Eingabe."
#define ORT_ERFASST_TXT  "Ort gespeichert."
#define PERSON_TXT       "Person auswählen"
#define PERSON1_TXT      "Person 1 auswählen"
#define PERSON2_TXT      "Person 2 auswählen"
#define ASPEKTE_TXT	     "Aspekte"
#define TASPEKTE_TXT	 "TAspekte"
#define POS_TXT          "Positionen"
#define TPOS_TXT         "TPositionen"
#define S1POS_TXT         "Positionen1"
#define S2POS_TXT         "Positionen2"
#define UNBEKANNT_TXT     "Unbekannt"
#define ORBEN_EIN_TXT     "Orben werden eingelesen (aus DEFAULT.DAT)!"
#define ORBEN_DEFEIN_TXT  "Orben werden eingelesen (aus DEFAULT.DAT)!"
#define ORBEN_S_TXT       "Orben werden gesichert!"
#define FARB_S_TXT        "Farbeinstellungen werden gesichert!"
#define FARB_EIN_TXT      "Default-Farbeinstellungen werden eingelesen!"
#define TRANS_S_TXT       "Transit Selektion wird gesichert!"
#define TRANS_EIN_TXT     "Transit Selektion wird eingelesen (aus ASTROINI.DAT)!"
#define SEL_S_TXT         "Einstellungen werden gesichert!"
#define SEL_EIN_TXT       "Einstellungen werden eingelesen!"
#define RADIX_CALC_TXT    "Radixhoroskop wird berechnet!"
#define TRANSIT_CALC_TXT  "Transithoroskop wird berechnet!"
#define SYNASTRIE1_CALC_TXT "Radixhoroskop von Person 1 wird berechnet!"
#define SYNASTRIE2_CALC_TXT "Radixhoroskop von Person 2 wird berechnet!"

//#define  ORTDAT "astroeur.dat"
#define  ORTDAT "astroger.dat"
#define  NAMDAT "astronam.dat"
#define  RADDAT "astrorad.dat"
#define  NOTDAT "astronot.dat"
#define  ORBDAT "default.dat"
#define  INIDAT "astroini.dat"
#define  EPHDAT "astroeph.dat"

//RadixTexte
#define PLANETPOS_TXT	 "Planetenpositionen"
#define HAUSPOS_TXT		 "Häuserpositionen"
#define HAUSSYS_TXT		 "Häuser"
#define TRANSITPOS_TXT	 "Transit Planeten"
#define QUALITAT_TXT	 "Qualitäten"
#define SONNE_TXT        "Sonne"
#define MOND_TXT         "Mond"
#define MERKUR_TXT       "Merkur"
#define VENUS_TXT        "Venus"
#define MARS_TXT         "Mars"
#define JUPITER_TXT      "Jupiter"
#define SATURN_TXT       "Saturn"
#define URANUS_TXT       "Uranus"
#define NEPTUN_TXT       "Neptun"
#define NMKL_TXT         "N.Mndknt."
#define PLUTO_TXT        "Pluto"
#define CHIRON_TXT       "Chiron"
#define CERES_TXT        "Ceres"
#define PALLAS_TXT       "Pallas"
#define JUNO_TXT         "Juno"
#define VESTA_TXT        "Vesta"
#define LILITH_TXT       "Lilith"

#define WIDDER_TXT  	 "Widder"
#define STIER_TXT  		 "Stier"
#define ZWILLING_TXT 	 "Zwilling"
#define KREBS_TXT 		 "Krebs"
#define LOWE_TXT  		 "Löwe"
#define JUNGFRAU_TXT  	 "Jungfrau"
#define WAAGE_TXT  		 "Waage"
#define SKORPION_TXT  	 "Skorpion"
#define SCHUTZE_TXT  	 "Schütze"
#define STEINBOCK_TXT  	 "Steinbock"
#define WASSERMANN_TXT   "Wassermann"
#define FISCHE_TXT  	 "Fische"
#define ASZENDENT_TXT  	 "Aszendent"
#define HAUS2_TXT  		 "Haus  2"
#define HAUS3_TXT  		 "Haus  3"
#define HAUS4_TXT  		 "Haus  4"
#define HAUS5_TXT  		 "Haus  5"
#define HAUS6_TXT  		 "Haus  6"
#define HAUS7_TXT  		 "Haus  7"
#define HAUS8_TXT  		 "Haus  8"
#define HAUS9_TXT  		 "Haus  9"
#define HAUS10_TXT  	 "Haus 10"
#define HAUS11_TXT  	 "Haus 11"
#define HAUS12_TXT  	 "Haus 12"

#define  QU_ERDE_TXT		"Erde"
#define  QU_WASSER_TXT      "Wasser"
#define  QU_FEUER_TXT       "Feuer"
#define  QU_LUFT_TXT        "Luft"
#define  QU_CARDINAL_TXT    "Cardinal"
#define  QU_FIX_TXT			"Fix"
#define  QU_BEWEGLICH_TXT	"Beweglich"
#define  QU_FRAU_TXT		"Weiblich"
#define  QU_MANN_TXT		"Männlich"
#define  CEND				#
//Indizes in TextTabelle
#define iWIDDER       0
#define iSTIER        1
#define iZWILLINGE    2
#define iKREBS        3
#define iLOWE         4
#define iJUNGFRAU     5
#define iWAAGE        6
#define iSKORPION     7
#define iSCHUTZE      8
#define iSTEINBOCK    9
#define iWASSERMANN  10
#define iFISCHE      11
#define HAUS_INDEX   12
#define iASZ         12
#define iHA2         13
#define iHA3         14
#define iHA4         15
#define iHA5         16
#define iHA6         17
#define iHA7         18
#define iHA8         19
#define iHA9         20
#define iHA10        21
#define iHA11        22
#define iHA12        23
#define PLANET_INDEX 24
/*
#define iSONNE       24
#define iMOND        25
#define iMERKUR      26
#define iVENUS       27
#define iMARS        28
#define iJUPITER     29
#define iSATURN      30
#define iURANUS      31
#define iNEPTUN      32
#define iPLUTO       33
#define iNMK         34
#define iCHIRON      35
#define iCERES       36
#define iPALLAS      37
#define iJUNO        38
#define iVESTA       39
#define iLILITH      40
*/
#define ASP_INDEX    41

//Planeten
#define P_ERDE  		0
#define P_SONNE 		0
#define P_MOND			1
#define P_MERKUR	    2
#define P_VENUS			3
#define P_MARS			4
#define P_JUPITER		5
#define P_SATURN	    6
#define P_URANUS	    7
#define P_NEPTUN	    8
#define P_PLUTO		    9
#define P_NKNOTEN      10
#define P_LILITH       11
#define P_CHIRON	   12
#define P_CERES		   13
#define P_PALLAS	   14
#define P_JUNO		   15
#define P_VESTA		   16
//#define P_WNKNOTEN     17
#define MAX_PLANET     17
#define MAX_HAUS	   12
#define MAX_QUALITATEN  9

/*
#define oSou 17
#define oFor 18
#define oVtx 19
#define oEP  20
*/

#define  INC_MINUTEN     1
#define  INC_STUNDEN     2
#define  INC_TAGE        3
#define  INC_WOCHEN      5
#define  INC_MONATE      6
#define  INC_JAHRE       7

//Häusertypen
#define  TYP_KOCH        0
#define  TYP_PLACIDUS    1
#define  TYP_EQUAL		 2
#define  TYP_EQUALMID    3
#define  TYP_WHOLE		 4
#define  TYP_TOPOZEN	 5
#define  TYP_CAMPANUS	 6
#define  TYP_MERIDIAN	 7
#define  TYP_REGIOMON	 8
#define  TYP_PORPHYRY	 9
#define  TYP_PORPHYRYN	 10
#define  TYP_MORINUS	 11
#define  TYP_ALCABIT	 12
#define  TYP_NULL		 13
#define  HAUS_ID_INDEX	 0x300
#define  RA_HAUS_BEGIN   TYP_KOCH+HAUS_ID_INDEX
#define  RA_HAUS_END     TYP_NULL+HAUS_ID_INDEX

//Horoskoptyp
#define  TYP_RADIX       0
#define  TYP_TRANSIT     1
#define  TYP_SYNASTRIE   2
#define  TYP_COMPOSIT    3
#define  HORO_ID_INDEX	 0x300
#define  RA_HORO_BEGIN   TYP_RADIX+HORO_ID_INDEX
#define  RA_HORO_END     TYP_COMPOSIT+HORO_ID_INDEX

//Planettyp
#define  P_TYP_NORM        0x1
#define  P_TYP_RUCK        0x2
#define  P_TYP_TRANSIT     0x4
#define  P_TYP_SYNASTRIE   0x8
#define  P_TYP_COMPOSIT    0x10
#define  P_TYP_IGNORE      -1

//Qualitäten
#define  QU_ERDE		   0
#define  QU_WASSER		   1
#define  QU_FEUER		   2
#define  QU_LUFT		   3
#define  QU_CARDINAL	   4
#define  QU_FIX     	   5
#define  QU_BEWEGLICH	   6
#define  QU_FRAU  		   7
#define  QU_MANN  		   8

//Aspekte
#define ASPEKTE        7
#define KEIN_ASP      -1
#define KONJUNKTION    0
#define HALBSEX       30
#define SEXTIL        60
#define QUADRATUR     90
#define TRIGON       120
#define QUINCUNX     150
#define OPOSITION    180

#define S_KON          0x1
#define S_SEX          0x2
#define S_QUA          0x4
#define S_TRI          0x8
#define S_OPO          0x10
#define S_HAL          0x20
#define S_QUI          0x40
#define S_PLAN         0x1000
#define S_ASTER        0x2000
#define S_HAUS         0x4000

#define S_3er          0x1
#define S_9er          0x2

//Diverse "Zeichen-Flags"
#define DRW_REFRESH     0x1
#define DRW_RADIX       0x2
#define DRW_HAUS        0x4
#define DRW_TEXT        0x8
#define DRW_RADIX_TABLE 0x10
#define DRW_TR_RA_TABLE 0x20
#define DRW_TR_TR_TABLE 0x40

#define PRN_NORMAL      0
#define PRN_SETUP       1
#define PRN_MULTITRANS  2

#define CALC_ON_FILE    0 //Radixdaten gespeichert in Datei
#define CALC_ON_MEM     1

#define SEEK_ORB_DEF     1
#define SEEK_ORB_OK      2
#define SEEK_TRANS_DEF   3
#define SEEK_TRANS_OK    4
#define SEEK_SEL_DEF     5
#define SEEK_SEL_OK      6

#define DRW_ORB_ASP      1
#define DRW_ORB_PLX      2
#define DRW_ORB_PLY      3
#define DRW_TRANS_ALL    4
#define DRW_TRANS_PLX    5
#define DRW_TRANS_PLY    6
#define DRW_FARB_ALL     7
#define MAX_CHECK 6
#define MAX_TRANS_SEL   (MAX_PLANET*(MAX_PLANET+5)*ASPEKTE)+MAX_CHECK

#define SET_TRANS_ALL    1
#define SET_TRANS_NONE   2
#define SET_TRANS_PLANET 3
#define SET_TRANS_ASTER  4
#define SET_TRANS_TPLANET 5
#define SET_TRANS_TASTER 6
#define SET_TRANS_HAUS   7
#define SET_TRANS_XY     8

#define DRW_LB_NORMAL   0x0
#define DRW_LB_FOCUS    0x1
#define DRW_LB_SELECT   0x2

#define DRW_LB_TXT_HAUS   0x100
#define DRW_LB_TXT_ASPEKT 0x200
#define DRW_LB_TXT_ORT    0x300
#define DRW_LB_TXT_DATEN  0x400
#define DRW_LB_TXT_TRANSIT 0x500
#define DRW_LB_MENU_ORTE -2

#define DRW_LB_INDEX    0x1
#define DRW_LB_BUFFER   0x2
#define DRW_LBD_INDEX   0x3
#define DRW_LBD_BUFFER  0x4
#define DRW_LBA_INDEX   0x5
#define DRW_LBA_BUFFER  0x6

#define LB_G			0x1
#define LB_A			0x2
#define LB_S			0x4

#define DEL_SRC			0x1 //RadixCopy Src-speicher freigeben
#define ALLOC_MEM	    0x2
#define MTRANS 	        0x4

//Radix-Rotationstypen
#define ROT_WIDDER	    0x1
#define ROT_ASC         0x10
#define ROT_SONNE       0x100

#define ORBEN_P         1
#define ORBEN_H         2
#define ORBEN_TP        4
#define ORBEN_TH        8
#define GET_ALL			0
#define GET_DOUBLE		1
#define GET_ONE			2

//Error
#define ERR_STRANSIT    3
#define ERR_CANCEL      2
#define ERR_OK          1
#define ERR_NOITEM      0
#define ERR_NAME       -1
#define ERR_DATE       -2
#define ERR_TIME	   -3
#define ERR_ZONE	   -4
#define ERR_STIME	   -5
#define ERR_ORT		   -6
#define ERR_HOUSE	   -7
#define ERR_CHART	   -8
#define ERR_MEM        -10
#define ERR_FON        -11
#define ERR_FILE       -12
#define ERR_PRINT      -13
#define ERR_ORBEN      -14
#define ERR_IGNORE     -15
#define ERR_EPHEM      -16
#define ERR_RADIX      -17
#define ERR_TRANSIT    -18
#define ERR_SYNASTRIE  -19
#define ERR_COMPOSIT   -20
#define ERR_INC        -21
#define ERR_SEEK       -22

//Farben
#define BLACK      RGB(0,0,0)
#define LBLUE      RGB(0,0,255)  
#define LGREEN     RGB(0,255,0)
#define LCYAN      RGB(0,255,255)
#define LRED       RGB(255,0,0)
#define LMAGENTA   RGB(255,0,255)
#define LWHITE     RGB(255,255,255)
#define BROWN      RGB(128,64,0)
#define WHITE      RGB(192,192,192)
#define GRAY       RGB(128,128,128)
#define BLUE       RGB(0,0,128)
#define CYAN       RGB(0,128,128)
#define GREEN      RGB(0,128,0)
#define RED        RGB(128,0,0)
#define MAGENTA    RGB(128,0,128)
#define YELLOW     RGB(255,255,0)

//FarbIndizes
#define COLORS         28
#define SCR_COLOR_INDEX 0
#define PRN_COLOR_INDEX sizeof(COLORREF)*COLORS
#define COL_ELEMENT     4
#define COL_PRN         0
#define COL_SCR         1
#define COL_EL          1

#define COL_HINTER      0
#define COL_FEUER       1
#define COL_ERDE        2
#define COL_LUFT        3
#define COL_WASSER      4
#define COL_TXT         5
#define COL_RADIX       6
#define COL_ASZ         7
#define COL_PLANET_TIC  8
#define COL_PLANET_TICT 9
#define COL_PLAN       10
#define COL_PLAN_T     11
#define COL_HAUSER     12
#define CKONJUNKTION   13
#define CSEXTIL        14
#define CQUADRATUR     15
#define CTRIGON        16
#define COPOSITION     17
#define CHALBSEX       18
#define CQUINCUNX      19
#define COL_LBTXT_NORM  20
#define COL_LBTXT_SEL   21
#define COL_LBTXT_FOCUS 22
#define COL_LBTXT_BK    23
#define COL_LBREC_NORM  24
#define COL_LBREC_SEL   25
#define COL_LBREC_FOCUS 26
#define COL_LBREC_BK    27

#define COLM_HINTER      WHITE
#define COLM_TXT         BLACK
#define COLM_RADIX       GRAY
#define COLM_ASZ         LWHITE
#define COLM_FEUER       LRED
#define COLM_WASSER      BLUE
#define COLM_ERDE        BROWN
#define COLM_LUFT        YELLOW
#define COLM_PLANET_TIC  MAGENTA
#define COLM_PLANET_TICT LMAGENTA
#define COLM_PL_TXT      BLUE
#define COLM_PL_TXT_T    RED
#define COLM_PLAN        LBLUE
#define COLM_PLAN_T      CYAN
#define COLM_HAUSER      GRAY
#define COLM_LBTXT_NORM  BLUE
#define COLM_LBTXT_SEL   BLUE
#define COLM_LBTXT_FOCUS BLUE
#define COLM_LBTXT_BK    LWHITE
#define COLM_LBREC_NORM  WHITE
#define COLM_LBREC_SEL   GRAY
#define COLM_LBREC_FOCUS GRAY
#define COLM_LBREC_BK    BLUE
#define CMKONJUNKTION    RED
#define CMSEXTIL         GREEN
#define CMHALBSEX        LGREEN
#define CMQUADRATUR      RED
#define CMTRIGON         GREEN
#define CMOPOSITION      MAGENTA
#define CMQUINCUNX       LRED

#define COLP_RADIX       GRAY
#define COLP_HAUSER      BLACK
#define COLP_ASZ         BLACK
#define COLP_LUFT        LCYAN

#define COLMo_HINTER      LWHITE
#define COLMo_TXT         BLACK
#define COLMo_RADIX       BLACK
#define COLMo_ASZ         BLACK
#define COLMo_FEUER       BLACK
#define COLMo_WASSER      BLACK
#define COLMo_ERDE        BLACK
#define COLMo_LUFT        BLACK
#define COLMo_PLANET_TIC  BLACK
#define COLMo_PLANET_TICT BLACK
#define COLMo_PL_TXT      BLACK
#define COLMo_PL_TXT_T    BLACK
#define COLMo_PLAN        BLACK
#define COLMo_PLAN_T      BLACK
#define COLMo_HAUSER      BLACK
#define COLMo_LBTXT_NORM  BLACK
#define COLMo_LBTXT_SEL   BLACK
#define COLMo_LBTXT_FOCUS BLACK
#define COLMo_LBTXT_BK    BLACK
#define COLMo_LBREC_NORM  LWHITE
#define COLMo_LBREC_SEL   LWHITE
#define COLMo_LBREC_FOCUS LWHITE
#define COLMo_LBREC_BK    LWHITE
#define CMoKONJUNKTION    BLACK
#define CMoSEXTIL         BLACK
#define CMoHALBSEX        BLACK
#define CMoQUADRATUR      BLACK
#define CMoTRIGON         BLACK
#define CMoOPOSITION      BLACK
#define CMoQUINCUNX       BLACK

typedef struct {
	COLORREF Pen;
    COLORREF Brush;
    COLORREF Text;
}LBCOLOR;

typedef struct {
		short   cGultig;
		long    lIndex;
		short   slRec;
		short   slOrt;
		char	szLand[4];
		float   fZone;
		double  dBreite;
		double  dLange;
}ORTE;

#define ORT_SIZE sizeof(ORTE)

typedef struct
{
  char    cGultig;					//Record gueltig?
  long    lIndexRadix;				//Index Record im Radix-Datenfile
  short   slRadix;                  //Recordlange RadixDaten
  long    lIndexName;				//Index im Namen-Datenfile
  short   slName;                   //Recordlänge Namen
  long    lIndexNotiz;              //Index Notiz-Datenfile
  short   slNotiz;                  //länge Notiz
  short   clVorName;
  short   clName;
  short   clBeruf;
  short   clOrt;
  double  fSommer;
  float   fZone;
  char    szLand[4];
  double  dLange;
  double  dBreite;
  double  dTime;
  short   sTag;
  short   sMonat;
  short   sJahr;
} RADIXFIX;
#define RFIX_SIZE sizeof(RADIXFIX)


typedef struct
  {
  RADIXFIX  rFix;
  char  * 	pszName;
  char  *	pszVorName;
  char  * 	pszBeruf;
  char  * 	pszOrt;
  char  * 	pszNotiz;
  char     *pszStzPlanet;
  char     *pszStzHaus;
  double   *pdPlanet;
  double   *pdPlanetRad;
  double   *pdHaus;
  double   *pdHausRad;
  char     *pszAspPlanet;
  double   *pdWinkelPlanet;
  char     *pszAspHaus;
  double   *pdWinkelHaus;
  short    *psPlanetTyp;
  char     *pszInHaus;
  short     sQualitaten[9];
  char      szDatum[11];
  char      szZeit[6];
  double  	dBreite;
  double  	dT;
  double 	dJD;
  double  	dSid;
  double  	VER;
  double  	MC;
  double 	ASC;
  double 	RA;
  double  	OB;
  char      cHausSys;
  char      cHoroTyp;
  short   	sAnzahlPlanet;
  long      lPaintFlag;
  short     sRotate;
  double  	dStzGrad;
  char    	cStzIndex;
  short	    cMemory;
  LOGFONT * MainFontRec;
  LOGFONT * AstroFontRec;
  HANDLE  	hBitmap;
  HANDLE    hBitOld;
  HDC       hDCBit;
  short     sLB;
  HWND		hWndLBLast;
  HWND		hWndLBG;
  HWND		hWndLBA;
  HWND		hWndLBD;
  HWND		hWnd;
  LPARAM    lParam;
  WPARAM    wParam;
  char      cMouse;
  long      lTransitNum;
  long      lRadixCount;
  long huge*plRadix;
  long      lMulti;
  long huge*plMulti;
  DLGPROC   lpRadixDlg;
  HWND      hWndRadixDlg;
  } RADIX;
#define RA_SIZE sizeof(RADIX)

#define SYS_PRN 0x1
typedef struct
  {
  short	    xScr;
  short	    yScr;
  short	    xScrDpi;
  short	    yScrDpi;
  short	    ScrCol;
  double    ScrFactor;
  double    ScrMul;
  short	    xPrn;
  short	    yPrn;
  short	    xPrnDpi;
  short	    yPrnDpi;
  short	    PrnCol;
  double    PrnFactor;
  double    PrnMul;
  short     Status;
  } SYS;
#define SYS_SIZE sizeof(SYS)

typedef struct {
         long    lDefault;
		 long    lPlanetI;
         char    cPlanetS;
         char    cPlanetE;
 		 long    lAsterI;
         char    cAsterS;
         char    cAsterE;
 		 long    lChironI;
         char    cChironS;
         char    cChironE;

}EPHEM;
#define  EP_SIZE sizeof(EPHEM)

typedef struct {
  short	  sSelHaus;
  short   sSelHoro;
  short   sRotRadix;
  short   sAspekte;
  short   sTeilung;
  float*  pfOrbenPlanet;
  float*  pfOrbenHaus;
  float*  pfOrbenTPlanet;
  float*  pfOrbenTHaus;
  float*  pfOrbenSPlanet;
  float*  pfOrbenSHaus;
//  char*   pszIgnore;
//  char*   pszIgnoreHaus;
  short   sRet;
  char    szBDatum[11];
  char    szBZeit[6];
  short   sModus;
  RADIX   Radix;
  EPHEM   Ep;
} AU_INIT;
#define AUI_SIZE sizeof(AU_INIT)

LRESULT  CALLBACK MainWndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT  CALLBACK RadixWndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT  CALLBACK DlgRadix (HWND, UINT, WPARAM, LPARAM);
LRESULT  CALLBACK TransitWndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT	 CALLBACK SubLB(HWND, UINT, WPARAM, LPARAM );
LRESULT	 CALLBACK SubEdit(HWND, UINT, WPARAM, LPARAM );
LRESULT	 CALLBACK SubText(HWND, UINT, WPARAM, LPARAM );
LRESULT	 CALLBACK SubTransitText(HWND, UINT, WPARAM, LPARAM );
LRESULT	 CALLBACK SubFarbTxt(HWND, UINT, WPARAM, LPARAM );
LRESULT	 CALLBACK DlgDDProc(HWND, UINT, WPARAM, LPARAM );
LRESULT	 CALLBACK DlgODroc(HWND, UINT, WPARAM, LPARAM );
LRESULT	 CALLBACK DlgDGProc(HWND, UINT, WPARAM, LPARAM );
LRESULT	 CALLBACK DlgPHProc(HWND, UINT, WPARAM, LPARAM );
LRESULT	 CALLBACK DlgPErfassen(HWND, UINT, WPARAM, LPARAM );
LRESULT	 CALLBACK DlgOErfassen(HWND, UINT, WPARAM, LPARAM );
LRESULT	 CALLBACK DlgOrtAuswahl(HWND, UINT, WPARAM, LPARAM );
LRESULT	 CALLBACK DlgHausAuswahl(HWND, UINT, WPARAM, LPARAM );
LRESULT	 CALLBACK DlgHoroAuswahl(HWND, UINT, WPARAM, LPARAM );
BOOL     CALLBACK _export DlgAbortProc(HDC, int );
LRESULT  CALLBACK DlgAbort(HWND, UINT, WPARAM, LPARAM);
LRESULT  CALLBACK DlgTransit(HWND, UINT, WPARAM, LPARAM);
LRESULT  CALLBACK DlgTransEin(HWND, UINT, WPARAM, LPARAM );
LRESULT  CALLBACK DlgOrbenEin(HWND, UINT, WPARAM, LPARAM );
LRESULT  CALLBACK DlgFarben(HWND, UINT, WPARAM, LPARAM );
LRESULT  CALLBACK DlgTransAbort(HWND, UINT, WPARAM, LPARAM);
LRESULT  CALLBACK DlgCalc(HWND, UINT, WPARAM, LPARAM);
LRESULT  CALLBACK DlgRuck(HWND, UINT, WPARAM, LPARAM);
LRESULT  CALLBACK DlgAspekte(HWND, UINT, WPARAM, LPARAM);


//LRESULT  CALLBACK DlgComposit(HWND, UINT, WPARAM, LPARAM);
BOOL 		InitApplication(void);
BOOL 		InitInstance(void);
void 		CMUFileOpen(HWND);

//HWND 		InitRadixWnd(HWND, char *);
HWND 		PaintRadixWnd(HWND, RADIX *);
void 		EndApplication(void);
short       sStartCalc(HWND, char*);
short       sEndCalc(void);

short 	 	sDrawTRad( RADIX * );
short	    sCalcMultiTransit(RADIX*, short);
short 		sDrawTText(RADIX*, DRAWITEMSTRUCT*, char*, char);
short 		sRucklauf(HWND);
short	    sIncDate(char*, char*, short, short, short);
//short       sSetTransWnd(HWND);
short       sEndTrans(HWND, DLGPROC);
short       sSetRadixScroll(RADIX*, HWND, HWND, short);
void        vSetRadixScrollIni(RADIX*, HWND);
short 		sGetWindowText(HWND, short, short*);


RADIX *		InitRadix ( HWND );
short		sInitRadixStruct( RADIX *, short );
void 		ExitRadix ( RADIX * );
short       sInitCT(RADIX*, HWND, short);
short		sInitFont(void);
short 	 	sDrawRad( RADIX * );
void        vRotateRadix(RADIX *, RADIX*, short);
short       sZoom(HDC, RADIX*);
short		sRadix( HDC, RADIX * );
short		sHaus(  HDC, RADIX *, char );
short		sPlanet( HDC, RADIX *, char );
void 		vPlanetDraw ( HDC, RADIX *, char*, char, short );
void 		vPlanetTicDraw ( HDC, RADIX *, char*, char, short );
void 		vSetFlag( RADIX*, char*, char *, short );
short 		sDrawAspekte( HDC, RADIX *, RADIX*, char );
short 		sDrawText( RADIX *, RADIX*, DRAWITEMSTRUCT*, char*, char );
short		sSetColor(HWND, HWND);
short		sGetColor(HWND);
COLORREF    colReturnCol(short);
short       scolSetCol();
short       sSetmono(HWND, short);
short 	    sSetR(HWND, char);
void		vInvalidateAsp(HWND);
short 		sOneAspekt (HDC, RADIX *);
void	    vSetColor(DRAWITEMSTRUCT*);
short 		sPutString(HDC, char*, short, short, COLORREF );
short 		sPrintMTransit(HDC, RADIX *, double);
short 		sPrintAsp(HDC, RADIX *, double);
short 		sPrintText(HDC, RADIX *, double);
short 		sPrintKoord(HDC, RADIX*, double);
short	    sDlgPrint(RADIX*, char);
HDC         hdcPrinterIni(char, char*);
short	    sSetViewport(HDC, RADIX*, short);
short 		sSetXY(short *, short *);
short       sSetSys(HDC, short);
short       sSetScrCol(void);
short  	    sSetOrbTxt(HWND, short);
short  	    sSetOrbEdit(HWND);
short  	    sSubText(HWND);
short  	    sSubTransitText(HWND);
short  	    sSubFarbText(HWND);
short 	    sPaintOrbTxt(HWND);
short 	    sPaintTransitTxt(HWND);
short 	    sPaintFarbTxt(HWND);
short 	    sSetTransSel(HWND, short, short);
short 	    sSetTransRect(HWND);
short       sAddOrb(HWND, short);
short       sSetOrbFocus(HWND, short);
RADIX*  	RadixCopy(RADIX*, RADIX*, char);
short 		sDrawLBState( DRAWITEMSTRUCT*, char *, short );
short 		sTextOut( DRAWITEMSTRUCT*, char *, short );
char*  		pszGetString( char *, char* );
void        vUpper(char*);
void        vLower(char*);

//short       sIgnore(void);
short  		sOrben(FILE*);
short  		sReadIni(void);
short  		sReadIniOrb(FILE*);
short  		sWriteOrb(FILE*);
short  		sWriteini(void);
short  		sOpenIni(short);
void		vDefTeilung(void);
short       sReadDefColor(FILE*, short, long);
short       sWriteDefColor(void);
void 		vSetHausOrb(short, short, short, float*, float);
void 		vCalcWinkel( RADIX *, RADIX*, short );
void 		vCalcAsp( RADIX *, short );
short 	sDrawAspTable( short );
short	sCalcXY( short );
short 	sRadixTable( short, short );
short 		sSetAspLine( HDC, short, char );
short 		sSetPlanetTyp(RADIX*, short);
short 		sStrToDate(RADIX*);
short 		sStrToTime(RADIX*);

short			sAllocRadix(RADIX *, short);
short           sWriteDat( RADIX *, HWND );
short           sReadDat( RADIX *, HWND, char * );
short           sDeleteDat( RADIX *, HWND, short );
short           sGetName( RADIX *, RADIX*, HWND, short, char* );
short           sFillNameLB (HWND, short, short);
void            vGetDate( HWND, short );
short           sOrteWrite(HWND);
short           sOrtAuswahl(long);
short           sGetOrt(HWND, RADIX*, short);
short 			sSetRadKoor(RADIX*, ORTE*);
short 			sDrawLBOrt(DRAWITEMSTRUCT*);
void 			vSetErfassen(HWND);
void            vSetNamenSuchen(HWND);
short 			sInitErfassenDialog(HWND, LPARAM);
short 			sSetLBTabs(HWND, short);
short           sSearchOrt(HWND, short, char*, ORTE*);
short           sAdd(HWND, short );
short           sAddOrt(HWND, short, ORTE*, char* );
short 			sSetOrtTxt(HWND, short, short, double*, char*);
short           sFormatTime(char*, short*, short*);
short           sFormatDatum(char*, short*, short*, short*);
short  			sGetValidRecords( RADIX*, RADIX *, short, short* );
void            vKillData( RADIX* );

short 			sCalc(short, double, short, double *, double *, double *, double *);
short 			sEpheFilePos(double, short);
double          dDeltat(double);
void 		   	vCalcVar(RADIX *);
double			dSpitzeMid(RADIX *);
double			dSpitzeAsc(RADIX *);
double			dSpitzeEP(RADIX *);
double 			dJulTag(short, short, short, double, short);
double 			dMod(double);
double 			dModRad(double);
double 			dWinkel(double, double);
double 			dVorz(double);
double 			dMinDist(double, double);
double 			dDez2Deg(double);
double 			dDeg2Dez(double);
void 			vGrad2GMS(double, short*, short*, short*);
void            vGrad2Str(float, char*, short, short);
long 			lTMJ2Jul(short, short, short);
void 			vPolToRec(double, double, double *, double *);
void 			vRecToPol(double, double, double*, double*);
double 			dRecToSph(double, double, double);
short 			sStart( char*, short);

short 			sCalcChart(RADIX *, RADIX*, short);
short 			sCalcHauser(RADIX *);
void 			vCalcPlanets(RADIX *);
void  	        vCalcMissing(RADIX *, RADIX*, short);
void 			vKoch(RADIX *);
double			dSpitzePlacidus(RADIX *, double, double, short);
void 			vHausPlacidus(RADIX *);
void 			vEqual(RADIX *);
double 			dSpitzeTopocentric(RADIX *, double);
void 			vTopocentric(RADIX *);
void 			vCampanus(RADIX *);
void 			vMeridian(RADIX *);
void 			vRegiomontanus(RADIX *);
void			vPorphyry(RADIX *);
void 			vMorinus(RADIX *);
void 			vEqualMid(RADIX *);
void 			vAlcabitius(RADIX *);
void 			vPorphyryNeo(RADIX *);
void 			vWhole(RADIX *);
void 			vNull(RADIX *);

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#define ID_CANCEL				    0x500
#define	PB_Ok						0x501
#define PB_Abbruch					0x502
#define	PB_Hilfe					0x503
#define PB_Default					0x504
#define TXT_CALC					0x505
#define ABORT_PRINTER				0x506


//MAINWINDOW
#define CM_U_NEW	                0x200
#define CM_U_FILEOPEN               0x201
#define CM_U_SAVE                   0x202
#define CM_U_SAVEU                  0x203
#define CM_U_PRINT                  0x204
#define CM_U_LAYOUT                 0x205
#define CM_U_PINST                  0x206
#define CM_U_EXIT                   0x207

#define CM_U_PERSON                 0x210
#define CM_U_ORT                    0x211

//#define CM_U_RADIX                  0x221
//#define CM_U_SYNASTRIE              0x222
//#define CM_U_TRANSIT                0x223
//#define CM_U_COMPOSIT               0x224
//#define CM_U_COMBINE                0x225
//#define CM_U_DIREKT                 0x226

#define CM_U_HOROTYP                0x220
#define CM_U_HAUSER                 0x221
#define CM_U_ORBEN	                0x222
#define CM_U_FARBEN	                0x223
#define CM_U_EINST	                0x224

#define CM_U_HELPINDEX              0x230
#define CM_U_HELPKEY                0x231
#define CM_U_HELPCOM                0x232
#define CM_U_HELPPROC               0x233
#define CM_U_HELP                   0x234
#define CM_U_HELPABOUT              0x235

//RADIXWINDOW
#define CM_UR_SAVE					0x240
#define CM_UR_PRINT                 0x241
#define CM_UR_EXIT                  0x242
#define CM_UR_HOROTYP               0x243
#define CM_UR_HELP                  0x244
                                        
//TRANSITSUCHE
#define CM_UT_SAVE					0x300
#define CM_UT_PRINT                 0x301
#define CM_UT_EXIT                  0x302
#define CM_UT_HOROTYP               0x303
#define CM_UT_HELP                  0x304
#define PB_UT_GRAF					0x305
#define PB_UT_RUCK					0x306
#define TXT_UT_DATUM				0x307
#define TXT_UT_BDATUM				0x308
#define TXT_UT_TDATUM				0x309
#define TXT_UT_TZEIT				0x30A
#define TXT_UT_TBDATUM				0x30B
#define TXT_UT_TBZEIT				0x30C
#define TXT_UT_WINKEL				0x30D
#define LB_UT_TRANS 				0x30E

//Personen Erfassen
#define PB_NamenSuchen				0x300
#define TXT_Vorname					0x301
#define ETXT_Vorname				0x302
#define TXT_Name					0x303
#define ETXT_Name					0x304
#define PB_OrtSuchen				0x305
#define TXT_Geburtsort				0x306
#define ETXT_Ort					0x307
#define TXT_Geburtszeit				0x308
#define ETXT_Zeit					0x309
#define TXT_Geburtsdatum			0x30A
#define ETXT_Datum					0x30B
#define TXT_Beruf					0x30C
#define ETXT_Beruf	     			0x30D
#define PB_Calc						0x30E
#define TXT_Sommerzeit				0x30F
#define ETXT_Sommer 				0x310
#define TXT_Zeitzone				0x311
#define TXT_L           			0x313
#define TXT_B        				0x314
#define TXT_Lange	    			0x315
#define TXT_Breite 		     		0x316
#define TXT_Erfasst		     		0x317
#define TXT_LAND 					0x318

//Ort Erfassen
#define ETXT_LangeG					0x300
#define ETXT_LangeM					0x301
#define ETXT_LangeS					0x302
#define ETXT_BreiteG				0x303
#define ETXT_BreiteM				0x304
#define ETXT_BreiteS				0x305
#define ETXT_Zeitzone				0x306
#define ETXT_Ort					0x307
#define ETXT_Land					0x308
#define RB_OST						0x309
#define RB_WEST					    0x310
#define RB_NORD					    0x311
#define RB_SUD					    0x312

//Transit
#define PB_TOrt						0x300
#define ETXT_TOrt					0x301
#define TXT_TName					0x302
#define ETXT_TName					0x303
#define TXT_TDatum					0x304
#define ETXT_TDatum					0x305
#define ETXT_TBDatum				0x306
#define TXT_TZeit					0x307
#define ETXT_TZeit					0x308
#define ETXT_TBZeit					0x309
#define RBT_START					0x30a
#define RB_TAG 						0x30a
#define RB_MONAT					0x30b
#define RB_JAHR                     0x30c
#define RBT_END                     0x30c

//Composit
#define ETXT_Person1			    0x300
#define ETXT_Person2			    0x301
#define PB_Person1				    0x303
#define PB_Person2				    0x304

//HausAuswahl
#define RA_KOCH					    0x300
#define RA_PLACIDUS					0x301
#define RA_EQUAL 				    0x302
#define RA_TOPO				   	    0x305
#define RA_CAMPANUS				    0x306
#define RA_REGIO			   	    0x308
#define RA_MORINUS				    0x311
#define RA_NULL					    0x313
#define TXT_Haus					0x317

//HoroAuswahl
#define RA_RADIX     			    0x300
#define RA_TRANSIT 					0x301
#define RA_SYNASTRIE			    0x302
#define RA_COMPOSIT					0x303
#define TXT_Horo					0x304
#define TXT_Haussys					0x305


// Ort Suchen
//#define	CB_O_OrtSuchen				0x330
#define	LB_O_OrtSuchen				0x300
#define	ETXT_O_OrtSuchen			0x301
#define TXT_O_Ortsname				0x302
#define PB_O_Abbruch				0x303
#define PB_O_OK						0x304
#define PB_O_Neu					0x305
#define PB_O_Help					0x306

//Daten Doppelt
#define CM_UDD_LB                   0x300
#define CM_UDD_JA                   0x301
#define CM_UDD_ADD                  0x302
#define CM_UDD_ESC                  0x303
#define CM_UDD_HELP                 0x304
#define CM_UDD_TXT                  0x305

//Person Auswahl
#define CM_UDG_LB                   0x300
#define CM_UDG_OK                   0x301
#define CM_UDG_ESC                  0x302
#define CM_UDG_HELP                 0x303
#define CM_UDG_TXT                  0x304

//Personen Handling
#define LB_PHNamen                  0x300
#define PB_PHLoeschen               0x301
#define TXT_PHAuswahl               0x302

//Farben
#define SB_R						0x300
#define SB_G						0x301
#define SB_B						0x302
#define EDIT_R						0x303
#define EDIT_G						0x304
#define EDIT_B						0x305
#define TXT_ALT						0x306
#define TXT_NEU						0x307
#define CHB_MONO_S					0x308
#define CHB_MONO_P					0x309
#define BL_HS                       200
#define BL_HP                       300
#define BL_HP1                      BL_HP+COL_HINTER
#define BL_HP2                      BL_HP+COL_RADIX
#define BL_HP3                      BL_HP+COL_ASZ
#define BL_HP4                      BL_HP+COL_HAUSER
#define BL_HP5                      BL_HP+COL_PLAN
#define BL_HP6                      BL_HP+COL_PLAN_T
#define BL_HP7                      BL_HP+COL_PLANET_TIC
#define BL_HP8                      BL_HP+COL_PLANET_TICT
#define BL_HP9                      BL_HP+COL_FEUER
#define BL_HP10                     BL_HP+COL_ERDE
#define BL_HP11                     BL_HP+COL_LUFT
#define BL_HP12                     BL_HP+COL_WASSER
#define BL_HPSELE                   BL_HP9
#define BL_HPEELE                   BL_HP12
#define BL_HP13                     BL_HP+CKONJUNKTION
#define BL_HP14                     BL_HP+CHALBSEX
#define BL_HP15                     BL_HP+CSEXTIL
#define BL_HP16                     BL_HP+CQUADRATUR
#define BL_HP17                     BL_HP+CTRIGON
#define BL_HP18                     BL_HP+CQUINCUNX
#define BL_HP19                     BL_HP+COPOSITION

#define BL_HS1                      BL_HS+COL_HINTER
#define BL_HS2                      BL_HS+COL_RADIX
#define BL_HS3                      BL_HS+COL_ASZ
#define BL_HS4                      BL_HS+COL_HAUSER
#define BL_HS5                      BL_HS+COL_PLAN
#define BL_HS6                      BL_HS+COL_PLAN_T
#define BL_HS7                      BL_HS+COL_PLANET_TIC
#define BL_HS8                      BL_HS+COL_PLANET_TICT
#define BL_HS9                      BL_HS+COL_FEUER
#define BL_HS10                     BL_HS+COL_ERDE
#define BL_HS11                     BL_HS+COL_LUFT
#define BL_HS12                     BL_HS+COL_WASSER
#define BL_HSSELE                   BL_HS9
#define BL_HSEELE                   BL_HS12
#define BL_HS13                     BL_HS+CKONJUNKTION
#define BL_HS14                     BL_HS+CHALBSEX
#define BL_HS15                     BL_HS+CSEXTIL
#define BL_HS16                     BL_HS+CQUADRATUR
#define BL_HS17                     BL_HS+CTRIGON
#define BL_HS18                     BL_HS+CQUINCUNX
#define BL_HS19                     BL_HS+COPOSITION

//Orben TRANSIT-EIN

#define RBP_START                   0x300
#define RBP_RADIX                   0x300
#define RBP_TRANSIT                 0x301
#define RBP_SYNASTRIE               0x302
#define RBP_COMPOSIT                0x303
#define RBP_END                     0x303
#define RBP_PSTART                  0x304
#define RBP_PLANET                  0x304
#define RBP_ASTER                   0x305
#define RBP_PEND                    0x305
#define CHB_ZOMBIE                  0x306
#define PB_ZAP                      0x307
#define SB_HOR                      0x308
#define SB_VER                      0x309
#define TXT_TYP                     0x30A
#define TXT_RAD                     0x30B
#define PB_ALL                      0x30C
#define PB_NONE                     0x30d
#define PB_ORB                      0x30e
#define RB_TSTART                   0x30f
#define RB_TPLANET                  0x30f
#define RB_TASTER                   0x310
#define RB_TEND                     0x310
#define CHB_PLANET                  0x311
#define CHB_ASTER                   0x312
#define CHB_HAUS                    0x313
#define TXT_DATE                    0x314
#define TXT_TIME                    0x315
#define TXT_BDATE                   0x316
#define TXT_BTIME                   0x317
#define TXT_HorSTART                300
#define TXT_HorOrbEND               310
#define XORB  (TXT_HorOrbEND-TXT_HorSTART+1)
#define TXT_VerSTART                350
#define TXT_VerOrbEND               357
#define YORB  (TXT_VerOrbEND-TXT_VerSTART+1)
#define I_KON	                    400
#define EDITP_START                 200
#define EDITP_END                   (EDITP_START+ASPEKTE*XORB-1)
#define TXTP_START                  200
#define TXTP_END                    (TXTP_START+ASPEKTE*XORB-1)

//Aspekte
#define RBA_KON                      0x300
#define RBA_HSX                      0x301
#define RBA_SEX                      0x302
#define RBA_QUA                      0x303
#define RBA_TRI                      0x304
#define RBA_QUI                      0x305
#define RBA_OPO                      0x306
#define RBA_3                        0x307
#define RBA_9                        0x308
#define RBA_PLAN                     0x309
#define RBA_ASTER                    0x30A
#define RBA_HAUS                     0x30B

//TransitAbort

#define TA_VON 						 0x300
#define TA_AKT 						 0x301
#define TA_BIS 						 0x302

#define SC_MIN         				 0x300
#define SC_STU         				 0x301
#define SC_TAG         				 0x302
#define SC_MON         				 0x303
#define SC_JAH         				 0x304
#define ED_MIN         				 0x305
#define ED_STU         				 0x306
#define ED_TAG         				 0x307
#define ED_MON         				 0x308
#define ED_JAH         				 0x309
#define PB_NEU         				 0x30A






















