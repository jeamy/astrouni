/*----------------------------------------------------------------------------*/
/* Autor:               JeaMY  Lee                                            */
/* Datum:               Samstag 27.01.1996                                   */
/*----------------------------------------------------------------------------*/

#include "astrouni.h"
#include "auwurzel.h"

#define DRW_LB_SPACE      -1
   
#define DRW_LB_RADIX      -2
#define DRW_LB_TRANSIT    -3
#define DRW_LB_SYNASTRIE  -4
#define DRW_LB_COMPOSIT   -5

#define DRW_LBH_MIN	      -5
#define DRW_LBH_MAX	      -2

#define DRW_LB_HAUS       -6
#define DRW_LB_PLANET     -7
#define DRW_LB_P1         -8
#define DRW_LB_P2         -9

#define DRW_LB_MIN	      -9
#define DRW_LB_MAX		  -6

#define DRW_LBD_TRANSIT  -10
#define DRW_LBD_HAUS     -11
#define DRW_LBD_TDATUM   -12
#define DRW_LBD_DATUM    -13
#define DRW_LBD_ZEIT     -14
#define DRW_LBD_TORT     -15
#define DRW_LBD_TZeit    -16
#define DRW_LBD_ORT      -17
#define DRW_LBD_LANGE    -18
#define DRW_LBD_BREITE   -19
#define DRW_LBD_P1       -20
#define DRW_LBD_P2       -21

#define DRW_LBD_MIN	     -21
#define DRW_LBD_MAX	      -8

#define DRW_LBA_PLANET   -22
#define DRW_LBA_HAUS     -23
#define DRW_LBA_MIN	     -23
#define DRW_LBA_MAX		 -22


char    szLBData[][22]={
						"",						
						"*Radix*",
                        "*Transit*",
						"*Synastrie*",
						"*Composit*",
						"-Häuser-",
						"-Planeten-",
						"-Person1-",
						"-Person2-",
						"-Transitplaneten-",
                        "-Häusersystem-",
                        "Transitdatum",
                        "Geburtsdatum",
                        "Geburtszeit",
                        "Wohnort",
                        "Transitzeit",
                        "Geburtsort",
                        "Länge:",
                        "Breite:",
						"-Person1-",
						"-Person2-",
						"-Planeten-",
						"-Häuser-",
						"-Person1-",
						"-Person2-",
						};



extern  int 	  fOrte;//astrouni.c
//extern  FILE* 	  fDaten;
extern  FILE* 	  fNamen;//astrouni.c
extern  FILE* 	  fNotiz;//astrouni.c
extern  char      szHausTyp[][14]; //auprint.c
//extern	char*	  pszOrte;//aupfil.c
extern  LBCOLOR   lbcolor;//aufil.c
extern  HINSTANCE hInstance;
extern  AU_INIT auinit;
extern  SYS     Sys;
LOGFONT MainFontRec;
LOGFONT AstroFontRec;
short   sMpX, sMpY;
short   sRadius;
short   sDruber;

