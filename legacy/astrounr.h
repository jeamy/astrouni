#define ID_CANCEL				    0x100
#define	PB_Ok						0x400
#define PB_Abbruch					0x401
#define	PB_Hilfe					0x402
#define PB_Default					0x403
#define TXT_CALC					0x404


//MAINWINDOW
#define CM_U_NEW	                0x100
#define CM_U_FILEOPEN               0x101
#define CM_U_SAVE                   0x102
#define CM_U_SAVEU                  0x103
#define CM_U_PRINT                  0x104
#define CM_U_LAYOUT                 0x105
#define CM_U_PINST                  0x106
#define CM_U_EXIT                   0x107

#define CM_U_PERSON                 0x110
#define CM_U_ORT                    0x111

//#define CM_U_RADIX                  0x121
//#define CM_U_SYNASTRIE              0x122
//#define CM_U_TRANSIT                0x123
//#define CM_U_COMPOSIT               0x124
//#define CM_U_COMBINE                0x125
//#define CM_U_DIREKT                 0x126

#define CM_U_HOROTYP                0x120
#define CM_U_HAUSER                 0x121
#define CM_U_ORBEN	                0x122
#define CM_U_FARBEN	                0x123
#define CM_U_EINST	                0x124

#define CM_U_HELPINDEX              0x1F0
#define CM_U_HELPKEY                0x1F1
#define CM_U_HELPCOM                0x1F2
#define CM_U_HELPPROC               0x1F3
#define CM_U_HELP                   0x1F4
#define CM_U_HELPABOUT              0x1F5

//RADIXWINDOW
#define CM_UR_SAVE					0x200
#define CM_UR_PRINT                 0x201
#define CM_UR_EXIT                  0x202
#define CM_UR_HOROTYP               0x203
#define CM_UR_HELP                  0x210

//TRANSITSUCHE
#define CM_UT_SAVE					0x220
#define CM_UT_PRINT                 0x221
#define CM_UT_EXIT                  0x222
#define CM_UT_HOROTYP               0x223
#define CM_UT_HELP                  0x230
#define PB_UT_GRAF					0x231
#define PB_UT_RUCK					0x232
#define TXT_UT_DATUM				0x233
#define TXT_UT_BDATUM				0x234
#define TXT_UT_TDATUM				0x235
#define TXT_UT_TZEIT				0x236
#define TXT_UT_TBDATUM				0x237
#define TXT_UT_TBZEIT				0x238
#define TXT_UT_WINKEL				0x239
#define LB_UT_TRANS 				0x23A

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
#define PB_TOrt						0x100
#define ETXT_TOrt					0x101
#define TXT_TName					0x102
#define ETXT_TName					0x103
#define TXT_TDatum					0x104
#define ETXT_TDatum					0x105
#define ETXT_TBDatum				0x106
#define TXT_TZeit					0x107
#define ETXT_TZeit					0x108
#define ETXT_TBZeit					0x109
#define RBT_START					0x10a
#define RB_TAG 						0x10a
#define RB_MONAT					0x10b
#define RB_JAHR                     0x10c
#define RBT_END                     0x10d

//Composit
#define ETXT_Person1			    0x300
#define ETXT_Person2			    0x301
#define PB_Person1				    0x303
#define PB_Person2				    0x304

//HausAuswahl
#define RA_KOCH					    100
#define RA_PLACIDUS					101
#define RA_EQUAL 				    102
#define RA_TOPO				   	    105
#define RA_CAMPANUS				    106
#define RA_REGIO			   	    108
#define RA_MORINUS				    111
#define RA_NULL					    113
#define TXT_Haus					117

//HoroAuswahl
#define RA_RADIX     			    100
#define RA_TRANSIT 					101
#define RA_SYNASTRIE			    102
#define RA_COMPOSIT					103
#define TXT_Horo					300
#define TXT_Haussys					301


// Ort Suchen
//#define	CB_O_OrtSuchen				0x330
#define	LB_O_OrtSuchen				0x330
#define	ETXT_O_OrtSuchen			0x331
#define TXT_O_Ortsname				0x332
#define PB_O_Abbruch				0x333
#define PB_O_OK						0x334
#define PB_O_Neu					0x335
#define PB_O_Help					0x336

//Daten Doppelt
#define CM_UDD_LB                   0x500
#define CM_UDD_JA                   0x501
#define CM_UDD_ADD                  0x502
#define CM_UDD_ESC                  0x503
#define CM_UDD_HELP                 0x504
#define CM_UDD_TXT                  0x505

//Person Auswahl
#define CM_UDG_LB                   0x500
#define CM_UDG_OK                   0x501
#define CM_UDG_ESC                  0x502
#define CM_UDG_HELP                 0x503
#define CM_UDG_TXT                  0x504

//Personen Handling
#define LB_PHNamen                  0x500
#define PB_PHLoeschen               0x501
#define TXT_PHAuswahl               0x502

//Farben
#define SB_R						0x300
#define SB_G						0x301
#define SB_B						0x302
#define EDIT_R						0x303
#define EDIT_G						0x304
#define EDIT_B						0x305
#define TXT_ALT						0x306
#define TXT_NEU						0x307
#define CHB_MONO					0x308
#define BL_HS                       0x100
#define BL_HP                       0x200
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
#define RBA_KON                      0x100
#define RBA_HSX                      0x101
#define RBA_SEX                      0x102
#define RBA_QUA                      0x103
#define RBA_TRI                      0x104
#define RBA_QUI                      0x105
#define RBA_OPO                      0x106
#define RBA_3                        0x107
#define RBA_9                        0x108

//TransitAbort

#define TA_VON 						 0x101
#define TA_AKT 						 0x102
#define TA_BIS 						 0x103

#define SC_MIN         				 0x100
#define SC_STU         				 0x101
#define SC_TAG         				 0x102
#define SC_MON         				 0x103
#define SC_JAH         				 0x104
#define ED_MIN         				 0x105
#define ED_STU         				 0x106
#define ED_TAG         				 0x107
#define ED_MON         				 0x108
#define ED_JAH         				 0x109
