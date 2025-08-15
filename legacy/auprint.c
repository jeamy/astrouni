/*----------------------------------------------------------------------------*/
/* Autor:               JeaMY  Lee                                            */
/* Datum:               Samstag 27.01.1996                                   */
/*----------------------------------------------------------------------------*/

#include <print.h>
#include "astrouni.h"

extern   HWND      hWndMain;
extern   HINSTANCE hInstance;  //astrouni.c
extern	 short     sMpX, sMpY; //auwurcel.c
extern   short     sRadius;	   //auwurcel.c
extern   short     sDruber;    //auwurcel.c
extern   char      szLNANamen[][2];   //auwurzel.h
//extern   char      szANamen[][2];      //auwurzel.h
extern   char      szBNamen[][2];      //auwurzel.h
extern   char      szQualitaten[][10];   //auwurzel.h
extern   char      cAspektGly[][2];
extern   SYS       Sys;                  //auhelper.c
extern   AU_INIT   auinit;
extern   char      ELF_TXT[];
extern   char      MainMenuText[];

static   short     xPrint;
static   char      cAbort;
static   HWND  	   hwndAbort;

char     szHoroTyp[][10]={
                        "Radix",
                    	"Transit",
						"Synastrie",
						"Composit",
						"Combine",
                  		"Direkt"};

char     szHausTyp[][14]={
             		     "Koch",
                         "Placidus",
                         "Equal",
             			 "Equal 1",
                         "Whole",
						 "Topozentrisch",
                		 "Campanus",
                         "Meridian",
              		     "Regimontanus",
                         "Porphyry",
                         "Porphyry New",
                         "Morinus",
                         "Alcabit",
                         "Null"};


/*----------------------------------------------------------------------------*/
/* sPrintAsp            AspekteTabelle drucken                                */
/*----------------------------------------------------------------------------*/
HDC hdcPrinterIni(char cMode, char * szPrinter)
{
  HDC        hDC;
  LPDEVMODE  lpDevMode=0;
  LPDEVNAMES lpDevNames;
  char * 	 pszDriverName;
  char * 	 pszDeviceName;
  char * 	 pszPortName;

  PRINTDLG   printDlg= {
				sizeof(PRINTDLG), (HWND)0, (HGLOBAL)0, (HGLOBAL)0, (HDC)0,
				PD_NOPAGENUMS | PD_NOSELECTION | PD_USEDEVMODECOPIES |
				PD_RETURNDC | PD_DISABLEPRINTTOFILE | PD_RETURNDEFAULT,
				0, 0, 0, 0, 1, (HINSTANCE)0, 0L, NULL, NULL, (LPCSTR)0, (LPCSTR)0,
		  		(HGLOBAL)0, (HGLOBAL)0};
  //PD_RETURNDC | PD_RETURNDEFAULT

  if (cMode==PRN_SETUP) {
	 printDlg.Flags |= PD_PRINTSETUP;
//	 printDlg.Flags ^= PD_RETURNDEFAULT;
     if (!PrintDlg((LPPRINTDLG)&printDlg))
        return ERR_PRINT;
	return ERR_OK;
  } /* endif */

  if (!PrintDlg((LPPRINTDLG)&printDlg))
     return ERR_PRINT;

  if (!printDlg.hDevNames)
	return ERR_PRINT;
  lpDevNames = (LPDEVNAMES)GlobalLock(printDlg.hDevNames);
  pszDriverName = (LPSTR)lpDevNames + lpDevNames->wDriverOffset;
  pszDeviceName = (LPSTR)lpDevNames + lpDevNames->wDeviceOffset;
  pszPortName = (LPSTR)lpDevNames + lpDevNames->wOutputOffset;
  GlobalUnlock(printDlg.hDevNames);
  if (printDlg.hDevMode)
     lpDevMode = (LPDEVMODE)GlobalLock(printDlg.hDevMode);
  if (printDlg.hDC)
     hDC = printDlg.hDC;
  else
	 hDC = CreateDC(pszDriverName, pszDeviceName, pszPortName, lpDevMode);
  if(szPrinter)
	sprintf(szPrinter, "%s -> %s", pszDeviceName, pszPortName);
    if (printDlg.hDevMode && lpDevMode)
       GlobalUnlock(printDlg.hDevMode);
    if (printDlg.hDevNames) {
       GlobalFree(printDlg.hDevNames);
       printDlg.hDevNames = (HGLOBAL)NULL;
    }/*endif*/
    if (printDlg.hDevMode) {
      GlobalFree(printDlg.hDevMode);
      printDlg.hDevMode = (HGLOBAL)NULL;
    }/*endif*/
//  }/*endelse*/
  sSetSys( hDC, 0);

  return hDC;
}