/*----------------------------------------------------------------------------*/
/* sRadix               Kreise und Gradeinteilung  zeichnen                   */
/*----------------------------------------------------------------------------*/
short sOneAspekt (HDC hDC, RADIX * Radix)
{
 short  sX, sY, sA;
 short  sPa, sPb;
 short  sRH=(short)((double)sRadius*KREIS_ASP);
 char   cD;
 double dA;
 long  dIndex;
 RADIX* plRadix=Radix;
 LOGPEN lpLogPen;
 char   szBuffer[BUF_SIZE];
 WORD   wIndex=SendMessage(Radix->hWndLBLast, LB_GETCURSEL, 0, 0);


 if (wIndex)
	dIndex=SendMessage(Radix->hWndLBLast, LB_GETITEMDATA, (WPARAM)wIndex, 0);

 switch(Radix->cMouse) {
 case MAUS_ZOOM:
	  sZoom(hDC, Radix);
 break;
 case MAUS_L:
 	  if (Radix->hWndLBLast==Radix->hWndLBD||
	 	  dIndex==LB_ERR||!wIndex||
	      dIndex==DRW_LB_PLANET||dIndex==DRW_LB_HAUS||
 		  dIndex==DRW_LBA_PLANET||dIndex==DRW_LBA_HAUS) {
         Radix->cMouse=0;
		 BitBlt(hDC, 0, 0, sMpX*2, sMpY*2, Radix->hDCBit, 0, 0, SRCCOPY);
 	     return ERR_OK;
     } /* endif*/
	 if (Radix->cMouse&MAUS_L) {
		 sA=(short)(sRadius*KREIS_ASP);
		 Ellipse( hDC, sMpX-sA, sMpY-sA, sMpX+sA, sMpY+sA );
         if (Radix->hWndLBLast==Radix->hWndLBA){
		     if (Radix->sLB&LB_A)
				plRadix=(RADIX*)Radix->plRadix[Radix->lRadixCount-1];

			 cD=(char)HIWORD(dIndex&0xFF0000);
             sPa=(short)dIndex&0xFF;
             sPb=HIBYTE(dIndex&0xFF00);
			 sSetAspLine(hDC, sAspektTable[cD], (char)Sys.ScrFactor);
			 dA=plRadix->pdPlanetRad[sPa];
		     sX=sMpX-(short)(cos(dA)*sRH);
		     sY=sMpY+(short)(sin(dA)*sRH);
		     MoveTo(hDC, sX, sY);
	         if (cD==KONJUNKTION) {
				 LineTo(hDC, sMpX, sMpY);
		     }/* endif*/
		     if (dIndex&0xF0000000)
				dA=Radix->pdPlanetRad[sPb];
        	 else
				dA=Radix->pdHausRad[sPb];
			 sX=sMpX-(short)(cos(dA)*sRH);
			 sY=sMpY+(short)(sin(dA)*sRH);                                                
			 LineTo(hDC, sX, sY);
		     if (dIndex&0xF0000000)
				dA=plRadix->pdWinkelPlanet[sPa*Radix->sAnzahlPlanet+sPb];
        	 else
				dA=plRadix->pdWinkelHaus[sPa*MAX_HAUS+sPb];
             sprintf(szBuffer, "%.2fÙ", dA);
		     sPutString( hDC, szBuffer, sMpX, sMpY-AstroFontRec.lfHeight/2,
			 				sColor[COL_LBTXT_NORM]);
	     }/* endif*/
         if (Radix->hWndLBLast==Radix->hWndLBG){
             if (Radix->sLB&LB_G)
				plRadix=(RADIX*)Radix->plRadix[Radix->lRadixCount-1];
			 lpLogPen.lopnStyle=PS_SOLID;
			 lpLogPen.lopnWidth.x=1;
		     MoveTo(hDC, sMpX, sMpY);
             if (HIWORD(dIndex)&P_TYP_NORM)
				lpLogPen.lopnColor=sColor[COL_PLANET_TIC];
             else
				lpLogPen.lopnColor=sColor[COL_PLANET_TICT];
			 DeleteObject(SelectObject(hDC, CreatePenIndirect(&lpLogPen)));
		     if (dIndex&0x1000)
				dA=plRadix->pdPlanetRad[(short)(dIndex&0xEFFF)];
        	 else
				dA=Radix->pdHausRad[dIndex];
		     sX=sMpX-(short)(cos(dA)*sRH);
		     sY=sMpY+(short)(sin(dA)*sRH);
		     LineTo(hDC, sX, sY);
    	 }/* endif*/
     }/* endif*/
     else
		BitBlt(hDC, 0, 0, sMpX*2, sMpY*2, Radix->hDCBit, 0, 0, SRCCOPY);
//	 BitBlt(Radix->hDCBit, 0, 0, 2*sA, 2*sA, hDC, 0, 0, SRCCOPY);
//	 BitBlt(Radix->hDCBit, sMpX-sX, sMpY-sX,
//	 				2*sA, 2*sA, hDC, sMpX-sX, sMpY-sX, SRCCOPY);
   	 Radix->cMouse=0;
 break;

 case MAUS_R:
	 BitBlt(hDC, 0, 0, sMpX*2, sMpY*2, Radix->hDCBit, 0, 0, SRCCOPY);
     Radix->cMouse=0;
 break;

 case MAUS_ASPEKT:
	 sA=(short)((double)sRadius*KREIS_ASP);
	 Ellipse( hDC, sMpX-sA, sMpY-sA, sMpX+sA, sMpY+sA );
	 if (Radix->sLB&LB_A)
		sDrawAspekte(hDC, (RADIX*)Radix->plRadix[Radix->lRadixCount-1], Radix, (char)Sys.ScrFactor);
     else
		sDrawAspekte(hDC, Radix, Radix, (char)Sys.ScrFactor);
//	 BitBlt(Radix->hDCBit, 0, 0,
// 				2*sA, 2*sA, hDC, sMpX-sA, sMpY-sA, SRCCOPY);
     Radix->cMouse=0;
//     Radix->cMouse^=MAUS_ASPEKT;
 break;
 default:
      sZoom(hDC, Radix );
 	  return 0;
 }/*endswitch*/
 return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/* sRadix               Kreise und Gradeinteilung  zeichnen                   */
/*----------------------------------------------------------------------------*/
short sRadix (HDC hDC, RADIX * Radix)
{
 short  sR, sA, sStz, sZ4=0, sZ=0;
 long	dWXC;
 short  sRI, sRHZ, sRG;       ///radien der kreise
 short  sX, sY, sX1, sY1;
 double dA;
 short  yAdd=HIWORD(GetTextExtent(hDC, "Ï", 1))/2;
// LOGPEN lpLogPen;
/*
 lpLogPen.lopnStyle=PS_SOLID;
 lpLogPen.lopnWidth.x=1;
 lpLogPen.lopnColor=sColor[COL_RADIX];
 DeleteObject(SelectObject(hDC, CreatePenIndirect(&lpLogPen)));
*/ 

///äußerster Kreis****************
 Ellipse(hDC, sMpX-sRadius, sMpY-sRadius,
 			  sMpX+sRadius, sMpY+sRadius );

///2. Kreis (STZ)****************
 sR=(short)((double)sRadius*KREIS_STZ);
 sRG=sR;
 Ellipse(hDC, sMpX-sR, sMpY-sR, sMpX+sR, sMpY+sR );
///3. Kreis (10 Grad)****************
 sR=(short)((double)sRadius*KREIS_10_GRAD);
 sRI=sR;
// Ellipse(hDC, sMpX-sR, sMpY-sR, sMpX+sR, sMpY+sR );
 sRHZ=sRadius-(sRadius-sRG)/2;
// +(short)((double)AstroFontRec.lfWidth/2);
///30 Grad Einteilung****************
 dA=Radix->dStzGrad;
 for (sA=0; sA<6; sA++) {
   sX=(short)(cos(dA)*sRI);
   sY=(short)(sin(dA)*sRI);                                                
   MoveTo(hDC, sMpX-sX, sMpY+sY);
   sX1=sX;
   sY1=sY;
   sX=(short)(cos(dA)*sRadius);
   sY=(short)(sin(dA)*sRadius);
   LineTo(hDC, sMpX-sX, sMpY+sY);
   MoveTo(hDC, sMpX+sX1, sMpY-sY1);             //2 Hälfte der. Einteilung
   LineTo(hDC, sMpX+sX, sMpY-sY);

   sStz=Radix->cStzIndex+sA;
   sStz%=12;
   sX=(short)(cos(dA+PI/12.0)*sRHZ);
   sY=(short)(sin(dA+PI/12.0)*sRHZ);
   sPutString( hDC, szBNamen[sStz], sMpX-sX, sMpY+(sY-yAdd), sColor[sStz%4+COL_EL] );
//   sPutString( hDC, szANamen[sStz], sMpX-sX, sMpY+(sY-yAdd), sColor[sStz%4+COL_EL] );
   sStz+=6;
   sStz%=12;
   sPutString( hDC, szBNamen[sStz], sMpX+sX, sMpY-(sY+yAdd), sColor[sStz%4+COL_EL] );
//   sPutString( hDC, szANamen[sStz], sMpX+sX, sMpY-(sY+yAdd), sColor[sStz%4+COL_EL] );
   dA+=PI/6;
 }; /*endfor*/

 AstroFontRec.lfHeight = (short)((double)AstroFontRec.lfHeight/1.6);
 AstroFontRec.lfWidth = (short)((double)AstroFontRec.lfHeight/2.5);
 DeleteObject(SelectObject(hDC, CreateFontIndirect(&AstroFontRec)));
 yAdd=HIWORD(GetTextExtent(hDC, "Ï", 1))/2;
 sRHZ=sRG-(sRG-sRI)/2;
// +(short)((double)AstroFontRec.lfWidth/2);
/* dA=Radix->dStzGrad;
 for (sA=0; sA<18; sA++){
   sX=(short)((cos(dA)*(double)sRG));
   sY=(short)((sin(dA)*(double)sRG));
   MoveTo(hDC, sMpX-sX, sMpY+sY);
   sX1=sX;                    
   sY1=sY;
   sX=(short)((cos(dA)*(double)sRI));
   sY=(short)((sin(dA)*(double)sRI));
   LineTo(hDC, sMpX-sX, sMpY+sY);
   MoveTo(hDC, sMpX+sX1, sMpY-sY1);             //2 Hälfte der. Einteilung
   LineTo(hDC, sMpX+sX, sMpY-sY);

   if (sZ4>11) {
      sZ4=0;
	  sZ=++sZ>5?0:sZ;
   }/* endif 
   sStz=Radix->cStzIndex+sZ+sZ4;
   sStz%=12;
   sZ4+=4;
   sX=(short)(cos(dA+PI/36.0)*sRHZ);
   sY=(short)(sin(dA+PI/36.0)*sRHZ);
   sPutString( hDC, szANamen[sStz], sMpX-sX, sMpY+(sY-yAdd), sColor[sStz%4+COL_EL] );
   sStz+=6;
   sStz%=12;
   sPutString( hDC, szANamen[sStz], sMpX+sX, sMpY-(sY+yAdd), sColor[sStz%4+COL_EL] );

   dA+=PI/18.0;
 }; /*endfor*/
 ///4. Kreis (3.33 Grad)****************

 sRG=sR;
 sR=(short)((double)sRadius*KREIS_3_GRAD);
 sRI=sR;
// Ellipse(hDC, sMpX-sR, sMpY-sR, sMpX+sR, sMpY+sR );
 sRHZ=sRG-(sRG-sRI)/2;
// +(short)((double)AstroFontRec.lfWidth/2);
 dA=Radix->dStzGrad;
// dPl=3.3333*PI/180;
// sZ=sZ4=0;

 /*switch(Radix->cStzIndex){
 case 0:
 case 4:
 case 8:
   sZ=0;
 break;
 case 1:
 case 5:
 case 9:
   sZ=9;
 break;
 case 2:
 case 6:
 case 10:
   sZ=6;
 break;
 case 3:
 case 7:
 case 11:
   sZ=3;
 break;
 }/*endswitch
 for (sA=0; sA<54; sA++){
   sX=(short)((cos(dA)*(double)sRG));
   sY=(short)((sin(dA)*(double)sRG));
   MoveTo(hDC, sMpX-sX, sMpY+sY);
   sX1=sX;
   sY1=sY;
   sX=(short)((cos(dA)*(double)sRI));
   sY=(short)((sin(dA)*(double)sRI));
   LineTo(hDC, sMpX-sX, sMpY+sY);
   MoveTo(hDC, sMpX+sX1, sMpY-sY1);             //2 Hälfte der. Einteilung
   LineTo(hDC, sMpX+sX, sMpY-sY);
/*
   if (sZ4>8) {
      sZ4=0;
	  sZ=++sZ>5?0:sZ;
   }/* endif 
   sStz=sZ;
   sX=(short)(cos(dA+PI/108.0)*sRHZ);
   sY=(short)(sin(dA+PI/108.0)*sRHZ);
   sPutString( hDC, szANamen[sStz], sMpX-sX, sMpY+(sY-yAdd), sColor[sStz%4+COL_EL] );
   sStz+=6;
   sStz%=12;
   sZ++;
   sZ%=12;
   sPutString( hDC, szANamen[sStz], sMpX+sX, sMpY-(sY+yAdd), sColor[sStz%4+COL_EL] );
   dA+=PI/54.0;
 }; /*endfor*/

///5. Kreis (5 Grad)****************
 sRG=sR;
 sR=(short)((double)sRadius*KREIS_5_GRAD);
 sRI=sR;
 Ellipse(hDC, sMpX-sR, sMpY-sR, sMpX+sR, sMpY+sR );           
 ///6. Kreis (Aspekte)****************
 sR=(short)((double)sRadius*KREIS_ASP);
 Ellipse(hDC, sMpX-sR, sMpY-sR, sMpX+sR, sMpY+sR );
///7. Kreis****************
// sR=(short)(sRadius-sRadius*KREIS_KLEIN);
// Ellipse(hDC, sMpX-sR, sMpY-sR, sMpX+sR, sMpY+sR );

///2 (5) Grad Einteilung****************
 dA=Radix->dStzGrad;
 for (sA=0; sA<90; sA++){
   sX=(short)((cos(dA)*(double)sRG));
   sY=(short)((sin(dA)*(double)sRG));
   MoveTo(hDC, sMpX-sX, sMpY+sY);
   sX1=sX;
   sY1=sY;
   sX=(short)((cos(dA)*(double)sRI));
   sY=(short)((sin(dA)*(double)sRI));
   LineTo(hDC, sMpX-sX, sMpY+sY);
   MoveTo(hDC, sMpX+sX1, sMpY-sY1);             //2 Hälfte der. Einteilung
   LineTo(hDC, sMpX+sX, sMpY-sY);
   dA+=PI/90.0;
 }; /*endfor*/

 return ERR_OK;
};

/*----------------------------------------------------------------------------*/
/* sHaus:               Häusersystem zeichnen                                 */
/*----------------------------------------------------------------------------*/
short sHaus (HDC hDC, RADIX * Radix, char cPen )
{
 short  sA;
 short  sRD, sRHx, sRG=(short)((double)sRadius*KREIS_STZ);
 short  sRH=(short)((double)sRadius*KREIS_ASP);
 short  sX, sY;
 double dA, dDezGrad;
 LOGPEN lpLogPen;
 short  yAdd=LOWORD(GetTextExtent(hDC, "¢", 1));
 //short  xAdd=HIWORD(GetTextExtent(hDC, "¢¢¢", 3))/2;
 char 	szNum[][4]={ "¢¦¥", "„", "…", "¤¥", "‡", "ˆ", "‰",
					 "Š", "‹", "£¥", "ƒƒ", "ƒ„" };
// short  xAdd=(short)(GetTextExtent(hDC, "a", 1)&0xFFFF);
// short  yAdd=(short)(GetTextExtent(hDC, "a", 1)>>16)/2;
// char 	szNum[][4]={ "a", "2", "3", "c", "5", "6", "7",
//					 "8", "9", "b", "11", "12" };


 lpLogPen.lopnStyle=PS_SOLID;
 lpLogPen.lopnWidth.x=1;
 lpLogPen.lopnColor=sColor[COL_RADIX];
 DeleteObject(SelectObject(hDC, CreatePenIndirect(&lpLogPen)));

///Häuser**************** 
 sRD=sRadius+sDruber;
// sRHx=sRH+xAdd;
 for (sA=0; sA<12; sA++){
   dA=Radix->pdHausRad[sA];
   sRHx=sRH+HIWORD(GetTextExtent(hDC, szNum[sA], strlen(szNum[sA])))/1.5;
   sX=sMpX-(short)(cos(dA+0.1)*sRHx);
   sY=sMpY+(short)(sin(dA+0.1)*sRHx);
   sPutString( hDC, szNum[sA], sX, sY-2*yAdd, sColor[COL_HAUSER]);
//   sPutString( hDC, szNum[sA], sX, sY-2*yAdd, RGB(0,0,0) );

   sX=sMpX-(short)(cos(dA)*sRH);
   sY=sMpY+(short)(sin(dA)*sRH);                                                
   MoveTo(hDC, sX, sY);
   if ( sA==0 || sA==3 || sA==6 || sA==9  ) {
     lpLogPen.lopnColor=sColor[COL_ASZ];
	 lpLogPen.lopnWidth.x=cPen;
	 DeleteObject(SelectObject(hDC, CreatePenIndirect(&lpLogPen)));
	 sX=sMpX-(short)(cos(dA)*sRD);
     sY=sMpY+(short)(sin(dA)*sRD);
	 LineTo(hDC, sX, sY);
	 lpLogPen.lopnColor=sColor[COL_RADIX];
	 lpLogPen.lopnWidth.x=1;
	 DeleteObject(SelectObject(hDC, CreatePenIndirect(&lpLogPen)));
   } /*endif*/
   else {
     sX=sMpX-(short)(cos(dA)*sRG);
     sY=sMpY+(short)(sin(dA)*sRG);
	 LineTo(hDC, sX, sY);
   } /*enelse*/
 }; /*endfor*/

 return ERR_OK;
}

/*----------------------------------------------------------------------------*/
/* sPlanet              Planetenpositionen zeichnen                           */
/*----------------------------------------------------------------------------*/
short sPlanet (HDC hDC, RADIX * Radix, char cAbstand)
{
//cAbstand nicht mehr notwendig...

 char     *szPlFlag;                   //Anzahl d. Plan. pro Stz.
 char     *szPlGrad;
 long     lRadix=Radix->lRadixCount;
 short    sAnzahl=Radix->sAnzahlPlanet;
 short    yAdd=HIWORD(GetTextExtent(hDC, "ª", 1))/2;
 RADIX*   plRadix;

 cAbstand=(short)((double)yAdd*1.5);
 lRadix=Radix->lRadixCount;
 while (lRadix--)
    sAnzahl+=Radix->sAnzahlPlanet;

 szPlFlag=(char*)calloc(sAnzahl, 1);
 if ( !szPlFlag)
   return ERR_MEM;

 szPlGrad=(char*)calloc(360, 1);
 if ( !szPlGrad)
   return ERR_MEM;

 lRadix=Radix->lRadixCount;
 vSetFlag(Radix, szPlFlag, szPlGrad, 0);
 sAnzahl=0;
 while (lRadix--){
    plRadix=(RADIX*)Radix->plRadix[lRadix];
    sAnzahl+=plRadix->sAnzahlPlanet;
    vSetFlag(plRadix, szPlFlag, szPlGrad, sAnzahl);
 } /*endwhile*/

 vPlanetTicDraw (hDC, Radix, szPlFlag, cAbstand, 0);
 lRadix=Radix->lRadixCount;
 sAnzahl=0;
 while (lRadix--){
    plRadix=(RADIX*)Radix->plRadix[lRadix];
	sAnzahl+=plRadix->sAnzahlPlanet;
    vPlanetTicDraw (hDC, plRadix, szPlFlag, cAbstand, sAnzahl);
 } /*endwhile*/

 vPlanetDraw (hDC, Radix, szPlFlag, cAbstand, 0);
 lRadix=Radix->lRadixCount;
 sAnzahl=0;
 while (lRadix--){
    plRadix=(RADIX*)Radix->plRadix[lRadix];
    sAnzahl+=plRadix->sAnzahlPlanet;
	vPlanetDraw (hDC, plRadix, szPlFlag, cAbstand, sAnzahl);
 } /*endwhile*/

 free(szPlGrad);
 free(szPlFlag);
 return ERR_OK;
};

/*----------------------------------------------------------------------------*/
/* vPlanetTicDraw          Planetenpositionen zeichnen                        */
/*----------------------------------------------------------------------------*/
void vPlanetTicDraw (HDC hDC, RADIX * Radix, char* szPlFlag,
				  char cAbstand, short sAnzahl)
{
 double   dA, dDezGrad;
 short    sRH=(short)((double)sRadius*KREIS_ASP);
 short    sA, sB, sC, sRP, sRPi, sRPit, sRHPlus=sRH+cAbstand/3;
 short    sX, sY;
 COLORREF sc;
 COLORREF scOld=RGB(0,0,0);
 short    sAnzahlPlanet=Radix->sAnzahlPlanet;
 LOGPEN   lpLogPen;
 char     szBuffer[BUF_SIZE];

 lpLogPen.lopnStyle=PS_SOLID;
 lpLogPen.lopnWidth.x=1;

 sRP=(short)((double)sRadius*KREIS_STZ);
 sRPi=(short)((double)sRadius*KREIS_PLANET);

 sA=0;
 do {
   if (Radix->psPlanetTyp[sA]&P_TYP_NORM)
 	  sc=sColor[COL_PLANET_TIC];
   else
 	  sc=sColor[COL_PLANET_TICT];
   if (sc!=scOld) {
     scOld=sc;
     lpLogPen.lopnColor=(long)sc;
 	 DeleteObject(SelectObject(hDC, CreatePenIndirect(&lpLogPen)));
   } /* endif */
   dA=Radix->pdPlanetRad[sA];
   sRPit=sRPi-(szPlFlag[sAnzahl+sA]*cAbstand);
   sX=sMpX-(short)(cos(dA)*sRP);
   sY=sMpY+(short)(sin(dA)*sRP);                                                
   MoveTo(hDC, sX, sY);
   sX=sMpX-(short)(cos(dA)*sRPit);
   sY=sMpY+(short)(sin(dA)*sRPit);                                                
   LineTo(hDC, sX, sY);
   sX=sMpX-(short)(cos(dA)*sRH);
   sY=sMpY+(short)(sin(dA)*sRH);                                                
   MoveTo(hDC, sX, sY);
   sX=sMpX-(short)(cos(dA)*sRHPlus);
   sY=sMpY+(short)(sin(dA)*sRHPlus);                                                
   LineTo(hDC, sX, sY);
 } while (++sA<sAnzahlPlanet); /*endwhile*/

 lpLogPen.lopnColor=sColor[COL_RADIX];
 DeleteObject(SelectObject(hDC, CreatePenIndirect(&lpLogPen)));

}
/*----------------------------------------------------------------------------*/
/* vPlanetDraw          Planetenpositionen zeichnen                           */
/*----------------------------------------------------------------------------*/
void vPlanetDraw (HDC hDC, RADIX * Radix, char* szPlFlag,
				  char cAbstand, short sAnzahl)
{
 double   dA, dDezGrad;
 short    sA, sB, sC, sRPi, sRPit;
 short    sX, sY;
 COLORREF sc;
 short    sAnzahlPlanet=Radix->sAnzahlPlanet;
 LOGPEN   lpLogPen;
 short    yAdd=HIWORD(GetTextExtent(hDC, "ª", 1))/2;
 char     szBuffer[BUF_SIZE];
 long     dBM, dBC;
// short    yAdd=(short)(GetTextExtent(hDC, "Q", 1)>>16)/2;

 sRPi=(short)((double)sRadius*KREIS_PLANET);

 sA=0;
 do {
   dA=Radix->pdPlanetRad[sA];
   sRPit=sRPi-(szPlFlag[sAnzahl+sA]*cAbstand);
   if ( (Radix->psPlanetTyp[sA]&P_TYP_RUCK) )
 	 sprintf(szBuffer,"œ%s", szLNANamen[sA]);
   else
  	 strcpy(szBuffer, szLNANamen[sA]);
   sX=sMpX-(short)(cos(dA)*sRPit);
   sY=sMpY+(short)(sin(dA)*sRPit);
   if (Radix->psPlanetTyp[sA]&P_TYP_NORM){
	  sc=sColor[COL_PLAN];
   } else {
	  sc=sColor[COL_PLAN_T];
	  if (Sys.ScrCol<3||(Sys.PrnCol<3&&Sys.Status==SYS_PRN))
		  if (Radix->psPlanetTyp[sA]&P_TYP_SYNASTRIE)
			 strcat(szBuffer,"¦");
		  else
         	 strcat(szBuffer,"¡");
   } /* endelse*/
   sPutString( hDC, szBuffer, sX, sY-yAdd, sc);
 } while (sA++<P_NKNOTEN); /*endwhile*/

 do {
   dA=Radix->pdPlanetRad[sA];
   sRPit=sRPi-(szPlFlag[sAnzahl+sA]*cAbstand);
   if ( (Radix->psPlanetTyp[sA]&P_TYP_RUCK) )
 	 sprintf(szBuffer,"œ%s", szLNANamen[sA]);
   else
  	 strcpy(szBuffer, szLNANamen[sA]);
   sX=sMpX-(short)(cos(dA)*sRPit);
   sY=sMpY+(short)(sin(dA)*sRPit);
   if (Radix->psPlanetTyp[sA]&P_TYP_NORM){
	  sc=sColor[COL_PLAN];
   } else {
	  sc=sColor[COL_PLAN_T];
	  if (Sys.ScrCol<3||(Sys.PrnCol<3&&Sys.Status==SYS_PRN))
		  if (Radix->psPlanetTyp[sA]&P_TYP_SYNASTRIE)
			 strcat(szBuffer,"¦");
		  else
         	 strcat(szBuffer,"¡");
   } /* endelse*/
   sPutString( hDC, szBuffer, sX, sY-yAdd, sc);
 } while (++sA<sAnzahlPlanet); /*endwhile*/

}
/*----------------------------------------------------------------------------*/
/* vSetFlag           abstand planeten von STZ-Kreis in Grafik               */
/*----------------------------------------------------------------------------*/

void vSetFlag(RADIX* Radix, char * szPlFlag, char * szPlGrad, short sPlanet)
{

 short sC, sD, sX, sA, sB, sE;
 #define GMIN -2
 #define GMAX  3

 for (sX=0; sX<Radix->sAnzahlPlanet; sX++){
	 sA=(short)floor(Radix->pdPlanet[sX]+.5);
     sE=0;
     for (sC=GMIN; sC<GMAX; sC++) {
      	  sD=sA+sC;
          sD=sD<0?360+sD:sD;
          sD=sD>359?sD-360:sD;
          sB=szPlGrad[sD];
          szPlGrad[sD]++;
 	      if (sB>szPlFlag[sX+sPlanet]) {
             szPlFlag[sX+sPlanet]=sB;
             sE=sB+1;
          }/* endif */
	 } /*endfor*/
     if (sE){
	     for (sC=GMIN; sC<GMAX; sC++) {
	      	  sD=sA+sC;
    	      sD=sD<0?360+sD:sD;
	          sD=sD>359?sD-360:sD;
			  szPlGrad[sD]=sE;
	     } /* endfor */
     } /* endif */
 } /* endfor*/

}
/*----------------------------------------------------------------------------*/
/* sDrawText           texte in LB                                           */
/*----------------------------------------------------------------------------*/
short sDrawText(RADIX * Radix, RADIX* RadixV, DRAWITEMSTRUCT* Di, char * szBuffer, char cFlag)
{
 short sA, sB;
 char  cD, cX;
 short sGrad, sMinuten, sSekunden;
 long  dIndex;
 char  szBuf[BUF_SIZE];
 RADIX* plRadix;

 switch (cFlag) {
 case DRW_LB_BUFFER:
	  dIndex=Di->itemData;
      if (dIndex>=DRW_LB_MIN&&dIndex<=DRW_LB_MAX){
          strcpy(szBuffer, szLBData[(dIndex^0xFFFFFFFF)]);
	      lbcolor.Pen=sColor[COL_LBREC_BK];
          lbcolor.Brush=sColor[COL_LBREC_BK];
          lbcolor.Text=sColor[COL_LBTXT_BK];
		  break;
      } /* endif */
      vSetColor(Di);
      if (dIndex&0x1000){
		  vGrad2GMS(dDeg2Dez(Radix->pdPlanet[(short)(dIndex&0xEFFF)]), &sGrad, &sMinuten, &sSekunden);
		  sprintf(szBuffer,"%-2s%02dÙ%02d'%02d\" %2s", szLNANamen[(short)(dIndex&0xEFFF)],
					 sGrad, sMinuten, sSekunden, szBNamen[Radix->pszStzPlanet[(short)(dIndex&0xEFFF)]] );
//		  sprintf(szBuffer,"%-2s%02dÙ%02d'%02d\" %2s", szLNANamen[(short)(dIndex&0xEFFF)],
//	                 sGrad, sMinuten, sSekunden, szANamen[Radix->pszStzPlanet[(short)(dIndex&0xEFFF)]] );
		  break;
  	  }else{
	                  vGrad2GMS(dDeg2Dez(Radix->pdHaus[(short)dIndex]), &sGrad, &sMinuten, &sSekunden);
          sprintf(szBuffer,"Ñ%02d %02dÙ%02d'%02d\" %2s", (short)dIndex+1,
			    sGrad, sMinuten, sSekunden, szBNamen[Radix->pszStzHaus[(short)dIndex]] );
//		  sprintf(szBuffer,"Ñ%02d %02dÙ%02d'%02d\" %2s", (short)dIndex+1,
//			    sGrad, sMinuten, sSekunden, szANamen[Radix->pszStzHaus[(short)dIndex]] );
	  }/* endelse*/
 break;
 case DRW_LB_INDEX:
	 if (auinit.sSelHoro==TYP_SYNASTRIE)
	    if (Radix->sLB&LB_G)
		   SendMessage(Radix->hWndLBG, LB_ADDSTRING, 0, (LPARAM)DRW_LB_P2);
		else
		   SendMessage(Radix->hWndLBG, LB_ADDSTRING, 0, (LPARAM)DRW_LB_P1);
	 SendMessage(Radix->hWndLBG, LB_ADDSTRING, 0, (LPARAM)DRW_LB_PLANET);
     for (sA=0; sA<Radix->sAnzahlPlanet; sA++){
		 SendMessage(Radix->hWndLBG, LB_ADDSTRING, 0,
		 	(LPARAM)((long)sA|0x1000|(long)Radix->psPlanetTyp[sA]<<16));
	 } /*endfor*/
	 if ( Radix->lPaintFlag&DRW_HAUS&&
		  Radix->cHoroTyp!=TYP_TRANSIT) {
		SendMessage(Radix->hWndLBG, LB_ADDSTRING, 0, (LPARAM)DRW_LB_HAUS);
	 	for (sA=0; sA<12; sA++){
 			 SendMessage(Radix->hWndLBG, LB_ADDSTRING, 0, (LPARAM)sA);
		 } /* endfor */
	 } /* endif */

 break;

 case DRW_LBD_BUFFER:
	  dIndex=Di->itemData;
      if ((dIndex>=DRW_LBD_MIN&&dIndex<=DRW_LBD_MAX)||
          (dIndex>=DRW_LBH_MIN&&dIndex<=DRW_LBH_MAX)){
          strcpy(szBuffer, szLBData[(dIndex^0xFFFFFFFF)]);
	      lbcolor.Pen=sColor[COL_LBREC_BK];
          lbcolor.Brush=sColor[COL_LBREC_BK];
          lbcolor.Text=sColor[COL_LBTXT_BK];
		  break;
      } /* endif */
      vSetColor(Di);
      switch (dIndex){
      case 1:
		  strcpy(szBuffer, szHausTyp[Radix->cHausSys]);
      break;
      case 2:
		  strcpy(szBuffer, Radix->szDatum);
      break;
      case 3:
		 sprintf(szBuffer, "%s Ortszeit", Radix->szZeit);
      break;
      case 4:
		 sprintf(szBuffer, "%s", Radix->pszOrt);
	  break;
      case 5:
		 vGrad2Str(Radix->rFix.dLange, szBuf, 1);
		 sprintf(szBuffer, "Länge: %s", szBuf);
      break;
      case 6:
         vGrad2Str(Radix->rFix.dBreite, szBuf, 0);
		 sprintf(szBuffer, "Breite: %s", szBuf);
      break;
      case 7:
		  plRadix=(RADIX*)Radix->plRadix[Radix->lRadixCount-1];
		  strcpy(szBuffer, plRadix->szDatum);
      break;
      case 8:
		  plRadix=(RADIX*)Radix->plRadix[Radix->lRadixCount-1];
	 	  sprintf(szBuffer, "%s Ortszeit", plRadix->szZeit);
      break;
      case 9:
		  plRadix=(RADIX*)Radix->plRadix[Radix->lRadixCount-1];
	  	  sprintf(szBuffer, "%s", plRadix->pszOrt);
      break;
      case 10:
		  plRadix=(RADIX*)Radix->plRadix[Radix->lRadixCount-1];
//		  sprintf(szBuffer, "Länge: %.2f", plRadix->rFix.dLange);
		  vGrad2Str(plRadix->rFix.dLange, szBuf, 1);
 		  sprintf(szBuffer, "Länge: %s", szBuf);
	  break;
	  case 11:
 	   plRadix=(RADIX*)Radix->plRadix[Radix->lRadixCount-1];
//    	  sprintf(szBuffer, "Breite: %.2f", plRadix->rFix.dBreite);
		  vGrad2Str(plRadix->rFix.dBreite, szBuf, 0);
     	  sprintf(szBuffer, "Breite: %s", szBuf);
      break;
      } /*endswitch*/
 break;
 case DRW_LBD_INDEX:
	 if (auinit.sSelHoro==TYP_SYNASTRIE)
		SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)(auinit.sSelHoro+1)^0xFFFFFFFF);
     else
		SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)(Radix->cHoroTyp+1)^0xFFFFFFFF);
	 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)DRW_LBD_HAUS);
	 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)1);
	 if (auinit.sSelHoro==TYP_SYNASTRIE)
		 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)DRW_LBD_P1);
	 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)DRW_LBD_DATUM);
	 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)2);
	 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)DRW_LBD_ZEIT);
	 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)3);
	 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)DRW_LBD_ORT);
	 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)4);
	 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)5);
     SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)6);
	 if (Radix->lRadixCount) {
		 plRadix=(RADIX*)Radix->plRadix[Radix->lRadixCount-1];
		 switch (plRadix->cHoroTyp) {
		 case TYP_TRANSIT:
			 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)(plRadix->cHoroTyp+1)^0xFFFFFFFF);
			 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)DRW_LBD_TDATUM);
			 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)7);
			 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)DRW_LBD_TZeit);
			 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)8);
			 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)DRW_LBD_TORT);
			 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)9);
			 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)10);
			 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)11);
    	 break;
		 case TYP_SYNASTRIE:
			 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)DRW_LBD_P2);
			 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)DRW_LBD_DATUM);
			 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)7);
			 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)DRW_LBD_ZEIT);
			 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)8);
			 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)DRW_LBD_ORT);
			 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)9);
			 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)10);
			 SendMessage(Radix->hWndLBD, LB_ADDSTRING, 0, (LPARAM)11);
		 break;
		 } /* endswitch*/
     } /* endif*/
 break;

 case DRW_LBA_BUFFER:
	  dIndex=Di->itemData;
      if (dIndex>=DRW_LBA_MIN&&dIndex<=DRW_LBA_MAX){
          strcpy(szBuffer, szLBData[(dIndex^0xFFFFFFFF)]);
	      lbcolor.Pen=sColor[COL_LBREC_BK];
          lbcolor.Brush=sColor[COL_LBREC_BK];
          lbcolor.Text=sColor[COL_LBTXT_BK];
		  break;
      } /* endif */
      vSetColor(Di);
      if (dIndex&0xF0000000)
		   sprintf(szBuffer,"%s in %s %s %s in %s",
		   		   szLNANamen[(short)(dIndex&0xFF)],
				   szBNamen[Radix->pszStzPlanet[(short)(dIndex&0xFF)]],
//				   szANamen[Radix->pszStzPlanet[(short)(dIndex&0xFF)]],
				   cAspektGly[HIWORD(dIndex&0xFF0000)],
				   szLNANamen[HIBYTE(dIndex&0xFF00)],
				   szBNamen[RadixV->pszStzPlanet[HIBYTE(dIndex&0xFF00)]] );
