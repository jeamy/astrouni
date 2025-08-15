
#include "astrouni.h"

char  szJeamy[]={"(c) SUSI & JEAMY 03/1997"};
char  ELF_TXT[]={"~VIP~Version for Andreas Starchel"};
//char  ELF_TXT[]={"~ViP~Version for Elfriede Puchner"};
//char  ELF_TXT[]={"~ViP~Version for Harald 'BABA' Ibener"};
//char  ELF_TXT[]={"~VIP~Version for JeaMY Lee"};
char  MainMenuText[]={"AstroUniverse v0.04 ©1997"};
char  SSS[]={"smile#"};
char  XXX[]={"Once you have their money, you never give it back. (First Ferengi Rule of Acquisition)"};
char  YYY[]={"Huch nobHa'bogh verenganpu''e' yIvoqQu'! - Don't trust Ferengi who give back money! (Klingon Proverb)"};
char  ZZZ[]={"Live long and prosper!"};

typedef struct {
         long    lStart;
		 long    lOrben;
		 long    lTrans;
		 long    lFarben;
}SEEK;
#define  SEEK_SIZE sizeof(SEEK)

static char szOrbHaus[][4]={"ac", "ic", "dc", "mc", "Ñ"};
static char szAspNamen[][15]={"[KONJUNKTION]",
                    "[HALBSEXTIL]",
				  	"[SEXTIL]",
                    "[QUADRATUR]",
                    "[TRIGON]",
                    "[QUINCUNX]",
                    "[OPPOSITION]",
                    "[TKONJUNKTION]",
                    "[THALBSEXTIL]",
					"[TSEXTIL]",
                    "[TQUADRATUR]",
                    "[TTRIGON]",
                    "[TQUINCUNX]",
                    "[TOPPOSITION]",
					"[SKONJUNKTION]",
                    "[SHALBSEXTIL]",
					"[SSEXTIL]",
                    "[SQUADRATUR]",
                    "[STRIGON]",
                    "[SQUINCUNX]",
                    "[SOPPOSITION]"};
static   float *pfOH;
static   float *pfOP;
static   SEEK  seek;
static   COLORREF sOldColor=0;
extern   sMpY;
extern   COLORREF  sColor[COLORS]; 		   //auwurzel.h
extern   COLORREF  sMono[COLORS]; 		   //auwurzel.h
extern   COLORREF  sPColor[COLORS]; 	   //auprint.c
extern   COLORREF  sSColor[COLORS]; 	   //auwurzel.h
extern   char      szLNANamen[][2];   //auwurzel.h
extern   char      cAspektGly[][2];   
extern   LOGFONT   AstroFontRec;      //auwurzel.c
extern   short   sOrbenTyp;        //astrofil.c
extern   short   sZombie;
extern   short   sXPlanet;
extern   short   sYPlanet;
extern   short   sMpX;
extern   short   sRadius;
extern   short   sDruber;
extern   short   sOldXPl;
extern   short   sOldYPl;
extern   short   sLastOrbFocus;
extern   char *  pszTransSel;
extern   short   sButton;
extern   RECT    rectButton;
extern   RECT    rectOButton;