/*----------------------------------------------------------------------------*/
/* sPrintAsp            AspekteTabelle drucken                                */
/*----------------------------------------------------------------------------*/
short sPrintAsp(HDC hDC, RADIX* Radix, double dFactor)
{
 short sA, sB, sAA, sAE, sBE;
 char  cD;
 short xText, yText, xBox, yBox, xBoxE, yBoxA, yBoxE, sAddX;
 short yAspStart;
 short sGrad, sMinuten, sSekunden;
 short xAdd, xBAdd;
 short yAdd, yBAdd;
 char  szBuffer[BUF_SIZE];
 LOGPEN lpLogPen;

 RADIX* plRadix=Radix;
 if (Radix->lRadixCount)
	plRadix=(RADIX*)Radix->plRadix[Radix->lRadixCount-1];

 lpLogPen.lopnStyle=PS_SOLID;
 lpLogPen.lopnWidth.x=1;
 lpLogPen.lopnColor=RGB(0,0,0);
 DeleteObject(SelectObject(hDC, CreatePenIndirect(&lpLogPen)));

 Radix->AstroFontRec->lfHeight = (short)(dFactor*26);
 Radix->AstroFontRec->lfWidth = (short)((double)Radix->AstroFontRec->lfHeight/2.3);
 DeleteObject(SelectObject(hDC, CreateFontIndirect(Radix->AstroFontRec)));

 xAdd=(short)((double)LOWORD(GetTextExtent(hDC, "ª", 1))*1.6);
 yAdd=(short)((double)HIWORD(GetTextExtent(hDC, "ª", 1))*1.5);
 xBAdd=(short)((double)xAdd/2);
 yBAdd=(short)((double)yAdd/4);

 SetTextAlign( hDC, TA_CENTER );
 yAspStart=sMpY+sRadius+3*sDruber;
// yAspStart=10*yAdd;

 xText=xPrint;
 yText=yAspStart;
 xBox=xText-xBAdd;
 yBox=yText-yBAdd;
 yBoxE=yBox+yAdd*(P_NKNOTEN+1);
 if (plRadix->cHoroTyp==TYP_RADIX) {
    sAA=1;
    sBE=0;
	xBoxE=xBox+xAdd*Radix->sAnzahlPlanet;
 }else{
    sAA=0;
    sBE=xAdd;
	xBoxE=xBox+xAdd*(plRadix->sAnzahlPlanet+1);
 }/*enelse*/
 MoveTo(hDC, xBox, yBox);
 LineTo(hDC, xBoxE, yBox);
 for (sA=0; sA<P_NKNOTEN;sA++ ) {
	 TextOut(hDC, xText, yText, szLNANamen[sA], strlen(szLNANamen[sA]));
     yBox+=yAdd;
	 MoveTo(hDC, xBox, yBox);
	 LineTo(hDC, xBoxE, yBox);
	 sB=(plRadix->cHoroTyp==TYP_RADIX)?sA+1:0;
	 sAddX=xText+xAdd*sB+sBE;
	 for (; sB<plRadix->sAnzahlPlanet; sB++ ) {
         cD=plRadix->pszAspPlanet[sA*plRadix->sAnzahlPlanet+sB];
  	     if (cD!=KEIN_ASP) {
			TextOut(hDC, sAddX, yText, cAspektGly[cD], strlen(cAspektGly[cD]));
		 };/*endif */
     	 sAddX+=xAdd;
     }; /*endfor*/
     yText+=yAdd;
 }; /*endfor*/
 yBox+=yAdd;
 MoveTo(hDC, xBox, yBox);
 LineTo(hDC, xBoxE, yBox);

 sAddX=xText+xAdd;
 yBox=yAspStart-yBAdd;
 MoveTo(hDC, xBox, yBox);
 LineTo(hDC, xBox, yBoxE);
 for (sA=sAA; sA<plRadix->sAnzahlPlanet; sA++ ) {
	 TextOut(hDC, sAddX, yText, szLNANamen[sA], strlen(szLNANamen[sA]));
	 sAddX+=xAdd;
	 xBox+=xAdd;
	 MoveTo(hDC, xBox, yBox);
	 LineTo(hDC, xBox, yBoxE);
 }; /*endfor*/
 xBox+=xAdd;
 MoveTo(hDC, xBox, yBox);
 LineTo(hDC, xBox, yBoxE);

/*----------------------------------------------------------------------------*/
 xText=sMpX+sDruber;
 xBox=xText-xBAdd;
 if (plRadix->cHoroTyp==TYP_RADIX) {
    sAA=P_NKNOTEN+1;
    sAE=Radix->sAnzahlPlanet-1;
    sBE=xAdd*P_NKNOTEN;
	yText=yAspStart+4*yAdd;
	yBox=yBoxA=yText-yBAdd;
	xBoxE=xBox+xAdd*(Radix->sAnzahlPlanet-P_NKNOTEN);
    yBoxE=yBox+yAdd*(Radix->sAnzahlPlanet-P_NKNOTEN);
 }else{
    sAA=0;
    sAE=plRadix->sAnzahlPlanet;
	sBE=-xAdd;
	yText=yAspStart+3*yAdd;
	yBox=yBoxA=yText-yBAdd;
	xBoxE=xBox+xAdd*(plRadix->sAnzahlPlanet+1);
    yBoxE=yBox+yAdd*(plRadix->sAnzahlPlanet-P_NKNOTEN+1);
 }/*enelse*/
 MoveTo(hDC, xBox, yBox);
 LineTo(hDC, xBoxE, yBox);
 for (sA=P_NKNOTEN; sA<sAE; sA++ ) {
	 TextOut(hDC, xText, yText, szLNANamen[sA], strlen(szLNANamen[sA]));
	 yBox+=yAdd;
	 MoveTo(hDC, xBox, yBox);
	 LineTo(hDC, xBoxE, yBox);
	 sB=(plRadix->cHoroTyp==TYP_RADIX)?sA+1:0;
	 sAddX=xText+xAdd*sB-sBE;
	 for (; sB<plRadix->sAnzahlPlanet; sB++ ) {
         cD=plRadix->pszAspPlanet[sA*plRadix->sAnzahlPlanet+sB];
  	     if (cD!=KEIN_ASP) {
			TextOut(hDC, sAddX, yText, cAspektGly[cD], strlen(cAspektGly[cD]));
		 };/*endif */
	     sAddX+=xAdd;
     }; /*endfor*/
     yText+=yAdd;
 }; /*endfor*/
 yBox+=yAdd;
 MoveTo(hDC, xBox, yBox);
 LineTo(hDC, xBoxE, yBox);

 sAddX=xText+xAdd;
 yBox=yBoxA;
 MoveTo(hDC, xBox, yBox);
 LineTo(hDC, xBox, yBoxE);
 for (sA=sAA; sA<Radix->sAnzahlPlanet; sA++ ) {
	 TextOut(hDC, sAddX, yText, szLNANamen[sA], strlen(szLNANamen[sA]));
	 sAddX+=xAdd;
	 xBox+=xAdd;
	 MoveTo(hDC, xBox, yBox);
	 LineTo(hDC, xBox, yBoxE);
 }; /*endfor*/
 xBox+=xAdd;
 MoveTo(hDC, xBox, yBox);
 LineTo(hDC, xBox, yBoxE);

 return ERR_OK;
}