//				   szANamen[RadixV->pszStzPlanet[HIBYTE(dIndex&0xFF00)]] );

        else
		   sprintf(szBuffer,"%s in %s %s Ñ%02d in %s",
		   		   szLNANamen[(short)(dIndex&0xFF)],
				   szBNamen[Radix->pszStzPlanet[(short)(dIndex&0xFF)]],
//				   szANamen[Radix->pszStzPlanet[(short)(dIndex&0xFF)]],
				   cAspektGly[HIWORD(dIndex&0xFF0000)],
				   (short)((dIndex&0xFF00)>>8)+1,
				   szBNamen[RadixV->pszStzHaus[HIBYTE(dIndex&0xFF00)]] );
//				   szANamen[RadixV->pszStzHaus[HIBYTE(dIndex&0xFF00)]] );
	  break;

 case DRW_LBA_INDEX:
	 dIndex=SendMessage(Radix->hWndLBA, LB_ADDSTRING, 0, (LPARAM)DRW_LBA_PLANET);
     cX=0;
	 for (sA=0; sA<Radix->sAnzahlPlanet; sA++ ) {
 	     sB=(Radix->cHoroTyp==TYP_RADIX)?sA+1:0;
		 for (; sB<Radix->sAnzahlPlanet; sB++ ) {
             cD=Radix->pszAspPlanet[sA*Radix->sAnzahlPlanet+sB];
	  	     if (cD!=KEIN_ASP) {
				 SendMessage(Radix->hWndLBA, LB_ADDSTRING, 0,
				 		(LPARAM)((long)sA|((long)sB<<8)|((long)cD<<16)|
						(Radix->psPlanetTyp[sA]&P_TYP_NORM?
						DRW_LBA_PLANETA:DRW_LBA_PLANETT)));
                 cX++;
			 };/*endif */
	     }; /*endfor*/
     }; /*endfor*/
     if (!cX)
 		SendMessage(Radix->hWndLBA, LB_DELETESTRING, (WPARAM)dIndex, 0);

	 dIndex=SendMessage(Radix->hWndLBA, LB_ADDSTRING, 0, (LPARAM)DRW_LBA_HAUS);
     cX=0;
	 for (sA=0; sA<Radix->sAnzahlPlanet; sA++ ) {
    	 for (sB=0; sB<MAX_HAUS; sB++ ) {
             cD=Radix->pszAspHaus[sA*MAX_HAUS+sB];
	       	 if (cD!=KEIN_ASP) {
				 SendMessage(Radix->hWndLBA, LB_ADDSTRING, 0,
				 		(LPARAM)((long)sA|((long)sB<<8)|((long)cD<<16)|
						(Radix->psPlanetTyp[sA]&P_TYP_NORM?
						DRW_LBA_HAUSA:DRW_LBA_HAUST)));
                 cX++;
	   		 };/*endif */
	     }; /*endfor*/
	 }; /*endfor*/
     if (!cX)
		SendMessage(Radix->hWndLBA, LB_DELETESTRING, (WPARAM)dIndex, 0);
 break;
 }/* endswitch*/

