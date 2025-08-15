/*----------------------------------------------------------------------------*/
/* Autor:               JeaMY  Lee                                            */
/* Datum:               Samstag 27.01.1996                                   */
/*----------------------------------------------------------------------------*/

#include "astrouni.h"

extern HWND      hWndMain;
extern HINSTANCE hInstance;  //astrouni.c
extern AU_INIT auinit;
extern char* pszTransSel;
static char  cAbort=0;
/*----------------------------------------------------------------------------*/
/* sDrawTRad             externer Aufruf für Transit zeichnen                    */
/*----------------------------------------------------------------------------*/
short sDrawTRad(RADIX * Radix)
{ 
RADIX* plRadix=0;
char szBuffer[BUF_SIZE];

if (sDrawTText(Radix, 0, 0, 0)<ERR_OK) //letzter parameter 0: LB indizieren
	DestroyWindow(Radix->hWnd);
SendMessage(Radix->hWndLBG, LB_SETCURSEL, 0, 0);
plRadix=(RADIX*)Radix->plRadix[0];
SetWindowText(GetDlgItem(Radix->hWnd, TXT_UT_DATUM), plRadix->szDatum);
SetWindowText(GetDlgItem(Radix->hWnd, TXT_UT_BDATUM), auinit.szBDatum);

SetWindowText(GetDlgItem(Radix->hWnd, TXT_UT_TDATUM), plRadix->szDatum);
SetWindowText(GetDlgItem(Radix->hWnd, TXT_UT_TBDATUM), auinit.szBDatum);

sprintf(szBuffer, "%s %s*%s",
 	    Radix->rFix.clName?Radix->pszName:UNBEKANNT_TXT,
		Radix->rFix.clVorName?Radix->pszVorName:UNBEKANNT_TXT,
 		Radix->rFix.clBeruf?Radix->pszBeruf:UNBEKANNT_TXT);
 SetWindowText(Radix->hWnd, (LPSTR)szBuffer);

 return ERR_OK;
}

/*----------------------------------------------------------------------------*/
short sCalcMultiTransit(RADIX* Radix, short sInc)
{

 RADIX* plRadix;
 HWND   hwndAbort;
 char   szZeit[6];
 char   szDatum[11];
 DLGPROC  lpTransAbort;

 cAbort=0;
 lpTransAbort = (DLGPROC)MakeProcInstance((FARPROC)DlgTransAbort, hInstance);
 hwndAbort = CreateDialog(hInstance, "TRANSABORT", hWndMain, lpTransAbort);

 plRadix=(RADIX*)Radix->plRadix[0];
 strcpy(szZeit, plRadix->szZeit);
 strcpy(szDatum, plRadix->szDatum);
 SetWindowText(GetDlgItem(hwndAbort, TA_VON), szDatum);
 SetWindowText(GetDlgItem(hwndAbort, TA_BIS), auinit.szBDatum);

 do {
	if (cAbort)
	   return sEndTrans(hwndAbort, lpTransAbort);

	if (sIncDate(szZeit, szDatum, 1, sInc, 0)<0)
	   break;
	if (sIncDate(szZeit, szDatum, 1, sInc, 1)<0)    //1: vergleich mit datum/zeit in auinit
	   break;

	Radix->plRadix=farrealloc(Radix->plRadix, sizeof(long)*(Radix->lRadixCount+1));
	if (!Radix->plRadix)
	   return sEndTrans(hwndAbort, lpTransAbort);
	Radix->plRadix[Radix->lRadixCount]=(long)calloc(1, RA_SIZE);
	if (!Radix->plRadix[Radix->lRadixCount])
	   return sEndTrans(hwndAbort, lpTransAbort);
	plRadix=(RADIX*)Radix->plRadix[Radix->lRadixCount];
	plRadix->lTransitNum=Radix->lRadixCount;
	Radix->lRadixCount++;
	if (sInitRadixStruct(plRadix, P_TYP_TRANSIT)<0)
	   return sEndTrans(hwndAbort, lpTransAbort);
	memcpy(plRadix, (RADIX*)Radix->plRadix[0], RFIX_SIZE);
    strcpy(plRadix->szZeit, szZeit);
	strcpy(plRadix->szDatum, szDatum);
	sStrToDate(plRadix);
	sStrToTime(plRadix);
	sSetPlanetTyp(plRadix, P_TYP_TRANSIT);
	if (cAbort)
	   return sEndTrans(hwndAbort, lpTransAbort);
	SetWindowText(GetDlgItem(hwndAbort, TA_AKT), szDatum);
	if (sCalcChart(plRadix, Radix, TYP_TRANSIT)<ERR_OK)
	   return sEndTrans(hwndAbort, lpTransAbort);
 } while(1);

 DestroyWindow(hwndAbort);
 FreeProcInstance((FARPROC)lpTransAbort);

 return ERR_OK;
}