/*----------------------------------------------------------------------------*/
/* sPrintHausAsp            AspekteTabelle drucken                                */
/*----------------------------------------------------------------------------*/
short sPrintHausAsp(HDC hDC, RADIX* Radix, short sYText, double dFactor)
{
 short sA, sB;
 char  cD;
 short xText, yText, xBox, yBox, xBoxE, yBoxE, sAddX;
 short xAdd, xBAdd;
 short yAdd, yBAdd;
 char  szBuffer[BUF_SIZE];
 LOGPEN lpLogPen;

 RADIX* plRadix=Radix;
 if (Radix->lRadixCount)
	plRadix=(RADIX*)Radix->plRadix[Radix->lRadixCount-1];

 lpLogPen.lopnStyle=PS_SOLID;
 lpLogPen.lopnWidth.x=1;
 lpLogPen.lopnColor=RGB(0,0,0);
 DeleteObject(SelectObject(hDC, CreatePenIndirect(&lpLogPen)));

 Radix->AstroFontRec->lfHeight = (short)(dFactor*26);
 Radix->AstroFontRec->lfWidth = (short)((double)Radix->AstroFontRec->lfHeight/2.3);
 DeleteObject(SelectObject(hDC, CreateFontIndirect(Radix->AstroFontRec)));

 xAdd=(short)((double)LOWORD(GetTextExtent(hDC, "ª", 1))*1.6);
 yAdd=(short)((double)HIWORD(GetTextExtent(hDC, "ª", 1))*1.5);
 xBAdd=(short)((double)xAdd/2);
 yBAdd=(short)((double)yAdd/4);

 SetTextAlign( hDC, TA_CENTER );

 xText=xPrint;
 yText=sYText;
 xBox=xText-xBAdd;
 yBox=sYText-yBAdd;
 yBoxE=yBox+yAdd*13;
 xBoxE=xBox+(xAdd*Radix->sAnzahlPlanet+1);
 MoveTo(hDC, xBox, yBox);
 LineTo(hDC, xBoxE, yBox);
 for (sA=0; sA<MAX_HAUS; sA++ ) {
	 sprintf(szBuffer,"Ñ%02d", sA+1);
	 TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
     yBox+=yAdd;
	 MoveTo(hDC, xBox, yBox);
	 LineTo(hDC, xBoxE, yBox);
	 sB=0;
	 if(sA)
		sAddX=xText+xAdd*sB;
	 else
		sAddX=xText+2*xAdd*sB;
	 for (sB=0; sB<plRadix->sAnzahlPlanet; sB++ ) {
		 cD=plRadix->pszAspHaus[sA*MAX_HAUS+sB];
  	     if (cD!=KEIN_ASP) {
			TextOut(hDC, sAddX, yText, cAspektGly[cD], strlen(cAspektGly[cD]));
		 };/*endif */
     	 sAddX+=xAdd;
     }; /*endfor*/
     yText+=yAdd;
 }; /*endfor*/
 yBox+=yAdd;
 MoveTo(hDC, xBox, yBox);
 LineTo(hDC, xBoxE, yBox);

 sAddX=xText+xAdd;
 yBox=sYText-yBAdd;
 MoveTo(hDC, xBox, yBox);
 LineTo(hDC, xBox, yBoxE);
 for (sA=0; sA<plRadix->sAnzahlPlanet; sA++ ) {
	 TextOut(hDC, sAddX, yText, szLNANamen[sA], strlen(szLNANamen[sA]));
	 sAddX+=xAdd;
	 xBox+=xAdd;
	 MoveTo(hDC, xBox, yBox);
	 LineTo(hDC, xBox, yBoxE);
 }; /*endfor*/
 xBox+=xAdd;
 MoveTo(hDC, xBox, yBox);
 LineTo(hDC, xBox, yBoxE);

 return ERR_OK;
}