return ERR_OK;
}

/*----------------------------------------------------------------------------*/
/* sDrawAspekte         Aspekte zeichnen                                      */
/*----------------------------------------------------------------------------*/
short sDrawAspekte(HDC hDC, RADIX * Radix, RADIX* RadixV, char cPen  )
{
 short   sA, sB, sX, sY;
 double  dA;
 char    cD;
 short   sAnzahlPlanet=Radix->sAnzahlPlanet;
 short   sRH=(short)((double)sRadius*KREIS_ASP);

 for (sA=0; sA<Radix->sAnzahlPlanet; sA++ ) {
   sB=(Radix->cHoroTyp==TYP_RADIX)?sA+1:0;
   for (; sB<Radix->sAnzahlPlanet; sB++ ) {
       cD=Radix->pszAspPlanet[sA*sAnzahlPlanet+sB];
	   if (cD!=KEIN_ASP) {
		 sSetAspLine(hDC, sAspektTable[cD], cPen);
		 dA=Radix->pdPlanetRad[sA];
         sX=sMpX-(short)(cos(dA)*sRH);
         sY=sMpY+(short)(sin(dA)*sRH);                                                
		 MoveTo(hDC, sX, sY);
		 dA=RadixV->pdPlanetRad[sB];
         sX=sMpX-(short)(cos(dA)*sRH);
         sY=sMpY+(short)(sin(dA)*sRH);                                                
		 LineTo(hDC, sX, sY);
	   };/*endif*/
	};/*endfor*/
  };/*endfor*/
 
 if ( Radix->lPaintFlag&DRW_HAUS ) {
   for (sA=0; sA<sAnzahlPlanet; sA++ ) {
	 for (sB=0; sB<MAX_HAUS; sB++ ) {
         cD=Radix->pszAspHaus[sA*MAX_HAUS+sB];
		 if (cD!=KEIN_ASP) {
		   sSetAspLine(hDC, sAspektTable[cD], cPen);
		   dA=Radix->pdPlanetRad[sA];;
           sX=sMpX-(short)(cos(dA)*sRH);
           sY=sMpY+(short)(sin(dA)*sRH);                                                
		   MoveTo(hDC, sX, sY);
		   dA=RadixV->pdHausRad[sB];
           sX=sMpX-(short)(cos(dA)*sRH);
		   sY=sMpY+(short)(sin(dA)*sRH);
		   LineTo(hDC, sX, sY);
         };/*endif*/
	 };/*endfor*/
   };/*endfor*/
 };/*endif*/

 return ERR_OK;
};