short sEndTrans(HWND hwndAbort, DLGPROC  lpTransAbort)
{

 DestroyWindow(hwndAbort);
 FreeProcInstance((FARPROC)lpTransAbort);

 return ERR_TRANSIT;
}

/*----------------------------------------------------------------------------*/
/* Datum adieren, subtrahieren
/*----------------------------------------------------------------------------*/
short sIncDate(char* szZeit, char* szDatum, short sSign, short sInc, short sCmp)
{
 short sStunde, sMinute;
 short sJahr, sMonat, sTag;
 short sBStunde, sBMinute;
 short sBJahr, sBMonat, sBTag;
 double fV, fB;

sFormatTime(szZeit, &sMinute, &sStunde);
sFormatDatum(szDatum, &sTag, &sMonat, &sJahr);
if(!sCmp){
   switch(sInc){
   case INC_MINUTEN:
		sMinute+=sSign;
		if (!sMinute||sMinute==-1) {
		   sMinute=59;
		   sprintf(szZeit, "%02d:%02d", sStunde, sMinute);
		   if (sIncDate(szZeit, szDatum, -1, INC_STUNDEN, 0)<0)
			  return ERR_INC;
           break;
        }/*endif*/
		sprintf(szZeit, "%02d:%02d", sStunde, sMinute);
		sFormatTime(szZeit, &sBMinute, &sBStunde);
		if (sMinute>sBMinute){
           sMinute=0;
		   sprintf(szZeit, "%02d:%02d", sStunde, sMinute);
		   if (sIncDate(szZeit, szDatum, 1, INC_STUNDEN, 0)<0)
			  return ERR_INC;
		}/*endif*/
   break;
   case INC_STUNDEN:
	  sStunde+=sSign;
	  if (!sStunde||sStunde==-1){
		 sMinute=59;
		 sStunde=23;
		 sprintf(szZeit, "%02d:%02d", sStunde, sMinute);
		 if (sIncDate(szZeit, szDatum, -1, INC_TAGE, 0)<0)
			  return ERR_INC;
         break;
	  }/*endif*/
	  sprintf(szZeit, "%02d:%02d", sStunde, sMinute);
	  sFormatTime(szZeit, &sBMinute, &sBStunde);
	  if (sStunde>sBStunde){
		 sMinute=0;
		 sStunde=0;
		 sprintf(szZeit, "%02d:%02d", sStunde, sMinute);
		 sprintf(szDatum, "%02d.%02d.%4d", sTag, sMonat, sJahr);
		 if (sIncDate(szZeit, szDatum, 1, INC_TAGE, 0)<0)
	        return ERR_INC;
	  }/*endif*/
   break;
   case INC_WOCHEN:
	  sTag+=(7*sSign);
	  if (!sTag){
		 sMinute=59;
		 sStunde=23;
         sTag=31;
		 sprintf(szZeit, "%02d:%02d", sStunde, sMinute);
		 sprintf(szDatum, "%02d.%02d.%4d", sTag, sMonat, sJahr);
		 if (sIncDate(szZeit, szDatum, -1, INC_MONATE, 0)<0)
			  return ERR_INC;
         break;
	  }/*endif*/
	  sprintf(szDatum, "%02d.%02d.%4d", sTag, sMonat, sJahr);
	  sFormatDatum(szDatum, &sBTag, &sBMonat, &sBJahr);
	  if (sTag>sBTag){
		 sMinute=0;
		 sStunde=0;
         sTag=1;
		 sprintf(szZeit, "%02d:%02d", sStunde, sMinute);
		 sprintf(szDatum, "%02d.%02d.%4d", sTag, sMonat, sJahr);
		 if (sIncDate(szZeit, szDatum, 1, INC_MONATE, 0)<0)
			return ERR_INC;
	  }/*endif*/
   break;
   case INC_MONATE:
	  sMonat+=sSign;
	  if (!sMonat){
		 sTag=31;
         sMonat=12;
		 sprintf(szDatum, "%02d.%02d.%4d", sTag, sMonat, sJahr);
		 if (sIncDate(szZeit, szDatum, -1, INC_JAHRE, 0)<0)
			  return ERR_INC;
		 break;
	  }/*endif*/
	  sprintf(szDatum, "%02d.%02d.%4d", sTag, sMonat, sJahr);
	  sFormatDatum(szDatum, &sBTag, &sBMonat, &sBJahr);
	  if (sMonat>sBMonat){
		 sTag=1;
         sMonat=1;
		 sprintf(szDatum, "%02d.%02d.%4d", sTag, sMonat, sJahr);
		 if(sIncDate(szZeit, szDatum, 1, INC_JAHRE, 0)<0)
           return ERR_INC;
	  }/*endif*/
   break;
   case INC_JAHRE:
	  sJahr+=sSign;
	  sprintf(szDatum, "%02d.%02d.%4d", sTag, sMonat, sJahr);
	  sFormatDatum(szDatum, &sBTag, &sBMonat, &sBJahr);
	  if (sJahr>sBJahr)
		 return ERR_INC;
	  if (sIncDate(szZeit, szDatum, 1, sInc, 1)<0)
		 return ERR_INC;
	  if (sJahr<1400)
		 return ERR_INC;
   break;
   default:
	  sTag+=sSign;
	  if (!sTag){
		 sMinute=59;
		 sStunde=23;
         sTag=31;
		 sprintf(szZeit, "%02d:%02d", sStunde, sMinute);
		 sprintf(szDatum, "%02d.%02d.%4d", sTag, sMonat, sJahr);
		 if (sIncDate(szZeit, szDatum, -1, INC_MONATE, 0)<0)
			  return ERR_INC;
		 break;
	  }/*endif*/
	  sprintf(szDatum, "%02d.%02d.%4d", sTag, sMonat, sJahr);
	  sFormatDatum(szDatum, &sBTag, &sBMonat, &sBJahr);
	  if (sTag>sBTag) {
		 sMinute=0;
		 sStunde=0;
         sTag=1;
		 sprintf(szZeit, "%02d:%02d", sStunde, sMinute);
		 sprintf(szDatum, "%02d.%02d.%4d", sTag, sMonat, sJahr);
		 if (sIncDate(szZeit, szDatum, 1, INC_MONATE, 0)<0)
			return ERR_INC;
      }/*endif*/
   break;
   }/*endswitch*/
 }/*endif*/
 else {
   sFormatTime(auinit.szBZeit, &sBMinute, &sBStunde);
   sFormatDatum(auinit.szBDatum, &sBTag, &sBMonat, &sBJahr);
   fB=(double)sBJahr*100000000+(double)sBMonat*1000000+(double)sBTag*10000+(double)sBStunde*100+(double)sBMinute;
   fV=(double)sJahr*100000000+(double)sMonat*1000000+(double)sTag*10000+(double)sStunde*100+(double)sMinute;
   if (fV>fB)
	  return ERR_INC;
   if (sCmp==2) {		//astrofil-transit
	  if(sBJahr-3>sJahr){
		 sprintf(auinit.szBDatum, "%02d.%02d.%4d", sBTag, sBMonat, sJahr+3);
		 return ERR_INC;
	  }/*endif*/
   }/*endif*/
 }/*endelse*/

return 0;
}