/*----------------------------------------------------------------------------*/
/* sPrintKoorf          texte Ausdruck                                        */
/*----------------------------------------------------------------------------*/
short sPrintKoord(HDC hDC, RADIX* Radix, double dFactor)
{
short sA;
short xText, yText, yPlanet, sAddX, sYAspekt;
short sGrad, sMinuten, sSekunden;
short xAdd;
short xAddGrad;
short xAddHaus;
short xAddQuali;
short sZeilenAbst;
char  szBuffer[BUF_SIZE];
RADIX* plRadix=Radix;

sPrintText(hDC, Radix, dFactor);

Radix->AstroFontRec->lfHeight = (short)(dFactor*30);
Radix->AstroFontRec->lfWidth = (short)((double)Radix->AstroFontRec->lfHeight/2.3);
DeleteObject(SelectObject(hDC, CreateFontIndirect(Radix->AstroFontRec)));

xAdd=LOWORD(GetTextExtent(hDC, "W", 1));
xAddGrad=LOWORD((GetTextExtent(hDC, " 00°00'00\" ", 11)));
xAddHaus=LOWORD((GetTextExtent(hDC, "Ñ10", 3)));
xAddQuali=LOWORD(GetTextExtent(hDC, QU_BEWEGLICH_TXT, strlen(QU_BEWEGLICH_TXT)));
sZeilenAbst=HIWORD(GetTextExtent(hDC, "W", 1));

SetTextColor(hDC, RGB(0,0,0));

SetTextAlign( hDC, TA_LEFT );
xText=xPrint;
yText=3*sDruber+5*sZeilenAbst;

if (Radix->cHoroTyp!=TYP_RADIX) {
   sprintf(szBuffer, "%s %s", Radix->pszVorName, Radix->pszName );
   TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
   yText+=sZeilenAbst;
}/*endif*/
TextOut(hDC, xText, yText, PLANETPOS_TXT, strlen(PLANETPOS_TXT));
yText+=sZeilenAbst/2;

for  (sA=0; sA<Radix->sAnzahlPlanet; sA++){
    yText+=sZeilenAbst;
	if ( (Radix->psPlanetTyp[sA]&P_TYP_RUCK) )
       TextOut(hDC, xText-xAdd/2, yText, "œ", 1);
	TextOut(hDC, xText, yText, szLNANamen[sA], strlen(szLNANamen[sA]));
    vGrad2GMS(dDeg2Dez(Radix->pdPlanet[sA]), &sGrad, &sMinuten, &sSekunden);
    sprintf(szBuffer," %02dÙ%02d'%02d\" ", sGrad, sMinuten, sSekunden);
	TextOut(hDC, xText+xAdd, yText, szBuffer, strlen(szBuffer));
	TextOut(hDC, xText+xAdd+xAddGrad, yText, szBNamen[Radix->pszStzPlanet[sA]],
				 strlen(szBNamen[Radix->pszStzPlanet[sA]]));
//	TextOut(hDC, xText+xAdd+xAddGrad, yText, szANamen[Radix->pszStzPlanet[sA]],
//				 strlen(szANamen[Radix->pszStzPlanet[sA]]));
	sprintf(szBuffer,"  in  Ñ%02d", Radix->pszInHaus[sA]);
	TextOut(hDC, xText+2*xAdd+xAddGrad, yText, szBuffer, strlen(szBuffer));
} /*endfor*/
yPlanet=yText;

sAddX=LOWORD(GetTextExtent(hDC, PLANETPOS_TXT, strlen(PLANETPOS_TXT)))+3*xAdd;
if ( Radix->lPaintFlag&DRW_HAUS ) {
    xText+=2*sDruber+sAddX;
    if (plRadix->cHoroTyp!=TYP_RADIX)
		yText=3*sDruber+6*sZeilenAbst;
    else
		yText=3*sDruber+5*sZeilenAbst;
	TextOut(hDC, xText, yText, HAUSPOS_TXT, strlen(HAUSPOS_TXT));
	yText+=sZeilenAbst/2;
	for (sA=0; sA<MAX_HAUS; sA++){
	     yText+=sZeilenAbst;
	     sprintf(szBuffer,"Ñ%02d", sA+1);
	  	 TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
	 	 vGrad2GMS(dDeg2Dez(Radix->pdHaus[sA]), &sGrad, &sMinuten, &sSekunden);
	     sprintf(szBuffer," %02dÙ%02d'%02d\" ", sGrad, sMinuten, sSekunden);
	  	 TextOut(hDC, xText+xAdd/2+xAddHaus, yText, szBuffer, strlen(szBuffer));
		 TextOut(hDC, xText+xAdd/2+xAddHaus+xAddGrad, yText, szBNamen[Radix->pszStzHaus[sA]],
		 strlen(szBNamen[Radix->pszStzHaus[sA]]));
//		 TextOut(hDC, xText+xAdd/2+xAddHaus+xAddGrad, yText, szANamen[Radix->pszStzHaus[sA]],
//		 strlen(szANamen[Radix->pszStzHaus[sA]]));
	} /* enfor*/
} /* endif */

sAddX=LOWORD(GetTextExtent(hDC, HAUSPOS_TXT, strlen(HAUSPOS_TXT)))+3*xAdd;
xText+=sAddX;
if (plRadix->cHoroTyp!=TYP_RADIX)
	yText=3*sDruber+6*sZeilenAbst;
else
	yText=3*sDruber+5*sZeilenAbst;
TextOut(hDC, xText, yText, QUALITAT_TXT, strlen(QUALITAT_TXT));
yText+=sZeilenAbst/2;
for (sA=0; sA<MAX_QUALITATEN; sA++){
	 yText+=sZeilenAbst;
	 sprintf(szBuffer,"%s", szQualitaten[sA]);
  	 TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
	 sprintf(szBuffer,"%02d", Radix->sQualitaten[sA]);
  	 TextOut(hDC, xText+xAdd+xAddQuali, yText, szBuffer, strlen(szBuffer));
} /* endif */

if (Radix->lRadixCount) {
	plRadix=(RADIX*)Radix->plRadix[0];
    xText=xPrint;
	yText=2*sZeilenAbst+yPlanet;
	if (plRadix->cHoroTyp==TYP_TRANSIT)
	   TextOut(hDC, xText, yText, TRANSITPOS_TXT, strlen(TRANSITPOS_TXT));
    else {
	   sprintf(szBuffer, "%s %s", plRadix->pszVorName, plRadix->pszName );
       TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
	   yText+=sZeilenAbst;
	   TextOut(hDC, xText, yText, PLANETPOS_TXT, strlen(PLANETPOS_TXT));
	}/*endif*/
	yText+=sZeilenAbst/2;
	for  (sA=0; sA<plRadix->sAnzahlPlanet; sA++){
	    yText+=sZeilenAbst;
	    if ( (plRadix->psPlanetTyp[sA]&P_TYP_RUCK) )
	       TextOut(hDC, xText-xAdd/2, yText, "œ", 1);
		TextOut(hDC, xText, yText, szLNANamen[sA], strlen(szLNANamen[sA]));
	    vGrad2GMS(dDeg2Dez(plRadix->pdPlanet[sA]), &sGrad, &sMinuten, &sSekunden);
	    sprintf(szBuffer," %02dÙ%02d'%02d\" ", sGrad, sMinuten, sSekunden);
		TextOut(hDC, xText+xAdd, yText, szBuffer, strlen(szBuffer));
		TextOut(hDC, xText+xAdd+xAddGrad, yText, szBNamen[plRadix->pszStzPlanet[sA]],
				 strlen(szBNamen[plRadix->pszStzPlanet[sA]]));
//		TextOut(hDC, xText+xAdd+xAddGrad, yText, szANamen[plRadix->pszStzPlanet[sA]],
//				 strlen(szANamen[plRadix->pszStzPlanet[sA]]));
		sprintf(szBuffer,"  in  Ñ%02d", plRadix->pszInHaus[sA]);
		TextOut(hDC, xText+2*xAdd+xAddGrad, yText, szBuffer, strlen(szBuffer));
	} /*endfor*/
	sAddX=LOWORD(GetTextExtent(hDC, PLANETPOS_TXT, strlen(PLANETPOS_TXT)))+3*xAdd;
	xText+=2*sDruber+sAddX;
	if ( plRadix->lPaintFlag&DRW_HAUS&&plRadix->cHoroTyp==TYP_SYNASTRIE ) {
		yText=3*sZeilenAbst+yPlanet;
		TextOut(hDC, xText, yText, HAUSPOS_TXT, strlen(HAUSPOS_TXT));
		yText+=sZeilenAbst/2;
		for (sA=0; sA<MAX_HAUS; sA++){
		     yText+=sZeilenAbst;
		     sprintf(szBuffer,"Ñ%02d", sA+1);
			 TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
		 	 vGrad2GMS(dDeg2Dez(plRadix->pdHaus[sA]), &sGrad, &sMinuten, &sSekunden);
		     sprintf(szBuffer," %02dÙ%02d'%02d\" ", sGrad, sMinuten, sSekunden);
		  	 TextOut(hDC, xText+xAdd/2+xAddHaus, yText, szBuffer, strlen(szBuffer));
			 TextOut(hDC, xText+xAdd/2+xAddHaus+xAddGrad, yText, szBNamen[plRadix->pszStzHaus[sA]],
			       strlen(szBNamen[plRadix->pszStzHaus[sA]]));
//			 TextOut(hDC, xText+xAdd/2+xAddHaus+xAddGrad, yText, szANamen[plRadix->pszStzHaus[sA]],
//			       strlen(szANamen[plRadix->pszStzHaus[sA]]));
		} /* enfor*/
	} /* endif */

	sYAspekt=yText+2*sZeilenAbst;
	sAddX=LOWORD(GetTextExtent(hDC, HAUSPOS_TXT, strlen(HAUSPOS_TXT)))+3*xAdd;
	xText+=sAddX;
	if (plRadix->cHoroTyp!=TYP_TRANSIT)
		yText=3*sZeilenAbst+yPlanet;
	else
		yText=2*sZeilenAbst+yPlanet;
	TextOut(hDC, xText, yText, QUALITAT_TXT, strlen(QUALITAT_TXT));
	yText+=sZeilenAbst/2;
	for (sA=0; sA<MAX_QUALITATEN; sA++){
	    yText+=sZeilenAbst;
		sprintf(szBuffer,"%s", szQualitaten[sA]);
	  	TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
		sprintf(szBuffer,"%02d", plRadix->sQualitaten[sA]);
	  	TextOut(hDC, xText+xAdd+xAddQuali, yText, szBuffer, strlen(szBuffer));
	} /* endif */
} /* endif */
/*
if (!cAbort){
   sPrintHausAsp(hDC, plRadix, sYAspekt, Sys.PrnFactor);
 } /* endif*/

return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/* sPrintText           texte Ausdruck                                        */
/*----------------------------------------------------------------------------*/
short sPrintText(HDC hDC, RADIX * Radix, double dFactor)
{
 short xText, yText, yBox;
 short sZeilenAbst;
 char  szBuffer[BUF_SIZE];
 char  szBuf[BUF_SIZE];
 RADIX* plRadix=Radix;
 
  if (Radix->lRadixCount)
	 plRadix=(RADIX*)Radix->plRadix[Radix->lRadixCount-1];

  Radix->AstroFontRec->lfHeight = (short)(dFactor*28);
  Radix->AstroFontRec->lfWidth = (short)((double)Radix->AstroFontRec->lfHeight/2.3);
  DeleteObject(SelectObject(hDC, CreateFontIndirect(Radix->AstroFontRec)));

  sZeilenAbst=LOWORD(GetTextExtent(hDC, "W", 1));
  SetTextColor(hDC, RGB(0,0,0));

  SetTextAlign( hDC, TA_RIGHT );
  xText=sMpX+sRadius;
  yText=3*sDruber;
  sprintf(szBuffer, "Geburtsdatum: %s",	Radix->szDatum);
  TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));

  yText+=sZeilenAbst;
  sprintf(szBuffer, "Geburtszeit: %s Ortszeit",	Radix->szZeit);
  TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
  if (Radix->lRadixCount){
	 yText+=sZeilenAbst;
	 if (plRadix->cHoroTyp==TYP_SYNASTRIE){
		sprintf(szBuffer, "Geburtsdatum: %s",	plRadix->szDatum);
	    TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
		sprintf(szBuffer, "Geburtszeit: %s Ortszeit",	plRadix->szZeit);
		yText+=sZeilenAbst;
	 } else
		 sprintf(szBuffer, "Datum/Zeit: %s %s",	plRadix->szDatum,
				 plRadix->szZeit);
	 TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
  } /* endif */

  xText=xPrint;
  yText=3*sDruber;
  SetTextAlign( hDC, TA_LEFT );
  sprintf(szBuffer, "%s für %s %s", szHoroTyp[plRadix->cHoroTyp],
	  			Radix->pszVorName,	Radix->pszName );
  TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
  yText+=sZeilenAbst;
  sprintf(szBuffer, "Geburtsort: %s", Radix->pszOrt);
  TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));

  if (Radix->lRadixCount){
 	 yText+=sZeilenAbst;
	 if (plRadix->cHoroTyp==TYP_SYNASTRIE){
		sprintf(szBuffer, "und %s %s", plRadix->pszVorName,	plRadix->pszName );
	    TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
	    yText+=sZeilenAbst;
	    sprintf(szBuffer, "Geburtsort: %s", plRadix->pszOrt);
	 }else
		sprintf(szBuffer, "Transitort: %s", plRadix->pszOrt);
	 TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
  } /* endif */

  SetTextAlign( hDC, TA_CENTER );
  xText=sMpX;
  yText=3*sDruber;
  sprintf(szBuffer, "%s nach %s", HAUSSYS_TXT, szHausTyp[Radix->cHausSys]);
  TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
  yText+=sZeilenAbst;
  vGrad2Str(Radix->rFix.dLange, szBuf, 1, 0);
  sprintf(szBuffer, "Länge: %s Breite: ", szBuf);
  vGrad2Str(Radix->rFix.dBreite, szBuf, 0, 0);
  strcat(szBuffer, szBuf);
  TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
  if (Radix->lRadixCount){
     if (plRadix->cHoroTyp!=TYP_TRANSIT)
		yText+=2*sZeilenAbst;
	 else
		yText+=sZeilenAbst;
	 vGrad2Str(plRadix->rFix.dLange, szBuf, 1, 0);
	 sprintf(szBuffer, "Länge: %s Breite: ", szBuf);
	 vGrad2Str(plRadix->rFix.dBreite, szBuf, 0, 0);
     strcat(szBuffer, szBuf);
	 TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
  } /* endif */

  xText=xPrint;
  yBox=yText+2*sDruber;
  yText=2*sDruber;
  MoveTo(hDC, xText-sDruber, yText);
  LineTo(hDC, sMpX+sRadius+sDruber, yText);
  LineTo(hDC, sMpX+sRadius+sDruber, yBox);
  LineTo(hDC, xText-sDruber, yBox);
  LineTo(hDC, xText-sDruber, yText);

  Radix->AstroFontRec->lfHeight = (short)(dFactor*10);
  Radix->AstroFontRec->lfWidth = (short)(dFactor*10);
  DeleteObject(SelectObject(hDC, CreateFontIndirect(Radix->AstroFontRec)));

  yText=sDruber;
  SetTextAlign( hDC, TA_RIGHT );
  xText=sMpX+sRadius+sDruber;
  TextOut(hDC, xText, yText, MainMenuText, strlen(MainMenuText));

  yText=yBox+sDruber/2;
  TextOut(hDC, xText, yText, ELF_TXT, strlen(ELF_TXT));


 //  Radix->AstroFontRec->lfHeight = yPrint/160;