WNDPROC lpTextProc=0;
WNDPROC lpTextTransitProc=0;
FILE* 	  fNamen;
FILE* 	  fNamen=0;
FILE*     fEphe=0;
FILE* 	  fOrte=0;
FILE* 	  fOrben=0;
FILE*     fIni=0;
AU_INIT   auinit;
SYS       Sys;
//FILE* 	  fNotiz=0;
//FILE* 	  fDaten=0;
//normal
double  dKREIS_STZ=KREIS_STZ;
double  dKREIS_10=KREIS_10_GRAD;
double  dKREIS_3=KREIS_3_GRAD;
double  dKREIS_5=KREIS_5_GRAD;
double  dKREIS_PLANET=KREIS_PLANET;

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sSetColor(HWND hDlg, HWND hwndSB)
{ 
 short sIndex=(short)GetDlgCtrlID(hwndSB);
 COLORREF sColor;
 char  szC[4];

 if ((SendMessage(GetDlgItem(hDlg, CHB_MONO_S), BM_GETCHECK, 0, 0)&&sYPlanet<BL_HP)||
     (SendMessage(GetDlgItem(hDlg, CHB_MONO_P), BM_GETCHECK, 0, 0)&&sYPlanet>=BL_HP)){
	SetScrollPos(GetDlgItem(hDlg, SB_R), SB_CTL, sXPlanet, 1);
	SetScrollPos(GetDlgItem(hDlg, SB_G), SB_CTL, sXPlanet, 1);
	SetScrollPos(GetDlgItem(hDlg, SB_B), SB_CTL, sXPlanet, 1);
 } else
	SetScrollPos(hwndSB, SB_CTL, sXPlanet, 1);
 sColor=RGB(GetScrollPos(GetDlgItem(hDlg, SB_R), SB_CTL),
		    GetScrollPos(GetDlgItem(hDlg, SB_G), SB_CTL),
			GetScrollPos(GetDlgItem(hDlg, SB_B), SB_CTL));
 scolSetCol(sColor);
 itoa((short)((sColor&0xFF0000)>>16), szC, 10);
 SetWindowText(GetDlgItem(hDlg, EDIT_B), szC);
 itoa((short)((sColor&0xFF00)>>8), szC, 10);
 SetWindowText(GetDlgItem(hDlg, EDIT_G), szC);
 itoa((short)((sColor&0xFF)), szC, 10);
 SetWindowText(GetDlgItem(hDlg, EDIT_R), szC);
 InvalidateRect(GetDlgItem(hDlg, sYPlanet), NULL, 1);
 InvalidateRect(GetDlgItem(hDlg, TXT_NEU), NULL, 1);
// InvalidateRect(GetDlgItem(hDlg, TXT_ALT), NULL, 1);
 SendMessage(GetDlgItem(hDlg, sIndex+3),
			EM_SETSEL, 0 , (LPARAM)MAKELONG(0, 32768)			);                          //	    cInput=1;

 return ERR_OK;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sGetColor(HWND hDlg)
{
 COLORREF sColor;
 char szC[4];

 InvalidateRect(GetDlgItem(hDlg, sOldYPl), NULL, 1);
 if (sYPlanet==TXT_ALT) {
	sColor=sOldColor;
	sYPlanet=sOldYPl;
	scolSetCol(sColor);
} else
	sColor=colReturnCol(sYPlanet);
 sOldColor=sColor;
 SetScrollPos(GetDlgItem(hDlg, SB_B), SB_CTL, (short)((sColor&0xFF0000)>>16), 1);
 SetScrollPos(GetDlgItem(hDlg, SB_G), SB_CTL, (short)((sColor&0xFF00)>>8), 1);
 SetScrollPos(GetDlgItem(hDlg, SB_R), SB_CTL, (short)((sColor&0xFF)), 1);
 itoa((short)((sColor&0xFF0000)>>16), szC, 10);
 SetWindowText(GetDlgItem(hDlg, EDIT_B), szC);
 itoa((short)((sColor&0xFF00)>>8), szC, 10);
 SetWindowText(GetDlgItem(hDlg, EDIT_G), szC);
 itoa((short)((sColor&0xFF)), szC, 10);
 SetWindowText(GetDlgItem(hDlg, EDIT_R), szC);
 InvalidateRect(GetDlgItem(hDlg, TXT_ALT), NULL, 1);
 InvalidateRect(GetDlgItem(hDlg, TXT_NEU), NULL, 1);

 return ERR_OK;
}

/****************************************************************************/
COLORREF colReturnCol(short sIndex)
{

  if (sIndex<BL_HP){
	 return sSColor[sIndex-BL_HS];
  } else {
	 return sPColor[sIndex-BL_HP];
  }/*endif*/

}
/****************************************************************************/
short scolSetCol(COLORREF sCol)
{

  if (sYPlanet<BL_HP){
	 sSColor[sYPlanet-BL_HS]=sCol;
  } else {
	 sPColor[sYPlanet-BL_HP]=sCol;
  }/*endif*/

return ERR_OK;
	}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sSetMono(HWND hDlg, short sID)
{
 short sF=0;;
 if (SendMessage(GetDlgItem(hDlg, CHB_MONO_S), BM_GETCHECK, 0, 0)&&sID==CHB_MONO_S){
	 memcpy(&sSColor, &sMono, sizeof(COLORREF)*COLORS);
	 sF=1;
 }/*endif*/

 if (SendMessage(GetDlgItem(hDlg, CHB_MONO_P), BM_GETCHECK, 0, 0)&&sID==CHB_MONO_P){
	 memcpy(&sPColor, &sMono, sizeof(COLORREF)*COLORS);
	 sF=1;
 }/*endif*/
 if (sF)
	sSetOrbTxt(hDlg, DRW_FARB_ALL);

 return 1;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sSubFarbTxt(HWND hDlg)
{

 short sA;
 for (sA=BL_HP; sA<BL_HP+COLORS; sA++){
	 lpTextProc=(WNDPROC)SetWindowLong(GetDlgItem(hDlg, sA),
			GWL_WNDPROC, (LPARAM)SubFarbTxt);
 }/*endfor*/

 for (sA=BL_HS; sA<BL_HS+COLORS; sA++){
	 lpTextProc=(WNDPROC)SetWindowLong(GetDlgItem(hDlg, sA),
			GWL_WNDPROC, (LPARAM)SubFarbTxt);
 }/*endfor*/
 lpTextProc=(WNDPROC)SetWindowLong(GetDlgItem(hDlg, TXT_ALT),
		GWL_WNDPROC, (LPARAM)SubFarbTxt);

 lpTextProc=(WNDPROC)SetWindowLong(GetDlgItem(hDlg, TXT_NEU),
		GWL_WNDPROC, (LPARAM)SubFarbTxt);
 return 0;
}
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sPaintFarbTxt(HWND hDlg)
{
 HDC          hDC;
 PAINTSTRUCT  PaintInfo;
 LOGPEN       lpLogPen;
 HANDLE       hFont;
 HANDLE 	  hBrush;
 HANDLE 	  hPen;
 RECT         rect;
 COLORREF     sColor;
 short        sIndex;
 HWND         hWndP;

  BeginPaint(hDlg, &PaintInfo);
  hDC = PaintInfo.hdc;
  SaveDC(hDC);
  lpLogPen.lopnStyle=PS_SOLID;
  lpLogPen.lopnWidth.x=1;
  lpLogPen.lopnColor=RGB(0,0,0);
  hPen=SelectObject(hDC, CreatePenIndirect(&lpLogPen));
  SetBkMode(hDC, TRANSPARENT);
  GetWindowRect(hDlg, &rect);
  sIndex=(short)((GetDlgCtrlID(hDlg)));
  switch (sIndex){
  case TXT_ALT:
	 sColor=colReturnCol(sYPlanet);
  break;
  case TXT_NEU:
    hWndP=(HWND)GetWindowWord(hDlg, GWW_HWNDPARENT); 
    sColor=RGB(GetScrollPos(GetDlgItem(hWndP, SB_R), SB_CTL),
			   GetScrollPos(GetDlgItem(hWndP, SB_G), SB_CTL),
			   GetScrollPos(GetDlgItem(hWndP, SB_B), SB_CTL));
  break;
  default:
     sColor=colReturnCol(sIndex);
  break;
  }/*endswitch*/

  hBrush=SelectObject(hDC, CreateSolidBrush(sColor));
  Rectangle(hDC, 1, 1, rect.right-rect.left-2, rect.bottom-rect.top-2);
  if (sIndex>=BL_HS&&sIndex<BL_HP+COLORS&&sIndex==sYPlanet){
	 AstroFontRec.lfHeight = 18;
	 AstroFontRec.lfWidth = (short)((double)AstroFontRec.lfHeight/2.5);
	 hFont=SelectObject(hDC, CreateFontIndirect(&AstroFontRec));
	 sColor=sColor^RGB(255,255,255);
	 sPutString( hDC, "x", (((rect.right-rect.left)-AstroFontRec.lfWidth))/2,
		((rect.bottom-rect.top)-AstroFontRec.lfHeight)/2, sColor);
	 DeleteObject(SelectObject(hDC, hFont));
  }/*endif*/
  DeleteObject(SelectObject(hDC, hPen));
  DeleteObject(SelectObject(hDC, hBrush));
  RestoreDC(hDC, -1);
  EndPaint(hDlg, &PaintInfo);

 return ERR_OK;
}
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sPaintTransitTxt(HWND hDlg)
{
 HDC          hDC;
 PAINTSTRUCT  PaintInfo;
 LOGPEN       lpLogPen;
 HANDLE       hFont;
 HANDLE 	  hBrush;
 HANDLE 	  hPen;
 RECT         rect;
 short        sIndex, sY, sC;

  BeginPaint(hDlg, &PaintInfo);
  hDC = PaintInfo.hdc;
  SaveDC(hDC);
  lpLogPen.lopnStyle=PS_SOLID;
  lpLogPen.lopnWidth.x=1;
  lpLogPen.lopnColor=RGB(0,0,0);
  hPen=SelectObject(hDC, CreatePenIndirect(&lpLogPen));
  SetBkMode(hDC, TRANSPARENT);
  GetWindowRect(hDlg, &rect);
  sIndex=(short)((GetDlgCtrlID(hDlg)-TXTP_START));
  sC=(short)(sIndex/XORB);
  sY=sXPlanet+sIndex%XORB;
  hBrush=SelectObject(hDC, CreateSolidBrush(sColor[COL_HINTER]));
  Rectangle(hDC, 6, 4, rect.right-rect.left-6, rect.bottom-rect.top-4);
  if (pszTransSel[sC*MAX_PLANET*(MAX_PLANET+5)+sYPlanet*(MAX_PLANET+5)+sY]) {
	 AstroFontRec.lfHeight = 18;
	 AstroFontRec.lfWidth = (short)((double)AstroFontRec.lfHeight/2.5);
	 hFont=SelectObject(hDC, CreateFontIndirect(&AstroFontRec));
	 sPutString( hDC, "x", (((rect.right-rect.left)-AstroFontRec.lfWidth))/2,
		((rect.bottom-rect.top)-AstroFontRec.lfHeight)/2, sColor[COL_HINTER]^RGB(255,255,255));
	 DeleteObject(SelectObject(hDC, hFont));
  }/*endif*/
 
  DeleteObject(SelectObject(hDC, hPen));
  DeleteObject(SelectObject(hDC, hBrush));
  RestoreDC(hDC, -1);
  EndPaint(hDlg, &PaintInfo);

 return 1;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sPaintOrbTxt(HWND hDlg)
{
 HDC          hDC;
 PAINTSTRUCT  PaintInfo;
 LOGPEN       lpLogPen;
 HANDLE       hFont;
 HANDLE 	  hBrush;
 HANDLE 	  hPen;
 RECT         rect;
 COLORREF     sCol;
 char 	      szText[BUF_SIZE];
 short        sIndex;
 short        sC, sB;
 HWND         hWnd=(HWND)GetWindowWord(hDlg, GWW_HWNDPARENT);

  BeginPaint(hDlg, &PaintInfo);
  hDC = PaintInfo.hdc;
  SaveDC(hDC);
  AstroFontRec.lfHeight = 18;
  AstroFontRec.lfWidth = (short)((double)AstroFontRec.lfHeight/2.5);
  lpLogPen.lopnStyle=PS_SOLID;
  lpLogPen.lopnWidth.x=1;
  lpLogPen.lopnColor=RGB(0,0,0);
  hPen=SelectObject(hDC, CreatePenIndirect(&lpLogPen));
  hBrush=SelectObject(hDC, CreateSolidBrush(sColor[COL_HINTER]));
  GetWindowRect(hDlg, &rect);
  Rectangle(hDC, 0, 0, rect.right-rect.left, rect.bottom-rect.top);

  sIndex=GetDlgCtrlID(hDlg);
  if (sIndex<TXT_VerSTART) {
	 sIndex-=TXT_HorSTART;
	 if (sButton){
		if(sYPlanet<P_LILITH){
		  if(sXPlanet+sIndex<P_LILITH){
			pszTransSel[MAX_TRANS_SEL-6]=3;
			CheckDlgButton(hWnd, CHB_PLANET, 3);
		  }else{
			if(sXPlanet+sIndex<MAX_PLANET){
			  pszTransSel[MAX_TRANS_SEL-5]=3;
			  CheckDlgButton(hWnd, CHB_ASTER, 3);
			}else{
			  pszTransSel[MAX_TRANS_SEL-4]=3;
			  CheckDlgButton(hWnd, CHB_HAUS, 3);
			}/*endelse*/
		  }/*endelse*/
        }else{
		  if(sXPlanet+sIndex<P_LILITH){
			pszTransSel[MAX_TRANS_SEL-3]=3;
			CheckDlgButton(hWnd, CHB_PLANET, 3);
		  }else{
			if(sXPlanet+sIndex<MAX_PLANET){
			  pszTransSel[MAX_TRANS_SEL-2]=3;
	  		  CheckDlgButton(hWnd, CHB_ASTER, 3);
			}else{
			  pszTransSel[MAX_TRANS_SEL-1]=3;
			  CheckDlgButton(hWnd, CHB_HAUS, 3);
			}/*endelse*/
		  }/*endelse*/
		}/*endelse*/
		for(sC=0; sC<ASPEKTE; sC++){
		   pszTransSel[sC*MAX_PLANET*(MAX_PLANET+5)+(MAX_PLANET+5)*sYPlanet+sXPlanet+sIndex]=
		   !pszTransSel[sC*MAX_PLANET*(MAX_PLANET+5)+(MAX_PLANET+5)*sYPlanet+sXPlanet+sIndex];
		}/*endfor*/
	    sSetOrbTxt(hWnd, DRW_TRANS_ALL);
	 }/*endif*/
	 sCol=sColor[COL_PLAN];
	 if ((sXPlanet+sIndex)<MAX_PLANET)
		strcpy(szText, szLNANamen[sXPlanet+sIndex]);
	 else
		strcpy(szText, szOrbHaus[sXPlanet+sIndex-MAX_PLANET]);
  }else{
	if(sIndex==TXT_VerSTART) {
	   if(sButton){
		 if (sYPlanet<P_LILITH){
			pszTransSel[MAX_TRANS_SEL-6]=3;
		    pszTransSel[MAX_TRANS_SEL-5]=3;
		    pszTransSel[MAX_TRANS_SEL-4]=3;
		 }else{
		    pszTransSel[MAX_TRANS_SEL-3]=3;
		    pszTransSel[MAX_TRANS_SEL-2]=3;
		    pszTransSel[MAX_TRANS_SEL-1]=3;
         }/*endif*/
		 CheckDlgButton(hWnd, CHB_PLANET, 3);
		 CheckDlgButton(hWnd, CHB_ASTER, 3);
		 CheckDlgButton(hWnd, CHB_HAUS, 3);
	     for(sC=0; sC<ASPEKTE; sC++){
		   for(sB=0; sB<(MAX_PLANET+5); sB++){
		      pszTransSel[sC*MAX_PLANET*(MAX_PLANET+5)+(MAX_PLANET+5)*sYPlanet+sB]=
			  !pszTransSel[sC*MAX_PLANET*(MAX_PLANET+5)+(MAX_PLANET+5)*sYPlanet+sB];
 		   }/*endfor*/
	    }/*endfor*/
	    sSetOrbTxt(hWnd, DRW_TRANS_ALL);
	  }/*endif*/
	  if (!sOrbenTyp)
  	     sCol=sColor[COL_PLAN];
      else 
  	     sCol=sColor[COL_PLAN_T];
	   strcpy(szText, szLNANamen[sYPlanet]);
	} else {
	   sIndex-=(I_KON+1);
	   strcpy(szText, cAspektGly[sIndex]);
	   
	   sCol=sColor[CKONJUNKTION+sIndex];
//	   sCol=sColor[((WORD)szText[0]-197)%4+COL_EL];
	   if(sButton){
		 if (sYPlanet<P_LILITH){
		    pszTransSel[MAX_TRANS_SEL-6]=3;
		    pszTransSel[MAX_TRANS_SEL-5]=3;
		    pszTransSel[MAX_TRANS_SEL-4]=3;
		 }else{
		    pszTransSel[MAX_TRANS_SEL-3]=3;
		    pszTransSel[MAX_TRANS_SEL-2]=3;
		    pszTransSel[MAX_TRANS_SEL-1]=3;
         }/*endif*/
		 CheckDlgButton(hWnd, CHB_PLANET, 3);
		 CheckDlgButton(hWnd, CHB_ASTER, 3);
		 CheckDlgButton(hWnd, CHB_HAUS, 3);
		 for(sC=0; sC<P_LILITH; sC++){
		    pszTransSel[sIndex*MAX_PLANET*(MAX_PLANET+5)+(MAX_PLANET+5)*sYPlanet+sC]=
		    !pszTransSel[sIndex*MAX_PLANET*(MAX_PLANET+5)+(MAX_PLANET+5)*sYPlanet+sC];
		 }/*endfor*/
		 for(sC=0; sC<MAX_PLANET-P_LILITH; sC++){
			pszTransSel[sIndex*MAX_PLANET*(MAX_PLANET+5)+(MAX_PLANET+5)*sYPlanet+P_LILITH+sC]=
			!pszTransSel[sIndex*MAX_PLANET*(MAX_PLANET+5)+(MAX_PLANET+5)*sYPlanet+P_LILITH+sC];
		 }/*endfor*/
		 for(sC=0; sC<5; sC++){
		    pszTransSel[sIndex*MAX_PLANET*(MAX_PLANET+5)+(MAX_PLANET+5)*sYPlanet+MAX_PLANET+sC]=
			!pszTransSel[sIndex*MAX_PLANET*(MAX_PLANET+5)+(MAX_PLANET+5)*sYPlanet+MAX_PLANET+sC];
		 }/*endfor*/
		 sSetOrbTxt(hWnd, DRW_TRANS_ALL);
	   }/*endif*/
    }/*endelse*/
  }/*endelse*/

  SetBkMode(hDC, TRANSPARENT);
  hFont=SelectObject(hDC, CreateFontIndirect(&AstroFontRec));
  sPutString( hDC, szText, (((rect.right-rect.left)-AstroFontRec.lfWidth*strlen(szText)))/2,
	   ((rect.bottom-rect.top)-AstroFontRec.lfHeight)/2, sCol);
  DeleteObject(SelectObject(hDC, hFont));
  DeleteObject(SelectObject(hDC, hPen));
  DeleteObject(SelectObject(hDC, hBrush));
  RestoreDC(hDC, -1);
  EndPaint(hDlg, &PaintInfo);
                
 return 1;
}
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sAddOrb( HWND hDlg, short sId)
{
 char  szBuffer[BUF_SIZE];
 short sC, sY, sE;
 float fOrb=-1;
 char* psz;

 sLastOrbFocus=sId;
 sE=sId-EDITP_START;
 sC=(short)(sE/XORB);
 sY=sXPlanet+sE%XORB;

 if (sYPlanet==sY&&!sOrbenTyp)
	return 1;

 SendMessage( GetDlgItem(hDlg, sId), WM_GETTEXT, 5, (LPARAM)szBuffer);
 psz=strchr(szBuffer, ',');
 if(psz)
   *psz='.';
 fOrb=atof(szBuffer);
 sprintf(szBuffer, "%.2f", fOrb);
 SendMessage( GetDlgItem(hDlg, sId), WM_SETTEXT, 0, (LPARAM)szBuffer);

 if(sY<MAX_PLANET)
	pfOP[(sYPlanet*MAX_PLANET+sY)+sC*MAX_PLANET*MAX_PLANET]=fOrb/2;
 else {
	vSetHausOrb(sC, sYPlanet, sY, pfOH, fOrb/2);
 }/*endelse*/

 return 1;
}

short sSetOrbFocus( HWND hDlg, short sId)
{
 short sNewId;
 short sY, sE;

 sE=sId-EDITP_START;
 sY=sXPlanet+sE%XORB;

if (sYPlanet==sY&&!sOrbenTyp)
   	sNewId=sLastOrbFocus<=sId?sId+1:sId-1;
else
	sNewId=sId;
/*
 sE=0;
 if (sNewId<EDITP_START){
	if (!sXPlanet){
	   sXPlanet--;
	   sNewId=EDITP_START;
	}/*endif
	else {
	   if (!sYPlanet){
		  sYPlanet--;
   	      sNewId=EDITP_START;
	   }/*endif
       else{
		  sXPlanet=MAX_PLANET+4;
		  sYPlanet=MAX_PLANET-1;
		  sNewId=EDITP_END;
	   }/*endelse
	}/*endelse
    sE=1;
 }/*endif

 if (sNewId>EDITP_END){
	if (sXPlanet<MAX_PLANET+3){
	   sXPlanet++;
	   sNewId=EDITP_END;
	}/*endif
	else {
	   if (sYPlanet<MAX_PLANET-1){
		  sYPlanet++;
   	      sNewId=EDITP_END;
	   }/*endif
       else{
		  sXPlanet=0;
		  sYPlanet=0;
		  sNewId=EDITP_START;
	   }/*endelse
	}/*endelse
	sE=1;
 }/*endif

 if (sE)
	sSetOrbEdit(hDlg);
*/

 SetFocus(GetDlgItem(hDlg, sNewId));

 return 1;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sSetOrbEdit(HWND hDlg)
{
 short sC, sB, sA, sY, sX;
 char  szBuffer[BUF_SIZE];
 
 switch (sOrbenTyp){
 case RBP_TRANSIT-RBP_RADIX:
      pfOH=auinit.pfOrbenTHaus;
      pfOP=auinit.pfOrbenTPlanet;
 break;
 case RBP_SYNASTRIE-RBP_RADIX:
	  pfOH=auinit.pfOrbenSHaus;
	  pfOP=auinit.pfOrbenSPlanet;
 break;
 default:
	 pfOP=auinit.pfOrbenPlanet;
	 pfOH=auinit.pfOrbenHaus;
 break;
 }/*endswitch*/

 for (sC=0; sC<ASPEKTE; sC++){
     sX=0;
	 for(sA=0; sA<XORB; sA++) {
		sY=sA+sXPlanet;
		if (sY==sYPlanet&&!sOrbenTyp)
		   strcpy(szBuffer, "*");
        else {
		  if(sY<MAX_PLANET)
			sprintf(szBuffer, "%f.2",
			        pfOP[(sYPlanet*MAX_PLANET+sY)+sC*MAX_PLANET*MAX_PLANET]*2);
		  else {
			if (sX>9)
               sX=1;
			sprintf(szBuffer, "%f.2",
					pfOH[(sYPlanet*MAX_HAUS+sX)+sC*MAX_PLANET*MAX_HAUS]*2);
			sX+=3;
          }/*endif*/
	    }/*endelse*/
		SendMessage( GetDlgItem(hDlg, EDITP_START+sC*XORB+sA),
			 WM_SETTEXT, 0, (LPARAM)szBuffer);
	 }/* endfor */
 }/* endfor */
 return 1;
}
/****************************************************************************/
/*SubClass TextControl ******************************************************/
/****************************************************************************/
short sSubText(HWND hDlg)
{

 short sA;

 for (sA=0; sA<XORB; sA++){
	 lpTextProc=(WNDPROC)SetWindowLong(GetDlgItem(hDlg, TXT_HorSTART+sA),
			GWL_WNDPROC, (LPARAM)SubText);
 }/*endfor*/
 lpTextProc=(WNDPROC)SetWindowLong(GetDlgItem(hDlg, TXT_VerSTART),
    		GWL_WNDPROC, (LPARAM)SubText);
 for (sA=0; sA<=ASPEKTE; sA++){
	 lpTextProc=(WNDPROC)SetWindowLong(GetDlgItem(hDlg, I_KON+sA),
 	 		GWL_WNDPROC, (LPARAM)SubText);
 }/*endfor*/
 return ERR_OK;
}

short sSubTransitText(HWND hDlg)
{

 short sA;
 for (sA=TXTP_START; sA<TXTP_END+1; sA++){
	 lpTextTransitProc=(WNDPROC)SetWindowLong(GetDlgItem(hDlg, sA),
			GWL_WNDPROC, (LPARAM)SubTransitText);
 }/*endfor*/

 return ERR_OK;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sPaintTransRect(HWND hDlg)
{
 HDC          hDC;
 PAINTSTRUCT  PaintInfo;
 LOGPEN       lpLogPen;
 LOGBRUSH     lpLogBrush;
 HANDLE 	  hPen;
 HANDLE 	  hBrush;
 short        sI;

  BeginPaint(hDlg, &PaintInfo);
  hDC = PaintInfo.hdc;
  SaveDC(hDC);
  lpLogPen.lopnStyle=PS_SOLID;
  lpLogPen.lopnWidth.x=1;
  lpLogPen.lopnColor=RGB(192,192,192);
  hPen=SelectObject(hDC, CreatePenIndirect(&lpLogPen));
  lpLogBrush.lbStyle=BS_HOLLOW;
  lpLogBrush.lbColor=RGB(255,255,255);
  hBrush=SelectObject(hDC, CreateBrushIndirect(&lpLogBrush));
  SetROP2(hDC, R2_XORPEN);
  Rectangle(hDC, rectOButton.left, rectOButton.top, rectOButton.right, rectOButton.bottom);
  if (sButton) 
	 Rectangle(hDC, rectButton.left, rectButton.top, rectButton.right, rectButton.bottom);
  memcpy(&rectOButton, &rectButton, sizeof(RECT));
  DeleteObject(SelectObject(hDC, hBrush));
  DeleteObject(SelectObject(hDC, hPen));
  RestoreDC(hDC, -1);
  EndPaint(hDlg, &PaintInfo);
 return 1;
}
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sSetTransSel(HWND hDlg, short sTyp, short sSet)
{
short sA, sB, sC;
short sMin, sMax;

switch (sTyp) {
case SET_TRANS_NONE:
	 memset(pszTransSel, 0, MAX_TRANS_SEL);
break;
case SET_TRANS_PLANET:
	 if (sSet==3)
        break;
	 if(SendMessage(GetDlgItem(hDlg, RB_TPLANET), BM_GETCHECK, 0, 0)){
	   sMin=0;
	   sMax=P_LILITH;
	   pszTransSel[MAX_TRANS_SEL-6]=sSet;
	 } else {
	   sMin=P_LILITH;
       sMax=MAX_PLANET;
	   pszTransSel[MAX_TRANS_SEL-3]=sSet;
	 }/*endelse*/
	 for(sA=0; sA<ASPEKTE; sA++) {
		for(sB=sMin; sB<sMax; sB++) {
		   for (sC=0; sC<P_LILITH; sC++){
			   pszTransSel[sA*MAX_PLANET*(MAX_PLANET+5)+(MAX_PLANET+5)*sB+sC]=sSet;
 		   }/*endfor*/
	    }/*endfor*/
	 }/*endfor*/
break;
case SET_TRANS_ASTER:
	 if (sSet==3)
        break;
	 if(SendMessage(GetDlgItem(hDlg, RB_TPLANET), BM_GETCHECK, 0, 0)){
	   sMin=0;
	   sMax=P_LILITH;
	   pszTransSel[MAX_TRANS_SEL-5]=sSet;
	 } else {
	   sMin=P_LILITH;
       sMax=MAX_PLANET;
	   pszTransSel[MAX_TRANS_SEL-2]=sSet;
	 }/*endelse*/
	 for(sA=0; sA<ASPEKTE; sA++) {
		for(sB=sMin; sB<sMax; sB++) {
		   for (sC=0; sC<MAX_PLANET-P_LILITH; sC++){
			   pszTransSel[sA*MAX_PLANET*(MAX_PLANET+5)+(MAX_PLANET+5)*sB+P_LILITH+sC]=sSet;
		   }/*endfor*/
	   }/*endfor*/
	 }/*endfor*/
break;
case SET_TRANS_HAUS:
	 if (sSet==3)
        break;
	 if(SendMessage(GetDlgItem(hDlg, RB_TPLANET), BM_GETCHECK, 0, 0)){
	   sMin=0;
	   sMax=P_LILITH;
	   pszTransSel[MAX_TRANS_SEL-4]=sSet;
	 } else {
	   sMin=P_LILITH;
       sMax=MAX_PLANET;
	   pszTransSel[MAX_TRANS_SEL-1]=sSet;
	 }/*endelse*/
	 for(sA=0; sA<ASPEKTE; sA++) {
		for(sB=sMin; sB<sMax; sB++) {
		   for (sC=0; sC<5; sC++){
			   pszTransSel[sA*MAX_PLANET*(MAX_PLANET+5)+(MAX_PLANET+5)*sB+MAX_PLANET+sC]=sSet;
		   }/*endfor*/
	   }/*endfor*/
	 }/*endfor*/
break;
default:
	 memset(pszTransSel, 1, MAX_TRANS_SEL);
break;

}/*endswitch*/

//sSetOrbTxt(hDlg, DRW_TRANS_ALL);

return 1;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sSetOrbTxt(HWND hDlg, short sTyp)
{
 short sA;
 HWND  hWnd;

 switch(sTyp){
 case DRW_ORB_PLX:
 case DRW_TRANS_PLX:
      if (sXPlanet!=sOldXPl){
	     for (sA=0; sA<XORB; sA++){
 		     InvalidateRect(GetDlgItem(hDlg, TXT_HorSTART+sA), NULL, 1);
		 }/*endfor*/
		 sOldXPl=sXPlanet;
	  }/*endif*/
      if (sTyp==DRW_ORB_PLX)
	     sSetOrbEdit(hDlg);
      else
		 sSetOrbTxt(hDlg, DRW_TRANS_ALL);
 break;

 case DRW_ORB_PLY:
 case DRW_TRANS_PLY:
	   InvalidateRect(GetDlgItem(hDlg, TXT_VerSTART), NULL, 1);
       if (sTyp==DRW_ORB_PLY)
		  sSetOrbEdit(hDlg);
	   else
		  sSetOrbTxt(hDlg, DRW_TRANS_ALL);

 break;
 case DRW_ORB_ASP:
	  for (sA=0; sA<=ASPEKTE; sA++)
		  InvalidateRect(GetDlgItem(hDlg, I_KON+sA), NULL, 1);
 break;
 case DRW_TRANS_ALL:
	  for (sA=TXTP_START; sA<=TXTP_END; sA++)
		  InvalidateRect(GetDlgItem(hDlg, sA), NULL, 1);
 break;
 case DRW_FARB_ALL:
	  for (sA=BL_HS; sA<BL_HS+COLORS; sA++){
		  hWnd=GetDlgItem(hDlg, sA);
          if (hWnd)
			  InvalidateRect(hWnd, NULL, 1);
	  }/*endif*/
	  for (sA=BL_HP; sA<BL_HP+COLORS; sA++) {
		  hWnd=GetDlgItem(hDlg, sA);
          if (hWnd)
			  InvalidateRect(hWnd, NULL, 1);
	  }/*endif*/
	  InvalidateRect(GetDlgItem(hDlg, TXT_NEU), NULL, 1);
	  InvalidateRect(GetDlgItem(hDlg, TXT_ALT), NULL, 1);
 break;
 }/*endswitch*/
 return 1;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sSetSys(HDC hDC, short sTyp)
{

short sX, sY, sXDpi, sYDpi, sCol;
POINT point;

//  sX = GetDeviceCaps(hDC, ASPECTX);
//  sY = GetDeviceCaps(hDC, ASPECTY);
  sX = GetDeviceCaps(hDC, HORZRES);
  sY = GetDeviceCaps(hDC, VERTRES);
//  sCol = GetDeviceCaps(hDC, PLANES);
//  sCol = GetDeviceCaps(hDC, BITSPIXEL);
  sCol = GetDeviceCaps(hDC, NUMCOLORS);
  sXDpi = GetDeviceCaps(hDC, LOGPIXELSX);
  sYDpi = GetDeviceCaps(hDC, LOGPIXELSY);

  if (sTyp) {
	 Sys.xScr = sX;
	 Sys.yScr = sY;
	 Sys.ScrCol = sCol;
	 Sys.xScrDpi = sXDpi;
	 Sys.yScrDpi = sYDpi;
	 Sys.ScrMul=sXDpi<sYDpi?(double)sYDpi/(double)sXDpi:
	  						(double)sXDpi/(double)sYDpi;
	 Sys.ScrFactor=sYDpi<sXDpi?(double)sYDpi/180:(double)sXDpi/180;
//	 Sys.ScrFactor=Sys.ScrFactor<1?1:Sys.ScrFactor;
     sSetScrCol();
  }else{
//   	 Escape(hDC, GETSCALINGFACTOR, NULL, NULL, &point);
//     Escape(hDC, GETPRINTINGOFFSET, NULL, NULL, &point);
//	 Escape(hDC, GETPHYSPAGESIZE, NULL, NULL, &point);
 	 Sys.xPrn = sX;
	 Sys.yPrn = sY;
	 Sys.PrnCol = sCol;
	 Sys.xPrnDpi = sXDpi;
	 Sys.yPrnDpi = sYDpi;
	 Sys.PrnMul=sXDpi<sYDpi?(double)sYDpi/(double)sXDpi:
	  						(double)sXDpi/(double)sYDpi;
	 Sys.PrnFactor=sYDpi<sXDpi?(double)sYDpi/180:(double)sXDpi/180;
//	 Sys.PrnFactor=Sys.PrnFactor<1?1:Sys.PrnFactor;
	 if (sCol<3){
		 memcpy(&sColor, &sMono, sizeof(COLORREF)*COLORS);
	 }else{
		 memcpy(&sColor, &sPColor, sizeof(COLORREF)*COLORS);
	 }/* endelse */
  }/*endelse*/

return 1;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sSetViewport(HDC hDC, RADIX* Radix, short sTyp )
{
  short xDpi, yDpi, xPrint, yPrint;
  double dMul;

  if ( sTyp ){
	 xDpi=Sys.xScrDpi;
	 yDpi=Sys.yScrDpi;
	 xPrint=Sys.xScr;
	 yPrint=Sys.yScr;
	 dMul=Sys.ScrMul;
  }else{
	 xDpi=Sys.xPrnDpi;
	 yDpi=Sys.yPrnDpi;
	 xPrint=Sys.xPrn;
	 yPrint=Sys.yPrn;
	 dMul=Sys.PrnMul;
  }/*endif*/

  SetMapMode(hDC, MM_ANISOTROPIC);
//  SetViewportOrg(hDC, sMpX, sMpY);
//  SetWindowOrg(hDC, sMpX, sMpY);
  SetViewportOrg(hDC, 0, 0);
  SetWindowOrg(hDC, 0, 0);

  if (xDpi<yDpi) {
//	 SetViewportExt(hDC, (short)((double)xPrint*dMul), yPrint);
	 SetViewportExt(hDC, xPrint, (short)((double)yPrint*dMul));
  } else {/* endif*/
	  if (yDpi<xDpi) {
//		 SetViewportExt(hDC, xPrint, (short)((double)xPrint*dMul));
		 SetViewportExt(hDC, (short)((double)xPrint*dMul), yPrint);
	  } else {
			SetViewportExt(hDC, xPrint, yPrint);
	  } /* endelse*/
  } /* endelse*/
  SetWindowExt(hDC, xPrint, yPrint);

 return ERR_OK;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sSetXY(short * sX, short * sY)
{

 if (Sys.xScrDpi<Sys.yScrDpi) {
	 *sY=(short)((double)(*sY)*Sys.ScrMul);
  } else {
	 *sX=(short)((double)(*sX)*Sys.ScrMul);
  } /* endelse*/

 return ERR_OK;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sSetScrCol(void)
{

 if (Sys.ScrCol<3){
	 memcpy(&sColor, &sMono, sizeof(COLORREF)*COLORS);
 }else{
	 memcpy(&sColor, &sSColor, sizeof(COLORREF)*COLORS);
 }/* endelse */

 return 1;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sSetPlanetTyp(RADIX* Radix, short sTyp)
{
 short sA;

 for (sA=0; sA<Radix->sAnzahlPlanet; sA++) {
//     if(auinit.pszIgnore[sA]){
//	   Radix->psPlanetTyp[sA]=P_TYP_IGNORE;
//     } else {
	   Radix->psPlanetTyp[sA]=sTyp;
//     }/* endelse*/
 } /* endif*/
 return ERR_OK;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sStrToDate(RADIX* Radix)
{
 char szBuffer[BUF_SIZE];

 szBuffer[2]=0;
 memcpy(szBuffer, Radix->szDatum, 2 );
 Radix->rFix.sTag=atoi(szBuffer);
 memcpy(szBuffer, &Radix->szDatum[3], 2 );
 Radix->rFix.sMonat=atoi(szBuffer);
 szBuffer[4]=0;
 memcpy(szBuffer, &Radix->szDatum[6], 4 );
 Radix->rFix.sJahr=atoi(szBuffer);

 return ERR_OK;
}

short sStrToTime(RADIX* Radix)
{
 char szBuffer[BUF_SIZE];

 szBuffer[2]=0;
 memcpy(szBuffer, &Radix->szZeit, 2);
 Radix->rFix.dTime=(double)atoi(szBuffer);
 memcpy(szBuffer, &Radix->szZeit[3], 2);
 Radix->rFix.dTime+=(double)(atoi(szBuffer))/60;                  //von hh.mm nach hh.dec
 Radix->rFix.dTime-=(dDez2Deg(Radix->rFix.fZone)+
	 		  		 dDez2Deg(Radix->rFix.fSommer));

 return ERR_OK;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sGetName( RADIX* Radix,  RADIX* RadixIn, HWND hWnd, short sFlag, char * szTitel )
{
  short sRet;

 if (RadixIn->cMemory==CALC_ON_MEM) {
     RadixCopy(Radix, RadixIn, sFlag);
     auinit.Radix.cMemory=CALC_ON_FILE;
 } else {
     sRet=sReadDat( Radix, hWnd, szTitel );
	 fclose (fNamen);
	 if ( sRet<ERR_OK )
   	    return ERR_NOITEM;
 } /*endelse*/

 return ERR_OK;
} 

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
RADIX* RadixCopy( RADIX* rDes, RADIX* rSrc, char cDel )
{
WORD  wIndex;
DWORD dIndex;
RADIX* plRadix;
char  szZeit[6];
char  szDatum[11];

 if (!(cDel&MTRANS))
	memcpy(rDes, rSrc, RFIX_SIZE);

 if (cDel&MTRANS) {
	wIndex=SendMessage(rSrc->hWndLBG, LB_GETCURSEL, 0, 0);
	dIndex=SendMessage(rSrc->hWndLBG, LB_GETITEMDATA, (WPARAM)wIndex, 0);
	if (dIndex==LB_ERR)
	   dIndex=0;

	plRadix=(RADIX*)rSrc->plRadix[rSrc->plMulti[dIndex]];
	strcpy(szZeit, plRadix->szZeit);
	strcpy(szDatum, plRadix->szDatum);

	rDes=(RADIX*)calloc(1, RA_SIZE);
	if (!rDes)
	   return (RADIX*)ERR_MEM;

    if (sInitRadixStruct(rDes, TYP_RADIX)<0) 
	   return (RADIX*)ERR_MEM;

	memcpy(rDes, rSrc, RFIX_SIZE);
 	sSetPlanetTyp(rDes, P_TYP_NORM);
 	if (sCalcChart(rDes, rDes, TYP_RADIX)<ERR_OK)
	   return (RADIX*)ERR_MEM;

    rDes->plRadix=farrealloc(rDes->plRadix, sizeof(long)*(rDes->lRadixCount+1));
    if (!rDes->plRadix)
	   return (RADIX*)ERR_MEM;

    rDes->plRadix[0]=(long)calloc(1, RA_SIZE);
    if (!rDes->plRadix[0])
	   return (RADIX*)ERR_MEM;

    plRadix=(RADIX*)rDes->plRadix[0];
    plRadix->lTransitNum=0;
    rDes->lRadixCount++;
    if (sInitRadixStruct(plRadix, P_TYP_TRANSIT)<0)
	   return (RADIX*)ERR_MEM;

    if (RadixCopy(plRadix, rSrc, ALLOC_MEM)<=0)
	   return (RADIX*)ERR_MEM;

    memcpy(plRadix, (RADIX*)rSrc->plRadix[rSrc->plMulti[dIndex]], RFIX_SIZE);
    strcpy(plRadix->szZeit, szZeit);
    strcpy(plRadix->szDatum, szDatum);
    sStrToDate(plRadix);
    sStrToTime(plRadix);
    sSetPlanetTyp(plRadix, P_TYP_TRANSIT);
    plRadix->cHoroTyp=TYP_TRANSIT;
    if (sCalcChart(plRadix, rSrc, TYP_TRANSIT)<ERR_OK)
	   return (RADIX*)ERR_MEM;
    rDes->sLB^=LB_A;
    rDes->sLB^=LB_G;
 }/*endif*/

 if (cDel&MTRANS||cDel&ALLOC_MEM){
    rDes->pszName=malloc(rSrc->rFix.clName+1);
    rDes->pszVorName=malloc(rSrc->rFix.clVorName+1);
    rDes->pszBeruf=malloc(rSrc->rFix.clBeruf+1);
    rDes->pszOrt=malloc(rSrc->rFix.clOrt+1);
    if (!rDes->pszOrt||!rDes->pszBeruf||!rDes->pszVorName||!rDes->pszName)
	   return (RADIX*)ERR_MEM;
 }/*endif*/

 if (rSrc->rFix.clName)
	 strcpy(rDes->pszName, rSrc->pszName);
 if (rSrc->rFix.clVorName) 
	 strcpy(rDes->pszVorName, rSrc->pszVorName);
 if (rSrc->rFix.clBeruf) 
	 strcpy(rDes->pszBeruf, rSrc->pszBeruf);
 strcpy(rDes->pszOrt, rSrc->pszOrt);
 strcpy(rDes->szDatum, rSrc->szDatum);
 strcpy(rDes->szZeit, rSrc->szZeit);

 if(cDel&DEL_SRC) {
   if (rSrc->pszName)
	  free(rSrc->pszName);
   if (rSrc->pszVorName)
	  free(rSrc->pszVorName);
   if (rSrc->pszOrt)
	  free(rSrc->pszOrt);
   if (rSrc->pszBeruf)
	  free(rSrc->pszBeruf);
  } /* endif */

 if (cDel&MTRANS) 
	return rDes;
 else
	return (RADIX*)ERR_OK;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/*
void CMUFileOpen( HWND hWnd )
{
	OPENFILENAME ofnTemp;
	DWORD Errval;
	char buf[5];
	char Errstr[50]="GetOpenFileName returned Error #";
	char szTemp[] = "All Files (*.*)\0*.*\0Text Files (*.txt)\0*.txt\0";
    char szBuffer[BUF_SIZE];

	ofnTemp.lStructSize = sizeof( OPENFILENAME );
	ofnTemp.hwndOwner = hWnd;	
	ofnTemp.hInstance = 0;
	ofnTemp.lpstrFilter = (LPSTR)szTemp;
	ofnTemp.lpstrCustomFilter = NULL;
	ofnTemp.nMaxCustFilter = 0;
	ofnTemp.nFilterIndex = 1;
	ofnTemp.lpstrFile = (LPSTR)szBuffer;	
	ofnTemp.nMaxFile = sizeof( szBuffer );
	ofnTemp.lpstrFileTitle = NULL;
	ofnTemp.nMaxFileTitle = 0;
	ofnTemp.lpstrInitialDir = NULL;
	ofnTemp.lpstrTitle = "OPENFILENAME";
	ofnTemp.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
	ofnTemp.nFileOffset = 0;
	ofnTemp.nFileExtension = 0;
	ofnTemp.lpstrDefExt = "*";
	ofnTemp.lCustData = NULL;
	ofnTemp.lpfnHook = NULL;
	ofnTemp.lpTemplateName = NULL;

	if(GetOpenFileName( &ofnTemp ) != TRUE)
	{
		Errval=CommDlgExtendedError();
		if(Errval!=0)
		{
			sprintf(buf,"%ld",Errval);
			strcat(Errstr, buf);
			MessageBox(hWnd, Errstr,"WARNING",MB_OK|MB_ICONSTOP);
		}

	}
	InvalidateRect( hWnd, NULL, TRUE );	// Repaint to display the new name
}
*/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/*
FILE *FileOpen(char *szFile, int nFileMode)
{
  FILE *file;
  char name[255], mode[3];
  char *env;

  sprintf(mode, "r%s", nFileMode == 2 ? "b" : "");

  file = fopen(szFile, mode);
  if (file != NULL)
    return file;

/*
#ifdef ENVIRON
  /* Next look for the file in the directory indicated by the version 
  /* specific system environment variable.                           
  sprintf(name, "%s%s", ENVIRONVER, szVersionCore);
  env = getenv(name);
  if (env && *env) {
    sprintf(name, "%s%c%s", env, chDirSep, szFile);
    file = fopen(name, mode);
    if (file != NULL)
      return file;
  }

  /* Next look in the directory in the general environment variable. 
  env = getenv(ENVIRONALL);
  if (env && *env) {
    sprintf(name, "%s%c%s", env, chDirSep, szFile);
    file = fopen(name, mode);
    if (file != NULL)
      return file;
  }

  /* Next look in the directory in the version prefix environment variable. 
  env = getenv(ENVIRONVER);
  if (env && *env) {
    sprintf(name, "%s%c%s", env, chDirSep, szFile);
    file = fopen(name, mode);
    if (file != NULL)
      return file;
  }
#endif
*/
  /* Finally look in one of several directories specified at compile time. 
  sprintf(name, "%s%c%s", nFileMode == 0 ? DEFAULT_DIR :
    (nFileMode == 1 ? CHART_DIR : EPHE_DIR), chDirSep, szFile);
  file = fopen(name, mode);
  if (file == NULL && nFileMode == 1) {
    /* If the file was never found, print an error (unless we were looking 
    /* for a certain file type, e.g. the optional astrolog.dat file).      
    sprintf(name, "File '%s' not found.", szFile);
    PrintError(name);
  }
  

  return file;
}
*/
/*----------------------------------------------------------------------------*/
/* sWriteIni            Au Ini-File schreiben                                 */
/*----------------------------------------------------------------------------*/
short sWriteIni(void)
{

 fIni=fopen( INIDAT, "w+b" );
 if (!fIni) 
    return ERR_FILE;

 seek.lStart=SEEK_SIZE;
 fwrite(&seek, 1, SEEK_SIZE, fIni);
 fwrite(&auinit.sSelHaus, sizeof(short), 5, fIni);
 seek.lOrben=ftell(fIni);
//auch in bei in sOpenIni anpassen (3*sizeof(short)
 sWriteOrb(fIni);
 seek.lTrans=ftell(fIni);
 fwrite(pszTransSel, 1, MAX_TRANS_SEL, fIni);
 seek.lFarben=ftell(fIni);
 sWriteDefColor();
 rewind(fIni);
 fwrite(&seek, 1, SEEK_SIZE, fIni);
 fclose(fIni);

 return ERR_OK;
}

/*----------------------------------------------------------------------------*/
/* sReadIni            Au Ini-File lesen                                     */
/*----------------------------------------------------------------------------*/

short sReadIni(void)
{

HDC          hDC;
PAINTSTRUCT  PaintInfo;
short sRet;

sStart(ELF_TXT, strlen(ELF_TXT)+strlen(MainMenuText)+strlen(SSS));

if(!pszTransSel)
   pszTransSel=(char*)calloc(MAX_TRANS_SEL, sizeof(char));
 if (!auinit.pfOrbenPlanet)
   auinit.pfOrbenPlanet=(float*)calloc(MAX_PLANET*MAX_PLANET*ASPEKTE, sizeof(float));
 if (!auinit.pfOrbenPlanet)
   return ERR_MEM;
 if (!auinit.pfOrbenHaus)
   auinit.pfOrbenHaus=(float*)calloc(MAX_PLANET*MAX_HAUS*ASPEKTE, sizeof(float));
 if (!auinit.pfOrbenHaus)
   return ERR_MEM;
 if (!auinit.pfOrbenTPlanet)
   auinit.pfOrbenTPlanet=(float*)calloc(MAX_PLANET*MAX_PLANET*ASPEKTE, sizeof(float));
 if (!auinit.pfOrbenTPlanet)
   return ERR_MEM;
 if (!auinit.pfOrbenTHaus)
   auinit.pfOrbenTHaus=(float*)calloc(MAX_PLANET*MAX_HAUS*ASPEKTE, sizeof(float));
 if (!auinit.pfOrbenTHaus)
   return ERR_MEM;
 if (!auinit.pfOrbenSPlanet)
   auinit.pfOrbenSPlanet=(float*)calloc(MAX_PLANET*MAX_PLANET*ASPEKTE, sizeof(float));
 if (!auinit.pfOrbenSPlanet)
   return ERR_MEM;
 if (!auinit.pfOrbenSHaus)
   auinit.pfOrbenSHaus=(float*)calloc(MAX_PLANET*MAX_HAUS*ASPEKTE, sizeof(float));
 if (!auinit.pfOrbenSHaus)
   return ERR_MEM;

 fOrte=fopen( ORTDAT, "rb" );
 if (!fOrte)
    return ERR_FILE;

 fEphe=fopen( EPHDAT, "rb" );
 if (!fEphe) {
	fclose (fOrte);
    return ERR_FILE;
 } /* endif */
 fread(&auinit.Ep, EP_SIZE, 1, fEphe);

/*	fDaten=fopen( RADDAT, "r+b" );
	if (!fDaten) {
	   fDaten=fopen( RADDAT, "w+b" );
	   if (!fDaten){
          fclose (fOrte);
		  fclose (fNamen);
		  return ERR_FILE;
	   } /* endif /
 	} /* endif */


 fIni=fopen( INIDAT, "rb" );
 if (fIni) {
	fseek(fIni, 0, SEEK_END);
	sRet=ftell(fIni);
	if (!sRet||ferror(fIni))
       fIni=0;
	else
	   fseek(fIni, 0, SEEK_SET);
 } /* endif*/
 if (fIni) {
	 fread(&seek, 1, SEEK_SIZE, fIni);
	 fread(&auinit.sSelHaus, sizeof(short), 5, fIni); 
	//auch in sOpenIni anpassen (3*sizeof(short)
	 sReadIniOrb(fIni);
	 sRet=fread(pszTransSel, 1, MAX_TRANS_SEL, fIni);
	 if ( sRet!=MAX_TRANS_SEL )
		memset(pszTransSel, 1, MAX_TRANS_SEL);
	 sReadDefColor(fIni, 0, SCR_COLOR_INDEX);
	 sReadDefColor(fIni, 0, PRN_COLOR_INDEX);
	 fclose(fIni);
 } else {
	 fOrben=fopen(ORBDAT, "rt");
	 sOrben(fOrben);
	 fclose(fOrben);
	 memset(pszTransSel, 1, MAX_TRANS_SEL);
	 auinit.sSelHaus=TYP_KOCH;
	 auinit.sSelHoro=TYP_RADIX;
	 auinit.sRotRadix=ROT_ASC;
	 auinit.sAspekte=0xFFFF;
	 auinit.sTeilung=0xFFFF;
	 sReadDefColor(fEphe, 1, SCR_COLOR_INDEX);
	 sReadDefColor(fEphe, 1, PRN_COLOR_INDEX);
	 sWriteIni();
  // auinit.sRotRadix=ROT_WIDDER;
 } /* endelse*/

 sInitFont();
// if ( sIgnore()<ERR_OK )
//     return ERR_IGNORE;

 vDefTeilung();

 return ERR_OK;
}

void vDefTeilung()
{

 if((auinit.sTeilung&S_3er)&&(auinit.sTeilung&S_9er)){
   dKREIS_STZ=KREIS_STZ;
   dKREIS_10=KREIS_10_GRAD;
   dKREIS_3=KREIS_3_GRAD;
   dKREIS_5=KREIS_5_GRAD;
   dKREIS_PLANET=KREIS_PLANET;
 }else{
    if((auinit.sTeilung&S_3er)||(auinit.sTeilung&S_9er)){
	  dKREIS_STZ=KREIS3_STZ;
      dKREIS_10=KREIS3_10_GRAD;
      dKREIS_3=KREIS3_3_GRAD;
      dKREIS_5=KREIS3_5_GRAD;
	  dKREIS_PLANET=KREIS3_PLANET;
	}else{
	  dKREIS_STZ=KREIS9_STZ;
      dKREIS_10=KREIS9_10_GRAD;
      dKREIS_3=KREIS9_3_GRAD;
      dKREIS_5=KREIS9_5_GRAD;
	  dKREIS_PLANET=KREIS9_PLANET;
	}/*endelse*/
 }/*endelse*/

}

/*----------------------------------------------------------------------------*/
 short sReadIniOrb(FILE* fIni)
 {

 fread(auinit.pfOrbenPlanet, sizeof(float), MAX_PLANET*MAX_PLANET*ASPEKTE, fIni);
 fread(auinit.pfOrbenHaus, sizeof(float), MAX_PLANET*MAX_HAUS*ASPEKTE, fIni);
 fread(auinit.pfOrbenTPlanet, sizeof(float), MAX_PLANET*MAX_PLANET*ASPEKTE, fIni);
 fread(auinit.pfOrbenTHaus, sizeof(float), MAX_PLANET*MAX_HAUS*ASPEKTE, fIni);
 fread(auinit.pfOrbenSPlanet, sizeof(float), MAX_PLANET*MAX_PLANET*ASPEKTE, fIni);
 fread(auinit.pfOrbenSHaus, sizeof(float), MAX_PLANET*MAX_HAUS*ASPEKTE, fIni);

 return 1;
}

/*----------------------------------------------------------------------------*/
 short sWriteOrb(FILE* fIni)
 {

 fwrite(auinit.pfOrbenPlanet, sizeof(float), MAX_PLANET*MAX_PLANET*ASPEKTE, fIni);
 fwrite(auinit.pfOrbenHaus, sizeof(float), MAX_PLANET*MAX_HAUS*ASPEKTE, fIni);
 fwrite(auinit.pfOrbenTPlanet, sizeof(float), MAX_PLANET*MAX_PLANET*ASPEKTE, fIni);
 fwrite(auinit.pfOrbenTHaus, sizeof(float), MAX_PLANET*MAX_HAUS*ASPEKTE, fIni);
 fwrite(auinit.pfOrbenSPlanet, sizeof(float), MAX_PLANET*MAX_PLANET*ASPEKTE, fIni);
 fwrite(auinit.pfOrbenSHaus, sizeof(float), MAX_PLANET*MAX_HAUS*ASPEKTE, fIni);

 return 1;
}

/*----------------------------------------------------------------------------*/
/* sIgnore              Objekte ignorieren                                    */
/*----------------------------------------------------------------------------*/

/*
short sIgnore (void)
{

 if (!auinit.pszIgnore)
	auinit.pszIgnore=(char*)calloc(MAX_PLANET, sizeof(char));
 if (!auinit.pszIgnore)
   return ERR_MEM;

 if (!auinit.pszIgnoreHaus)
 	auinit.pszIgnoreHaus=calloc(MAX_HAUS, sizeof(char));
 if (!auinit.pszIgnoreHaus)
  	return ERR_MEM;

 // zu ignorierende Planeten einlesen....

 return ERR_OK;
}
*/
/*----------------------------------------------------------------------------*/
/* sOrben               Orben festlegen                                       */
/*----------------------------------------------------------------------------*/
short sOrben (FILE* fOrben)
{
 short sA, sPlanetA, sPlanetB;
 char* pszBuffer;
 char* psz;
 char* psz1;
 float * pfOH;
 float * pfOP;
 float fOrb;


 for (sA=0; sA<MAX_PLANET*MAX_PLANET*ASPEKTE; sA++)
	 auinit.pfOrbenPlanet[sA]=2.5;
 for (sA=0; sA<MAX_PLANET*MAX_HAUS*ASPEKTE; sA++)
   	 auinit.pfOrbenHaus[sA]=.2;
 for (sA=0; sA<MAX_PLANET*MAX_PLANET*ASPEKTE; sA++)
	 auinit.pfOrbenTPlanet[sA]=.8;
 for (sA=0; sA<MAX_PLANET*MAX_HAUS*ASPEKTE; sA++)
	 auinit.pfOrbenTHaus[sA]=.15;
 for (sA=0; sA<MAX_PLANET*MAX_PLANET*ASPEKTE; sA++)
	 auinit.pfOrbenSPlanet[sA]=2.0;
 for (sA=0; sA<MAX_PLANET*MAX_HAUS*ASPEKTE; sA++)
	 auinit.pfOrbenSHaus[sA]=.15;

 if ( fOrben ) {
    pszBuffer=calloc(1024, 1);
    if (!pszBuffer)
        return (ERR_MEM);
    for (sA=0; sA<3*ASPEKTE; sA++) {
        sPlanetA=0;
        if ( sA<ASPEKTE) {
           pfOH=auinit.pfOrbenHaus;
           pfOP=auinit.pfOrbenPlanet;
		}else{
           if (sA<2*ASPEKTE) {
              pfOH=auinit.pfOrbenTHaus;
			  pfOP=auinit.pfOrbenTPlanet;
		   } else {
			  pfOH=auinit.pfOrbenSHaus;
			  pfOP=auinit.pfOrbenSPlanet;
		   }/*endelse*/
		}/*endelse*/
		fseek(fOrben, 0, SEEK_SET);
        do{
		    fgets(pszBuffer, 1024, fOrben);
		    if (ferror(fOrben))
			   return ERR_FILE;
   		    if (feof(fOrben))
        	   break;
	        if ((pszBuffer[0]==';'))
               continue;
            vUpper(pszBuffer);
        } while(!(psz=strstr(pszBuffer, szAspNamen[sA])));
        if (!psz)
           continue;
        do{
		    fgets(pszBuffer, 1024, fOrben);
		    if (ferror(fOrben))
			   return ERR_FILE;
   		    if (feof(fOrben))
        	   break;
	        if ((pszBuffer[0]==';'))
               continue;
            pszBuffer[strlen(pszBuffer)-1]=0;
            if (sA<ASPEKTE)
	           sPlanetB=1;
            else
	           sPlanetB=0;
            psz=strchr(pszBuffer, ' ');
            do {
	           while(*psz==' ')
                  psz++;
               psz1=strchr(psz, ' ');
               if (psz1)
	              *psz1=0;
               fOrb=atof(psz);
               if ( fOrb!=-1) {
	               if (sPlanetB<MAX_PLANET)
					  pfOP[(sPlanetA*MAX_PLANET+sPlanetB)+
					  	  sA%ASPEKTE*MAX_PLANET*MAX_PLANET]=fOrb/2;
				   else
					  vSetHausOrb(sA, sPlanetA, sPlanetB, pfOH, fOrb/2);
			   } /* endif*/
               sPlanetB++;
               psz=psz1+1;
            } while(psz1&&sPlanetB<MAX_PLANET+5);/*enddo*/
            sPlanetA++;
        }while (sPlanetA<MAX_PLANET)/* enddo*/;
    }/* endfor*/
	free(pszBuffer);
 } /* endif*/

 return ERR_OK;
}

void vSetHausOrb(short sA, short sPlanetA, short sPlanetB, float* pfOH, float fOrb)
{
 short sH;

 switch (sPlanetB){
 case MAX_PLANET:	//ASC
	  pfOH[(sPlanetA*MAX_HAUS)+sA%ASPEKTE*MAX_PLANET*MAX_HAUS]=fOrb;
 break;
 case MAX_PLANET+1: //IC
      pfOH[(sPlanetA*MAX_HAUS+3)+sA%ASPEKTE*MAX_PLANET*MAX_HAUS]=fOrb;
 break;
 case MAX_PLANET+2: //DSC
      pfOH[(sPlanetA*MAX_HAUS+6)+sA%ASPEKTE*MAX_PLANET*MAX_HAUS]=fOrb;
 break;
 case MAX_PLANET+3: //MC
      pfOH[(sPlanetA*MAX_HAUS+9)+sA%ASPEKTE*MAX_PLANET*MAX_HAUS]=fOrb;
 break;
 case MAX_PLANET+4: //HAUS
      for (sH=1; sH<MAX_HAUS; sH++)
          if (sH!=3&&sH!=6&&sH!=9)
             pfOH[(sPlanetA*MAX_HAUS+sH)+sA%ASPEKTE*MAX_PLANET*MAX_HAUS]=fOrb;
 break;
 } /* enswitch*/


}
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

void vRotateRadix(RADIX* Radix, RADIX* RadixV, short sTyp)
{
 short i;
 short sRot;

 sRot=Radix->sRotate<ROT_ASC ? ROT_WIDDER :
 	  Radix->sRotate<ROT_SONNE ? ROT_ASC : ROT_SONNE; 

 switch (sRot) {

 case ROT_SONNE:
 case ROT_WIDDER:
      Radix->dStzGrad=0;
	  Radix->cStzIndex=0;
	  break;

 case ROT_ASC:
      Radix->dStzGrad=dMod(360-RadixV->pdHaus[0])/DEG2RAD;
      Radix->cStzIndex=RadixV->pszStzHaus[0];
      if (Radix->dStzGrad>0)
      	 Radix->cStzIndex++;
	  Radix->cStzIndex%=12;

	  break;
  default:
  break;
  }/* endswitch*/

  for (i=0; i<Radix->sAnzahlPlanet; i++)
	  Radix->pdPlanetRad[i]=
	  dModRad(Radix->pdPlanet[i]/DEG2RAD+Radix->dStzGrad);

  if (sTyp==TYP_RADIX){
	  for (i=0; i<MAX_HAUS; i++)
		  Radix->pdHausRad[i]=
		  dModRad(Radix->pdHaus[i]/DEG2RAD+Radix->dStzGrad);
  } /* endif*/

  while(Radix->dStzGrad>PI/6)
	  Radix->dStzGrad-=PI/6;
  Radix->lPaintFlag|=DRW_RADIX;

}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
void EndApplication(void)
{

 sWriteIni();
 if ( fEphe )
	fclose(fEphe);
 if ( fOrte )
	fclose(fOrte);
// if ( fNotiz )
//	fclose(fNotiz);
// if ( fDaten )
//	fclose(fDaten);
 if ( fNamen )
	fclose(fNamen);
// if (pszOrte)
//    free(pszOrte);
// if ( auinit.pszIgnore )
//	free( auinit.pszIgnore );
// if ( auinit.pszIgnoreHaus )
//	free( auinit.pszIgnoreHaus );
 if ( auinit.pfOrbenPlanet )
	free( auinit.pfOrbenPlanet );
 if ( auinit.pfOrbenHaus )
	free( auinit.pfOrbenHaus );
 if ( auinit.pfOrbenPlanet )
	free( auinit.pfOrbenTPlanet );
 if ( auinit.pfOrbenHaus )
	free( auinit.pfOrbenTHaus );
 if ( auinit.pfOrbenPlanet )
	free( auinit.pfOrbenSPlanet );
 if ( auinit.pfOrbenHaus )
	free( auinit.pfOrbenSHaus );
 if ( pszTransSel )
	free( pszTransSel );
}
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sAllocRadix(RADIX* Radix, short sTyp )
{
 short sAnzPl=Radix->sAnzahlPlanet;

//Aspekte u. Winkel
Radix->pdWinkelPlanet=calloc(sAnzPl*sAnzPl, sizeof(double));
if (!Radix->pdWinkelPlanet)
	return ERR_MEM;
Radix->pszAspPlanet=calloc(sAnzPl*sAnzPl, sizeof(char));
if (!Radix->pszAspPlanet)
	return ERR_MEM;
Radix->pdPlanet=calloc(sAnzPl, sizeof(double));
if (!Radix->pdPlanet)
	return ERR_MEM;
Radix->pdPlanetRad=calloc(sAnzPl, sizeof(double));
if (!Radix->pdPlanetRad)
	return ERR_MEM;
Radix->pszStzPlanet=calloc(sAnzPl, sizeof(char));
if (!Radix->pszStzPlanet)
	return ERR_MEM;
Radix->pszInHaus=calloc(sAnzPl, sizeof(char));
if (!Radix->pszInHaus)
	return ERR_MEM;
Radix->psPlanetTyp=calloc(sAnzPl, sizeof(short));
if (!Radix->psPlanetTyp)
   return ERR_MEM;
Radix->pdWinkelHaus=calloc(sAnzPl*MAX_HAUS, sizeof(double));
if (!Radix->pdWinkelHaus)
	return ERR_MEM;
Radix->pszAspHaus=calloc(sAnzPl*MAX_HAUS, sizeof(char));
if (!Radix->pszAspHaus)
	return ERR_MEM;


if (sTyp==TYP_RADIX||sTyp==P_TYP_SYNASTRIE){
	Radix->pdHaus=calloc(MAX_HAUS, sizeof(double));
	if (!Radix->pdHaus)
		return ERR_MEM;
	Radix->pdHausRad=calloc(MAX_HAUS, sizeof(double));
	if (!Radix->pdHausRad)
		return ERR_MEM;
	Radix->pszStzHaus=calloc(MAX_HAUS, sizeof(char));
	if (!Radix->pszStzHaus)
		return ERR_MEM;
} /* endif */

 
/* Radix->pdPlanetAlt=calloc(sAnzPl, sizeof(double));
 if (!Radix->pdPlanetAlt)
	return ERR_MEM;
 Radix->pdPlanetRet=calloc(sAnzPl, sizeof(double));
 if (!Radix->pdPlanetRet)
	return ERR_MEM;
*/

 return ERR_OK;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
void vGetDate(HWND hDlg, short sAction)
{
 struct date date;
 char szDatum[32], szZeit[6]={"12:00"};
 short sTag, sMonat, sJahr;

 SendMessage(GetDlgItem(hDlg, ETXT_TDatum), WM_GETTEXT, 11, (LPARAM)szDatum);
 szDatum[11]=0;
 sFormatDatum(szDatum, &sTag, &sMonat, &sJahr);
 switch (sAction) {
 case 1:
	if (sIncDate(szZeit, szDatum, 1, INC_MONATE, 0)<0)
	   sprintf(szDatum, "%02d.%02d.%4d", sTag, sMonat, sJahr);
 break;
 case 2:
	sJahr++;
    sprintf(szDatum, "%02d.%02d.%4d", sTag, sMonat, sJahr);
	sFormatDatum(szDatum, NULL, NULL, NULL);
 break;
 case 4:
   getdate(&date);
   sprintf(szDatum, "%02d.%02d.%4d", date.da_day, date.da_mon, date.da_year);
   SendMessage( GetDlgItem(hDlg, ETXT_TDatum), WM_SETTEXT, 0, (LPARAM)szDatum);
 break;
 }/*enswitch*/
 SendMessage( GetDlgItem(hDlg, ETXT_TBDatum), WM_SETTEXT, 0, (LPARAM)szDatum);

}

/****************************************************************************/
/*vUpper( char* )                                                           */
/****************************************************************************/
void vUpper(char* szBuffer)
{
short sLen=strlen(szBuffer);

do{
  szBuffer[sLen-1]=toupper(szBuffer[sLen-1]);
}while(--sLen);

}

/****************************************************************************/
/*vUpper( char* )                                                           */
/****************************************************************************/
void vLower(char* szBuffer)
{
short sLen=strlen(szBuffer);

do{
  szBuffer[sLen-1]=tolower(szBuffer[sLen-1]);
}while(--sLen);

}

/****************************************************************************/
/* sEpheFilePos(  )                                                          */
/****************************************************************************/

short sEpheFilePos(double dJd, short sTyp)
{
  short sFilenr;
  long  lPosit, lJDlong;

  lJDlong = (long)floor(dJd);
  sFilenr = (short)(lJDlong / E_DAYS_PER_FILE);
//  if (jlong < 0 && filenr * EPHE_DAYS_PER_FILE != jlong)
//	filenr--;
  lPosit = lJDlong - sFilenr * E_DAYS_PER_FILE;

  switch (sTyp) {
  case   E_OUTER:
         if (sFilenr<auinit.Ep.cPlanetS||sFilenr>auinit.Ep.cPlanetE)
	         return ERR_EPHEM;
         lPosit = (lPosit / (short)E_STEP) * E_OUTER_BSIZE;
         lPosit+= (sFilenr-auinit.Ep.cPlanetS)*E_OUTER_FSIZE;
		 lPosit+= auinit.Ep.lPlanetI;
  break;
  case   E_ASTER:
         if (sFilenr<auinit.Ep.cAsterS||sFilenr>auinit.Ep.cAsterE)
	         return ERR_EPHEM;
         lPosit = (lPosit / (short)E_STEP) * E_ASTER_BSIZE;
         lPosit+= (sFilenr-auinit.Ep.cAsterS)*E_ASTER_FSIZE;
		 lPosit+= auinit.Ep.lAsterI;
  break;
  case   E_CHIRON:
         if (sFilenr<auinit.Ep.cChironS||sFilenr>auinit.Ep.cChironE)
	         return ERR_EPHEM;
         lPosit = (lPosit / (short)E_STEP) * E_CHIRON_BSIZE;
         lPosit+= (sFilenr-auinit.Ep.cChironS)*E_CHIRON_FSIZE;
         lPosit+= auinit.Ep.lChironI;
  break;
  } /* endswitch*/

  if (!fseek (fEphe, lPosit, SEEK_SET))
	return ERR_OK;

  return ERR_EPHEM; 
}

/*
int AddDay(month, day, year, delta)
int month, day, year, delta;
{
  int d;

  d = day + delta;
  if (year == yeaJ2G && month == monJ2G) {     /* Check for Julian to  
    if (d > dayJ2G1 && d < dayJ2G2)            /* Gregorian crossover. 
      d += NSgn(delta)*(dayJ2G2-dayJ2G1-1);
  }
  return d;
}
*/



/****************************************************************************/
/* sFormatTime																*/	
/****************************************************************************/

short sFormatTime(char* pszTime,  short* sMinute, short * sStunde)
{

 char  szT[6];
 char  szD[5];
 short sA, sD, sE, sLen=strlen(pszTime);
 short sM=0;
 short sH=0;
 char *psz=pszTime;
 struct time time;
 memset(szT, 0, 6);

 if (!sLen){
	 gettime(&time);
	 sprintf(pszTime, "%02d:%02d", time.ti_hour, time.ti_min);
	 return ERR_OK;
 } /* endif*/
 else
    szT[2]=':';

 sE=0;
 for (sA=0; sA<sLen; sA++){
     if (!isdigit(pszTime[sA])) {
        pszTime[sA]=0;
		sD=atoi(psz);
		if (sD||(*psz)=='0') {
           sprintf(szD, "%02d", sD);
           memcpy(&szT[sE], szD, 2);
           sE+=3;
 	    } /* endif*/
        psz=&pszTime[sA]+1;
	 } /* endif*/
 } /* endfor*/
 pszTime[sA]=0;

 if ( sE<6) {
 	sD=atoi(psz);
	sprintf(szD, "%02d", sD);
	memcpy(&szT[sE], szD, 2);
 }/* endif*/

 strcpy(pszTime, szT);
 szT[2]=0;
 memcpy(szT, pszTime, 2 );
 sH=atoi(szT);
 memcpy(szT, &pszTime[3], 2);
 sM=atoi(szT);
 while(sM>59) {
      sM-=60;
      sH++;
 }/* endwhile*/
 if (sH>23)
    sH=0;

 sprintf(pszTime, "%02d:%02d", sH, sM);
 if ( sStunde )
    *sStunde=sH;
 if ( sMinute )
    *sMinute=sM;

 return ERR_OK;
}

/****************************************************************************/
/* sFormatDatum																*/	
/****************************************************************************/

short sFormatDatum(char* pszDatum, short * sTag, short* sMonat, short* sJahr)
{

 #define JJUL2GREG  1582
 #define MJUL2GREG    10
 #define DJUL2GREG    15-4-1

 char  szT[11];
 char  szD[5];
 short sA, sE, sLen=strlen(pszDatum);
 short sD, sM, sJ;
 char *psz=pszDatum;
 struct date date;

 getdate(&date);
 sprintf(szT, "%02d.%02d.%4d", date.da_day, date.da_mon, date.da_year);

 sE=0;
 for (sA=0; sA<sLen; sA++){
     if (!isdigit(pszDatum[sA])) {
        pszDatum[sA]=0;
        sD=atoi(psz);
        if (sD) {
           sprintf(szD, "%02d", sD);
           memcpy(&szT[sE], szD, 2);
           sE+=3;
 	    } /* endif*/
        psz=&pszDatum[sA]+1;
	 } /* endif*/
 } /* endfor*/
 pszDatum[sA]=0;

 sD=atoi(psz);
 if (!sD&&!isdigit(*(psz-1)))
    sD=atoi(&szT[sE-3]);
 if (sD>0) {
	 switch(sE){
	 case 0:
	 case 3:
		  sprintf(szD, "%02d", sD);
     	  memcpy(&szT[sE], szD, 2);
	 break;
	 default:
    	  if (sD<99)
	         sD+=1900;
          if (sD<1600)
             sD=1600;
          if (sD>2400)
             sD=2400;
	      sprintf(szD, "%04d", sD);
	      memcpy(&szT[6], szD, 4);
     break;
	 }/*endswitch*/
 }/*endif*/

 strcpy(pszDatum, szT);
 szT[2]=0;
 memcpy(szT, pszDatum, 2 );
 sD=atoi(szT);
 memcpy(szT, &pszDatum[3], 2);
 sM=atoi(szT);
 szT[4]=0;
 memcpy(szT, &pszDatum[6], 4);
 sJ=atoi(szT);
 if (sM>12)
    sM=date.da_mon;

 if ((sM==4||sM==6||sM==9||sM==11)&&sD>30) {
    sD=30;
 } else {
 	if (sM==2){
	   if (sD>28) {
	      if (!(sJ%4)&&((sJ%100)||!(sJ%400)||(sJ<=JJUL2GREG)))
             sD=29;
          else
             sD=28;
       }/*endif*/
	} else {
       if (sD>31)
	      sD=31;
    } /*endelse*/
 }/* endif*/

/* if (sJ==JJUL2GREG&&sM==MJUL2GREG)
    sD-=DJUL2GREG;
*/
 sprintf(pszDatum, "%02d.%02d.%4d", sD, sM, sJ);
 if ( sJahr )
    *sJahr=sJ;
 if ( sMonat )
	*sMonat=sM;
 if ( sTag )
	*sTag=sD;

 return ERR_OK;
}
/****************************************************************************/
/* sSetRadKoor 																*/
/****************************************************************************/

short sSetRadKoor(RADIX* tRad, ORTE* stOrt)
{

 tRad->rFix.clOrt=stOrt->slOrt;
 tRad->rFix.dLange=-stOrt->dLange;
 tRad->rFix.dBreite=stOrt->dBreite;
 tRad->rFix.dBreite = min(tRad->rFix.dBreite, 89.9999);        
 tRad->rFix.dBreite = max(tRad->rFix.dBreite, -89.9999);       
 tRad->rFix.fZone=stOrt->fZone;
 strcpy(tRad->rFix.szLand, stOrt->szLand);

 return 0;
}

/*----------------------------------------------------------------------------*/
/* formal Parameters: char * szText Buffer to decrypt                         */
/*                    short  sLen   length of text                            */
/*----------------------------------------------------------------------------*/

short sStart( char * szText, short sLen )
{

 char    cSub=64;
 short   sI=0;

 sLen--;
 while ( sLen-- ){
   if ( !szText[sI] ) {
     sI++;
     sLen++;
     continue;
   } /* endif */

   szText[sI]-=cSub;
   cSub<<=1;
   if ( !cSub )
     cSub=1;
   sI++;
 } /* endwhile */
 szText[sI]=0;

 return 1;
}

/*----------------------------------------------------------------------------*/
/* grad in einen string umwandeln: 00°00' */
/*----------------------------------------------------------------------------*/
void vGrad2Str(float fLGrad, char * szBuffer, short sTyp, short sChar)
{
 double fL=(fabs(fLGrad)-floor(fabs(fLGrad)))*100;
 char szC[2]={"Ù"};

 if (sChar) 
	szC[0]='°';

 if (fLGrad<0) {
	sprintf(szBuffer, "%3.f%s%02.f'%s", fabs(fLGrad), szC, fL, sTyp?"O":"S");
 } else {
	if (fLGrad>0)
	   sprintf(szBuffer, "%3.f%s%02.f'%s", fabs(fLGrad), szC, fL, sTyp?"W":"N");
	else
	   sprintf(szBuffer, "%3.f%s%02.f'", fLGrad, szC, fL);
 }/*endif*/
}

/*----------------------------------------------------------------------------*/
/* INI-file öffnen verchiedene ini-ddaten schreiben
/*----------------------------------------------------------------------------*/
short sOpenIni(short sSeek)
{

 fIni=fopen( INIDAT, "r+b" );
 if (!fIni) 
	 return 0;

 switch(sSeek){
 case SEEK_ORB_DEF:
	  fseek(fIni, 0, SEEK_END);
	  if (!ftell(fIni)||ferror(fIni))
	     fIni=0;
	  if ( fIni) {
		 fseek(fIni, seek.lOrben, SEEK_SET);
      	 sReadIniOrb(fIni);
		 fclose(fIni);
         break;
	  }/*endif*/
	  else 
		 return 0;

 case SEEK_ORB_OK:
	  if (fseek(fIni, seek.lOrben, SEEK_SET))
         return ERR_SEEK;
	  sWriteOrb(fIni);
	  fclose(fIni);
      break;

 case SEEK_TRANS_OK:
	  if (fseek(fIni, seek.lTrans, SEEK_SET))
         return ERR_SEEK;
	  fwrite(pszTransSel, 1, MAX_TRANS_SEL, fIni);
	  fclose(fIni);
      break;

 case SEEK_TRANS_DEF:
	  fseek(fIni, 0, SEEK_END);
	  if (!ftell(fIni)||ferror(fIni))
	     fIni=0;
	  if ( fIni) {
		 if (fseek(fIni, seek.lTrans, SEEK_SET))
            return ERR_SEEK;
		 fread(pszTransSel, 1, MAX_TRANS_SEL, fIni);
		 fclose(fIni);
         break;
	  }/*endif*/
	  else {
		 memset(pszTransSel, 1, MAX_TRANS_SEL);
		 return 0;
      }/*endelse*/

 case SEEK_SEL_OK:
	  if (fseek(fIni, seek.lStart, SEEK_SET))
		 return ERR_SEEK;
	  fwrite(&auinit.sSelHaus, sizeof(short), 5, fIni);
	  fclose(fIni);
 	  vDefTeilung();
	  break;

 case SEEK_SEL_DEF:
	  if (fseek(fIni, seek.lStart, SEEK_SET))
		 return ERR_SEEK;
	  fread(&auinit.sSelHaus, sizeof(short), 5, fIni);
	  fclose(fIni);
	  vDefTeilung();
	  break;
 }/*endswitch*/
 
 return 1;
}

/*----------------------------------------------------------------------------*/
/* Default Farben lesen/schreiben                  
/*----------------------------------------------------------------------------*/
short sReadDefColor(FILE* fR, short sTyp, long lWhere)
{
 short sSize;			//sWhat 0 oder sizeof(COLORREF)*COLORS

 if (!sTyp) {
	if (fseek (fR, seek.lFarben+lWhere, SEEK_SET))
     	return ERR_EPHEM;
 }else{
	if (fseek (fR, auinit.Ep.lDefault+lWhere, SEEK_SET))
    	return ERR_EPHEM;
 }/*endif*/

 if (!lWhere) {
	sSize=fread(sSColor, sizeof(COLORREF), COLORS, fR);
	if (sSize!=COLORS)
	   return ERR_EPHEM;
	if (fseek (fR, seek.lFarben+sizeof(COLORREF)*COLORS, SEEK_SET))
     	return ERR_EPHEM;
 }else{
	 sSize=fread(sPColor, sizeof(COLORREF), COLORS, fR);
	 if (sSize!=COLORS)
		return ERR_EPHEM;
 }/*endelse*/
 sSize=fread(sMono, sizeof(COLORREF), COLORS, fR);
 if (sSize!=COLORS)
	return ERR_EPHEM;

 return ERR_OK;
}

short sWriteDefColor(void)
{
 short sSize;

 if (fseek (fIni, seek.lFarben, SEEK_SET))
   	return ERR_EPHEM;

 sSize=fwrite(sSColor, sizeof(COLORREF), COLORS, fIni);
 if (sSize!=COLORS)
	return ERR_EPHEM;
 sSize=fwrite(sPColor, sizeof(COLORREF), COLORS, fIni);
 if (sSize!=COLORS)
	return ERR_EPHEM;
 sSize=fwrite(sMono, sizeof(COLORREF), COLORS, fIni);
 if (sSize!=COLORS)
	return ERR_EPHEM;

 return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/* Radixwindow Scrollbar und Editfelder setzen
/*----------------------------------------------------------------------------*/

void vSetRadixScrollIni(RADIX* Radix, HWND hDlg)
{

 RADIX*  plRadix=Radix;
 short sMin, sStu, sTag, sMon, sJahr;
 char szC[11];

 if (Radix->lRadixCount) {
	plRadix=(RADIX*)Radix->plRadix[0];
 }/*endelse*/

 sFormatTime(plRadix->szZeit, &sMin, &sStu);
 sFormatDatum(plRadix->szDatum, &sTag, &sMon, &sJahr);
/* SetScrollRange(GetDlgItem(hDlg, SC_MIN), SB_CTL, 0, 59, 0);
 SetScrollPos(GetDlgItem(hDlg, SC_MIN), SB_CTL, sMin, 1);
 SetScrollRange(GetDlgItem(hDlg, SC_STU), SB_CTL, 0, 23, 0);
 SetScrollPos(GetDlgItem(hDlg, SC_STU), SB_CTL, sStu, 1);
 SetScrollRange(GetDlgItem(hDlg, SC_TAG), SB_CTL, 0, 31, 0);
 SetScrollPos(GetDlgItem(hDlg, SC_TAG), SB_CTL, sTag, 0);
 SetScrollRange(GetDlgItem(hDlg, SC_MON), SB_CTL, 0, 12, 0);
 SetScrollPos(GetDlgItem(hDlg, SC_MON), SB_CTL, sMon, 1);
 SetScrollRange(GetDlgItem(hDlg, SC_JAH), SB_CTL, 1600, 2400, 0);
 SetScrollPos(GetDlgItem(hDlg, SC_JAH), SB_CTL, sJahr, 1);
*/
 sprintf(szC, "%02d", sMin);
 SetWindowText(GetDlgItem(hDlg, ED_MIN), szC);
 sprintf(szC, "%02d", sStu);
 SetWindowText(GetDlgItem(hDlg, ED_STU), szC);
 sprintf(szC, "%02d", sTag);
 SetWindowText(GetDlgItem(hDlg, ED_TAG), szC);
 sprintf(szC, "%02d", sMon);
 SetWindowText(GetDlgItem(hDlg, ED_MON), szC);
 sprintf(szC, "%4d", sJahr);
 SetWindowText(GetDlgItem(hDlg, ED_JAH), szC);
/* sTag=32;
 sprintf(szC, "%02d.%02d.%4d", sTag, sMon, sJahr);
 sFormatDatum(szC, &sTag, &sMon, &sJahr);
 SetScrollRange(GetDlgItem(hDlg, SC_TAG), SB_CTL, 0, sTag, 1);
*/

}/*endif*/


short sSetRadixScroll(RADIX* Radix, HWND hDlg, HWND hWnd, short sSign)

{

 short sIndex=(short)GetDlgCtrlID(hWnd);
 short sMin, sStu, sTag, sMon, sJahr, sTyp, sRx, sRy;
 short sMpXX=sMpX;
 short sMpYY=sMpY;
 RADIX* plRadix=Radix;
// RECT   rect;
 HRGN  hRgn;


 if (Radix->lRadixCount) {
	plRadix=(RADIX*)Radix->plRadix[0];
 }/*endelse*/

 sprintf(auinit.szBDatum, "31.12.2400", 0, 0, 0);
 sprintf(auinit.szBZeit, "23:59", 0, 0, 0);
 sFormatTime(plRadix->szZeit, &sMin, &sStu);
 sFormatDatum(plRadix->szDatum, &sTag, &sMon, &sJahr);

 switch(sIndex){
 case SC_MIN:
    if(sSign)
	  sIncDate(plRadix->szZeit, plRadix->szDatum, sSign, INC_MINUTEN, 0);
	else
	  sGetWindowText(hDlg, ED_MIN, &sMin);
 break;
 case SC_STU:
	if(sSign)
	  sIncDate(plRadix->szZeit, plRadix->szDatum, sSign, INC_STUNDEN, 0);
	else
	  sGetWindowText(hDlg, ED_STU, &sStu);
 break;
 case SC_TAG:
	if(sSign)
	  sIncDate(plRadix->szZeit, plRadix->szDatum, sSign, INC_TAGE, 0);
	else
	  sGetWindowText(hDlg, ED_TAG, &sTag);
 break;
 case SC_MON:
	if(sSign)
	  sIncDate(plRadix->szZeit, plRadix->szDatum, sSign, INC_MONATE, 0);
	else
	  sGetWindowText(hDlg, ED_MON, &sMon);
 break;
 case SC_JAH:
	if(sSign)
	  sIncDate(plRadix->szZeit, plRadix->szDatum, sSign, INC_JAHRE, 0);
	else
	  sGetWindowText(hDlg, ED_JAH, &sJahr);
 break;
 }/*endswitch*/

 if(sIncDate(plRadix->szZeit, plRadix->szDatum, sSign, INC_JAHRE, 1)<0
		||sJahr<1600||!sSign){
	sprintf(plRadix->szDatum, "%02d.%02d.%4d", sTag, sMon, sJahr);
	sprintf(plRadix->szZeit, "%02d:%02d", sStu, sMin );
 }/*endif*/
 vSetRadixScrollIni(Radix, hDlg);

 if (Radix->lRadixCount) {
	sTyp=TYP_TRANSIT;
	sSetPlanetTyp(plRadix, P_TYP_TRANSIT);
	sStartCalc(hDlg, TRANSIT_CALC_TXT);
 }else{
	sTyp=TYP_RADIX;
	sSetPlanetTyp(Radix, P_TYP_NORM);
	sStartCalc(hDlg, RADIX_CALC_TXT);
 }/*endelse*/
 
 sStrToDate(plRadix);
 sStrToTime(plRadix);
 if (sCalcChart(plRadix, Radix, sTyp)<ERR_OK){
	sEndCalc();
	return ERR_RADIX;
 } /* endif */
 sEndCalc();
 Radix->lPaintFlag^=DRW_REFRESH;
// rect.left=rect.top=1;
// rect.right=rect.bottom=2*sMpX-2;
// InvalidateRect(Radix->hWnd, &rect, 1);

 sRx=sRy=sRadius+sDruber;
 sSetXY(&sRx, &sRy);
 sSetXY(&sMpXX, &sMpYY);
 hRgn=CreateEllipticRgn( sMpXX-sRx, sMpYY-sRy, sMpXX+sRx, sMpYY+sRy );
 InvalidateRgn(Radix->hWnd, hRgn, 1);
 DeleteObject(hRgn);

 return ERR_OK;
}

short sGetWindowText(HWND hDlg, short sID, short* sAdd)

{

 char sz[11];

 GetWindowText(GetDlgItem(hDlg, sID), sz, 5);
 (*sAdd)=atoi(sz);
// itoa((*sAdd), sz, 10);
// SetWindowText(GetDlgItem(hDlg, sID), sz);

 return ERR_OK;
}