/*----------------------------------------------------------------------------*/
/* Listbox zeichnen
/*----------------------------------------------------------------------------*/

short sDrawTText(RADIX* Radix, DRAWITEMSTRUCT* Di, char* szBuffer, char cAction)
{
 long   dIndex;
 long   lNum, lCount=0;
 short  sA, sB, sC, sD;
 short  sX=1;
 RADIX* plRadix;

 #define MAX_ITEM   64
 #define MAX_COUNT  3*MAX_ITEM
 #define MAX_BUFFER 3*MAX_ITEM*sizeof(long) //0:beginn, 1:ende, 2:planet, aspekt, planet

 switch (cAction){
 case 0:
	 Radix->plMulti=(long*)farrealloc(Radix->plMulti, MAX_BUFFER);
	 if (!Radix->plMulti)
		return ERR_TRANSIT;
	 for (sA=0; sA<Radix->sAnzahlPlanet; sA++ ) {
	   for (sC=0; sC<ASPEKTE; sC++){
		 for (sB=0; sB<Radix->sAnzahlPlanet; sB++ ) {
		   if (!pszTransSel[sC*MAX_PLANET*(MAX_PLANET+5)+sA*(MAX_PLANET+5)+sB])
                  continue;
		   for (lNum=0; lNum<Radix->lRadixCount; lNum++){
			 plRadix=(RADIX*)Radix->plRadix[lNum];
			 if (plRadix->pszAspPlanet[sA*plRadix->sAnzahlPlanet+sB]==sC) {
				if (lCount>=MAX_COUNT*sX){
				   sX++;
				   Radix->plMulti=(long*)farrealloc(Radix->plMulti, MAX_BUFFER*sX);
				   if (!Radix->plMulti)
				      return ERR_TRANSIT;
				}/*endif*/
				Radix->plMulti[lCount]=lNum;
			    Radix->plMulti[lCount+1]=lNum;
				Radix->plMulti[lCount+2]=((long)sA|((long)sB<<8)|((long)sC<<16));
				Radix->lMulti++;
				SendMessage(Radix->hWndLBG, LB_ADDSTRING, 0, (LPARAM)lCount);
				lNum++;
				for (; lNum<Radix->lRadixCount; lNum++){
				  plRadix=(RADIX*)Radix->plRadix[lNum];
				  if (plRadix->pszAspPlanet[sA*plRadix->sAnzahlPlanet+sB]!=sC){
					 Radix->plMulti[lCount+1]=lNum>0?lNum-1:0;
					 break;
				  }/*endif*/
			    }; /*endfor*/
				if (lNum==Radix->lRadixCount)
				    Radix->plMulti[lCount+1]=-1;
				lCount+=3;
				if(lCount>65535*3)
				  return ERR_TRANSIT;
			 }; /*endif*/
		   }; /*endfor*/
		 }; /*endfor*/
		 for (sB=0; sB<MAX_HAUS; sB++ ) {
		   switch(sB){
		   case 0:
			  sD=0;
		   break;
		   case 3:
			  sD=1;
		   break;
		   case 6:
		   	  sD=2;
           break;
		   case 9:
		      sD=3;
		   break;
		   default:
		      sD=4;
		   break;
		   }/*endswitch*/
		   if (!pszTransSel[sC*MAX_PLANET*(MAX_PLANET+5)+sA*(MAX_PLANET+5)+MAX_PLANET+sD])
              continue;
		   for (lNum=0; lNum<Radix->lRadixCount; lNum++){
			 plRadix=(RADIX*)Radix->plRadix[lNum];
			 if (plRadix->pszAspHaus[sA*MAX_HAUS+sB]==sC) {
				if (lCount>=MAX_COUNT*sX){
				   sX++;
				   Radix->plMulti=(long*)farrealloc(Radix->plMulti, MAX_BUFFER*sX);
				   if (!Radix->plMulti)
					  return ERR_TRANSIT;
				}/*endif*/
				Radix->plMulti[lCount]=lNum;
			 	Radix->plMulti[lCount+1]=lNum;
				Radix->plMulti[lCount+2]=((long)sA|((long)sB<<8)|((long)sC<<16)|0x01000000);
				Radix->lMulti++;
				SendMessage(Radix->hWndLBG, LB_ADDSTRING, 0, (LPARAM)lCount);
				lNum++;
				for (; lNum<Radix->lRadixCount; lNum++){
				  plRadix=(RADIX*)Radix->plRadix[lNum];
				  if (plRadix->pszAspHaus[sA*MAX_HAUS+sB]!=sC){
					 Radix->plMulti[lCount+1]=lNum;
					 break;
				  }/*endif*/
				}; /*endfor*/
				if (lNum==Radix->lRadixCount)
				    Radix->plMulti[lCount+1]=-1;
				lCount+=3;
				if(lCount>64535*3)
				  return ERR_TRANSIT;
			 }; /*endif*/
		   }; /*endfor*/
		 }; /*endfor*/
	   }/*endfor*/
	 }/*endfor*/
 break;
 }/* endswitch*/

 return ERR_OK;
}

