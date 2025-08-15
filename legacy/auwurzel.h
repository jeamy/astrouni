/*----------------------------------------------------------------------------*/
/* Autor:               JeaMY  Lee                                            */
/* Datum:               Samstag 27.01.1996                                   */
/*----------------------------------------------------------------------------*/
#define STRICT

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>     
#include <conio.h>     
#include <math.h>
#include <malloc.h>

COLORREF sColor[COLORS]=
               { COLM_HINTER, 
                 COLM_FEUER, COLM_WASSER, COLM_ERDE, COLM_LUFT,
                 COLM_TXT, COLM_RADIX, COLM_ASZ,
                 COLM_PLANET_TIC, COLM_PLANET_TICT, 
				 COLM_HAUSER,
                 CMKONJUNKTION, CMSEXTIL, CMQUADRATUR, CMTRIGON, CMOPOSITION,
                 CMHALBSEX, CMQUINCUNX,
				 COLM_LBTXT_NORM, COLM_LBTXT_SEL, COLM_LBTXT_FOCUS, COLM_LBTXT_BK,
				 COLM_LBREC_NORM, COLM_LBREC_SEL, COLM_LBREC_FOCUS, COLM_LBREC_BK };

COLORREF sSColor[COLORS]=
               { COLM_HINTER, 
				 COLM_FEUER, COLM_ERDE, COLM_LUFT, COLM_WASSER,
                 COLM_TXT, COLM_RADIX, COLM_ASZ,
                 COLM_PLANET_TIC, COLM_PLANET_TICT, 
				 COLM_PLAN, COLM_PLAN_T, COLM_HAUSER,
                 CMKONJUNKTION, CMSEXTIL, CMQUADRATUR, CMTRIGON, CMOPOSITION,
                 CMHALBSEX, CMQUINCUNX,
				 COLM_LBTXT_NORM, COLM_LBTXT_SEL, COLM_LBTXT_FOCUS, COLM_LBTXT_BK,
				 COLM_LBREC_NORM, COLM_LBREC_SEL, COLM_LBREC_FOCUS, COLM_LBREC_BK };

COLORREF sMono[COLORS]=
               { COLMo_HINTER, 
                 COLMo_FEUER, COLMo_ERDE, COLMo_LUFT, COLMo_WASSER,
                 COLMo_TXT, COLMo_RADIX, COLMo_ASZ,
                 COLMo_PLANET_TIC, COLMo_PLANET_TICT, 
				 COLMo_HAUSER,
                 CMoKONJUNKTION, CMoSEXTIL, CMoQUADRATUR, CMoTRIGON, CMoOPOSITION,
                 CMoHALBSEX, CMoQUINCUNX,
				 COLMo_LBTXT_NORM, COLMo_LBTXT_SEL, COLMo_LBTXT_FOCUS, COLMo_LBTXT_BK,
				 COLMo_LBREC_NORM, COLMo_LBREC_SEL, COLMo_LBREC_FOCUS, COLMo_LBREC_BK };

COLORREF sPColor[COLORS]=
			   { COLM_HINTER,
				 COLM_FEUER, COLM_ERDE, COLP_LUFT, COLM_WASSER,
                 COLM_TXT, COLP_RADIX, COLP_ASZ,
				 COLM_PLANET_TIC, COLM_PLANET_TICT,
				 COLM_PLAN, COLM_PLAN_T, COLP_HAUSER,
                 CMKONJUNKTION, CMSEXTIL, CMQUADRATUR, CMTRIGON, CMOPOSITION,
                 CMHALBSEX, CMQUINCUNX,
				 COLM_LBTXT_NORM, COLM_LBTXT_SEL, COLM_LBTXT_FOCUS, COLM_LBTXT_BK,
				 COLM_LBREC_NORM, COLM_LBREC_SEL, COLM_LBREC_FOCUS, COLM_LBREC_BK };

short sAspektTable[ASPEKTE]=
{ KONJUNKTION, SEXTIL, QUADRATUR, TRIGON, OPOSITION, HALBSEX, QUINCUNX };


char cAspektGly[][2]={ "‘", "—", "“", "”", "’", "•", "–"};
//145, 151, 147, 148, 146};

char szQualitaten[][10]={
                 QU_ERDE_TXT,		   
				 QU_WASSER_TXT,
				 QU_FEUER_TXT,
	   			 QU_LUFT_TXT,
				 QU_CARDINAL_TXT,
				 QU_FIX_TXT,
				 QU_BEWEGLICH_TXT,
				 QU_FRAU_TXT,
				 QU_MANN_TXT};



char szLNANamen[][2]={"ª", "«", "¬", "­", "®", "¯",
				 	  "°", "±", "²", "³",
					  "´", "µ", "¶", "·", "¸", "¹", "º" };
//char szLNANamen[][3]={"Q", "R", "S", "T", "U", "V",
//				      "W", "X", "Y", "Z",
//					  "<", "L", "Ch", "C", "P", "J", "V" };

char szLStzNamen[][11]={
						WIDDER_TXT,
						STIER_TXT,
						ZWILLING_TXT,
						KREBS_TXT,
						LOWE_TXT,
						JUNGFRAU_TXT,
						WAAGE_TXT,
						SKORPION_TXT,
						SCHUTZE_TXT,
						STEINBOCK_TXT,
						WASSERMANN_TXT,
						FISCHE_TXT,
						ASZENDENT_TXT,
						HAUS2_TXT,
						HAUS3_TXT,
						HAUS4_TXT,
						HAUS5_TXT,
						HAUS6_TXT,
						HAUS7_TXT,
						HAUS8_TXT,
						HAUS9_TXT,
						HAUS10_TXT,
						HAUS11_TXT,
						HAUS12_TXT};

char szLNamen[][11]={
					 SONNE_TXT,
					 MOND_TXT,
					 MERKUR_TXT,
					 VENUS_TXT,
					 MARS_TXT,
					 JUPITER_TXT,
					 SATURN_TXT,
					 URANUS_TXT,
					 NEPTUN_TXT,
					 PLUTO_TXT,
					 NMKL_TXT,
					 LILITH_TXT,
					 CHIRON_TXT,
					 CERES_TXT,
					 PALLAS_TXT,
					 JUNO_TXT,
					 VESTA_TXT,
					  };

//char szANamen[]={"A", "B", "C", "D", "E", "F",
//					"G", "H", "I", "J", "K", "L" };
char szANamen[][2]={"Å", "Æ", "Ç", "È", "É", "Ê",
			        "Ë", "Ì", "Í", "Î", "Ï", "Ð" };
char szBNamen[][2]={"å", "æ", "ç", "è", "é", "ê",
					"ë", "ì", "í", "î", "ï", "ð" };