//   Radix->AstroFontRec->lfWidth = yPrint/200;
//   DeleteObject(SelectObject(hDC, CreateFontIndirect(Radix->AstroFontRec)));


  return ERR_OK;
}

/*------------------------------------------------------------------------*/
short sDlgPrint(RADIX* Radix, char cMode)
{
  HDC 	     hDC;
  FARPROC    lpAbortDlg;
  FARPROC    lpAbortProc;
  short 	 sRet;
  LOGPEN     lpLogPen;
  HANDLE     hFont;
  HANDLE 	 hBrush;
  HANDLE 	 hPen;
  char  	 szBuffer[BUF_SIZE];
  char  	 szPrinter[BUF_SIZE];

  if(!(hDC=hdcPrinterIni(cMode, szPrinter)))
     return ERR_PRINT;
  lpAbortProc = (FARPROC)MakeProcInstance((FARPROC)DlgAbortProc, hInstance);
  lpAbortDlg =  (FARPROC)MakeProcInstance((FARPROC)DlgAbort, hInstance);
  cAbort = 0;
  GetWindowText( Radix?Radix->hWnd:hWndMain, szBuffer, 31);
//  SetAbortProc(hDC, lpAbortProc);
  hwndAbort=CreateDialog(hInstance, "DLGABORT", hWndMain, lpAbortDlg);
  Escape(hDC, SETABORTPROC, NULL, lpAbortProc, NULL);
  if(!hwndAbort||(Escape(hDC, STARTDOC, strlen(szBuffer), szBuffer, NULL)<0)){
	FreeProcInstance(lpAbortDlg);
    FreeProcInstance(lpAbortProc);
    DeleteDC(hDC);
	return ERR_PRINT;
  }/*endif*/
  SetWindowText(GetDlgItem(hwndAbort, ABORT_PRINTER), szPrinter);
/*----------------------Drucker Initialisierung--------------------------*/
  Escape(hDC, FLUSHOUTPUT, NULL, NULL, NULL);
//  sRet=StartPage(hDC);

  Sys.Status=SYS_PRN;
  sSetViewport( hDC, Radix, 0);
  SetBkMode(hDC, TRANSPARENT);
  hBrush=SelectObject(hDC, GetStockObject(HOLLOW_BRUSH));
  lpLogPen.lopnStyle=PS_SOLID;
  lpLogPen.lopnWidth.x=1;
  lpLogPen.lopnColor=RGB(0,0,0);
  hPen=SelectObject(hDC, CreatePenIndirect(&lpLogPen));
  Radix->AstroFontRec->lfHeight = (short)(Sys.PrnFactor*34);
  Radix->AstroFontRec->lfWidth = (short)(Sys.PrnFactor*15);
  hFont=SelectObject(hDC, CreateFontIndirect(Radix->AstroFontRec));
  sMpY=sMpY=(short)((double)Sys.yPrn/2.5);
  sMpX=Sys.xPrn/2;
  sDruber=(short)(sMpX/DRUBERFACTOR);
  sRadius=(short)(sMpX-5*sDruber);
  xPrint=sMpX-sRadius;

  switch (cMode){
  case PRN_MULTITRANS:
	   sPrintMTransit(hDC, Radix, Sys.PrnFactor);
  break;
  default:
  case PRN_NORMAL:
	   sPrintText(hDC, Radix, Sys.PrnFactor);
	   SetTextAlign( hDC, TA_CENTER );
	   if (auinit.sTeilung&S_3er||auinit.sTeilung&S_9er)
		  Radix->AstroFontRec->lfHeight = (short)(Sys.PrnFactor*48);
	   else
		  Radix->AstroFontRec->lfHeight = (short)(Sys.PrnFactor*52);
	   Radix->AstroFontRec->lfWidth = (short)((double)Radix->AstroFontRec->lfHeight/2.5);
	   DeleteObject(SelectObject(hDC, CreateFontIndirect(Radix->AstroFontRec)));

	   sRadix(hDC, Radix);

	   Radix->AstroFontRec->lfHeight = (short)(Sys.PrnFactor*30);
	   Radix->AstroFontRec->lfWidth = (short)((double)Radix->AstroFontRec->lfHeight/2.5);
	   DeleteObject(SelectObject(hDC, CreateFontIndirect(Radix->AstroFontRec)));
	   if ( Radix->lPaintFlag&DRW_HAUS&&!cAbort )
	 	  sHaus(hDC, Radix, (char)(3*Sys.PrnFactor));                   	//2==Linienedicke ASC,MC

	   if (auinit.sTeilung&S_3er||auinit.sTeilung&S_9er)
		   Radix->AstroFontRec->lfHeight = (short)(Sys.PrnFactor*46);
	   else
		   Radix->AstroFontRec->lfHeight = (short)(Sys.PrnFactor*50);
	   Radix->AstroFontRec->lfWidth = (short)((double)Radix->AstroFontRec->lfHeight/2.5);
	   DeleteObject(SelectObject(hDC, CreateFontIndirect(Radix->AstroFontRec)));
	   if (!cAbort)
		  sRet=sPlanet(hDC, Radix, (short)(Sys.PrnFactor*25));
	   if (!cAbort){
		   if (Radix->lRadixCount) {
			  sDrawAspekte(hDC, (RADIX*)Radix->plRadix[0], Radix, (char)(Sys.PrnFactor));
			  sPrintAsp(hDC, (RADIX*)Radix->plRadix[0], Sys.PrnFactor);
		   } else {
			  sDrawAspekte(hDC, Radix, Radix, (char)(Sys.PrnFactor));
	    	  sPrintAsp(hDC, Radix, Sys.PrnFactor);
		   } /* endielse*/
 	   } /* endif*/

	   if (!cAbort){
		  if(Escape(hDC, NEWFRAME, NULL, NULL, NULL)<0)
         	cAbort=1;
	   } /* endif*/
	   if (!cAbort)
		   sPrintKoord(hDC, Radix, Sys.PrnFactor);
  }/*endswitch*/

  SetTextAlign( hDC, TA_LEFT );
  DeleteObject(SelectObject(hDC, hPen));
  DeleteObject(SelectObject(hDC, hBrush));
  DeleteObject(SelectObject(hDC, hFont));

  if (!cAbort) {
	 sRet=Escape(hDC, NEWFRAME, NULL, NULL, NULL);
	 sRet=Escape(hDC, ENDDOC, NULL, NULL, NULL);
//     sRet=EndPage(hDC);
//     sRet=EndDoc(hDC);
  }  /*endif*/
  
  DestroyWindow(hwndAbort);
  FreeProcInstance((FARPROC)lpAbortProc);
  FreeProcInstance((FARPROC)lpAbortDlg);
  DeleteDC(hDC);
  Sys.Status=0;

  if ( sRet<=0 )
     return ERR_PRINT;
  else
	 return ERR_OK;
}