/*----------------------------------------------------------------------------*/
/*                 
/*----------------------------------------------------------------------------*/
/*short sSetTransWnd(HWND hDlg)
{

long dIndex;
RADIX* plRadix;
RADIX* Radix=(RADIX *)GetWindowLong((HWND)GetWindowWord(hDlg, GWW_HWNDPARENT), 0);
WORD wIndex=SendMessage(Radix->hWndLBG, LB_GETCURSEL, 0, 0);

return 1;
 dIndex=SendMessage(Radix->hWndLBG, LB_GETITEMDATA, (WPARAM)wIndex, 0);
 if (dIndex==LB_ERR)
	return 0;

 plRadix=(RADIX*)Radix->plRadix[Radix->plMulti[dIndex]];
 SetWindowText(GetDlgItem(Radix->hWnd, TXT_UT_TDATUM), plRadix->szDatum);
 if (Radix->plMulti[dIndex+1]!=-1){
    plRadix=(RADIX*)Radix->plRadix[Radix->plMulti[dIndex+1]];
	SetWindowText(GetDlgItem(Radix->hWnd, TXT_UT_TBDATUM), plRadix->szDatum);
 }else
	SetWindowText(GetDlgItem(Radix->hWnd, TXT_UT_TBDATUM), "NE");

 return 1;
}
*/
/*----------------------------------------------------------------------------*/
/* Rückläufigkeit anzeigen
/*----------------------------------------------------------------------------*/