short sSetAspLine(HDC hDC, short cD, char cPen)
{
 LOGPEN      lpLogPen;

 switch(cD) {
 case KONJUNKTION:
	 lpLogPen.lopnStyle=PS_SOLID;
	 lpLogPen.lopnColor=sColor[CKONJUNKTION];
   break;
  case HALBSEX:
	 lpLogPen.lopnStyle=PS_DOT;
	 lpLogPen.lopnColor=sColor[CHALBSEX];
	break;
  case SEXTIL:
	 lpLogPen.lopnStyle=PS_DASH;
	 lpLogPen.lopnColor=sColor[CSEXTIL];
	break;
  case QUADRATUR:
	 lpLogPen.lopnStyle=PS_SOLID;
	 lpLogPen.lopnColor=sColor[CQUADRATUR];
  break;
  case TRIGON:
	 lpLogPen.lopnStyle=PS_DASH;
	 lpLogPen.lopnColor=sColor[CTRIGON];
	 break;
  case QUINCUNX:
	 lpLogPen.lopnStyle=PS_DOT;
	 lpLogPen.lopnColor=sColor[CQUINCUNX];
     break;
  case OPOSITION:
	 lpLogPen.lopnStyle=PS_SOLID;
	 lpLogPen.lopnColor=sColor[COPOSITION];
  break;
  default:
	 lpLogPen.lopnStyle=PS_SOLID;
	 lpLogPen.lopnColor=RGB(0,0,0);
  break;
 };/*endswitch*/

 lpLogPen.lopnWidth.x=cPen;
 DeleteObject(SelectObject(hDC, CreatePenIndirect(&lpLogPen)));
 return ERR_OK;
}


