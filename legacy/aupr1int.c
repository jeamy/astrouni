/*----------------------------------------------------------------------------*/
/* Autor:               JeaMY  Lee                                            */
/* Datum:               Samstag 27.01.1996                                   */
/*----------------------------------------------------------------------------*/

#include <print.h>
#include "astrouni.h"

extern   HINSTANCE hInstance;  //astrouni.c
extern	 short     sMpX, sMpY; //auwurcel.c
extern   short     sRadius;	   //auwurcel.c
extern   short     sDruber;    //auwurcel.c
extern   COLORREF  sElement; 	   //auwurzel.h
extern   COLORREF  sColor; 		   //auwurzel.h
extern   COLORREF  sMono; 		   //auwurzel.h
extern   char      szLNANamen[][2];   //auwurzel.h
extern   char      szANamen[][2];      //auwurzel.h
extern   char      szQualitaten[][10];   //auwurzel.h
extern   char      cAspektGly[][2];
static   COLORREF  sBColor[COLORS];
static   COLORREF  sBElement[4];

char     cAbort;
HWND  	 hwndAbort;

char     szHoroTyp[][10]={
                        "Radix",
                    	"Transit",
                        "Composit",
                        "Synastrie",
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
short sPrintAsp(HDC hDC, RADIX* Radix, short sFactor)
{
 short sA, sB, sEndA;
 char  cD;
 short xText, yText, xBox, yBox, xBoxE, yBoxE, sAddX;
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

 Radix->AstroFontRec->lfHeight = sFactor*23;
 Radix->AstroFontRec->lfWidth = (short)((double)Radix->AstroFontRec->lfHeight/2.3);
 DeleteObject(SelectObject(hDC, CreateFontIndirect(Radix->AstroFontRec)));

 xAdd=(short)((double)LOWORD(GetTextExtent(hDC, "�", 1))*1.8);
 yAdd=(short)((double)HIWORD(GetTextExtent(hDC, "�", 1))*1.6);
 xBAdd=(short)((double)xAdd/2);
 yBAdd=(short)((double)yAdd/4);

 SetTextAlign( hDC, TA_CENTER );
// yAspStart=sMpY+sRadius+2*sDruber;
 yAspStart=10*yAdd;
 xText=sMpX-sRadius-2*sDruber;
 yText=yAspStart;
 xBox=xText-xBAdd;
 yBox=yText-yBAdd;
 sEndA=(plRadix->cHoroTyp==TYP_RADIX)?0:1;
 xBoxE=xBox+xAdd*(P_NKNOTEN+sEndA);
 MoveTo(hDC, xBox, yBox);
 LineTo(hDC, xBoxE, yBox);
 for (sA=0; sA<P_NKNOTEN-1; sA++ ) {
	 TextOut(hDC, xText, yText, szLNANamen[sA], strlen(szLNANamen[sA]));
     yBox+=yAdd;
	 MoveTo(hDC, xBox, yBox);
	 LineTo(hDC, xBoxE, yBox);
	 sB=(plRadix->cHoroTyp==TYP_RADIX)?sA+1:sA;
	 sAddX=xText+xAdd*sB;
	 for (; sB<P_NKNOTEN; sB++ ) {
         cD=plRadix->pszAspPlanet[sA*Radix->sAnzahlPlanet+sB];
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
 xBox=xText-xBAdd;
 yBox=yAspStart-yBAdd;
 yBoxE=yBox+yAdd*(P_NKNOTEN);
 MoveTo(hDC, xBox, yBox);
 LineTo(hDC, xBox, yBoxE);
 sA=(plRadix->cHoroTyp==TYP_RADIX)?1:0;
 for (; sA<P_NKNOTEN; sA++ ) {
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
 sEndA=(plRadix->cHoroTyp==TYP_RADIX)?1:0;
 yText=yAspStart+3*yAdd;
 xBox=xText-xBAdd;
 yBox=yText-yBAdd;
 sEndA=(plRadix->cHoroTyp==TYP_RADIX)?0:1;
 xBoxE=xBox+xAdd*(Radix->sAnzahlPlanet-P_NKNOTEN+sEndA);
 MoveTo(hDC, xBox, yBox);
 LineTo(hDC, xBoxE, yBox);
 sEndA=(plRadix->cHoroTyp==TYP_RADIX)?Radix->sAnzahlPlanet-1:Radix->sAnzahlPlanet;
 for (sA=P_NKNOTEN; sA<sEndA; sA++ ) {
	 TextOut(hDC, xText, yText, szLNANamen[sA], strlen(szLNANamen[sA]));
	 yBox+=yAdd;
	 MoveTo(hDC, xBox, yBox);
	 LineTo(hDC, xBoxE, yBox);
	 sB=(plRadix->cHoroTyp==TYP_RADIX)?sA+1:sA;
	 sAddX=xText+xAdd;
	 for (; sB<Radix->sAnzahlPlanet; sB++ ) {
         cD=plRadix->pszAspPlanet[sA*Radix->sAnzahlPlanet+sB];
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
 xBox=xText-xBAdd;
 sEndA=(plRadix->cHoroTyp==TYP_RADIX)?1:0;
 yBox=yAspStart+(3+sEndA)*yAdd-yBAdd;
 sEndA=(plRadix->cHoroTyp==TYP_RADIX)?1:0;
 yBoxE=yBox+yAdd*(Radix->sAnzahlPlanet-P_NKNOTEN-sEndA);
 MoveTo(hDC, xBox, yBox);
 LineTo(hDC, xBox, yBoxE);
 sA=(plRadix->cHoroTyp==TYP_RADIX)?P_NKNOTEN+1:P_NKNOTEN;
 for (; sA<Radix->sAnzahlPlanet; sA++ ) {
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
short sPrintKoord(HDC hDC, RADIX* Radix, short sFactor)
{
short sA;
short xText, yText, yPlanet, sAddX;
short sGrad, sMinuten, sSekunden;
short xAdd;
short xAddGrad;
short xAddHaus;
short xAddQuali;
short sZeilenAbst;
char  szBuffer[BUF_SIZE];
RADIX* plRadix=Radix;

Radix->AstroFontRec->lfHeight = sFactor*28;
Radix->AstroFontRec->lfWidth = (short)((double)Radix->AstroFontRec->lfHeight/2.3);
DeleteObject(SelectObject(hDC, CreateFontIndirect(Radix->AstroFontRec)));
sPrintText(hDC, Radix, Radix->sFactor);

Radix->AstroFontRec->lfHeight = sFactor*30;
Radix->AstroFontRec->lfWidth = (short)((double)Radix->AstroFontRec->lfHeight/2.3);
DeleteObject(SelectObject(hDC, CreateFontIndirect(Radix->AstroFontRec)));

xAdd=LOWORD(GetTextExtent(hDC, "W", 1));
xAddGrad=LOWORD((GetTextExtent(hDC, "00�00'00\"", 9)));
xAddHaus=LOWORD((GetTextExtent(hDC, "�10", 3)));
xAddQuali=LOWORD(GetTextExtent(hDC, QU_BEWEGLICH_TXT, strlen(QU_BEWEGLICH_TXT)));
sZeilenAbst=HIWORD(GetTextExtent(hDC, "W", 1));

SetTextColor(hDC, RGB(0,0,0));

SetTextAlign( hDC, TA_LEFT );
xText=sMpX-sRadius-2*sDruber;
yText=3*sDruber+5*sZeilenAbst;

TextOut(hDC, xText, yText, PLANETPOS_TXT, strlen(PLANETPOS_TXT));
yText+=sZeilenAbst/2;

for  (sA=0; sA<Radix->sAnzahlPlanet; sA++){
    yText+=sZeilenAbst;
    if ( (Radix->psPlanetTyp[sA]&P_TYP_RUCK) )
       TextOut(hDC, xText-xAdd/2, yText, "�", 1);
	TextOut(hDC, xText, yText, szLNANamen[sA], strlen(szLNANamen[sA]));
    vGrad2GMS(dDeg2Dez(Radix->pdPlanet[sA]), &sGrad, &sMinuten, &sSekunden);
    sprintf(szBuffer,"%02d�%02d'%02d\"", sGrad, sMinuten, sSekunden);
	TextOut(hDC, xText+xAdd, yText, szBuffer, strlen(szBuffer));
	TextOut(hDC, xText+xAdd+xAddGrad, yText, szANamen[Radix->pszStzPlanet[sA]],
				 strlen(szANamen[Radix->pszStzPlanet[sA]]));
    sprintf(szBuffer," in �%d", Radix->pszInHaus[sA]);
	TextOut(hDC, xText+2*xAdd+xAddGrad, yText, szBuffer, strlen(szBuffer));
} /*endfor*/
yPlanet=yText;

sAddX=LOWORD(GetTextExtent(hDC, PLANETPOS_TXT, strlen(PLANETPOS_TXT)))+xAdd;
if ( Radix->lPaintFlag&DRW_HAUS ) {
    xText+=2*sDruber+sAddX;
	yText=3*sDruber+5*sZeilenAbst;
	TextOut(hDC, xText, yText, HAUSPOS_TXT, strlen(HAUSPOS_TXT));
	yText+=sZeilenAbst/2;
	for (sA=0; sA<MAX_HAUS; sA++){
	     yText+=sZeilenAbst;
	     sprintf(szBuffer,"�%d", sA+1);
	  	 TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
	 	 vGrad2GMS(dDeg2Dez(Radix->pdHaus[sA]), &sGrad, &sMinuten, &sSekunden);
	     sprintf(szBuffer,"%02d�%02d'%02d\"", sGrad, sMinuten, sSekunden);
	  	 TextOut(hDC, xText+xAdd/2+xAddHaus, yText, szBuffer, strlen(szBuffer));
	  	 TextOut(hDC, xText+xAdd+xAddHaus+xAddGrad, yText, szANamen[Radix->pszStzHaus[sA]],
		 	strlen(szANamen[Radix->pszStzHaus[sA]]));
    } /* enfor*/
} /* endif */

sAddX=LOWORD(GetTextExtent(hDC, HAUSPOS_TXT, strlen(HAUSPOS_TXT)));
xText+=2*sDruber+sAddX;
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
    plRadix=(RADIX*)Radix->plRadix[Radix->lRadixCount-1];
    xText=sMpX-sRadius-2*sDruber;
    yText=2*sZeilenAbst+yPlanet;
    TextOut(hDC, xText, yText, TRANSITPOS_TXT, strlen(TRANSITPOS_TXT));
	yText+=sZeilenAbst/2;
	for  (sA=0; sA<plRadix->sAnzahlPlanet; sA++){
	    yText+=sZeilenAbst;
	    if ( (plRadix->psPlanetTyp[sA]&P_TYP_RUCK) )
	       TextOut(hDC, xText-xAdd/2, yText, "�", 1);
		TextOut(hDC, xText, yText, szLNANamen[sA], strlen(szLNANamen[sA]));
	    vGrad2GMS(dDeg2Dez(plRadix->pdPlanet[sA]), &sGrad, &sMinuten, &sSekunden);
	    sprintf(szBuffer,"%02d�%02d'%02d\"", sGrad, sMinuten, sSekunden);
		TextOut(hDC, xText+xAdd, yText, szBuffer, strlen(szBuffer));
		TextOut(hDC, xText+xAdd+xAddGrad, yText, szANamen[plRadix->pszStzPlanet[sA]],
				 strlen(szANamen[plRadix->pszStzPlanet[sA]]));
	    sprintf(szBuffer," in �%d", plRadix->pszInHaus[sA]);
		TextOut(hDC, xText+2*xAdd+xAddGrad, yText, szBuffer, strlen(szBuffer));
	} /*endfor*/
} /* endif */

return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/* sPrintText           texte Ausdruck                                        */
/*----------------------------------------------------------------------------*/
short sPrintText(HDC hDC, RADIX * Radix, short sFactor)
{
 short sA;
 short xText, yText, yBox;
 short sZeilenAbst;
 char  szBuffer[BUF_SIZE];
 RECT  rect;
 RADIX* plRadix=Radix;
 
  if (Radix->lRadixCount)
     plRadix=(RADIX*)Radix->plRadix[Radix->lRadixCount-1];

  sZeilenAbst=LOWORD(GetTextExtent(hDC, "W", 1));
  SetTextColor(hDC, RGB(0,0,0));

  SetTextAlign( hDC, TA_RIGHT );
  xText=sMpX+sRadius+2*sDruber;
  yText=3*sDruber;
  sprintf(szBuffer, "Geburtsdatum: %s",	Radix->szDatum);
  TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));

  yText+=sZeilenAbst;
  sprintf(szBuffer, "Geburtszeit: %s Ortszeit",	Radix->szZeit);
  TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));

  if (Radix->lRadixCount){
	 yText+=sZeilenAbst;
 	 sprintf(szBuffer, "Transitdatum/Zeit: %s %s",	plRadix->szDatum,
	 			plRadix->szZeit);
	 TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
  } /* endif */

  xText=sMpX-sRadius-2*sDruber;
  yText=3*sDruber;
  SetTextAlign( hDC, TA_LEFT );
  sprintf(szBuffer, "%s f�r %s %s", szHoroTyp[plRadix->cHoroTyp],
	  			Radix->pszVorName,	Radix->pszName );
  TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));

  yText+=sZeilenAbst;
  sprintf(szBuffer, "Geburtsort: %s", Radix->pszOrt);
  TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
  if (Radix->lRadixCount){
	 yText+=sZeilenAbst;
	 sprintf(szBuffer, "Transitort: %s", plRadix->pszOrt);
	 TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
  } /* endif */

  SetTextAlign( hDC, TA_CENTER );
  xText=sMpX;
  yText=3*sDruber;
  sprintf(szBuffer, "%s nach %s", HAUSSYS_TXT, szHausTyp[Radix->cHausSys]);
  TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
  yText+=sZeilenAbst;
  sprintf(szBuffer, "L�nge: %2.2f Breite: %2.2f ", Radix->rFix.dLange,
  						Radix->rFix.dBreite);
  TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
  if (Radix->lRadixCount){
	 yText+=sZeilenAbst;
     sprintf(szBuffer, "L�nge: %2.2f Breite: %2.2f ", plRadix->rFix.dLange, plRadix->rFix.dBreite);
	 TextOut(hDC, xText, yText, szBuffer, strlen(szBuffer));
  } /* endif */

  xText=sMpX-sRadius-2*sDruber;
  yBox=yText+2*sDruber;
  yText=2*sDruber;
  MoveTo(hDC, xText-sDruber, yText);
  LineTo(hDC, sMpX+sRadius+3*sDruber, yText);
  LineTo(hDC, sMpX+sRadius+3*sDruber, yBox);
  LineTo(hDC, xText-sDruber, yBox);
  LineTo(hDC, xText-sDruber, yText);

  Radix->AstroFontRec->lfHeight = sFactor*10;
  Radix->AstroFontRec->lfWidth = sFactor*10;
  DeleteObject(SelectObject(hDC, CreateFontIndirect(Radix->AstroFontRec)));

  yText=sDruber;
  SetTextAlign( hDC, TA_RIGHT );
  xText=sMpX+sRadius+2*sDruber;
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
  LPDEVMODE  lpDevMode=0;
  LPDEVNAMES lpDevNames;
  DLGPROC    lpAbortDlg;
  DLGPROC    lpAbortProc;
  char * 	 pszDriverName;
  char * 	 pszDeviceName;
  char * 	 pszPortName;
  short 	 sRet;
  LOGPEN     lpLogPen;
  HANDLE     hFont;
  HANDLE 	 hBrush;
  HANDLE 	 hPen;
  char  	 szBuffer[BUF_SIZE];

  PRINTDLG   printDlg= {
		  		sizeof(PRINTDLG), (HWND)0, (HGLOBAL)0, (HGLOBAL)0, (HDC)0,
		 	 	PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC | PD_USEDEVMODECOPIES,
			 	0, 0, 0, 0, 1, (HINSTANCE)0, 0L, NULL, NULL, (LPCSTR)0, (LPCSTR)0,
		  		(HGLOBAL)0, (HGLOBAL)0};

/*----------------------Drucker Initialisierung--------------------------*/
  if (cMode==PD_PRINTSETUP) {
     printDlg.Flags |= PD_PRINTSETUP;
	 if (!PrintDlg((LPPRINTDLG)&printDlg))
   	    return ERR_PRINT;
     return ERR_OK;
  } /* endif */

  if (!PrintDlg((LPPRINTDLG)&printDlg))
     return ERR_PRINT;

  if (printDlg.hDC)
    hDC = printDlg.hDC;
  else {
    if (!printDlg.hDevNames)
       return ERR_PRINT;
    lpDevNames = (LPDEVNAMES)GlobalLock(printDlg.hDevNames);
    pszDriverName = (LPSTR)lpDevNames + lpDevNames->wDriverOffset;
    pszDeviceName = (LPSTR)lpDevNames + lpDevNames->wDeviceOffset;
    pszPortName = (LPSTR)lpDevNames + lpDevNames->wOutputOffset;
    GlobalUnlock(printDlg.hDevNames);
    if (printDlg.hDevMode)
       lpDevMode = (LPDEVMODE)GlobalLock(printDlg.hDevMode);
    hDC = CreateDC(pszDriverName, pszDeviceName, pszPortName, (char*)lpDevMode);
    if (printDlg.hDevMode && lpDevMode)
       GlobalUnlock(printDlg.hDevMode);
  }/*endif*/
  if (printDlg.hDevNames) {
     GlobalFree(printDlg.hDevNames);
     printDlg.hDevNames = (HGLOBAL)NULL;
  }/*endif*/
  if (printDlg.hDevMode) {
     GlobalFree(printDlg.hDevMode);
     printDlg.hDevMode = (HGLOBAL)NULL;
  }/*endif*/

  GetWindowText( Radix->hWnd, szBuffer, 31);
  lpAbortProc = (DLGPROC)MakeProcInstance((FARPROC)DlgAbortProc, hInstance);
  lpAbortDlg =  (DLGPROC)MakeProcInstance((FARPROC)DlgAbort, hInstance);
  hwndAbort = CreateDialog(hInstance, "DLGABORT", Radix->hWndMain, lpAbortDlg);
  cAbort = 0;
  if (hwndAbort)
  	 SetAbortProc(hDC, lpAbortProc);
  if (!hwndAbort||(Escape(hDC, STARTDOC, strlen(szBuffer), szBuffer, NULL) < 0)) {
    FreeProcInstance(lpAbortDlg); 
    FreeProcInstance(lpAbortProc);
    DeleteDC(hDC);
    return ERR_PRINT;
  }/*endif*/
/*----------------------Drucker Initialisierung--------------------------*/
  Escape(hDC, FLUSHOUTPUT, NULL, NULL, NULL);
//  sRet=StartPage(hDC);

  sSetViewport( hDC, Radix );
/*  if ( Radix->hBitmap ) {
	 BitBlt(hDC, sMpX-sRadius-8*sDruber, sMpY-sRadius-8*sDruber,
	 		Radix->yPrint, Radix->yPrint,
	 		Radix->hDCBit, 0, 0, SRCCOPY);
     if (!cAbort) {
        sRet=EndPage(hDC);
        sRet=EndDoc(hDC);
      }  /*endif
  
	  DestroyWindow(hwndAbort);
	  FreeProcInstance((FARPROC)DlgAbort);
	  FreeProcInstance(DlgAbortProc);
 	  return ERR_OK;
  } /* endif */

  SetBkMode(hDC, TRANSPARENT);
  hBrush=SelectObject(hDC, GetStockObject(HOLLOW_BRUSH));
  lpLogPen.lopnStyle=PS_SOLID;
  lpLogPen.lopnWidth.x=1;
  lpLogPen.lopnColor=RGB(0,0,0);
  hPen=SelectObject(hDC, CreatePenIndirect(&lpLogPen));
  Radix->AstroFontRec->lfHeight = Radix->sFactor*28;
  Radix->AstroFontRec->lfWidth = Radix->sFactor*12;
  hFont=SelectObject(hDC, CreateFontIndirect(Radix->AstroFontRec));

  sMpY=sMpY=(short)((double)Radix->yPrint/2.5);
  sMpX=Radix->xPrint/2;
//  sMpX=sMpY;
  sDruber=(short)(sMpX/DRUBERFACTOR);
  sRadius=(short)(sMpX-4*sDruber);
//	 sDruber=(short)(sMpY/DRUBERFACTOR);
//	 sRadius=(short)(sMpY-8*sDruber);
//sMpY=sMpX-4*sDruber;


/*  sPrintText(hDC, Radix, Radix->sFactor);
  SetTextAlign( hDC, TA_CENTER );
  Radix->AstroFontRec->lfHeight = Radix->sFactor*38;
  Radix->AstroFontRec->lfWidth = (short)((double)Radix->AstroFontRec->lfHeight/2.5);
  DeleteObject(SelectObject(hDC, CreateFontIndirect(Radix->AstroFontRec)));

  sRadix(hDC, Radix);

  Radix->AstroFontRec->lfHeight = Radix->sFactor*24;
  Radix->AstroFontRec->lfWidth = (short)((double)Radix->AstroFontRec->lfHeight/2.5);
  DeleteObject(SelectObject(hDC, CreateFontIndirect(Radix->AstroFontRec)));
  if ( Radix->lPaintFlag&DRW_HAUS )
     sHaus(hDC, Radix, 3*Radix->sFactor);                   	//2==Linienedicke ASC,MC

  Radix->AstroFontRec->lfHeight = Radix->sFactor*34;
  Radix->AstroFontRec->lfWidth = (short)((double)Radix->AstroFontRec->lfHeight/2.5);
  DeleteObject(SelectObject(hDC, CreateFontIndirect(Radix->AstroFontRec)));
  sRet=sPlanet(hDC, Radix, Radix->sFactor*25);
*/
  if (Radix->lRadixCount) { 
//    sDrawAspekte(hDC, (RADIX*)Radix->plRadix[Radix->lRadixCount-1], Radix, Radix->sFactor);
    sPrintAsp(hDC, (RADIX*)Radix->plRadix[Radix->lRadixCount-1], Radix->sFactor);
  } else {
//	sDrawAspekte(hDC, Radix, Radix, Radix->sFactor);
    sPrintAsp(hDC, Radix, Radix->sFactor);
  } /* endielse*/


//  if (!cAbort)
//	 Escape(hDC, NEWFRAME, NULL, NULL, NULL);
//  if (!cAbort)
//     sRet=EndPage(hDC);
//  sRet=StartPage(hDC);

//  sPrintKoord(hDC, Radix, Radix->sFactor);

  vResetColor(Radix);
  SetTextAlign( hDC, TA_LEFT );
  DeleteObject(SelectObject(hDC, hPen));
  DeleteObject(SelectObject(hDC, hBrush));
  DeleteObject(SelectObject(hDC, hFont));

  if (!cAbort) {
	 Escape(hDC, NEWFRAME, NULL, NULL, NULL);
//     sRet=EndPage(hDC);
     sRet=EndDoc(hDC);
  }  /*endif*/
  
  DestroyWindow(hwndAbort);
  FreeProcInstance((FARPROC)DlgAbort);
  FreeProcInstance(DlgAbortProc);
  DeleteDC(hDC);

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

short sSetViewport(HDC hDC, RADIX* Radix )

{

//POINT point;

//  Escape(hDC, GETSCALINGFACTOR, NULL, NULL, &point);
//  Escape(hDC, GETPRINTINGOFFSET, NULL, NULL, &point);
//  Escape(hDC, GETPHYSPAGESIZE, NULL, NULL, &point);
  Radix->xPrint = GetDeviceCaps(hDC, HORZRES);
  Radix->yPrint = GetDeviceCaps(hDC, VERTRES);
  Radix->sCol = GetDeviceCaps(hDC, PLANES);
  Radix->sCol = GetDeviceCaps(hDC, BITSPIXEL);
  Radix->sCol = GetDeviceCaps(hDC, NUMCOLORS);
  Radix->xDpi = GetDeviceCaps(hDC, LOGPIXELSX);
  Radix->yDpi = GetDeviceCaps(hDC, LOGPIXELSY);
  SetMapMode(hDC, MM_ANISOTROPIC);
  SetViewportOrg(hDC, 0, 0);
  SetWindowOrg(hDC, 0, 0);

  if (Radix->xDpi<Radix->yDpi) {
     SetViewportExt(hDC, (short)((double)Radix->xPrint*(double)Radix->xDpi/
	 							 (double)Radix->yDpi), Radix->yPrint);
     Radix->sFactor=Radix->xDpi/180;
  } else {/* endif*/
	  if (Radix->yDpi<Radix->xDpi) {
         SetViewportExt(hDC, Radix->xPrint, (short)((double)Radix->yPrint*
 							 (double)Radix->yDpi/(double)Radix->xDpi));
         Radix->sFactor=Radix->yDpi/180;
	  } else {
			SetViewportExt(hDC, Radix->xPrint, Radix->yPrint);
    	    Radix->sFactor=Radix->xDpi/=180;
	  } /* endelse*/
  } /* endelse*/
  SetWindowExt(hDC, Radix->xPrint, Radix->yPrint);
  Radix->sFactor=Radix->sFactor<=1?1:Radix->sFactor;
  if (Radix->sCol<3){
	 memcpy(&sBElement, &sElement, sizeof(COLORREF)*COL_ELEMENT);
	 memset(&sElement, 0, sizeof(COLORREF)*COL_ELEMENT);
	 memcpy(&sBColor, &sColor, sizeof(COLORREF)*COLORS);
	 memcpy(&sColor, &sMono, sizeof(COLORREF)*COLORS);
  }/* endif */

 return ERR_OK;
}


void vResetColor(RADIX* Radix)
{

 if (Radix->sCol<3) {
 	memcpy(&sColor, &sBColor, sizeof(COLORREF)*COLORS);
	memcpy(&sElement, &sBElement, sizeof(COLORREF)*COL_ELEMENT);
 } /* endif */

}