short sRucklauf(HWND hWnd)
{
 long   lCount;
 short  sA;
 char   szDatum[22];
 WORD   wIndex;
 DWORD  dIndex;
 RADIX* plRadix;
 DLGPROC  lpRuck;

 RADIX * Radix=(RADIX *)GetWindowLong(hWnd, 0);

 wIndex=SendMessage(Radix->hWndLBG, LB_GETCURSEL, 0, 0);
 dIndex=SendMessage(Radix->hWndLBG, LB_GETITEMDATA, (WPARAM)wIndex, 0);
 if (dIndex==LB_ERR)
    dIndex=0;

 lCount=-1;
 sA=(short)(Radix->plMulti[dIndex+2]&0xFF);
 do{
   strcpy(szDatum, "N/A");
   strcpy(&szDatum[11], "N/A");
   while(++lCount<Radix->lRadixCount){
   	 plRadix=(RADIX*)Radix->plRadix[lCount];
	 if (plRadix->psPlanetTyp[sA]&P_TYP_RUCK){
	   strcpy(szDatum, plRadix->szDatum);
       break;
	 }/*endif*/
   }; /*endwhile*/
   while(++lCount<Radix->lRadixCount){
   	 plRadix=(RADIX*)Radix->plRadix[lCount];
	 if (!(plRadix->psPlanetTyp[sA]&P_TYP_RUCK)){
	   strcpy(&szDatum[11], plRadix->szDatum);
       break;
	 }/*endif*/
   }; /*endwhile*/
   lpRuck = (DLGPROC)MakeProcInstance((FARPROC)DlgRuck, hInstance);
   DialogBoxParam(hInstance, "RUCK", hWnd, lpRuck, (DWORD)szDatum);
   FreeProcInstance((FARPROC)lpRuck);
 }while(++lCount<Radix->lRadixCount); /*endwhile*/

 return ERR_OK;
}

/*abort*/
LRESULT CALLBACK DlgTransAbort(HWND hDlg, UINT message,
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
/*Rückläufigkeit  
/*----------------------------------------------------------------------------*/
LRESULT CALLBACK DlgRuck(HWND hDlg, UINT message,
					   WPARAM wParam, LPARAM lParam)
{
  char* pszDatum;

  switch (message) {
  case WM_INITDIALOG:
    pszDatum=(char*)lParam;
    SetWindowText(GetDlgItem(hDlg, TA_VON), pszDatum);
	SetWindowText(GetDlgItem(hDlg, TA_BIS), &pszDatum[11]);
	SetFocus(GetDlgItem(hDlg, PB_Ok));
	return FALSE;

  case WM_COMMAND:
	if (wParam == PB_Ok) {
	  EndDialog(hDlg, 1);
      break;
    }
    break;

  default:
	 return FALSE;
  }
  return TRUE;
}