/*----------------------------------------------------------------------------*/
/* sCalcAsp             Aspekte-Brechnen                                      */
/*----------------------------------------------------------------------------*/
void vCalcAsp( RADIX * Radix, short sTyp )
{
 short   sA, sB, sC;
 short   sAnzahlPlanet=Radix->sAnzahlPlanet;
 float*  pfOH;
 float*  pfOP;

 switch ( sTyp ) {
 case TYP_TRANSIT:
	pfOH=auinit.pfOrbenTHaus;
	pfOP=auinit.pfOrbenTPlanet;
 break;
 case TYP_SYNASTRIE:
	pfOH=auinit.pfOrbenSHaus;
	pfOP=auinit.pfOrbenSPlanet;
 break;
 default:
    pfOH=auinit.pfOrbenHaus;
	pfOP=auinit.pfOrbenPlanet;
 break;
 }/*endswitch*/

 for (sA=0; sA<sAnzahlPlanet; sA++ ) {
	 for (sB=0; sB<sAnzahlPlanet; sB++ ) {
		 if ((sA==sB)&&(sTyp==TYP_RADIX)) {
	 	    Radix->pszAspPlanet[sA*sAnzahlPlanet+sB]=KEIN_ASP;
 	 		continue;
         } /*endif */
		 for (sC=0; sC<ASPEKTE; sC++ ) {
			 if (fabs(Radix->pdWinkelPlanet[sA*sAnzahlPlanet+sB]-(double)sAspektTable[sC])<=
			 	 pfOP[(sA*MAX_PLANET+sB)+sC*MAX_PLANET*MAX_PLANET]){
			   Radix->pszAspPlanet[sA*sAnzahlPlanet+sB]=sC;
               break;
             }
			 else
			   Radix->pszAspPlanet[sA*sAnzahlPlanet+sB]=KEIN_ASP;
	   	}; /*endfor*/
   	}; /*endfor*/
 }; /*endfor*/

 for (sA=0; sA<sAnzahlPlanet; sA++ ) {
	 for (sB=0; sB<MAX_HAUS; sB++ ) {
		 for (sC=0; sC<ASPEKTE; sC++ ) {
			 if (fabs(Radix->pdWinkelHaus[sA*MAX_HAUS+sB]-(double)sAspektTable[sC])<=
				 pfOH[(sA*MAX_HAUS+sB)+sC*MAX_PLANET*MAX_HAUS]){
			   Radix->pszAspHaus[sA*MAX_HAUS+sB]=sC;
               break;
             }
             else
			   Radix->pszAspHaus[sA*MAX_HAUS+sB]=KEIN_ASP;
		 }; /*endfor*/
	 }; /*endfor*/
 }; /*endfor*/
}

/*----------------------------------------------------------------------------*/
/* sCalcWinkel          Winkel-Brechnen                                       */
/*----------------------------------------------------------------------------*/
void vCalcWinkel( RADIX * Radix, RADIX* RadixV, short sTyp  )
{
 short   sA, sB;
 double  dA, dB;
 short   sAnzahlPlanet=Radix->sAnzahlPlanet;

 Radix->rFix.slRadix+=sAnzahlPlanet*MAX_HAUS*sizeof(double);
 for (sA=0; sA<sAnzahlPlanet; sA++ ) {
     dA=Radix->pdPlanet[sA];
  	 for (sB=0; sB<MAX_HAUS; sB++ ) {
		 dB=RadixV->pdHaus[sB];
	     dB=fabs(dA-dB);                                                
		 if (dB>209)
			dB=360-dB;
		 Radix->pdWinkelHaus[sA*MAX_HAUS+sB]=dB;
	 }; /*endfor*/
 }; /*endfor*/

 Radix->rFix.slRadix+=sAnzahlPlanet*sAnzahlPlanet*sizeof(double);

 for (sA=0; sA<sAnzahlPlanet; sA++ ) {
   dA=Radix->pdPlanet[sA];
   for (sB=0; sB<sAnzahlPlanet; sB++ ) {
	 if ((sA==sB)&&(sTyp==TYP_RADIX)) {
	   continue;
     } /* endif*/
	 dB=RadixV->pdPlanet[sB];
	 dB=fabs(dA-dB);
     if (dB>209)
	   dB=360-dB;
	 Radix->pdWinkelPlanet[sA*sAnzahlPlanet+sB]=dB;
   }; /*endfor*/
 }; /*endfor*/
};