/* Message loop function for the printing abort dialog. Loops until       */
/* printing is completed or the user hits cancel, returning which result. */

BOOL CALLBACK _export DlgAbortProc(HDC hDC, int iCode )
{
  MSG msg;

  while (!cAbort && PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE))
    if (!IsDialogMessage(hwndAbort, &msg)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

  return !cAbort;
}


/* Processing function for the printing abort modeless dialog, as brought */
/* up temporarily when printing via the File Print menu command.          */


LRESULT CALLBACK DlgAbort(HWND hDlg, UINT message,
					   WPARAM wParam, LPARAM lParam)
{
  switch (message) {
  case WM_INITDIALOG:
    SetFocus(GetDlgItem(hDlg, ID_CANCEL));      
    return FALSE;

  case WM_COMMAND:
    if (wParam == ID_CANCEL) {
      cAbort = 1;
      EndDialog(hDlg, 1);
      break;
    }
    break;

  default:
	 return FALSE;
  }
  return TRUE;
}

/*----------------------------------------------------------------------------*/
/* sPrintMultiTrans     Transite drucken                                     */
/*----------------------------------------------------------------------------*/

short sPrintMTransit(HDC hDC, RADIX*Radix, double dFactor)
{
 short xText, yText, yBox;
 short sZeilenAbst;
 char  szBuffer[BUF_SIZE];
 RADIX* plRadix=Radix;
 DRAWITEMSTRUCT Di;
 DWORD dIndex=0;
 WORD  wCount=0;
 short sCount=0;
 long  lC=Radix->lRadixCount;

 sZeilenAbst=HIWORD(GetTextExtent(hDC, "W", 1));
 SetTextColor(hDC, RGB(0,0,0));

 if (Radix->lRadixCount)
	plRadix=(RADIX*)Radix->plRadix[0];
 Radix->lRadixCount=1;

 Di.hwndItem=Radix->hWndLBG;
 Di.hDC=hDC;
 sprintf( szBuffer, "Transite von %s bis %s", plRadix->szDatum, auinit.szBDatum);
 while((dIndex=SendMessage(Radix->hWndLBG, LB_GETITEMDATA, (WPARAM)wCount++, 0))!=LB_ERR)
 {
	if(!sCount){
	   sPrintText(hDC, Radix, Sys.PrnFactor);
	   Radix->AstroFontRec->lfHeight = (short)(dFactor*30);
	   Radix->AstroFontRec->lfWidth = (short)((double)Radix->AstroFontRec->lfHeight/2.3);
	   DeleteObject(SelectObject(hDC, CreateFontIndirect(Radix->AstroFontRec)));
	   SetTextAlign( hDC, TA_LEFT );
 	   Di.rcItem.left=xPrint;
	   Di.rcItem.top=3*sDruber+4*sZeilenAbst;
	   Di.rcItem.right=sMpX+sRadius+sDruber;
	   Di.rcItem.bottom=Di.rcItem.top+sZeilenAbst;
	   TextOut(hDC, xPrint, Di.rcItem.top, szBuffer, strlen(szBuffer));
	   Di.rcItem.top+=sZeilenAbst;
	   sCount=0;
    }/*endif*/
	Di.itemData=dIndex;
	sTextOut( &Di, NULL, DRW_LB_TXT_TRANSIT);
	Di.rcItem.top+=sZeilenAbst;
	Di.rcItem.bottom=Di.rcItem.top+sZeilenAbst;
	sCount++;
	if (sCount==50){
	   Escape(hDC, NEWFRAME, NULL, NULL, NULL);
	   sCount=0;
    }/*endif*/

 }; /*ENDwhile*/

 Radix->lRadixCount=lC;
 return ERR_OK;
}