/*----------------------------------------------------------------------------*/
/* sDrawRad             externer Aufruf für Radix zeichnen                    */
/*----------------------------------------------------------------------------*/
short sDrawRad(RADIX * Radix)
{ 
 char    	 szBuffer[BUF_SIZE];
 short 		 sRet, sKb;
 short 		 sTy, sTx1;
 RADIX *     plRadix;
 PAINTSTRUCT PaintInfo;
 LOGPEN      lpLogPen;
 HDC         hDC, hDCBit;
 HANDLE      hFont;
 HANDLE 	 hBrush;
 HANDLE 	 hPen;
 HANDLE	     hBMP;
 WORD 	     wY;
 RECT		 rect;
 short 		 sSub;

/*if (Radix->lPaintFlag&DRW_RADIX) {                  //Repaint Radix
	DeleteMetaFile(Radix->hMetaFile);
	Radix->hMetaFile=0;
	Radix->lPaintFlag^=DRW_RADIX;
} /* endif */

/*if ( Radix->hMetaFile ) {
	BeginPaint(Radix->hWnd, &PaintInfo);
    hDC = PaintInfo.hdc;
	PlayMetaFile(hDC, Radix->hMetaFile);
	EndPaint(Radix->hWnd, &PaintInfo);
    return ERR_OK;
 } /* endif */

BeginPaint(Radix->hWnd, &PaintInfo);
hDC = PaintInfo.hdc;
SaveDC(hDC);
//SetMapperFlags(hDC, 1);

sSetViewport(hDC, Radix, 1);

hBrush=SelectObject(hDC, CreateSolidBrush(sColor[COL_HINTER]));
SetTextAlign( hDC, TA_CENTER );
SetBkMode(hDC, TRANSPARENT);
sSub=GetSystemMetrics(SM_CYMENU)+GetSystemMetrics(SM_CYCAPTION);

GetWindowRect(Radix->hWnd, &rect);
wY=rect.bottom-rect.top-sSub;
/// Mittelpunkt, Radius und Aspectratio vom Radix definieren
sMpY=sMpX=wY/2;
sDruber=(short)(sMpY/DRUBERFACTOR);
sRadius=(short)(sMpY-sDruber);

//AstroFontRec.lfHeight = wY/22;
AstroFontRec.lfHeight = wY/18;
AstroFontRec.lfWidth = (short)((double)AstroFontRec.lfHeight/2.5);
hFont=SelectObject(hDC, CreateFontIndirect(&AstroFontRec));

lpLogPen.lopnStyle=PS_SOLID;
lpLogPen.lopnWidth.x=1;
lpLogPen.lopnColor=sColor[COL_HINTER];
hPen=SelectObject(hDC, CreatePenIndirect(&lpLogPen));

Rectangle(hDC, 0, 0, rect.right, rect.bottom);

lpLogPen.lopnColor=sColor[COL_RADIX];
DeleteObject(SelectObject(hDC, CreatePenIndirect(&lpLogPen)));
if ( Radix->hBitmap ) {
   sOneAspekt(hDC, Radix);
   DeleteObject(SelectObject(hDC, hPen));
   DeleteObject(SelectObject(hDC, hBrush));
   DeleteObject(SelectObject(hDC, hFont));
   RestoreDC(hDC, -1);
   EndPaint(Radix->hWnd, &PaintInfo);
   return ERR_OK;
} /* endif */

// hDC=(HDC)CreateMetaFile(NULL);
 if (Radix->lPaintFlag&DRW_TEXT) {
	sDrawText(Radix, Radix, 0, 0, DRW_LBD_INDEX);
	sDrawText(Radix, Radix, 0, 0, DRW_LB_INDEX);
	sDrawText(Radix, Radix, 0, 0, DRW_LBA_INDEX);
    Radix->lPaintFlag^=DRW_TEXT;
 }/* endif*/

// DeleteObject(SelectObject(hDC, GetStockObject(HOLLOW_BRUSH)));
/* hFont=SelectObject(hDC, GetStockObject(SYSTEM_FONT));
MainFontRec.lfHeight = wY/30;
 Radix->MainFontRec->lfWidth = wY/40;
MainFontRec.lfWidth = MainFontRec.lfHeight/2;
hFont=SelectObject(hDC, CreateFontIndirect(&MainFontRec));
*/
// sprintf( szBuffer, "%s %s, %s", (char*)Radix->pszVorName,
//		 (char*)Radix->pszName, (char*)Radix->pszOrt);
 if (Radix->cHoroTyp==TYP_SYNASTRIE) {
   plRadix=(RADIX*)Radix->plRadix[0];
   sprintf(szBuffer, "%s %s*%s %s",
	 	    Radix->rFix.clName?Radix->pszName:UNBEKANNT_TXT,
			Radix->rFix.clVorName?Radix->pszVorName:UNBEKANNT_TXT,
			plRadix->rFix.clName?plRadix->pszName:UNBEKANNT_TXT,
			plRadix->rFix.clVorName?plRadix->pszVorName:UNBEKANNT_TXT);
 } else
   sprintf(szBuffer, "%s %s*%s*%s*%s %s",
	 	    Radix->rFix.clName?Radix->pszName:UNBEKANNT_TXT,
			Radix->rFix.clVorName?Radix->pszVorName:UNBEKANNT_TXT,
	 		Radix->rFix.clBeruf?Radix->pszBeruf:UNBEKANNT_TXT,
			Radix->pszOrt, Radix->szDatum, Radix->szZeit );

 SetWindowText(Radix->hWnd, (LPSTR)szBuffer); 

 sRadix(hDC, Radix);

// AstroFontRec.lfHeight = wY/24;
 AstroFontRec.lfHeight = wY/22;
 AstroFontRec.lfWidth = (short)((double)AstroFontRec.lfHeight/2.5);
 DeleteObject(SelectObject(hDC, CreateFontIndirect(&AstroFontRec)));
 if ( Radix->lPaintFlag&DRW_HAUS )
   sHaus(hDC, Radix, (char)Sys.ScrFactor);

 sRet=sPlanet(hDC, Radix, wY/45);

 Radix->cMouse^=MAUS_ASPEKT;
 sOneAspekt(hDC, Radix);

 Radix->hBitmap=CreateCompatibleBitmap(hDC, wY, wY);
 if (Radix->hBitmap) {
	 Radix->hDCBit = CreateCompatibleDC(hDC);
	 Radix->hBitOld = SelectObject(Radix->hDCBit, Radix->hBitmap);
	 BitBlt(Radix->hDCBit, 0, 0, wY, wY, hDC, 0, 0, SRCCOPY);
 } /* endif */

 SetTextAlign( hDC, TA_LEFT );
 DeleteObject(SelectObject(hDC, hPen));
 DeleteObject(SelectObject(hDC, hBrush));
 DeleteObject(SelectObject(hDC, hFont));
 RestoreDC(hDC, -1);

// Radix->hMetaFile=CloseMetaFile(hDC);
// BeginPaint(Radix->hWnd, &PaintInfo);
// hDC = PaintInfo.hdc;
// PlayMetaFile(hDC, Radix->hMetaFile);
 EndPaint(Radix->hWnd, &PaintInfo);

 if ( sRet<=0 )
    return sRet;

 return ERR_OK;
};

/*----------------------------------------------------------------------------*/
short sPutString( HDC hDC, char* szText, short sX, short sY, COLORREF Col )
{

 SetTextColor(hDC, Col);
 TextOut(hDC, sX, sY, szText, strlen(szText));

return ERR_OK;
}


/*----------------------------------------------------------------------------*/
short sInitFont( void )
{

/* MainFontRec.lfHeight = 8;
 MainFontRec.lfWidth = 8;
 MainFontRec.lfEscapement = 0;
 MainFontRec.lfOrientation = 0;
 MainFontRec.lfWeight = FW_NORMAL;
 MainFontRec.lfItalic = 0;
 MainFontRec.lfUnderline = 0;
 MainFontRec.lfStrikeOut = 0;
 MainFontRec.lfCharSet = ANSI_CHARSET;
 MainFontRec.lfOutPrecision = OUT_DEFAULT_PRECIS;
 MainFontRec.lfClipPrecision = CLIP_DEFAULT_PRECIS;
 MainFontRec.lfQuality = 0;
 MainFontRec.lfPitchAndFamily = 0;
 strcpy(MainFontRec.lfFaceName,"MS Sans Serif");*/

 MainFontRec.lfHeight = 16;
 MainFontRec.lfWidth = 12;
 MainFontRec.lfEscapement = 0;
 MainFontRec.lfOrientation = 0;
 MainFontRec.lfWeight = FW_NORMAL;
 MainFontRec.lfItalic = 0;
 MainFontRec.lfUnderline = 0;
 MainFontRec.lfStrikeOut = 0;
 MainFontRec.lfCharSet = ANSI_CHARSET;
 MainFontRec.lfOutPrecision = OUT_DEFAULT_PRECIS;
 MainFontRec.lfClipPrecision = CLIP_DEFAULT_PRECIS;
 MainFontRec.lfQuality = 0;
 MainFontRec.lfPitchAndFamily = 0;
 strcpy(MainFontRec.lfFaceName,"AstroUniverse");

 AstroFontRec.lfHeight = 16;
 AstroFontRec.lfWidth = 12;
 AstroFontRec.lfEscapement = 0;
 AstroFontRec.lfOrientation = 0;
 AstroFontRec.lfWeight = FW_NORMAL;
 AstroFontRec.lfItalic = 0;
 AstroFontRec.lfUnderline = 0;
 AstroFontRec.lfStrikeOut = 0;
 AstroFontRec.lfCharSet = ANSI_CHARSET;
 AstroFontRec.lfOutPrecision = OUT_DEFAULT_PRECIS;
 AstroFontRec.lfClipPrecision = CLIP_DEFAULT_PRECIS;
 AstroFontRec.lfQuality = 0;
 AstroFontRec.lfPitchAndFamily = 0;
 strcpy(AstroFontRec.lfFaceName,"AstroUniverse");
 return ERR_OK;
}

/*----------------------------------------------------------------------------*/
/* initialisations-kack                                                        */
/*----------------------------------------------------------------------------*/
short sInitRadixStruct(RADIX* Radix, short sTyp)
{
// Radix->rFix.lIndexRadix=0;
// Radix->rFix.slRadix=0;
// Radix->rFix.lIndexName=0;
// Radix->rFix.slName=RFIX_SIZE;
// Radix->rFix.lIndexNotiz=0;
// Radix->rFix.slNotiz=0;
// Radix->dStzGrad=0;
// Radix->cStzIndex=0;
 Radix->hBitmap=0;
 Radix->hBitOld=0;
 Radix->hDCBit=0;
 Radix->sAnzahlPlanet=MAX_PLANET;
 Radix->cMemory=CALC_ON_FILE;
 Radix->rFix.cGultig=1;
 Radix->lPaintFlag=-1;
 Radix->cHausSys=auinit.sSelHaus;
 Radix->cHoroTyp=TYP_RADIX;
 if (auinit.sSelHaus==TYP_NULL)
	Radix->lPaintFlag^=DRW_HAUS;
 Radix->sRotate=auinit.sRotRadix;
 Radix->MainFontRec=&MainFontRec;
 Radix->AstroFontRec=&AstroFontRec;
 Radix->lRadixCount=0;
 Radix->plRadix=0;
 Radix->lMulti=0;
 Radix->plMulti=0;
 Radix->sLB=0;
 if ( sTyp!=P_TYP_IGNORE ){
	 if (sAllocRadix(Radix, sTyp)==ERR_MEM)
	    return ERR_MEM;
 } /* endif */

return ERR_OK;
}

RADIX * InitRadix( HWND hWnd )
{
 short  sA;
 double dA;
 short  sRet;
 RADIX* Radix;

 auinit.sRet=ERR_OK;
 Radix=(RADIX*)calloc(1, RA_SIZE);
 if (!Radix)
    return ERR_NOITEM;

 if (sInitRadixStruct(Radix, TYP_RADIX)<0) {
    auinit.sRet=ERR_RADIX;
    return Radix;
 } /* endif */

 if (sGetName(Radix, &auinit.Radix, hWnd, DEL_SRC|ALLOC_MEM,
		 auinit.sSelHoro==TYP_SYNASTRIE?PERSON1_TXT:PERSON_TXT)==ERR_NOITEM){
    auinit.sRet=ERR_RADIX;
    return Radix;
 } /* endif */

 sSetPlanetTyp(Radix, P_TYP_NORM);

 sStartCalc(hWnd, auinit.sSelHoro==TYP_SYNASTRIE?SYNASTRIE1_CALC_TXT:RADIX_CALC_TXT);
 if (sCalcChart(Radix, Radix, TYP_RADIX)<ERR_OK){
	sEndCalc();
	auinit.sRet=ERR_RADIX;
    return Radix;
 } /* endif */
 sEndCalc();

 switch(auinit.sSelHoro) {
 case  TYP_SYNASTRIE:
	   sRet=sInitCT(Radix, hWnd, TYP_SYNASTRIE);
       if (sRet<ERR_OK)
          return Radix; 
 break;
 case  TYP_TRANSIT:
       sRet=sInitCT(Radix, hWnd, TYP_TRANSIT);
       if (sRet<ERR_OK)
          return Radix; 
 break;
 case  TYP_COMPOSIT:
		MessageBox(hWnd, "gibets noch nicht.;-) \nRadix wird berechnet.", 0, MB_OK);
 break;
 }/*endswitch*/
 
// sWriteDat( Radix, hWnd );
// fclose (fNamen);
 return Radix;
}

/*----------------------------------------------------------------------------*/
/* sInitCT Transit/Synastrie Initialisieren                                     */
/*----------------------------------------------------------------------------*/
short sInitCT ( RADIX * Radix, HWND hWnd, short sTyp )
{
 RADIX* plRadix;
 DLGPROC lpDlgProc=0;
 short  sRet;
 short sErr=sTyp==TYP_TRANSIT?ERR_TRANSIT:ERR_SYNASTRIE;
 short sPTyp=sTyp==TYP_TRANSIT?P_TYP_TRANSIT:P_TYP_SYNASTRIE;


 Radix->plRadix=farrealloc(Radix->plRadix, sizeof(long)*(Radix->lRadixCount+1));
 if (!Radix->plRadix){
	auinit.sRet=sErr;
	return sErr;
 } /* endif */
 Radix->plRadix[0]=(long)calloc(1, RA_SIZE);
 if (!Radix->plRadix[0]){
    auinit.sRet=sErr;
    return sErr;
 } /* endif */
 plRadix=(RADIX*)Radix->plRadix[0];
 plRadix->lTransitNum=0;
 Radix->lRadixCount++;
 if (sInitRadixStruct(plRadix, sPTyp)<0){
	 auinit.sRet=sErr;
     return sErr;
 } /* endif */

 switch(sTyp) {
 case  TYP_SYNASTRIE:
	 if (sGetName(plRadix, &auinit.Radix, hWnd, DEL_SRC|ALLOC_MEM, PERSON2_TXT)==ERR_NOITEM){
	    auinit.sRet=sErr;
	    return sErr;
	 } /* endif */
	 Radix->sLB^=LB_S;
	 plRadix->cHoroTyp=TYP_SYNASTRIE;
	 sSetPlanetTyp(plRadix, P_TYP_SYNASTRIE);
     sStartCalc(hWnd, auinit.sSelHoro==TYP_SYNASTRIE?SYNASTRIE2_CALC_TXT:RADIX_CALC_TXT);
	 if (sCalcChart(plRadix, Radix, sTyp)<ERR_OK){
		sEndCalc();
		auinit.sRet=sErr;
	    return sErr;
	 } /* endif */
     sEndCalc();
 break;
 case  TYP_TRANSIT:
 	 Radix->cMemory=CALC_ON_MEM;
	 sGetName(plRadix, Radix, hWnd, ALLOC_MEM, NULL);
     lpDlgProc=(DLGPROC)MakeProcInstance((FARPROC)DlgTransit, hInstance);
	 sRet=DialogBoxParam(hInstance, "TRANSIT", Radix->hWnd, lpDlgProc, (long)plRadix);
	 FreeProcInstance ( (FARPROC) lpDlgProc );
	 if  (!sRet){
		auinit.sRet=sErr;
	    return sErr;         
	 } /* endif */
	 plRadix->cHoroTyp=TYP_TRANSIT;
	 if (stricmp(auinit.szBZeit, plRadix->szZeit)||
		 stricmp(auinit.szBDatum, plRadix->szDatum)) {
		lpDlgProc=(DLGPROC)MakeProcInstance((FARPROC)DlgTransEin, hInstance);
    	sRet=DialogBoxParam(hInstance, "TRANS_EIN", Radix->hWnd, lpDlgProc, (long)plRadix);
		FreeProcInstance ( (FARPROC) lpDlgProc );
	 }/*endif*/
	 if  (!sRet){
    	auinit.sRet=sErr;
	    return sErr;         
	 } /* endif */
	 sSetPlanetTyp(plRadix, P_TYP_TRANSIT);
	 sStartCalc(hWnd, TRANSIT_CALC_TXT);
	 if (sCalcChart(plRadix, Radix, sTyp)<ERR_OK){
		auinit.sRet=sErr;
		sEndCalc();
		return sErr;
	 } /* endif */
	 sEndCalc();
	 if (stricmp(auinit.szBZeit, plRadix->szZeit)||
		 stricmp(auinit.szBDatum, plRadix->szDatum)) {
		if (sCalcMultiTransit(Radix, INC_TAGE)<0) {
		   auinit.sRet=sErr;
		   return sErr;
		}/*endif*/
        else {
		   auinit.sRet=ERR_STRANSIT;
		   return ERR_STRANSIT;
        }/*endelse*/
	 }/*endif*/
 }/* endswitch*/
 Radix->sLB^=LB_A;
 Radix->sLB^=LB_G;

 return ERR_OK;
}

/*----------------------------------------------------------------------------*/
/* sExit Aufräumen                                                             */
/*----------------------------------------------------------------------------*/
void ExitRadix ( RADIX * Radix )
{

 if (Radix->hBitmap) { 
	DeleteObject(SelectObject(Radix->hDCBit, Radix->hBitOld));
    DeleteDC(Radix->hDCBit);
 } /*endif*/
 if ( Radix->pszVorName )
	free(Radix->pszVorName );
 if ( Radix->pszName )
	free(Radix->pszName );
 if ( Radix->pszBeruf )
	free(Radix->pszBeruf );
 if ( Radix->pszOrt )
	free(Radix->pszOrt );
 if ( Radix->pszNotiz )
	free(Radix->pszNotiz );
 if ( Radix->pszAspPlanet )
	free(Radix->pszAspPlanet );
 if ( Radix->pszAspHaus )
	free(Radix->pszAspHaus );
 if ( Radix->pdWinkelPlanet )
	free(Radix->pdWinkelPlanet );
 if ( Radix->pdWinkelHaus )
	free(Radix->pdWinkelHaus );
 if ( Radix->pdPlanet )
	free(Radix->pdPlanet);
 if ( Radix->pdHaus )
	free(Radix->pdHaus);
 if ( Radix->pdPlanetRad)
	free(Radix->pdPlanetRad);
 if ( Radix->pdHausRad )
	free(Radix->pdHausRad);
 if ( Radix->pszStzHaus )
	free(Radix->pszStzHaus);
 if ( Radix->pszStzPlanet )
	free(Radix->pszStzPlanet);
 if ( Radix->pszInHaus )
	free(Radix->pszInHaus);
 if ( Radix->psPlanetTyp)
	free(Radix->psPlanetTyp);
 if( Radix->plMulti )
	farfree(Radix->plMulti);
 if( Radix->plRadix )
    farfree(Radix->plRadix);
 if ( Radix )
	free(Radix );
}

/****************************************************************************/
/*RadixZoooooom																 */
/****************************************************************************/
short sZoom(HDC hDC, RADIX* Radix)
{
 short wY=sMpY*2;
 short sMX, sMY, sX, sY;

 if (Radix->cMouse&MAUS_ZOOM) {
	 sMX=LOWORD(Radix->lParam);
     sMY=HIWORD(Radix->lParam);
     sX=sMX<sMpY?0:sMpY;
     sY=sMY<sMpY?0:sMpY;
	 StretchBlt(hDC, 0, 0, wY, wY, Radix->hDCBit, sX, sY, sMpY, sMpY, SRCCOPY);
 }else{
	 BitBlt(hDC, 0, 0, wY, wY, Radix->hDCBit, 0, 0, SRCCOPY);
 }/* endelse*/

 return ERR_OK;
}






