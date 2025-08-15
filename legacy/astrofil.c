#include "astrouni.h"

extern  HINSTANCE hInstance; //astrouni.c
extern  HWND      hWndMain;
extern  FILE* 	  fEphe;
extern  FILE* 	  fOrte;
extern  FILE* 	  fNamen;
extern  FILE* 	  fOrben;
extern  FILE* 	  fIni;
//extern  FILE* 	  fNotiz;
//extern  FILE* 	  fDaten;
extern  char 	  szHausTyp[][14]; //auprint.c
extern  char 	  szHoroTyp[][10]; //auprint.c
extern  AU_INIT   auinit;//auwurzel.c
extern  short     sMpX, sMpY, sRadius;
extern  short     sLastOrbFocus;
extern  WNDPROC   lpTextProc;
extern  WNDPROC   lpTextTransitProc;
extern  SYS       Sys;

static  RADIX* 	tRad;
static  ORTE    stOrte;
static  char    szOrte[BUF_SIZE];
static  char * 	pszBuf=0;
static  long *  plNamen=0;
static  short   sIndex=0;
//static  char    cInput=0;
static  WNDPROC lpEditProc=0;
static  DLGPROC lpCalcProc=0;
static  HWND    hWndCalc=0;
static  short   sXTPlanet=0;
static  short   sYTPlanet=0;

char *  pszBuffer=0;
WNDPROC lpLbProc=0;
short   sOrbenTyp=0;
short   sZombie=1;
short   sXPlanet=0;
short   sYPlanet=0;
short   sOldXPl=-1;
short   sOldYPl=-1;
short   sLastOrbFocus=-1;
char  * pszTransSel=0;
short   sButton=0;
RECT    rectButton;
RECT    rectOButton;

/****************************************************************************/
/* Person hinzufügen           											    */
/****************************************************************************/
short sAdd( HWND hDlg, short sID )
{
short  sRet, sC;
char   szBuffer[BUF_SIZE];

switch (sID) {
   case ETXT_Name:
		tRad->rFix.clName=SendMessage( GetDlgItem(hDlg, ETXT_Name),
										 WM_GETTEXTLENGTH, 0, 0L);
 		tRad->pszName=realloc( tRad->pszName, tRad->rFix.clName+1 );
		if (!tRad->pszName) {
	   	   MessageBox(hDlg, "Speicherfehler:(", 0, MB_OK);
		   return ERR_MEM;
        } /* endif*/
		SendMessage(GetDlgItem(hDlg, ETXT_Name),
		   		    WM_GETTEXT, tRad->rFix.clName+1,
					(LPARAM)tRad->pszName);
		if (tRad->rFix.clName){
//		    cInput=1;
		    vLower(tRad->pszName);
    		tRad->pszName[0]=toupper(tRad->pszName[0]);
		    SendMessage(GetDlgItem(hDlg, ETXT_Name), WM_SETTEXT,
							0, (LPARAM)tRad->pszName);
		}/*endif*/
  break;

   case ETXT_Vorname:
		tRad->rFix.clVorName=SendMessage( GetDlgItem(hDlg, ETXT_Vorname),
										 WM_GETTEXTLENGTH, 0, 0L);
		tRad->pszVorName=realloc( tRad->pszVorName, tRad->rFix.clVorName+1 );
		if (!tRad->pszVorName) {
	   	   MessageBox(hDlg, "Speicherfehler:(", 0, MB_OK);
		   return ERR_MEM;
        } /* endif*/
		SendMessage(GetDlgItem(hDlg, ETXT_Vorname),
					WM_GETTEXT, tRad->rFix.clVorName+1,
					(LPARAM)tRad->pszVorName);
	    if (tRad->rFix.clVorName){
//		    cInput=1;
		    vLower(tRad->pszVorName);
    		tRad->pszVorName[0]=toupper(tRad->pszVorName[0]);
		    SendMessage(GetDlgItem(hDlg, ETXT_Vorname), WM_SETTEXT,
							0, (LPARAM)tRad->pszVorName);
        } /* endif*/
   break;

   case ETXT_Beruf:
		tRad->rFix.clBeruf=SendMessage( GetDlgItem(hDlg, ETXT_Beruf),
										 WM_GETTEXTLENGTH, 0, 0L);
		tRad->pszBeruf=realloc( tRad->pszBeruf, tRad->rFix.clBeruf+1 );
		if (!tRad->pszBeruf) {
	   	   MessageBox(hDlg, "Speicherfehler:(", 0, MB_OK);
		   return ERR_MEM;
        } /* endif*/
		SendMessage(GetDlgItem(hDlg, ETXT_Beruf),
					WM_GETTEXT, tRad->rFix.clBeruf+1,
					(LPARAM)tRad->pszBeruf);
        if (tRad->rFix.clBeruf){
//		   cInput=1;
		   vLower(tRad->pszBeruf);
    	   tRad->pszBeruf[0]=toupper(tRad->pszBeruf[0]);
		   SendMessage(GetDlgItem(hDlg, ETXT_Beruf), WM_SETTEXT,
							0, (LPARAM)tRad->pszBeruf);
        } /* endif*/
   break;

   case ETXT_Ort:
        if (sGetOrt(hDlg, tRad, ETXT_Ort)<ERR_OK){
   		    return ERR_ORT;
	    } /* endif*/
		vSetErfassen(hDlg);
		SetFocus(GetDlgItem(hDlg, ETXT_Zeitzone));
        SendMessage(GetDlgItem(hDlg, ETXT_Zeitzone),
					EM_SETSEL, 0 , (LPARAM)MAKELONG(0, 32768)			);                          //	    cInput=1;
		return ERR_OK;
  	
   case ETXT_Datum:
		sC=SendMessage( GetDlgItem(hDlg, ETXT_Datum),
						 WM_GETTEXTLENGTH, 0, 0L);
		SendMessage(GetDlgItem(hDlg, ETXT_Datum), WM_GETTEXT,
				  			sC+1, (LPARAM)tRad->szDatum);
		sFormatDatum(tRad->szDatum, NULL, NULL, NULL);
        sStrToDate(tRad);
		SendMessage(GetDlgItem(hDlg, ETXT_Datum), WM_SETTEXT,
				  			0, (LPARAM)tRad->szDatum);
//        cInput=1;
   break;

   case ETXT_Zeit:
		sC=SendMessage( GetDlgItem(hDlg, ETXT_Zeit),
						 WM_GETTEXTLENGTH, 0, 0L);
		SendMessage(GetDlgItem(hDlg, ETXT_Zeit), WM_GETTEXT,
							sC+1, (LPARAM)tRad->szZeit);
		sFormatTime(tRad->szZeit, NULL, NULL);
        sStrToTime(tRad);
		SendMessage(GetDlgItem(hDlg, ETXT_Zeit), WM_SETTEXT,
							0, (LPARAM)tRad->szZeit);
//        cInput=1;
   break;

   case ETXT_Zeitzone:
		sC=SendMessage( GetDlgItem(hDlg, ETXT_Zeitzone),
						 WM_GETTEXTLENGTH, 0, 0L);
		if (!sC) {
		   return ERR_ZONE;
		} /* endif*/
		SendMessage(GetDlgItem(hDlg, ETXT_Zeitzone), WM_GETTEXT,
							sC+1, (LPARAM)szBuffer); 
  		tRad->rFix.fZone=(double)atof(szBuffer);
		if (tRad->rFix.fZone<-12.0)
           tRad->rFix.fZone=-12.0;
        if (tRad->rFix.fZone>12.0)
           tRad->rFix.fZone=12.0;
		sprintf(szBuffer, "%2.2f", tRad->rFix.fZone);
		SendMessage(GetDlgItem(hDlg, ETXT_Zeitzone), WM_SETTEXT,
							0, (LPARAM)szBuffer);
//        cInput=1;
   break;

   case ETXT_Sommer:
		sC=SendMessage( GetDlgItem(hDlg, ETXT_Sommer),
						 WM_GETTEXTLENGTH, 0, 0L);
		if (!sC) {
		   return ERR_STIME;
		} /* endif*/
		SendMessage(GetDlgItem(hDlg, ETXT_Sommer), WM_GETTEXT,
							sC+1, (LPARAM)szBuffer);
  		tRad->rFix.fSommer=(double)atof(szBuffer);
        sprintf(szBuffer, "%.2f", tRad->rFix.fSommer);
		SendMessage(GetDlgItem(hDlg, ETXT_Sommer), WM_SETTEXT,
							0, (LPARAM)szBuffer);
//        cInput=1;
   break;
   case 0:
 	   	sRet=sAdd(hDlg, ETXT_Vorname );
        sRet=sAdd(hDlg, ETXT_Name);
 	   	if ((sRet=sAdd(hDlg, ETXT_Ort))<=0) {
		   MessageBox(hDlg, "Ort Eingeben.", 0, MB_OK);
           SetFocus(GetDlgItem(hDlg, ETXT_Ort));
           return sRet;
        } /* endif*/
 	   	if ((sRet=sAdd(hDlg, ETXT_Zeitzone))<=0) {
		   MessageBox(hDlg, "Zeitzone Eingeben (-12 bis +12).", 0, MB_OK);
           SetFocus(GetDlgItem(hDlg, ETXT_Zeitzone));
           return sRet;
        } /* endif*/
 	   	sRet=sAdd(hDlg, ETXT_Zeit);
 	   	sRet=sAdd(hDlg, ETXT_Datum);
 	   	if ((sRet=sAdd(hDlg, ETXT_Sommer))<=0) {
		   MessageBox(hDlg, "Sommerzeit Eingeben.", 0, MB_OK);
           SetFocus(GetDlgItem(hDlg, ETXT_Sommer));
           return sRet;
        } /* endif*/
 	   	sRet=sAdd(hDlg, ETXT_Beruf);
	    if ( tRad->cMemory==CALC_ON_FILE) {
			sRet=sWriteDat(tRad, hDlg);
 	        fclose (fNamen);
           	if ( sRet<ERR_OK ){
				MessageBox(hDlg, "Fehler beim Sichern!", 0, MB_OK);
				return sRet;
			 } /* endif */
		} /* endif */
        SetFocus(GetDlgItem(hDlg, ETXT_Beruf));
		SendMessage(GetDlgItem(hDlg, ETXT_Beruf),
					EM_SETSEL, 0 , (LPARAM)MAKELONG(0, 32768));
//        cInput=0;
   break;
   default:                        
   break;
}/*endswitch*/


// if (sCalcChart(tRad)<ERR_OK)
//	return ERR_CHART;

 return ERR_OK;
}


/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sWriteDat( RADIX * Radix, HWND hWnd )
{
 short  sReadLen=0;
 short  sSize=0;
 short  sRet;
 //short  sAnzPl=Radix->rFix.sAnzahlPlanet;
 short  sBufSize=Radix->rFix.clVorName+Radix->rFix.clName+
				Radix->rFix.clBeruf+Radix->rFix.clOrt;
 RADIX  tRadix;
 DLGPROC lpDlgProc=0;

//search for name, vname in database

 fNamen=fopen( NAMDAT, "r+b" );
 if (!fNamen) {
    fNamen=fopen( NAMDAT, "w+b" );
    if (!fNamen) {
	  return ERR_FILE;
	} /* endif */
 } /* endif */
 memset(&tRadix, 0, RA_SIZE);
 sRet=sGetValidRecords( &tRadix, Radix, GET_DOUBLE, &sReadLen );
 if ( sRet<ERR_OK) {
	vKillData(&tRadix);
	return sRet;
 } /* endif */

 if ( sIndex ) {
	lpDlgProc=(DLGPROC)MakeProcInstance((FARPROC)DlgDDProc, hInstance);
	DialogBox(hInstance, "DATEN_DOPPELT", hWnd, lpDlgProc);
	FreeProcInstance ( (FARPROC) lpDlgProc );
 } /* endif */

 if( sIndex==-1) {
    return ERR_CANCEL;
 } /* endif */

 //Wenn Record schon vorhanden, Record als ungültig markieren...
 if ( sIndex ) {
	if (fseek(fNamen, plNamen[sIndex-1], SEEK_SET))
       return ERR_FILE;
	tRadix.rFix.cGultig=0;
	sSize=fwrite( &tRadix, 1, 1, fNamen );
	if ( sSize!=1 ) {
		vKillData(&tRadix);
		return ERR_FILE;
	} /* endif */
 } /* endif */
 vKillData(&tRadix);

/* fseek(fDaten, 0, SEEK_END);
 Radix->rFix.lIndexRadix=ftell(fDaten);
 if (Radix->rFix.lIndexRadix==-1L) {  
	return ERR_FILE;
 } /* endif */

 if (fseek(fNamen, 0, SEEK_END))
    return ERR_FILE;
 Radix->rFix.lIndexName=ftell(fNamen);
 if (Radix->rFix.lIndexName==-1L) {
	return ERR_FILE;
 } /* endif */
 sSize=fwrite( Radix, 1, RFIX_SIZE, fNamen );
 if ( sSize!=RFIX_SIZE ) {
	return ERR_FILE;
 } /* endif */

 sSize=fprintf( fNamen, "%s%s%s%s", Radix->pszName,
				Radix->pszVorName, Radix->pszBeruf, Radix->pszOrt );
 if ( sSize!=sBufSize)
	return ERR_FILE;

/* sSize=fwrite(Radix->pdPlanet, sizeof(double), sAnzPl, fDaten );
 if (sSize!=sAnzPl) 
	return ERR_FILE;

 sSize=fwrite(Radix->pdWinkelPlanet, sizeof(double), sAnzPl*sAnzPl, fDaten );
 if (sSize!=sAnzPl*sAnzPl)
	return ERR_FILE;

 sSize=fwrite(Radix->pdHaus, sizeof(double), MAX_HAUS, fDaten );
 if (sSize!=MAX_HAUS)
	return ERR_FILE;

 sSize=fwrite(Radix->pdWinkelHaus, sizeof(double), sAnzPl*MAX_HAUS, fDaten );
 if (sSize!=sAnzPl*MAX_HAUS) 
	return ERR_FILE;

 sSize=fwrite(Radix->psPlanetTyp, sizeof(short), sAnzPl, fDaten );
 if (sSize!=sAnzPl)
	return ERR_FILE;
*/

 return ERR_OK;
}
/****************************************************************************/
/*sDeleteDat( )       *******************************************************/
/****************************************************************************/
short sDeleteDat( RADIX * Radix, HWND hDlg, short sAction)
{

#define IREAD 0
#define IACT  1 
#define IGET  2

 short  sReadLen=0;
 short  sSize=0;
 short  sRet;
 RADIX  tRadix;
 short  sI=0;

 fNamen=fopen( NAMDAT, "r+b" );
 if (!fNamen) {
    fNamen=fopen( NAMDAT, "w+b" );
    if (!fNamen) {
	  return ERR_FILE;
	} /* endif */
 } /* endif */
 memset(&tRadix, 0, RA_SIZE);
 switch(sAction){
 case IREAD:
	 rewind(fNamen);
	 sRet=sGetValidRecords( &tRadix, Radix, GET_ALL, &sReadLen );
	 if ( sRet<ERR_OK ) {
	 	vKillData(&tRadix);
		return sRet;
	 } /* endif */
 break;

 case IGET:
 case IACT:
      sIndex=SendMessage(GetDlgItem(hDlg, LB_PHNamen),
					LB_GETCURSEL, 0 , 0L);
	  if (sIndex==LB_ERR) { 
	     sIndex=0;
	     return ERR_NOITEM;
 	  } else
	   	 sIndex++;

 	  if (fseek(fNamen, plNamen[sIndex-1], SEEK_SET))
	     return ERR_FILE;

      if (sAction==IGET) {
          sI=sIndex;
		  sRet=sGetValidRecords( &tRadix, Radix, GET_ONE, &sReadLen );
          sIndex=sI;
		  if ( sRet<ERR_OK) {
			 vKillData(&tRadix);
			 return sRet;
	 	  } /* endif */
		  if ( plNamen ) {
		   	 free( plNamen );
			 plNamen=0;
		  } /*endif */
		  if ( pszBuffer ) {
			 free( pszBuffer );
			 pszBuffer=0;
		  } /*endif */
		  if ( pszBuf ) {
			 free( pszBuf );
			 pszBuf=0;
		  } /*endif */
		  memcpy(Radix, &tRadix, RA_SIZE);
 	  } /* endif */
      else {
		  tRadix.rFix.cGultig=0;
		  sSize=fwrite( &tRadix, 1, 1, fNamen );
		  if ( sSize!=1 ) {
			vKillData(&tRadix);
			return ERR_FILE;
		  } /* endif */
  	  	  vKillData(&tRadix);
	  } /* endelse */
 break;
 }/* endswitch*/

return ERR_OK;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sReadDat( RADIX * Radix, HWND hWnd, char * szTitel )
{
 short  sReadLen=0;
 short  sRet;
// short  sAnzPl;
 RADIX  tRadix;
 DLGPROC lpDlgProc=0;

 fNamen=fopen( NAMDAT, "r+b" );
 if (!fNamen) {
    fNamen=fopen( NAMDAT, "w+b" );
    if (!fNamen) {
	  return ERR_FILE;
	} /* endif */
 } /* endif */
 memcpy(&tRadix, Radix, RA_SIZE);
 sRet=sGetValidRecords( &tRadix, Radix, GET_ALL, &sReadLen );
 if ( sRet<ERR_OK ) {
	vKillData(&tRadix);
	return sRet;
 } /* endif */

 if ( sIndex>1 ) {
	lpDlgProc=(DLGPROC)MakeProcInstance((FARPROC)DlgDGProc, hInstance);
	DialogBoxParam(hInstance, "ITEM_AUSWAHL", hWnd, lpDlgProc, (DWORD)szTitel);
	FreeProcInstance ( (FARPROC) lpDlgProc );
 } /* endif */

 if ( !sIndex )
   return ERR_NOITEM;

 if (fseek(fNamen, plNamen[sIndex-1], SEEK_SET))
    return ERR_FILE;
 sRet=sGetValidRecords( &tRadix, Radix, GET_ONE, &sReadLen );
 if ( plNamen ) {
	free( plNamen );
	plNamen=0;
 } /*endif */
 if ( pszBuffer ) {
	free( pszBuffer );
	pszBuffer=0;
 } /*endif */
 if ( pszBuf ) {
	free( pszBuf );
	pszBuf=0;
 } /*endif */
 if ( sRet<ERR_OK) {
	vKillData(&tRadix);
	return sRet;
 } /* endif */

 memcpy(Radix, &tRadix, RA_SIZE);
 //sAnzPl=Radix->rFix.sAnzahlPlanet;
// fseek(fDaten, Radix->rFix.lIndexRadix, SEEK_SET);

/* if (sAllocRadix(Radix)<ERR_OK) {
	vKillData(Radix);
	return ERR_MEM;
 } /* endif */

/* sSize=fread(Radix->pdPlanet, sizeof(double), sAnzPl, fDaten );
 if (sSize!=sAnzPl) {
	vKillData(Radix);
	return ERR_FILE;
 } /* endif */

/* sSize=fread(Radix->pdWinkelPlanet, sizeof(double), sAnzPl*sAnzPl, fDaten );
 if (sSize!=sAnzPl*sAnzPl) {
	vKillData(Radix);
	return ERR_FILE;
 } /* endif */

/* sSize=fread(Radix->pdHaus, sizeof(double), MAX_HAUS, fDaten );
 if (sSize!=MAX_HAUS) {
	vKillData(Radix);
	return ERR_FILE;
 } /* endif */

/* sSize=fread(Radix->pdWinkelHaus, sizeof(double), sAnzPl*MAX_HAUS, fDaten );
 if (sSize!=sAnzPl*MAX_HAUS) {
	vKillData(Radix);
	return ERR_FILE;
 } /* endif */

/* sSize=fread(Radix->psPlanetTyp, sizeof(short), sAnzPl, fDaten );
 if (sSize!=sAnzPl) {
	vKillData(Radix);
	return ERR_FILE;
 } /* endif */

 return ERR_OK;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
short sGetValidRecords( RADIX* tRadix, RADIX* Radix,
						short sVerify, short * sReadLen )
{
 short  sLen, sStunde, sMinuten;
 double dDeg;
 short  sSize;
 long   lBufSize;

 sIndex=0;
 do {
	 sSize=fread( tRadix, 1, RFIX_SIZE, fNamen );
	 if ( sSize!=RFIX_SIZE ) {
		 if (feof(fNamen))
			break;
		 else {
			vKillData(tRadix);
			return ERR_FILE;
		 } /* endelse */
	 } /* endif */

 // Record gueltig? **************************************
	 if ( !tRadix->rFix.cGultig ) {
		lBufSize=(long)(tRadix->rFix.clVorName+tRadix->rFix.clName+
						tRadix->rFix.clBeruf+tRadix->rFix.clOrt);
		if (fseek(fNamen, lBufSize, SEEK_CUR))
           return ERR_FILE;
		continue;
	 }

	 tRadix->pszName=realloc(tRadix->pszName, tRadix->rFix.clName+1);
	 tRadix->pszVorName=realloc(tRadix->pszVorName, tRadix->rFix.clVorName+1);
	 if (!tRadix->pszName||!tRadix->pszVorName) {
		vKillData(tRadix);
		return ERR_MEM;
	 } /* endif */
	 sSize=fread( tRadix->pszName, 1, tRadix->rFix.clName, fNamen );
	 if ( sSize!=tRadix->rFix.clName ) {
		vKillData(tRadix);
		return ERR_FILE;
	 } /* endif */
	 tRadix->pszName[sSize]=0;
	 sSize=fread( tRadix->pszVorName, 1, tRadix->rFix.clVorName, fNamen );
	 if ( sSize!=tRadix->rFix.clVorName ) {
		vKillData(tRadix);
		return ERR_FILE;
	 } /* endif */
	 tRadix->pszVorName[sSize]=0;
 // Bei GET_DOUBLE: Record schon Vorhanden?  ***********************************
	 if ( sVerify==GET_DOUBLE ) {
		if ( strcmp(tRadix->pszName, Radix->pszName) ||
			 strcmp(tRadix->pszVorName, Radix->pszVorName)){
			lBufSize=(long)(tRadix->rFix.clBeruf+tRadix->rFix.clOrt);
			if (fseek(fNamen, lBufSize, SEEK_CUR))
               return ERR_FILE;
			continue;
		} /* endif */
	 } /* endif */
	 tRadix->pszBeruf=realloc(tRadix->pszBeruf, tRadix->rFix.clBeruf+1);
	 tRadix->pszOrt=realloc(tRadix->pszOrt, tRadix->rFix.clOrt+1);
	 if (!tRadix->pszBeruf||!tRadix->pszOrt) {
		vKillData(tRadix);
		return ERR_MEM;
	 } /* endif */
	 sSize=fread( tRadix->pszBeruf, 1, tRadix->rFix.clBeruf, fNamen );
	 if ( sSize!=tRadix->rFix.clBeruf ) {
		vKillData(tRadix);
		return ERR_FILE;
	 } /* endif */
	 tRadix->pszBeruf[sSize]=0;
	 sSize=fread( tRadix->pszOrt, 1, tRadix->rFix.clOrt, fNamen );
	 if ( sSize!=tRadix->rFix.clOrt ) {
		vKillData(tRadix);
		return ERR_FILE;
	 } /* endif */
	 tRadix->pszOrt[sSize]=0;
	 dDeg=dDeg2Dez(tRadix->rFix.dTime+(tRadix->rFix.fZone+tRadix->rFix.fSommer));
	 sStunde=(short)floor(dDeg);
 	 dDeg=(dDeg-(double)sStunde)*100;
	 sMinuten=(short)floor(dDeg+RUNDEN);
	 sprintf(tRadix->szZeit, "%02d:%02d", sStunde, sMinuten);
	 sprintf(tRadix->szDatum, "%02d.%02d.%4d", tRadix->rFix.sTag,
			 tRadix->rFix.sMonat, tRadix->rFix.sJahr);

 // Bei GET_ONE: arbeit erledigt, break ***********************************
	 if (sVerify==GET_ONE)
        break;

	 sIndex++;
	 sLen=tRadix->rFix.clVorName+tRadix->rFix.clName+
				tRadix->rFix.clBeruf+tRadix->rFix.clOrt+15;
	 if (sLen<BUF_SIZE)
	   sLen=BUF_SIZE;
	 if (sLen>*sReadLen)
		*sReadLen=sLen;
	 pszBuf=realloc(pszBuf, sLen+1 );
	 pszBuffer=realloc(pszBuffer, sIndex*BUF_SIZE+1 );
	 plNamen=realloc(plNamen, sIndex*sizeof(long) );
	 if ( !pszBuf||!pszBuffer||!plNamen ) {
		vKillData(tRadix);
		return ERR_MEM;
	 } /* endif */
	 sprintf(pszBuf, "%-.12s\t%-.12s\t%-.12s\t%-.12s\t%s\t%s",
		 	    tRadix->rFix.clName?tRadix->pszName:UNBEKANNT_TXT,
				tRadix->rFix.clVorName?tRadix->pszVorName:UNBEKANNT_TXT,
		 		tRadix->rFix.clBeruf?tRadix->pszBeruf:UNBEKANNT_TXT,
				tRadix->pszOrt, tRadix->szDatum, tRadix->szZeit );
	 pszBuf[BUF_SIZE-1]=0;
	 strcpy(&pszBuffer[(sIndex-1)*BUF_SIZE], pszBuf);
	 plNamen[sIndex-1]=tRadix->rFix.lIndexName;
 } while(1);

 return ERR_OK;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
void vKillData(RADIX* Radix)
{

 if ( pszBuffer ) { 
	free( pszBuffer );
	    pszBuffer=0;
 } /*endif */
 if ( pszBuf ) {
	free( pszBuf );
        pszBuf=0;
 } /*endif */
 if ( plNamen ) {
	free( plNamen );
		plNamen=0;
 } /*endif */
 if ( Radix->pszName ){
	free(Radix->pszName);
    Radix->pszName=0;
 } /*endif */
 if ( Radix->pszVorName ){
	free(Radix->pszVorName );
    Radix->pszVorName=0;
 } /*endif */
 if ( Radix->pszBeruf){
	free(Radix->pszBeruf);
    Radix->pszBeruf=0;
 } /*endif */
 if ( Radix->pszOrt ){
	free(Radix->pszOrt);
    Radix->pszOrt=0;
 } /*endif */

}

/****************************************************************************/
/* Ort Auswählen															*/	
/****************************************************************************/
short sOrtAuswahl(long lIndexOrte)
{
ORTE 	    stOrt;

 if (fseek(fOrte, lIndexOrte, SEEK_SET))
    return ERR_FILE;

 fread(&stOrt, ORT_SIZE, 1, fOrte);
 if (ferror(fOrte))
    return ERR_FILE;

 tRad->pszOrt=realloc(tRad->pszOrt, stOrt.slOrt+1);
 if (!tRad->pszOrt) {
	return ERR_MEM;
 }/*endif*/
 fread(tRad->pszOrt, 1, stOrt.slOrt, fOrte);
 if (ferror(fOrte))
    return ERR_FILE;
 tRad->pszOrt[stOrt.slOrt]=0;
 sSetRadKoor(tRad, &stOrt);

 return ERR_OK;
}

/****************************************************************************/
/* Orte in ComboBox auchen 													 */
/****************************************************************************/
short sSearchOrt(HWND hWnd, short sChar, char * szB, ORTE * stO)
{

char  szBuffer[33];
char  szOrt[33];
ORTE  stOrt;
short sA=0, sIndex=1, sSel=1, sNum, sMax;
long  lIndex=0;
long  lLen;

if (fseek(fOrte, 0, SEEK_END))
   return ERR_FILE;
lLen=ftell(fOrte);
if (lLen==-1L)
   return ERR_FILE;


 rewind(fOrte);
 if (sChar) {
	GetWindowText(hWnd, szBuffer, 32);
	szOrt[0]=(char)sChar;
	szOrt[1]=0;
	strcat(szBuffer, szOrt);
} /* endif */
else
    strcpy(szBuffer, szB);

sA=strlen(szBuffer);
if (!sA)
   return ERR_MEM;

do {
	fread(&stOrt, ORT_SIZE, 1, fOrte);
	if (ferror(fOrte))
    	return ERR_FILE;
    lIndex+=ORT_SIZE;
	fread(szOrt, 1, stOrt.slOrt, fOrte);
	if (ferror(fOrte))
    	return ERR_FILE;
    szOrt[stOrt.slOrt]=0;
//	OemToAnsi(szOrt, szOrt);
	lIndex+=stOrt.slOrt;
    sNum=0;
    sMax=-1;
    for (sNum=0; sNum<sA; sNum++) {
	   if (toupper(szBuffer[sNum])==toupper(szOrt[sNum])){
          if (sNum>sMax) {
             sMax=sNum;
             if (sChar)
              	sSel=sIndex;
             else {
                strcpy(szB, szOrt);
                memcpy(stO, &stOrt, ORT_SIZE);
			 } /* endelse */
		   } /* endif */
	   } /* endif */
       else 
		 break;
    }/* endfor */
    if (sNum==sA)
       break;
    sIndex++;
 } while(lIndex<lLen);

 if (sChar) { 
	 SendMessage(GetDlgItem((HWND)GetWindowWord(hWnd, GWW_HWNDPARENT),
 			 			LB_O_OrtSuchen),
 			 LB_SETCURSEL, sSel , 0L);
	 SetWindowText(hWnd, szBuffer);                            
	 SendMessage(hWnd, EM_SETSEL, 0 , (LPARAM)MAKELONG(sA, sA));
 }/* endif*/

 return sSel;
}

/****************************************************************************/
/* Orte in ComboBox schreiben												*/
/****************************************************************************/
short sOrteWrite(HWND hDlg)
{

HWND      hWndC;
DWORD     dIndex;
char      szOrt[33];
ORTE      stOrt;
long	  lIndex=0;
long      lLen;

 if (fseek(fOrte, 0, SEEK_END))
    return ERR_FILE;
 lLen=ftell(fOrte);
 if (lLen==-1L)
    return ERR_FILE;

 rewind(fOrte);
 hWndC=GetDlgItem(hDlg, LB_O_OrtSuchen);
 SendMessage(hWndC, LB_RESETCONTENT, 0 , 0L);
 dIndex=SendMessage(hWndC, LB_ADDSTRING, 0, DRW_LB_MENU_ORTE);
 if ( dIndex==LB_ERR || dIndex==LB_ERRSPACE) {
	MessageBox(hDlg, "Kann leider nicht alle Orte Anzeigen! :(", 0, MB_OK);
	return ERR_FILE;
 }/*endif*/
  do {
	fread(&stOrt, ORT_SIZE, 1, fOrte);
	if (ferror(fOrte))
    	return ERR_FILE;
    lIndex+=ORT_SIZE;
	fread(szOrt, 1, stOrt.slOrt, fOrte);
	if (ferror(fOrte))
		return ERR_FILE;
	if (!stOrt.cGultig)
	   continue;
    szOrt[stOrt.slOrt]=0;
    lIndex+=stOrt.slOrt;
    dIndex=SendMessage(hWndC, LB_ADDSTRING, 0, stOrt.lIndex);
	if ( dIndex==LB_ERR || dIndex==LB_ERRSPACE) {
		MessageBox(hDlg, "Kann leider nicht alle Orte Anzeigen! :(", 0, MB_OK);
		break;
	}/*endif*/
 } while(lIndex<lLen);
 SendMessage(hWndC, LB_SETCURSEL, 1 , 0L);

 return ERR_OK;
}

/****************************************************************************/
/*	Ort aus editfeld lesen und in datenbank suchen                           */
/****************************************************************************/

short sGetOrt(HWND hDlg, RADIX* Radix, short sID)
{

HWND   hWnd=GetDlgItem(hDlg, sID);
ORTE    stOrt;
char   szBuffer[33];

 if (!SendMessage(hWnd, WM_GETTEXT, 32,	(LPARAM)szBuffer))
    return ERR_ORT;
 
 sSearchOrt((HWND)(hDlg), 0, szBuffer, &stOrt);
 SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)szBuffer);
 Radix->rFix.clOrt=strlen(szBuffer);
 Radix->pszOrt=realloc( Radix->pszOrt, Radix->rFix.clOrt+1 );
 if (!Radix->pszOrt) {
    MessageBox(hDlg, "Speicherfehler:(", 0, MB_OK);
    return ERR_MEM;
 } /* endif*/
 strcpy(Radix->pszOrt, szBuffer);
 sSetRadKoor(Radix, &stOrt);
 
 return ERR_OK;
} 

/****************************************************************************/
/* ErfassenDialog Initialisieren
/****************************************************************************/
short sInitErfassenDialog(HWND hDlg, LPARAM lParam)
{

 RADIX* Radix=(RADIX*)lParam;
 char szBuffer[BUF_SIZE];

 tRad=calloc(1, RA_SIZE);
 if (!tRad)
    return ERR_MEM;

 sInitRadixStruct(tRad, P_TYP_IGNORE);
 SendMessage( GetDlgItem(hDlg, ETXT_Zeit), EM_LIMITTEXT, 5, 0L);
 SendMessage( GetDlgItem(hDlg, ETXT_Datum), EM_LIMITTEXT, 10, 0L);
 SendMessage( GetDlgItem(hDlg, ETXT_Ort), EM_LIMITTEXT, 32, 0L);
 SendMessage( GetDlgItem(hDlg, ETXT_Zeitzone), EM_LIMITTEXT, 5, 0L);
 SendMessage( GetDlgItem(hDlg, ETXT_Sommer), EM_LIMITTEXT, 5, 0L);
 SendMessage( GetDlgItem(hDlg, ETXT_Sommer),
			 WM_SETTEXT, 0, (LPARAM)"0");
 if(Radix){
   tRad->rFix.fZone=Radix->rFix.fZone;
   tRad->rFix.dBreite=Radix->rFix.dBreite;
   tRad->rFix.dLange=Radix->rFix.dLange;
   strcpy(tRad->rFix.szLand, Radix->rFix.szLand);
   vSetErfassen(hDlg);
   SendMessage(GetDlgItem(hDlg, ETXT_Name), WM_SETTEXT,
			0, (LPARAM)Radix->pszName);
   SendMessage(GetDlgItem(hDlg, ETXT_Vorname), WM_SETTEXT,
			0, (LPARAM)Radix->pszVorName);
   SendMessage(GetDlgItem(hDlg, ETXT_Beruf), WM_SETTEXT,
			0, (LPARAM)Radix->pszBeruf);
   SendMessage(GetDlgItem(hDlg, ETXT_Ort), WM_SETTEXT,
			0, (LPARAM)Radix->pszOrt);
   sprintf(szBuffer, "%.2f", Radix->rFix.fSommer);
   SendMessage( GetDlgItem(hDlg, ETXT_Sommer),
			 WM_SETTEXT, 0, (LPARAM)szBuffer);
   SendMessage( GetDlgItem(hDlg, ETXT_Datum),
			 WM_SETTEXT, 0, (LPARAM)Radix->szDatum);
   SendMessage( GetDlgItem(hDlg, ETXT_Zeit),
 			 WM_SETTEXT, 0, (LPARAM)Radix->szZeit);
   SetFocus(GetDlgItem(hDlg, ETXT_Beruf));
   SendMessage(GetDlgItem(hDlg, ETXT_Beruf),
					EM_SETSEL, 0 , (LPARAM)MAKELONG(0, 32768)			);                          //	    cInput=1;

 }/*endif*/
 return ERR_OK;
}

/****************************************************************************/
/*																			*/
/****************************************************************************/
void vSetErfassen(HWND hDlg)
{
 char    szBuffer[BUF_SIZE];
 char    szB[32];

 
 sprintf(szBuffer,"%2.2f", tRad->rFix.fZone);
 SendMessage( GetDlgItem(hDlg, ETXT_Zeitzone),
 			 WM_SETTEXT, 0, (LPARAM)szBuffer);

 vGrad2Str(tRad->rFix.dBreite, szB, 0, 1);
 sprintf(szBuffer,"%s", szB);
 SendMessage( GetDlgItem(hDlg, TXT_Breite),
 			 WM_SETTEXT, 0, (LPARAM)szBuffer);

 vGrad2Str(tRad->rFix.dLange, szB, 1, 1);
 sprintf(szBuffer,"%s", szB);
 SendMessage( GetDlgItem(hDlg, TXT_Lange),
			 WM_SETTEXT, 0, (LPARAM)szBuffer);

 SendMessage( GetDlgItem(hDlg, TXT_LAND),
			 WM_SETTEXT, 0, (LPARAM)tRad->rFix.szLand);
}
/****************************************************************************/
/*																			*/
/****************************************************************************/
void vSetNamenSuchen(HWND hDlg)
{
 char    szBuffer[BUF_SIZE];

 SendMessage( GetDlgItem(hDlg, ETXT_Name),
 			 WM_SETTEXT, 0, (LPARAM)tRad->pszName);
 SendMessage( GetDlgItem(hDlg, ETXT_Vorname),
			 WM_SETTEXT, 0, (LPARAM)tRad->pszVorName);
 SendMessage( GetDlgItem(hDlg, ETXT_Ort),
 			 WM_SETTEXT, 0, (LPARAM)tRad->pszOrt);
 SendMessage( GetDlgItem(hDlg, ETXT_Beruf),
 			 WM_SETTEXT, 0, (LPARAM)tRad->pszBeruf);
 SendMessage( GetDlgItem(hDlg, ETXT_Datum),
			 WM_SETTEXT, 0, (LPARAM)tRad->szDatum);
 SendMessage( GetDlgItem(hDlg, ETXT_Zeit),
 			 WM_SETTEXT, 0, (LPARAM)tRad->szZeit);
 sprintf(szBuffer,"%.2f", tRad->rFix.fSommer);
 SendMessage( GetDlgItem(hDlg, ETXT_Sommer),
			 WM_SETTEXT, 0, (LPARAM)szBuffer);
}

/****************************************************************************/
/* SubClass LB/vSetButtunData    */
/****************************************************************************/

short sSetR(HWND hDlg, char cBt)
{
RADIX*Radix=0;
HWND  hWnd;

hWnd=(HWND)GetWindowWord(hDlg, GWW_HWNDPARENT);
Radix=(RADIX *)GetWindowLong(hWnd, 0);
if (Radix->cMouse&MAUS_ZOOM)
   return 0;

Radix->cMouse^=cBt;
vInvalidateAsp(hWnd);
return ERR_OK;
}

void vInvalidateAsp(HWND hWnd)
{
short sRx=(short)((double)sRadius*KREIS_ASP);
short sRy=(short)((double)sRadius*KREIS_ASP);
HRGN  hRgn;
short sMpXX=sMpX;
short sMpYY=sMpY;

sSetXY(&sMpXX, &sMpYY);
sSetXY(&sRx, &sRy);
hRgn=CreateEllipticRgn( sMpXX-sRx, sMpYY-sRy, sMpXX+sRx, sMpYY+sRy );
InvalidateRgn(hWnd, hRgn, 1);
DeleteObject(hRgn);

}

/****************************************************************************/
/* sSetLBTabs  LB-Tabstops setzen doppelte einträge und personen auswahl    */
/****************************************************************************/
short sSetLBTabs(HWND hWnd, short sTyp)
{
short sTabs1[5]={48, 96, 144, 192, 232};
short sTabs0[5]={120, 158, 196, 232};

 switch (sTyp){
 case 1:
	 SendMessage(hWnd, LB_SETTABSTOPS, 5, (LPARAM)&sTabs1);
 break;
 default:
	 SendMessage(hWnd, LB_SETTABSTOPS, 5, (LPARAM)&sTabs0);
 break;
 }/*endswitch*/

 return ERR_OK;
}

/****************************************************************************/
/* SubClass Edit  */
/****************************************************************************/
LRESULT	 CALLBACK SubEdit(HWND hDlg, UINT message,
						   WPARAM wParam, LPARAM lParam)
{

switch (message) {
    case WM_CHAR:
       if (!iscntrl(wParam))
           sSearchOrt((HWND)(hDlg), wParam, 0, 0);
       else
         return (LRESULT)CallWindowProc(lpEditProc, hDlg, message, wParam, lParam);
    break;
    default:
         return (LRESULT)CallWindowProc(lpEditProc, hDlg, message, wParam, lParam);
 } /* endswitch*/
 return TRUE;

}

/****************************************************************************/
/* SubClass LB    */
/****************************************************************************/
LRESULT	 CALLBACK SubLB(HWND hDlg, UINT message,
						   WPARAM wParam, LPARAM lParam)
{

switch (message) {
	case WM_LBUTTONDOWN:
		   if (GetDlgCtrlID(hDlg)!=LB_UT_TRANS)
//			  sSetTransWnd(hDlg);
//           else
			  sSetR(hDlg, MAUS_L);
    break;
    case WM_LBUTTONDBLCLK:
		 if (GetDlgCtrlID(hDlg)==LB_UT_TRANS)
		    SendMessage((HWND)GetWindowWord(hDlg, GWW_HWNDPARENT), WM_COMMAND, PB_UT_GRAF, 0);
	break;
    case WM_RBUTTONDOWN:
		 if (GetDlgCtrlID(hDlg)==LB_UT_TRANS)
			SendMessage((HWND)GetWindowWord(hDlg, GWW_HWNDPARENT), WM_COMMAND, PB_UT_RUCK, 0);
         else
			sSetR(hDlg, MAUS_R);
	break;
    case WM_RBUTTONDBLCLK:
//	       sSetR(hDlg, MAUS_RD);
    break;
    case WM_CHAR:
    break;
    default:
        return (LRESULT)CallWindowProc(lpLbProc, hDlg, message, wParam, lParam);
 } /* endswitch*/
 return (LRESULT)CallWindowProc(lpLbProc, hDlg, message, wParam, lParam);
}

/****************************************************************************/
/*	FUNCTION: DlgPHProc(HWND, unsigned, WORD, LONG)                         */
/****************************************************************************/
LRESULT CALLBACK DlgPHProc(HWND hDlg, UINT message,
                       WPARAM wParam, LPARAM lParam)
{

switch (message) {
	case WM_INITDIALOG:
         if (sDeleteDat(tRad, hDlg, IREAD)<ERR_OK){
			fclose (fNamen);
			MessageBox(hDlg, "Fehler beim lesen der Namen! :(", 0, MB_OK);
			EndDialog(hDlg, TRUE);
         }/* endif*/
		 fclose (fNamen);
		 if (sIndex)
	        sFillNameLB(hDlg, LB_PHNamen, sIndex);
         else {
  		  	MessageBox(hDlg, "Keine Personen in Datenbank vorhanden! :(", 0, MB_OK);
			EndDialog(hDlg, TRUE);
         }/* endelse*/
		 sSetLBTabs(GetDlgItem(hDlg, LB_PHNamen), 1);
 	     SetFocus(GetDlgItem(hDlg, LB_PHNamen));
		 break;

	case WM_COMMAND:
		switch (wParam) {
			case PB_Ok:
                 if (sDeleteDat(tRad, hDlg, IGET)<ERR_OK)
		  		  	MessageBox(hDlg, "Fehler beim lesen der Daten! :(", 0, MB_OK);
	     		 fclose (fNamen);
				 EndDialog(hDlg, TRUE);
                 break;
			case PB_PHLoeschen:
                 if (sDeleteDat(tRad, hDlg, IACT)<ERR_OK)
					  MessageBox(hDlg, "Fehler beim löschen! :(", 0, MB_OK);
				 fclose (fNamen);
				 if (sDeleteDat(tRad, hDlg, IREAD)<ERR_OK)
  				  	MessageBox(hDlg, "Fehler beim lesen der Namen! :(", 0, MB_OK);
				 fclose (fNamen);
				 SendMessage(GetDlgItem(hDlg, LB_PHNamen),	LB_RESETCONTENT, 0 , 0L);
				 if (sIndex)
                    sFillNameLB(hDlg, LB_PHNamen, sIndex);
                 else
					EndDialog(hDlg, TRUE);
		        break;
			case PB_Abbruch:
				sIndex=0;
				EndDialog(hDlg, TRUE);
				break;
			case PB_Hilfe:
				MessageBox(hDlg, "Keine Hilfe :)", 0, MB_OK);
				break;;				
		} /* endswitch */
		break;

	 default:
         return FALSE;
}  /* endswitch  */

return(TRUE);
}

/****************************************************************************/
/*	FUNCTION: DlgDGOrtAuswahl(HWND, unsigned, WORD, LONG)                   */
/****************************************************************************/
LRESULT	 CALLBACK DlgOrtAuswahl(HWND hDlg, UINT message,
								WPARAM wParam, LPARAM lParam )
{
short  sRet=0;
DLGPROC lpDlgProc=0;

switch (message) {
	case WM_INITDIALOG:
//		 lpLbProc=(WNDPROC)SetWindowLong(GetDlgItem(hDlg, LB_O_OrtSuchen),
//				 		GWL_WNDPROC, (LPARAM)SubLB);
		 lpEditProc=(WNDPROC)SetWindowLong(GetDlgItem(hDlg, ETXT_O_OrtSuchen),
		 		GWL_WNDPROC, (LPARAM)SubEdit);
		 sRet=sOrteWrite(hDlg);
		 if ( sRet<ERR_OK ) {
			MessageBox(hDlg, "Error Ort Auswahl :(", 0, MB_OK);
			EndDialog(hDlg, TRUE);
			break;
		 } /* endif */
         SetFocus(GetDlgItem(hDlg, ETXT_O_OrtSuchen));
	return FALSE;

    case WM_DRAWITEM:
         sDrawLBOrt((DRAWITEMSTRUCT*)lParam);
    break;
	case WM_COMMAND:
		switch (wParam) {
  			case PB_O_OK:
				sIndex=SendMessage(GetDlgItem(hDlg, LB_O_OrtSuchen),
							LB_GETCURSEL, 0 , 0L);
                if ( !sIndex )
                  break;
				if (sIndex==LB_ERR) {
				   MessageBox(hDlg, "Error:\nOrt nicht gefunden!", 0, MB_OK);
				   sIndex=0;
                } /* endif */
				else {
	  			   sOrtAuswahl(SendMessage(GetDlgItem(hDlg, LB_O_OrtSuchen),
		 			 				LB_GETITEMDATA,	(WPARAM)sIndex, 0));
 				   EndDialog(hDlg, TRUE);
				} /* endelse */
				break;
                          	
			case PB_O_Neu:
				 lpDlgProc=(DLGPROC)MakeProcInstance((FARPROC)DlgOErfassen, hInstance);
				 DialogBox(hInstance, "E_ORT", hDlg, lpDlgProc);
				 FreeProcInstance ( (FARPROC) lpDlgProc );
				 sRet=sOrteWrite(hDlg);
				 if ( sRet<ERR_OK ) {
					MessageBox(hDlg, "Error Ort Auswahl :(", 0, MB_OK);
					EndDialog(hDlg, TRUE);
				 } /* endif */
				 SetWindowText(GetDlgItem(hDlg, ETXT_O_OrtSuchen), szOrte);
				 SetFocus(GetDlgItem(hDlg, ETXT_O_OrtSuchen));
				 SendMessage(GetDlgItem(hDlg, ETXT_O_OrtSuchen), WM_CHAR, (WPARAM)0x20, (LPARAM)1);
			break;

			case PB_O_Abbruch:
				sIndex=0;
				EndDialog(hDlg, TRUE);
			break;

			case PB_O_Help:
				MessageBox(hDlg, "Keine Hilfe :)", 0, MB_OK);
			break;
		} /* endswitch */
		break;

     default:
         return (NULL);
}  /* endswitch  */

return TRUE;
}

/****************************************************************************/
/*	FUNCTION: DlgPErfassen(HWND, unsigned, WORD, LONG)                         */
/****************************************************************************/
LRESULT CALLBACK DlgPErfassen(HWND hDlg, UINT message,
                           WPARAM wParam, LPARAM lParam)
{
DLGPROC lpDlgProc=0;

switch (message) {
	case WM_INITDIALOG:
		 if (sInitErfassenDialog(hDlg, lParam)<ERR_OK) {
			MessageBox(hDlg, "Memory or File Error!", 0, MB_OK);
			EndDialog(hDlg, TRUE);
			break;
		 }/* endif */
		 SetWindowText(GetDlgItem(hDlg, TXT_Erfasst), PERSON_NEU_TXT);
		 break;

    case WM_COMMAND:
		switch (wParam) {
			case PB_NamenSuchen:
				lpDlgProc=(DLGPROC)MakeProcInstance((FARPROC)DlgPHProc, hInstance);
				DialogBox(hInstance, "PERSON_HAND", hDlg, lpDlgProc);
				FreeProcInstance ( (FARPROC) lpDlgProc );
				if (sIndex) {
                   vSetErfassen(hDlg);
                   vSetNamenSuchen(hDlg);
                } /* endif */
			break;

			case PB_OrtSuchen:
				 lpDlgProc=(DLGPROC)MakeProcInstance((FARPROC)DlgOrtAuswahl, hInstance);
				 DialogBox(hInstance, "ORT_AUSWAHL", hDlg, lpDlgProc);
				 FreeProcInstance ( (FARPROC) lpDlgProc );
                 if (sIndex) {
				    vSetErfassen(hDlg);
 					SendMessage( GetDlgItem(hDlg, ETXT_Ort),
								 WM_SETTEXT, 0, (LPARAM)tRad->pszOrt);
 	                SetFocus(GetDlgItem(hDlg, ETXT_Zeitzone));
					SendMessage(GetDlgItem(hDlg, ETXT_Zeitzone),
						EM_SETSEL, 0 , (LPARAM)MAKELONG(0, 32768)			);                          //	    cInput=1;
				 } else {
                    SetFocus(GetDlgItem(hDlg, ETXT_Ort));
				 }/*endelse*/
			break;
			case PB_Ok:					//hinzufügen
                tRad->cMemory=CALC_ON_FILE;
				if (sAdd(hDlg, NULL )>ERR_CANCEL)
					SetWindowText(GetDlgItem(hDlg, TXT_Erfasst), PERSON_ERFASST_TXT);
				break;
            case PB_Calc:
                tRad->cMemory=CALC_ON_MEM;
				if (sAdd(hDlg, NULL)<ERR_OK) {
                    return(TRUE);
                } /* endif */
                auinit.Radix.cMemory=CALC_ON_MEM;
				RadixCopy(&auinit.Radix, tRad, ALLOC_MEM);
				PostMessage(hWndMain, WM_COMMAND, CM_U_HOROTYP, 0);
				EndDialog(hDlg, TRUE);
        		break;

			case PB_Abbruch:
/*                if ( cInput ){
				   if (MessageBox(hDlg, "Wirklich Abbrechen?", 0, MB_OKCANCEL)==IDOK)
					  EndDialog(hDlg, TRUE);
                } else {
					  EndDialog(hDlg, TRUE);
                } /* endelse*/
 				EndDialog(hDlg, TRUE);
				break;

			case PB_Hilfe:
				MessageBox(hDlg, "Keine Hilfe :)", 0, MB_OK);
				break;
		} /* endswitch */
        if (HIWORD(lParam)==EN_KILLFOCUS) {
			SetWindowText(GetDlgItem(hDlg, TXT_Erfasst), PERSON_NEU_TXT);
			sAdd( hDlg, wParam );
			break;
        };
		break;

	 case WM_DESTROY:
		  vKillData(tRad);
		  free(tRad);
 	 break;

	 default:
         return (NULL);
}  /* endswitch  */

return(TRUE);
}

/****************************************************************************/
/*	FUNCTION: DlgDDProc(HWND, unsigned, WORD, LONG)                         */
/****************************************************************************/

LRESULT CALLBACK DlgDDProc(HWND hDlg, UINT message,
                           WPARAM wParam, LPARAM lParam)
{

switch (message) {
	case WM_INITDIALOG:
         if (sIndex)
	         sFillNameLB(hDlg, CM_UDD_LB, sIndex);
         sSetLBTabs(GetDlgItem(hDlg, CM_UDD_LB), 1);
 	     SetFocus(GetDlgItem(hDlg, CM_UDD_LB));
		 break;

	case WM_COMMAND:
		switch (wParam) {
			case CM_UDD_JA:
				sIndex=SendMessage(GetDlgItem(hDlg, CM_UDD_LB),
							LB_GETCURSEL, 0 , 0L);
				if (sIndex==LB_ERR)
					  sIndex=0;
				else 
					sIndex++;
				EndDialog(hDlg, TRUE);
				break;
			case CM_UDD_ADD:
				sIndex=0;
				EndDialog(hDlg, TRUE);
				break;
			case CM_UDD_ESC:
                sIndex=-1;
				EndDialog(hDlg, FALSE);
				break;;
			case CM_UDD_HELP:
				MessageBox(hDlg, "Keine Hilfe :)", 0, MB_OK);
				break;				
		} /* endswitch */
		break;

  		default:
           return FALSE;
}  /* endswitch  */

return(TRUE);
}

/****************************************************************************/
/*	FUNCTION: DlgDGProc(HWND, unsigned, WORD, LONG)                         */
/****************************************************************************/
LRESULT CALLBACK DlgDGProc(HWND hDlg, UINT message,
                       WPARAM wParam, LPARAM lParam)
{

switch (message) {
	case WM_INITDIALOG:
         if (sIndex)
	         sFillNameLB(hDlg, CM_UDG_LB, sIndex);
         sSetLBTabs(GetDlgItem(hDlg, CM_UDG_LB), 1);
	     SetFocus(GetDlgItem(hDlg, CM_UDG_LB));
		 SetWindowText(hDlg, (char*)lParam);
		 break;

	case WM_COMMAND:
		switch (wParam) {
			case CM_UDG_OK:
				sIndex=SendMessage(GetDlgItem(hDlg, CM_UDG_LB),
							LB_GETCURSEL, 0 , 0L);
				if (sIndex==LB_ERR) 
				   sIndex=0;
				else 
					sIndex++;
				EndDialog(hDlg, TRUE);
                break;
			case CM_UDG_ESC:
				sIndex=0;
				EndDialog(hDlg, TRUE);
				break;
			case CM_UDG_HELP:
				MessageBox(hDlg, "Keine Hilfe :)", 0, MB_OK);
				break;;				
		} /* endswitch */
		break;

	 default:
         return FALSE;
}  /* endswitch  */

return(TRUE);
}

/****************************************************************************/
/*	FUNCTION: DlgHausAuswahl(HWND, unsigned, WORD, LONG)                    */
/****************************************************************************/
LRESULT 	 CALLBACK DlgHausAuswahl(HWND hDlg, UINT message,
								WPARAM wParam, LPARAM lParam )
{

switch (message) {
	case WM_INITDIALOG:
 	     auinit.sRet=auinit.sSelHaus;
         CheckRadioButton(hDlg, RA_HAUS_BEGIN, RA_HAUS_END,
		 		auinit.sSelHaus+HAUS_ID_INDEX);
		 break;

	case WM_COMMAND:
        if ((HIWORD(lParam)==BN_CLICKED)&&wParam>=RA_HAUS_BEGIN&&wParam<=RA_HAUS_END) {
		   auinit.sSelHaus=wParam-HAUS_ID_INDEX;
           CheckRadioButton(hDlg, RA_HAUS_BEGIN, RA_HAUS_END,
		   			auinit.sSelHaus+HAUS_ID_INDEX);
  		   break;;
        }/*endif*/

		switch (wParam) {
  			case PB_Ok:
				EndDialog(hDlg, TRUE);
				break;
			case PB_Abbruch:
	  		    auinit.sSelHaus=auinit.sRet;
				EndDialog(hDlg, FALSE);
				break;

			case PB_Hilfe:
				MessageBox(hDlg, "Keine Hilfe :)", 0, MB_OK);
				break;
		} /* endswitch */
		break;

     default:
		return(FALSE);
}  /* endswitch  */

return(TRUE);
}

/****************************************************************************/
/*	FUNCTION: DlgHoroAuswahl(HWND, unsigned, WORD, LONG)                    */
/*****************************************************************************/
LRESULT 	 CALLBACK DlgHoroAuswahl(HWND hDlg, UINT message,
								WPARAM wParam, LPARAM lParam )
{

switch (message) {
	case WM_INITDIALOG:
	     auinit.sRet=auinit.sSelHoro;
         CheckRadioButton(hDlg, RA_HORO_BEGIN, RA_HORO_END,
		 		auinit.sSelHoro+HORO_ID_INDEX);
	   	 SetWindowText(GetDlgItem(hDlg, TXT_Haussys),
		 				(LPSTR)szHausTyp[auinit.sSelHaus]);
         break;

	case WM_COMMAND:
		if ((HIWORD(lParam)==BN_CLICKED)&&wParam>=RA_HORO_BEGIN&&wParam<=RA_HORO_END ) {
		   auinit.sSelHoro=wParam-HORO_ID_INDEX;
           CheckRadioButton(hDlg, RA_HORO_BEGIN, RA_HORO_END,
		   				auinit.sSelHoro+HORO_ID_INDEX);
  		   break;
        }/*endif*/

		switch (wParam) {
  			case PB_Ok:
				EndDialog(hDlg, TRUE);
				break;
			case PB_Abbruch:
	  		    auinit.sSelHoro=auinit.sRet;
				EndDialog(hDlg, FALSE);
				break;

			case PB_Hilfe:
				MessageBox(hDlg, "Keine Hilfe :)", 0, MB_OK);
				break;
		} /* endswitch */
		break;

     default:
		 return (FALSE);
}  /* endswitch  */

return(TRUE);
}

/****************************************************************************/
/*	FUNCTION: DlgTransite(HWND, unsigned, WORD, LONG)                         */
/****************************************************************************/
LRESULT CALLBACK DlgTransit(HWND hDlg, UINT message,
                           WPARAM wParam, LPARAM lParam)
{
DLGPROC lpDlgProc=0;

switch (message) {
	case WM_INITDIALOG:
         tRad=(RADIX*)lParam;
 		 SendMessage( GetDlgItem(hDlg, ETXT_TZeit), EM_LIMITTEXT, 5, 0L);
		 SendMessage( GetDlgItem(hDlg, ETXT_TDatum), EM_LIMITTEXT, 10, 0L);
		 SendMessage( GetDlgItem(hDlg, ETXT_TBZeit), EM_LIMITTEXT, 5, 0L);
		 SendMessage( GetDlgItem(hDlg, ETXT_TBDatum), EM_LIMITTEXT, 10, 0L);
		 SendMessage( GetDlgItem(hDlg, ETXT_TOrt), WM_SETTEXT,
		 				0, (LPARAM)tRad->pszOrt);
		 SendMessage( GetDlgItem(hDlg, ETXT_TName), WM_SETTEXT,
		 				0, (LPARAM)tRad->pszName);
		 SendMessage( GetDlgItem(hDlg, ETXT_TZeit), WM_SETTEXT,
		 				0, (LPARAM)"12:00");
		 SendMessage( GetDlgItem(hDlg, ETXT_TBZeit), WM_SETTEXT,
						0, (LPARAM)"12:00");
		 CheckRadioButton(hDlg, RBT_START, RBT_END, RB_TAG);
		 vGetDate(hDlg, 4);
//		 vGetDate(hDlg, 0);
		 auinit.sModus=RB_TAG-RBT_START;
		 SetFocus(GetDlgItem(hDlg, ETXT_TDatum));
		 SendMessage(GetDlgItem(hDlg, ETXT_TDatum),
					EM_SETSEL, 0 , (LPARAM)MAKELONG(0, 32768));
		 wParam=(WPARAM)GetDlgItem(hDlg, ETXT_TDatum);
		 return 0;

    case WM_COMMAND:
		if (HIWORD(lParam)==BN_CLICKED){
		   if (wParam>=RBT_START&&wParam<=RBT_END ) {
			   auinit.sModus=wParam-RBT_START;
			   CheckRadioButton(hDlg, RBT_START, RBT_END, wParam);
			   switch(wParam){
			   case RB_JAHR:
					vGetDate(hDlg, 2);
			   break;
			   case RB_MONAT:
					vGetDate(hDlg, 1);
			   break;
			   default:
					vGetDate(hDlg, 0);
			   break;
			   }/*endif*/
		   }/*endif*/
		}/*endif*/
		switch (wParam) {
			case PB_TOrt:
				 lpDlgProc=(DLGPROC)MakeProcInstance((FARPROC)DlgOrtAuswahl, hInstance);
				 DialogBox(hInstance, "ORT_AUSWAHL", hDlg, lpDlgProc);
				 FreeProcInstance ( (FARPROC) lpDlgProc );
                 if (sIndex) {
					 SendMessage( GetDlgItem(hDlg, ETXT_TOrt), WM_SETTEXT,
				 					0, (LPARAM)tRad->pszOrt);
 	                SetFocus(GetDlgItem(hDlg, ETXT_TDatum));
				 } else
                    SetFocus(GetDlgItem(hDlg, ETXT_TOrt));
			break;
			case PB_Ok:					//hinzufügen
				SendMessage(GetDlgItem(hDlg, ETXT_TDatum), WM_GETTEXT,
				  			11, (LPARAM)tRad->szDatum);
				sFormatDatum(tRad->szDatum, NULL, NULL, NULL);
                sStrToDate(tRad);
				SendMessage(GetDlgItem(hDlg, ETXT_TZeit), WM_GETTEXT,
				  			6, (LPARAM)tRad->szZeit);
				sFormatTime(tRad->szZeit, NULL, NULL);
                sStrToTime(tRad);
				SendMessage(GetDlgItem(hDlg, ETXT_TBDatum), WM_GETTEXT,
							21, (LPARAM)auinit.szBDatum);
				sFormatDatum(auinit.szBDatum, NULL, NULL, NULL);
				SendMessage(GetDlgItem(hDlg, ETXT_TBZeit), WM_GETTEXT,
							6, (LPARAM)auinit.szBZeit);
				sFormatTime(auinit.szBZeit, NULL, NULL);
				if (sIncDate(tRad->szZeit, tRad->szDatum, 1, INC_MINUTEN, 2)<0){    //1: vergleich mit datum/zeit in auinit
				   SendMessage(GetDlgItem(hDlg, ETXT_TBZeit), WM_SETTEXT,
								0, (LPARAM)auinit.szBZeit);
				   SendMessage(GetDlgItem(hDlg, ETXT_TBDatum), WM_SETTEXT,
								0, (LPARAM)auinit.szBDatum);
				   break;
                }/*endif*/
				EndDialog(hDlg, TRUE);
            break;

			case PB_Abbruch:
 				EndDialog(hDlg, FALSE);
				break;

			case PB_Hilfe:
				MessageBox(hDlg, "Keine Hilfe :)", 0, MB_OK);
				break;
		} /* endswitch */
        if ((HIWORD(lParam)==EN_KILLFOCUS)) {
            switch(wParam) {
            case ETXT_TOrt:
		        if (sGetOrt(hDlg, tRad, ETXT_TOrt)<ERR_OK){
					MessageBox(hDlg, "Ort nicht gefunden! :(", 0, MB_OK);
					SetFocus(GetDlgItem(hDlg, ETXT_TOrt));
        		};  /* endif*/
            break;
            case ETXT_TZeit:
				SendMessage(GetDlgItem(hDlg, ETXT_TZeit), WM_GETTEXT, 6, (LPARAM)tRad->szZeit);
				sFormatTime(tRad->szZeit, NULL, NULL);
				SendMessage(GetDlgItem(hDlg, ETXT_TZeit), WM_SETTEXT,
								0, (LPARAM)tRad->szZeit);
				SendMessage(GetDlgItem(hDlg, ETXT_TBZeit), WM_SETTEXT,
								0, (LPARAM)tRad->szZeit);
			break;
            case ETXT_TDatum:
				SendMessage(GetDlgItem(hDlg, ETXT_TDatum), WM_GETTEXT,
								11,	(LPARAM)tRad->szDatum);
				sFormatDatum(tRad->szDatum, NULL, NULL, NULL);
				SendMessage(GetDlgItem(hDlg, ETXT_TDatum), WM_SETTEXT,
								0, (LPARAM)tRad->szDatum);
				SendMessage(GetDlgItem(hDlg, ETXT_TBDatum), WM_SETTEXT,
								0, (LPARAM)tRad->szDatum);
			break;
			case ETXT_TBZeit:
				SendMessage(GetDlgItem(hDlg, ETXT_TBZeit), WM_GETTEXT, 6, (LPARAM)auinit.szBZeit);
				sFormatTime(auinit.szBZeit, NULL, NULL);
				SendMessage(GetDlgItem(hDlg, ETXT_TBZeit), WM_SETTEXT,
								0, (LPARAM)auinit.szBZeit);
			break;
			case ETXT_TBDatum:
				SendMessage(GetDlgItem(hDlg, ETXT_TBDatum), WM_GETTEXT, 11, (LPARAM)auinit.szBDatum);
				sFormatDatum(auinit.szBDatum, NULL, NULL, NULL);
				SendMessage(GetDlgItem(hDlg, ETXT_TBDatum), WM_SETTEXT,
								0, (LPARAM)auinit.szBDatum);
            break;
			};  /* endswitch*/
        };  /* endif*/
		break;

	 default:
         return (NULL);
	 }  /* endswitch  */

return(TRUE);
}

/****************************************************************************/
/*	FUNCTION: DlgOrbenEin(HWND, unsigned, WORD, LONG)                    */
/*****************************************************************************/
LRESULT 	 CALLBACK DlgOrbenEin(HWND hDlg, UINT message,
									 WPARAM wParam, LPARAM lParam )
{

switch (message) {
	case WM_INITDIALOG:
		 sXTPlanet=sXPlanet;
		 sYTPlanet=sYPlanet;
		 sLastOrbFocus=-1;
		 sOldXPl=-1;
		 sOldYPl=-1;
		 sXPlanet=0;
		 sYPlanet=0;
		 CheckRadioButton(hDlg, RBP_START, RBP_END, sOrbenTyp+RBP_RADIX);
		 CheckDlgButton(hDlg, CHB_ZOMBIE, 0);
		 sZombie=1;
		 SetScrollRange(GetDlgItem(hDlg, SB_HOR), SB_CTL, 0, MAX_PLANET+5-XORB, 0);
		 SetScrollPos(GetDlgItem(hDlg, SB_HOR), SB_CTL, 0, 1);
		 SetScrollRange(GetDlgItem(hDlg, SB_VER), SB_CTL, 0, MAX_PLANET-1, 0);
		 SetScrollPos(GetDlgItem(hDlg, SB_VER), SB_CTL, 0, 1);
		 SetWindowText(GetDlgItem(hDlg, TXT_TYP), szHoroTyp[0]);
		 sSubText(hDlg);
		 sSetOrbTxt(hDlg, DRW_ORB_PLX);
		 sSetOrbTxt(hDlg, DRW_ORB_PLY);
		 sSetOrbTxt(hDlg, DRW_ORB_ASP);
		 sSetOrbEdit(hDlg);
		 break;

	case WM_HSCROLL:
		switch(wParam){
		case SB_THUMBTRACK:
			sXPlanet=LOWORD(lParam);
        break;
		case SB_LINEDOWN:
			 sXPlanet=sXPlanet<(MAX_PLANET+5-XORB)?sXPlanet+1:sXPlanet;
		break;
		case SB_LINEUP:
			 sXPlanet=sXPlanet>0?sXPlanet-1:0;
		break;
		case SB_PAGEDOWN:                     
			 sXPlanet=sXPlanet+XORB-1<(MAX_PLANET+5-XORB)?sXPlanet+XORB:MAX_PLANET+5-XORB;
		break;
		case SB_PAGEUP:
			 sXPlanet=sXPlanet>XORB?sXPlanet-XORB:0;
		break;
		}/*endswitch*/
	    SetScrollPos(GetDlgItem(hDlg, SB_HOR), SB_CTL, sXPlanet, 1);
		sSetOrbTxt(hDlg, DRW_ORB_PLX);
	break;
	case WM_VSCROLL:
		switch(wParam){
		case SB_THUMBTRACK:
			sYPlanet=LOWORD(lParam);
		break;
		case SB_LINEDOWN:
			 sYPlanet=sYPlanet<MAX_PLANET-1?sYPlanet+1:sYPlanet;
		break;
		case SB_LINEUP:
			 sYPlanet=sYPlanet>0?sYPlanet-1:0;
		break;
		case SB_PAGEDOWN:
			 sYPlanet=P_LILITH;
		break;
		case SB_PAGEUP:
			 sYPlanet=0;
		break;
		}/*endswitch*/
		SetScrollPos(GetDlgItem(hDlg, SB_VER), SB_CTL, sYPlanet, 1);
		sSetOrbTxt(hDlg, DRW_ORB_PLY);
	break;

	case WM_COMMAND:
		if (HIWORD(lParam)==BN_CLICKED){
		   if (wParam>=RBP_START&&wParam<=RBP_END ) {
			   sOrbenTyp=wParam-RBP_RADIX;
			   CheckRadioButton(hDlg, RBP_START, RBP_END, wParam);
			   SetWindowText(GetDlgItem(hDlg, TXT_TYP), szHoroTyp[wParam-RBP_RADIX]);
			   sOldXPl=-1;
			   sSetOrbTxt(hDlg, DRW_ORB_PLX);
			   sSetOrbTxt(hDlg, DRW_ORB_PLY);
			   break;
		   }/*endif*/
		   if(wParam==CHB_ZOMBIE)
			 CheckDlgButton(hDlg, CHB_ZOMBIE,
				!SendMessage((HWND)LOWORD(lParam), BM_GETCHECK, 0, 0));
		   }/*endif*/
		switch (wParam) {
			case PB_Ok:
				 sStartCalc(hDlg, ORBEN_S_TXT);
				 if (!sOpenIni(SEEK_ORB_OK))
					MessageBox(hDlg, "Probleme beim Schreiben des INI-Files! :(", 0, MB_OK);
			     else {
					EndDialog(hDlg, TRUE);
			     }/*endif*/
				 sSetOrbEdit(hDlg);
				 sEndCalc();
				 break;

			case PB_ZAP:
				 sStartCalc(hDlg, ORBEN_DEFEIN_TXT);
				 fOrben=fopen( ORBDAT, "rt" );
				 sOrben(fOrben);
				 fclose(fOrben);
				 sSetOrbEdit(hDlg);
				 sEndCalc();
				break;

			case PB_Default:
				 sStartCalc(hDlg, ORBEN_EIN_TXT);
				 if (sOpenIni(SEEK_ORB_DEF))
					sSetOrbEdit(hDlg);
				 else
					SendMessage(hDlg, WM_COMMAND, PB_ZAP, 0);
				 sEndCalc();
			     break;

            case PB_Abbruch:
				SendMessage(hDlg, WM_COMMAND, PB_Default, 0);
				EndDialog(hDlg, FALSE);
				break;

			case PB_Hilfe:
				MessageBox(hDlg, "Keine Hilfe :)", 0, MB_OK);
			    break;
		}  /* endswitch  */
		if (HIWORD(lParam)==EN_KILLFOCUS)
		   sAddOrb( hDlg, wParam );
		if (HIWORD(lParam)==EN_SETFOCUS) 
		   sSetOrbFocus( hDlg, wParam);
		break;

	 case WM_DESTROY:
		  sXPlanet=sXTPlanet;
		  sYPlanet=sYTPlanet;
	 break;

	default:
		return (FALSE);
}  /* endswitch  */

return(TRUE);
}
/****************************************************************************/
/* SubClass Text  */
/****************************************************************************/
LRESULT	 CALLBACK SubText(HWND hDlg, UINT message,
						   WPARAM wParam, LPARAM lParam)
{

switch (message) {
	case WM_PAINT:
		 sPaintOrbTxt(hDlg);
	break;
	default:
		 return (LRESULT)CallWindowProc(lpTextProc, hDlg, message, wParam, lParam);
 } /* endswitch*/
 return TRUE;
}

/****************************************************************************/
/* SubClass Text  */
/****************************************************************************/
LRESULT	 CALLBACK SubTransitText(HWND hDlg, UINT message,
						  		 WPARAM wParam, LPARAM lParam)
{

switch (message) {
	case WM_PAINT:
		 sPaintTransitTxt(hDlg);
	break;
	default:
		 return (LRESULT)CallWindowProc(lpTextTransitProc, hDlg, message, wParam, lParam);
 } /* endswitch*/
 return TRUE;
}

/****************************************************************************/
/*	FUNCTION: DlgTransitEin(HWND, unsigned, WORD, LONG)                    */
/*****************************************************************************/
LRESULT 	 CALLBACK DlgTransEin(HWND hDlg, UINT message,
									 WPARAM wParam, LPARAM lParam )
{
DLGPROC lpDlgProc=0;
POINT   point;
short   sID=0;
short   sY=0;
short   sC=0;

switch (message) {
	case WM_INITDIALOG:
		 memset(&rectOButton, 0, sizeof(RECT));
		 tRad=(RADIX*)lParam;
		 sOldXPl=-1;
		 sOldYPl=-1;
         sXPlanet=0;
		 sYPlanet=0;
		 sOrbenTyp=RBP_TRANSIT-RBP_RADIX;
		 SetScrollRange(GetDlgItem(hDlg, SB_HOR), SB_CTL, 0, MAX_PLANET+5-XORB, 0);
		 SetScrollPos(GetDlgItem(hDlg, SB_HOR), SB_CTL, 0, 1);
		 SetScrollRange(GetDlgItem(hDlg, SB_VER), SB_CTL, 0, MAX_PLANET-1, 0);
		 SetScrollPos(GetDlgItem(hDlg, SB_VER), SB_CTL, 0, 1);
		 SendMessage( GetDlgItem(hDlg, TXT_DATE), WM_SETTEXT,
		 				0, (LPARAM)tRad->szDatum);
		 SendMessage( GetDlgItem(hDlg, TXT_BDATE), WM_SETTEXT,
						0, (LPARAM)auinit.szBDatum);
		 SendMessage( GetDlgItem(hDlg, TXT_TIME), WM_SETTEXT,
		 				0, (LPARAM)tRad->szZeit);
		 SendMessage( GetDlgItem(hDlg, TXT_BTIME), WM_SETTEXT,
						0, (LPARAM)auinit.szBZeit);
		 sSubText(hDlg);
		 sSubTransitText(hDlg);
		 sSetOrbTxt(hDlg, DRW_TRANS_ALL);
		 sSetOrbTxt(hDlg, DRW_TRANS_PLX);
		 sSetOrbTxt(hDlg, DRW_TRANS_PLY);
		 sSetOrbTxt(hDlg, DRW_ORB_ASP);
		 SendMessage(hDlg, WM_COMMAND, PB_Default, 0);
		 break;

	case WM_PAINT:
	     sPaintTransRect(hDlg);
	break;

	case WM_LBUTTONDOWN:
		 sButton=1;
		 rectButton.left=LOWORD(lParam);
		 rectButton.top=HIWORD(lParam);
		 rectButton.right=LOWORD(lParam)+1;
		 rectButton.bottom=HIWORD(lParam)+1;
		 point.x=rectButton.left;
		 point.y=rectButton.top;
		 sID=GetDlgCtrlID(ChildWindowFromPoint(hDlg, point));
		 if (sID<=TXTP_END&&sID>=TXTP_START){
			InvalidateRect(GetDlgItem(hDlg, sID), NULL, 1);
			sID-=TXTP_START;
			sC=(short)(sID/XORB);
			sY=sXPlanet+sID%XORB;
			pszTransSel[sC*MAX_PLANET*(MAX_PLANET+5)+sYPlanet*(MAX_PLANET+5)+sY]=
	 		!pszTransSel[sC*MAX_PLANET*(MAX_PLANET+5)+sYPlanet*(MAX_PLANET+5)+sY];
		   break;
		 }/*endif*/
		 InvalidateRect(hDlg, &rectButton, 1);
//		 memset(&rectOButton, 0, sizeof(RECT));
	break;
/*	case WM_MOUSEMOVE:
         if (sButton){
			 rectButton.right=LOWORD(lParam);
			 rectButton.bottom=HIWORD(lParam);
             InvalidateRect(hDlg, &rectButton, 0);
		 }/*endif*/
//	break;
	case WM_LBUTTONUP:
		 sButton=0;
//		 rectOButton.right=LOWORD(lParam);
//		 rectOButton.bottom=HIWORD(lParam);
//		 InvalidateRect(hDlg, &rectButton, 0);
	break;

	case WM_HSCROLL:
		switch(wParam){
		case SB_THUMBTRACK:
			sXPlanet=LOWORD(lParam);
        break;
		case SB_LINEDOWN:
			 sXPlanet=sXPlanet<(MAX_PLANET+5-XORB)?sXPlanet+1:sXPlanet;
		break;
		case SB_LINEUP:
			 sXPlanet=sXPlanet>0?sXPlanet-1:0;
		break;
		case SB_PAGEDOWN:                     
			 sXPlanet=sXPlanet+XORB-1<(MAX_PLANET+5-XORB)?sXPlanet+XORB:MAX_PLANET+5-XORB;
		break;
		case SB_PAGEUP:
			 sXPlanet=sXPlanet>XORB?sXPlanet-XORB:0;
		break;
		}/*endswitch*/
		SetScrollPos(GetDlgItem(hDlg, SB_HOR), SB_CTL, sXPlanet, 1);
		sSetOrbTxt(hDlg, DRW_TRANS_PLX);
	break;
	case WM_VSCROLL:
		switch(wParam){
		case SB_THUMBTRACK:
			sYPlanet=LOWORD(lParam);
		break;
		case SB_LINEDOWN:
			 sYPlanet=sYPlanet<MAX_PLANET-1?sYPlanet+1:sYPlanet;
		break;
		case SB_LINEUP:
			 sYPlanet=sYPlanet>0?sYPlanet-1:0;
		break;
		case SB_PAGEDOWN:
			 sYPlanet=P_LILITH;
		break;
		case SB_PAGEUP:
			 sYPlanet=0;
		break;
		}/*endswitch*/
		if (sYPlanet<P_LILITH&&LOWORD(lParam)!=1)
			  SendMessage(hDlg, WM_COMMAND, RB_TPLANET, MAKELONG(1, BN_CLICKED));
		if (sYPlanet>=P_LILITH&&LOWORD(lParam)!=1)
			  SendMessage(hDlg, WM_COMMAND, RB_TASTER, MAKELONG(1, BN_CLICKED));
		SetScrollPos(GetDlgItem(hDlg, SB_VER), SB_CTL, sYPlanet, 1);
		sSetOrbTxt(hDlg, DRW_TRANS_PLY);
	break;

	case WM_COMMAND:
		if (HIWORD(lParam)==BN_CLICKED&&wParam){
		   switch(wParam) {
		   case RB_TPLANET:
				 if (LOWORD(lParam)!=1)
					SendMessage(hDlg, WM_VSCROLL, SB_PAGEUP, 1);
				 CheckRadioButton(hDlg, RB_TSTART, RB_TEND, RB_TPLANET);
				 CheckDlgButton(hDlg, CHB_PLANET, pszTransSel[MAX_TRANS_SEL-6]);
				 sSetTransSel(hDlg, SET_TRANS_PLANET, 3);
				 CheckDlgButton(hDlg, CHB_ASTER, pszTransSel[MAX_TRANS_SEL-5]);
				 sSetTransSel(hDlg, SET_TRANS_ASTER, 3);
				 CheckDlgButton(hDlg, CHB_HAUS, pszTransSel[MAX_TRANS_SEL-4]);
				 sSetTransSel(hDlg, SET_TRANS_HAUS, 3);
		   break;
		   case RB_TASTER:
				 if (LOWORD(lParam)!=1)
				    SendMessage(hDlg, WM_VSCROLL, SB_PAGEDOWN, 1);
				 CheckRadioButton(hDlg, RB_TSTART, RB_TEND, RB_TASTER);
				 CheckDlgButton(hDlg, CHB_PLANET, pszTransSel[MAX_TRANS_SEL-3]);
				 sSetTransSel(hDlg, SET_TRANS_PLANET, 3);
				 CheckDlgButton(hDlg, CHB_ASTER, pszTransSel[MAX_TRANS_SEL-2]);
				 sSetTransSel(hDlg, SET_TRANS_ASTER, 3);
				 CheckDlgButton(hDlg, CHB_HAUS, pszTransSel[MAX_TRANS_SEL-1]);
				 sSetTransSel(hDlg, SET_TRANS_HAUS, 3);
		   break;
		   case CHB_PLANET:
				 CheckDlgButton(hDlg, wParam,
				   !SendMessage((HWND)LOWORD(lParam), BM_GETCHECK, 0, 0));
				 sSetTransSel(hDlg, SET_TRANS_PLANET, SendMessage((HWND)LOWORD(lParam), BM_GETCHECK, 0, 0));
		   break;
		   case CHB_ASTER:
				 CheckDlgButton(hDlg, wParam,
				   !SendMessage((HWND)LOWORD(lParam), BM_GETCHECK, 0, 0));
				 sSetTransSel(hDlg, SET_TRANS_ASTER, SendMessage((HWND)LOWORD(lParam), BM_GETCHECK, 0, 0));
		   break;
		   case CHB_HAUS:
				 CheckDlgButton(hDlg, wParam,
				   !SendMessage((HWND)LOWORD(lParam), BM_GETCHECK, 0, 0));
				 sSetTransSel(hDlg, SET_TRANS_HAUS, SendMessage((HWND)LOWORD(lParam), BM_GETCHECK, 0, 0));
		   break;
		   }/*endswitch*/
		   sSetOrbTxt(hDlg, DRW_TRANS_ALL);
		}/*endif*/
		switch (wParam) {
			case PB_ORB:
				lpDlgProc=(DLGPROC)MakeProcInstance((FARPROC)DlgOrbenEin, hInstance);
				DialogBox(hInstance, "ORBEN_EIN", hDlg, lpDlgProc);
				FreeProcInstance ( (FARPROC) lpDlgProc );
			break;
			case PB_ALL:
				 CheckDlgButton(hDlg, CHB_PLANET, 1);
				 CheckDlgButton(hDlg, CHB_ASTER, 1);
				 CheckDlgButton(hDlg, CHB_HAUS, 1);
				 sSetTransSel(hDlg, SET_TRANS_ALL, 1);
				 sSetOrbTxt(hDlg, DRW_TRANS_ALL);
			break;
			case PB_NONE:
				 CheckDlgButton(hDlg, CHB_PLANET, 0);
				 CheckDlgButton(hDlg, CHB_ASTER, 0);
				 CheckDlgButton(hDlg, CHB_HAUS, 0);
				 sSetTransSel(hDlg, SET_TRANS_NONE, 0);
				 sSetOrbTxt(hDlg, DRW_TRANS_ALL);
			break;
			case PB_Ok:
				 sStartCalc(hDlg, TRANS_S_TXT);
				 if (!sOpenIni(SEEK_TRANS_OK))
					MessageBox(hDlg, "Probleme beim Schreiben des INI-Files! :(", 0, MB_OK);
			     else 
					EndDialog(hDlg, TRUE);
				 sEndCalc();
				 break;

			case PB_Default:
				 sStartCalc(hDlg, TRANS_EIN_TXT);
				 sOpenIni(SEEK_TRANS_DEF);
				 CheckRadioButton(hDlg, RB_TSTART, RB_TEND, RB_TPLANET);
				 CheckDlgButton(hDlg, CHB_PLANET, pszTransSel[MAX_TRANS_SEL-6]);
				 sSetTransSel(hDlg, SET_TRANS_PLANET, 3);
				 CheckDlgButton(hDlg, CHB_ASTER, pszTransSel[MAX_TRANS_SEL-5]);
				 sSetTransSel(hDlg, SET_TRANS_ASTER, 3);
				 CheckDlgButton(hDlg, CHB_HAUS, pszTransSel[MAX_TRANS_SEL-4]);
				 sSetTransSel(hDlg, SET_TRANS_HAUS, 3);
				 sSetOrbTxt(hDlg, DRW_TRANS_ALL);
				 sEndCalc();
				 break;

            case PB_Abbruch:
				SendMessage(hDlg, WM_COMMAND, PB_Default, 0);
				EndDialog(hDlg, FALSE);
				break;

			case PB_Hilfe:
				MessageBox(hDlg, "Keine Hilfe :)", 0, MB_OK);
			    break;
		}  /* endswitch  */
		break;

	default:
		return (FALSE);
}  /* endswitch  */
return(TRUE);
}

/****************************************************************************/
/* SubClass Text  */
/****************************************************************************/
LRESULT	 CALLBACK SubFarbTxt(HWND hDlg, UINT message,
						 	 WPARAM wParam, LPARAM lParam)
{

switch (message) {
	case WM_PAINT:
		 sPaintFarbTxt(hDlg);
	break;
	default:
		 return (LRESULT)CallWindowProc(lpTextProc, hDlg, message, wParam, lParam);
 } /* endswitch*/
 return TRUE;
}

/****************************************************************************/
/*	FUNCTION: DlgFarben(HWND, unsigned, WORD, LONG)                    */
/*****************************************************************************/
LRESULT 	 CALLBACK DlgFarben(HWND hDlg, UINT message,
									 WPARAM wParam, LPARAM lParam )
{
POINT   point;
char szBuffer[4];
switch (message) {
	case WM_INITDIALOG:
		 sXPlanet=0; //scrollpos
		 sYPlanet=BL_HS; //id
		 SetScrollRange(GetDlgItem(hDlg, SB_R), SB_CTL, 0, 255, 0);
		 SetScrollPos(GetDlgItem(hDlg, SB_R), SB_CTL, 0, 1);
		 SetScrollRange(GetDlgItem(hDlg, SB_G), SB_CTL, 0, 255, 0);
		 SetScrollPos(GetDlgItem(hDlg, SB_G), SB_CTL, 0, 1);
		 SetScrollRange(GetDlgItem(hDlg, SB_B), SB_CTL, 0, 255, 0);
		 SetScrollPos(GetDlgItem(hDlg, SB_B), SB_CTL, 0, 1);
		 SendMessage( GetDlgItem(hDlg, EDIT_R), EM_LIMITTEXT, 3, 0L);
		 SendMessage( GetDlgItem(hDlg, EDIT_G), EM_LIMITTEXT, 3, 0L);
		 SendMessage( GetDlgItem(hDlg, EDIT_B), EM_LIMITTEXT, 3, 0L);
		 sReadDefColor(fIni, 0, SCR_COLOR_INDEX);
		 sReadDefColor(fIni, 0, PRN_COLOR_INDEX);
		 if(Sys.ScrCol<3){ 
			CheckDlgButton(hDlg, CHB_MONO_S, 1);
			sSetMono(hDlg, CHB_MONO_S);
		 }
		 if(Sys.PrnCol<3){
			CheckDlgButton(hDlg, CHB_MONO_P, 1);
			sSetMono(hDlg, CHB_MONO_P);
		 }
		 sSubFarbTxt(hDlg);
		 sGetColor(hDlg);
//		 sSetOrbTxt(hDlg, DRW_FARB_ALL);
		 break;

	case WM_LBUTTONDOWN:
		 point.x=LOWORD(lParam);
		 point.y=HIWORD(lParam);
		 sOldYPl=sYPlanet;
		 sYPlanet=GetDlgCtrlID(ChildWindowFromPoint(hDlg, point));
		 if (sYPlanet>=BL_HS&&sYPlanet<BL_HP+COLORS||sYPlanet==TXT_ALT){
			sGetColor(hDlg);
			InvalidateRect(GetDlgItem(hDlg, sYPlanet), NULL, 1);
		 }/*endif*/
         else
		   sYPlanet=sOldYPl;
	break;

	case WM_HSCROLL:
		sXPlanet=GetScrollPos((HWND)HIWORD(lParam), SB_CTL);
		switch(wParam){
		case SB_THUMBTRACK:
			sXPlanet=LOWORD(lParam);
        break;
		case SB_LINEDOWN:
			 sXPlanet=sXPlanet<256?sXPlanet+1:sXPlanet;
		break;
		case SB_LINEUP:
			 sXPlanet=sXPlanet>0?sXPlanet-1:0;
		break;
		case SB_PAGEDOWN:                     
			 sXPlanet=sXPlanet+16<240?sXPlanet+16:255;
		break;
		case SB_PAGEUP:
			 sXPlanet=sXPlanet>15?sXPlanet-16:0;
		break;
		}/*endswitch*/
		sSetColor(hDlg, (HWND)HIWORD(lParam));
	break;

	case WM_COMMAND:
		 if (HIWORD(lParam)==BN_CLICKED&&wParam&&Sys.ScrCol>2){
		    switch(wParam) {
			case CHB_MONO_P:
			case CHB_MONO_S:
			     CheckDlgButton(hDlg, wParam,
					!SendMessage((HWND)LOWORD(lParam), BM_GETCHECK, 0, 0));
				 sSetMono(hDlg, wParam);
		    break;
			} /*endswitch*/
		 } /*endif*/
		 switch (wParam) {
			case PB_Default:
				sStartCalc(hDlg, FARB_EIN_TXT);
				sReadDefColor(fEphe, 1, SCR_COLOR_INDEX);
				sReadDefColor(fEphe, 1, PRN_COLOR_INDEX);
				sEndCalc();
				sYPlanet=BL_HS;
				if(Sys.ScrCol>2)
				  CheckDlgButton(hDlg, CHB_MONO_S, 0);
			    CheckDlgButton(hDlg, CHB_MONO_P, 0);
				sGetColor(hDlg);
				sSetOrbTxt(hDlg, DRW_FARB_ALL);
				break;

			case PB_Ok:
				sStartCalc(hDlg, FARB_S_TXT);
				sWriteDefColor();
				sSetScrCol();
				sEndCalc();
				EndDialog(hDlg, TRUE);
				break;

			case PB_Abbruch:
				sStartCalc(hDlg, FARB_EIN_TXT);
				SendMessage(hDlg, WM_COMMAND, PB_Default, 0);
				EndDialog(hDlg, FALSE);
				sEndCalc();
				break;

			case PB_Hilfe:
				MessageBox(hDlg, "Keine Hilfe :)", 0, MB_OK);
				break;
		} /* endswitch */
		switch(HIWORD(lParam)){
		case EN_KILLFOCUS:
			SendMessage(GetDlgItem(hDlg, wParam), WM_GETTEXT, 4, (LPARAM)szBuffer);
			sXPlanet=atoi(szBuffer);
			SetScrollPos(GetDlgItem(hDlg, wParam-3), SB_CTL, sXPlanet, 1);
			sSetColor(hDlg, GetDlgItem(hDlg, wParam-3));
			break;
        case EN_SETFOCUS:
			SendMessage(GetDlgItem(hDlg, wParam),
					EM_SETSEL, 0 , (LPARAM)MAKELONG(0, 32768)			);                          //	    cInput=1;
			break;
		}/*endswitch*/;
		break;

     default:
		 return (FALSE);
}  /* endswitch  */

return(TRUE);
}

/****************************************************************************/
/*	FUNCTION: sEndCalc, sStartCalc                                          */
/*****************************************************************************/
short sStartCalc(HWND hDlg, char* pszText)
{

 lpCalcProc=(DLGPROC)MakeProcInstance((FARPROC)DlgCalc, hInstance);
 hWndCalc = CreateDialog(hInstance, "CALC", hDlg, lpCalcProc);
 SetWindowText(GetDlgItem(hWndCalc, TXT_CALC), pszText);

 return ERR_OK;
}

short sEndCalc(void)
{

 DestroyWindow(hWndCalc);
 FreeProcInstance((FARPROC)lpCalcProc);
 hWndCalc=0;
 lpCalcProc=0;

 return ERR_OK;
}

/****************************************************************************/
/*	FUNCTION: DlgCalc(HWND, unsigned, WORD, LONG)                           */
/*****************************************************************************/
LRESULT CALLBACK DlgCalc(HWND hDlg, UINT message,
					   WPARAM wParam, LPARAM lParam)
{
switch (message) {
	case WM_INITDIALOG:
	break;

	 default:
		 return (FALSE);
}  /* endswitch  */

return(TRUE);
}

/****************************************************************************/
/*	FUNCTION: DlgOErfassen(HWND, unsigned, WORD, LONG)                         */
/****************************************************************************/
LRESULT CALLBACK DlgOErfassen(HWND hDlg, UINT message,
                           WPARAM wParam, LPARAM lParam)
{

switch (message) {
	case WM_INITDIALOG:
		 SendMessage( GetDlgItem(hDlg, ETXT_LangeG), EM_LIMITTEXT, 3, 0L);
		 SendMessage( GetDlgItem(hDlg, ETXT_LangeM), EM_LIMITTEXT, 2, 0L);
		 SendMessage( GetDlgItem(hDlg, ETXT_LangeS), EM_LIMITTEXT, 2, 0L);
		 SendMessage( GetDlgItem(hDlg, ETXT_BreiteG), EM_LIMITTEXT, 2, 0L);
		 SendMessage( GetDlgItem(hDlg, ETXT_BreiteM), EM_LIMITTEXT, 2, 0L);
		 SendMessage( GetDlgItem(hDlg, ETXT_BreiteS), EM_LIMITTEXT, 2, 0L);
		 SendMessage( GetDlgItem(hDlg, ETXT_Ort), EM_LIMITTEXT, 32, 0L);
		 SendMessage( GetDlgItem(hDlg, ETXT_Zeitzone), EM_LIMITTEXT, 5, 0L);
		 SendMessage( GetDlgItem(hDlg, ETXT_Land), EM_LIMITTEXT, 3, 0L);
		 CheckRadioButton(hDlg, RB_OST, RB_WEST, RB_OST);
		 CheckRadioButton(hDlg, RB_NORD, RB_SUD, RB_NORD);
		 break;

	case WM_COMMAND:
		if (HIWORD(lParam)==BN_CLICKED&&wParam){
		   switch(wParam) {
		   case RB_OST:
				 CheckRadioButton(hDlg, RB_OST, RB_WEST, RB_OST);
		   break;
		   case RB_WEST:
				 CheckRadioButton(hDlg, RB_OST, RB_WEST, RB_WEST);
		   break;
		   case RB_NORD:
				 CheckRadioButton(hDlg, RB_NORD, RB_SUD, RB_NORD);
		   break;
		   case RB_SUD:
				 CheckRadioButton(hDlg, RB_NORD, RB_SUD, RB_SUD);
		   break;
		   }/*endswitch*/
		}/*endif*/
		switch (wParam) {
			case PB_Ok:					//hinzufügen
				if (sAddOrt(hDlg, NULL, &stOrte, szOrte)>0)
					SetWindowText(GetDlgItem(hDlg, TXT_Erfasst), ORT_ERFASST_TXT);
				break;

			case PB_Abbruch:
/*                if ( cInput ){
				   if (MessageBox(hDlg, "Wirklich Abbrechen?", 0, MB_OKCANCEL)==IDOK)
					  EndDialog(hDlg, TRUE);
                } else {
					  EndDialog(hDlg, TRUE);
                } /* endelse*/
 				EndDialog(hDlg, TRUE);
				break;

			case PB_Hilfe:
				MessageBox(hDlg, "Keine Hilfe :)", 0, MB_OK);
				break;
		} /* endswitch */
		switch(HIWORD(lParam)){
		case EN_KILLFOCUS:
			SetWindowText(GetDlgItem(hDlg, TXT_Erfasst), PERSON_NEU_TXT);
			sAddOrt( hDlg, wParam, &stOrte, szOrte );
			break;
        case EN_SETFOCUS:
			SendMessage(GetDlgItem(hDlg, wParam),
					EM_SETSEL, 0 , (LPARAM)MAKELONG(0, 32768)			);                          //	    cInput=1;
			break;
		}/*endswitch*/;
		break;

	 default:
		 return (NULL);
}  /* endswitch  */

return(TRUE);
}

/****************************************************************************/
/*	FUNCTION: DlgODProc(HWND, unsigned, WORD, LONG) Ort schon vorhanden     */
/****************************************************************************/

LRESULT CALLBACK DlgODProc(HWND hDlg, UINT message,
                           WPARAM wParam, LPARAM lParam)
{

switch (message) {
	case WM_INITDIALOG:
         if (sIndex)
	         sFillNameLB(hDlg, CM_UDD_LB, sIndex);
		 sSetLBTabs(GetDlgItem(hDlg, CM_UDD_LB), 0);
 	     SetFocus(GetDlgItem(hDlg, CM_UDD_LB));
		 break;

	case WM_COMMAND:
		switch (wParam) {
			case CM_UDD_JA:
                sIndex=1;
				EndDialog(hDlg, TRUE);
				break;
//			case CM_UDD_ADD:
//				sIndex=0;
//				EndDialog(hDlg, TRUE);
//				break;
			case CM_UDD_ESC:
                sIndex=-1;
				EndDialog(hDlg, FALSE);
				break;;
			case CM_UDD_HELP:
				MessageBox(hDlg, "Keine Hilfe :)", 0, MB_OK);
				break;				
		} /* endswitch */
		break;

  		default:
           return FALSE;
}  /* endswitch  */

return(TRUE);
}

/****************************************************************************/
/* Ort editfelder setzen    											    */
/****************************************************************************/
short sSetOrtTxt(HWND hDlg, short sID, short sID1, double * dD, char* szBuffer)
{

 sprintf(szBuffer, "%.f", (float)*dD);
 SendMessage(GetDlgItem(hDlg, sID), WM_SETTEXT, 0, (LPARAM)szBuffer);
 SendMessage(GetDlgItem(hDlg, sID1), WM_GETTEXT, 3, (LPARAM)szBuffer);
 *dD=floor(atof(szBuffer));

 return ERR_OK;
}

/****************************************************************************/
/* Ort hinzufügen           											    */
/****************************************************************************/
short sAddOrt( HWND hDlg, short sID, ORTE* stOrte, char * szOrte )
{
short  sRet, sC;
char   szBuffer[BUF_SIZE];
char   szL[32];
char   szB[32];
double dD;
RADIX  Radix;
ORTE   stOrteV;
//DLGPROC lpDlgProc=0;

 switch (sID) {
   case ETXT_LangeG:
   case ETXT_LangeS:
   case ETXT_LangeM:
		SendMessage(GetDlgItem(hDlg, ETXT_LangeG), WM_GETTEXT, 4, (LPARAM)szBuffer);
		stOrte->dLange=floor(atof(szBuffer));
		if(stOrte->dLange>=180)
		  stOrte->dLange=0;
		dD=stOrte->dLange;
		sSetOrtTxt(hDlg, ETXT_LangeG, ETXT_LangeM, &dD, szBuffer);
		if (dD>=60)
           dD=0;
		stOrte->dLange+=dD/100;
		sSetOrtTxt(hDlg, ETXT_LangeM, ETXT_LangeS, &dD, szBuffer);
		if (dD>=60)
           dD=0;
		stOrte->dLange+=dD/10000;
		sSetOrtTxt(hDlg, ETXT_LangeS, ETXT_LangeS, &dD, szBuffer);
		if(SendMessage(GetDlgItem(hDlg, RB_WEST), BM_GETCHECK, 0, 0))
		  stOrte->dLange*=-1;
   break;
   case ETXT_BreiteG:
   case ETXT_BreiteS:
   case ETXT_BreiteM:
		SendMessage(GetDlgItem(hDlg, ETXT_BreiteG), WM_GETTEXT, 3, (LPARAM)szBuffer);
		stOrte->dBreite=floor(atof(szBuffer));
		if(stOrte->dBreite>=90)
		  stOrte->dBreite=90;
		dD=stOrte->dBreite;
		sSetOrtTxt(hDlg, ETXT_BreiteG, ETXT_BreiteM, &dD, szBuffer);
		if (dD>=60)
           dD=0;
		stOrte->dBreite+=dD/100;
		sSetOrtTxt(hDlg, ETXT_BreiteM, ETXT_BreiteS, &dD, szBuffer);
		if (dD>=60)
		   dD=0;
		stOrte->dBreite+=dD/10000;
		sSetOrtTxt(hDlg, ETXT_BreiteS, ETXT_BreiteS, &dD, szBuffer);
		if(SendMessage(GetDlgItem(hDlg, RB_SUD), BM_GETCHECK, 0, 0))
		  stOrte->dBreite*=-1;
   break;
   case ETXT_Zeitzone:
		sC=SendMessage( GetDlgItem(hDlg, ETXT_Zeitzone),
						 WM_GETTEXTLENGTH, 0, 0L);
		if (!sC) {
		   return ERR_ZONE;
		} /* endif*/
		SendMessage(GetDlgItem(hDlg, ETXT_Zeitzone), WM_GETTEXT,
					sC+1, (LPARAM)szBuffer); 
  		stOrte->fZone=(double)atof(szBuffer);
		if (stOrte->fZone<-12.0)
           stOrte->fZone=-12.0;
        if (stOrte->fZone>12.0)
           stOrte->fZone=12.0;
		sprintf(szBuffer, "%2.2f", stOrte->fZone);
		SendMessage(GetDlgItem(hDlg, ETXT_Zeitzone), WM_SETTEXT,
							0, (LPARAM)szBuffer);
//        cInput=1;
   case ETXT_Ort:
		sC=SendMessage( GetDlgItem(hDlg, ETXT_Ort),
						 WM_GETTEXTLENGTH, 0, 0L);
		if (!sC) {
		   return ERR_ORT;
		} /* endif*/
		stOrte->slOrt=sC;
		SendMessage(GetDlgItem(hDlg, ETXT_Ort), WM_GETTEXT, sC+1,
					(LPARAM)szOrte);
		vLower(szOrte);
		szOrte[0]=toupper(szOrte[0]);
		SendMessage(GetDlgItem(hDlg, ETXT_Ort), WM_SETTEXT,
							0, (LPARAM)szOrte);
   break;
   case ETXT_Land:
		sC=SendMessage( GetDlgItem(hDlg, ETXT_Land),
						 WM_GETTEXTLENGTH, 0, 0L);
		if (!sC) {
		   return ERR_ORT;
		} /* endif*/
		SendMessage(GetDlgItem(hDlg, ETXT_Land), WM_GETTEXT, sC+1,
					(LPARAM)stOrte->szLand);
		vUpper(stOrte->szLand);
		SendMessage(GetDlgItem(hDlg, ETXT_Land), WM_SETTEXT,
							0, (LPARAM)stOrte->szLand);
   break;
   case 0:
		if ((sRet=sAddOrt(hDlg, ETXT_Ort, stOrte, szOrte))<=0) {
		   MessageBox(hDlg, "Namen für Ort Eingeben.", 0, MB_OK);
           SetFocus(GetDlgItem(hDlg, ETXT_Ort));
		   return sRet;
        } /* endif*/
		if ((sRet=sAddOrt(hDlg, ETXT_LangeG, stOrte, szOrte))<=0) {
		   MessageBox(hDlg, "Geographische Länge Eingeben (-Ost/+West).", 0, MB_OK);
           SetFocus(GetDlgItem(hDlg, ETXT_LangeG));
           return sRet;
        } /* endif*/
		if ((sRet=sAddOrt(hDlg, ETXT_BreiteG, stOrte, szOrte))<=0) {
		   MessageBox(hDlg, "Geographische Breite Eingeben.", 0, MB_OK);
           SetFocus(GetDlgItem(hDlg, ETXT_BreiteG));
		   return sRet;
        } /* endif*/
		if ((sRet=sAddOrt(hDlg, ETXT_Zeitzone, stOrte, szOrte))<=0) {
		   MessageBox(hDlg, "Zeitzone Eingeben (+Ost/-West).", 0, MB_OK);
           SetFocus(GetDlgItem(hDlg, ETXT_Zeitzone));
           return sRet;
        } /* endif*/
		if ((sRet=sAddOrt(hDlg, ETXT_Land, stOrte, szOrte))<=0) {
		   MessageBox(hDlg, "Land Eingeben.", 0, MB_OK);
		   SetFocus(GetDlgItem(hDlg, ETXT_Land));
           return sRet;
		} /* endif*/
		memcpy(&stOrteV, stOrte, ORT_SIZE);
		strcpy(szBuffer, szOrte);
		sSearchOrt((HWND)(hDlg), 0, szBuffer, &stOrteV);
        fclose(fOrte);
		fOrte=fopen( ORTDAT, "r+b" );
		if (!fOrte)
		   return ERR_FILE;
		if (!memicmp(szBuffer, szOrte, strlen(szBuffer))){
		   MessageBox(hDlg, "Dieser Ort ist bereits\nin der Datenbank!", 0, MB_OK);
		   return ERR_FILE;
/*		   sIndex=1;
		   pszBuf=realloc(pszBuf, 128 );
		   pszBuffer=realloc(pszBuffer, sIndex*BUF_SIZE+1 );
//		   plNamen=realloc(plNamen, sIndex*sizeof(long) );
		   if ( !pszBuf||!pszBuffer) 
			  return ERR_MEM;
		   vGrad2Str(-stOrteV.dLange, szL, 1, 1);
		   vGrad2Str(stOrteV.dBreite, szB, 0, 1);
		   sprintf(pszBuf, "%-32s\t%s\t%s\t%s\t%.2f", szBuffer,  szL, szB, stOrteV.szLand, stOrteV.fZone );
		   pszBuf[BUF_SIZE-1]=0;
		   strcpy(&pszBuffer[(sIndex-1)*BUF_SIZE], pszBuf);
//		   plNamen[sIndex-1]=stOrte.lIndex;
		   lpDlgProc=(DLGPROC)MakeProcInstance((FARPROC)DlgODProc, hInstance);
		   DialogBox(hInstance, "ORT_DOPPELT", hDlg, lpDlgProc);
		   FreeProcInstance ( (FARPROC) lpDlgProc );
		   if(sIndex==-1) 
			  return ERR_CANCEL;

		   //Wenn Record schon vorhanden, Record als ungültig markieren...
		   if ( sIndex==1 ) {
			  if (fseek(fOrte, plNamen[sIndex-1], SEEK_SET))
			  if (fseek(fOrte, stOrteV.lIndex, SEEK_SET))
				 return ERR_FILE;
			  stOrte->cGultig=0;
			  sRet=fwrite( stOrte, sizeof(short), 1, fOrte );
			  if ( sRet!=1 ) 
				 return ERR_FILE;
			 }/*endif*/
		 }/*endif*/
		 if (fseek(fOrte, 0, SEEK_END))
			return ERR_FILE;
		 stOrte->cGultig=MAIN_VERSION;
		 stOrte->slOrt=strlen(szOrte);
		 stOrte->slRec=ORT_SIZE+stOrte->slOrt;
		 stOrte->lIndex=ftell(fOrte);
		 sRet=fwrite( stOrte, ORT_SIZE, 1, fOrte );
		 sC=fwrite( szOrte, 1, stOrte->slOrt, fOrte );
		 fclose(fOrte);
		 fOrte=fopen( ORTDAT, "rb" );
		 if (!fOrte)
			return ERR_FILE;
		 SendMessage(GetDlgItem(hDlg, ETXT_Ort),
					EM_SETSEL, 0 , (LPARAM)MAKELONG(0, 32768));
//        cInput=0;
   break;
 
   default:                        
   break;
 }/*endswitch*/

 return ERR_OK;
}
/****************************************************************************/
/*	FUNCTION: DlgAspekte (HWND, unsigned, WORD, LONG)                    */
/*****************************************************************************/
LRESULT    CALLBACK DlgAspekte(HWND hDlg, UINT message,
									 WPARAM wParam, LPARAM lParam )
{

switch (message) {
	case WM_INITDIALOG:
		 SendMessage(hDlg, WM_COMMAND, PB_Default, 0);
		 break;

	case WM_COMMAND:
		if (HIWORD(lParam)==BN_CLICKED&&wParam){
		   switch(wParam) {
		   case RBA_KON:
				auinit.sAspekte^=S_KON;
				CheckDlgButton(hDlg, RBA_KON, auinit.sAspekte&S_KON);
		   break;
		   case RBA_HSX:
				auinit.sAspekte^=S_HAL;
				CheckDlgButton(hDlg, RBA_HSX, auinit.sAspekte&S_HAL);
		   break;
		   case RBA_SEX:
				auinit.sAspekte^=S_SEX;
				CheckDlgButton(hDlg, RBA_SEX, auinit.sAspekte&S_SEX);
		   break;
		   case RBA_QUA:
				auinit.sAspekte^=S_QUA;
				CheckDlgButton(hDlg, RBA_QUA, auinit.sAspekte&S_QUA);
		   break;
		   case RBA_TRI:
				auinit.sAspekte^=S_TRI;
				CheckDlgButton(hDlg, RBA_TRI, auinit.sAspekte&S_TRI);
		   break;
		   case RBA_QUI:
				auinit.sAspekte^=S_QUI;
				CheckDlgButton(hDlg, RBA_QUI, auinit.sAspekte&S_QUI);
		   break;
		   case RBA_OPO:
				auinit.sAspekte^=S_OPO;
				CheckDlgButton(hDlg, RBA_OPO, auinit.sAspekte&S_OPO);
		   break;
		   case RBA_3:
				auinit.sTeilung^=S_3er;
				CheckDlgButton(hDlg, RBA_3, auinit.sTeilung&S_3er);
		   break;
		   case RBA_9:
				auinit.sTeilung^=S_9er;
				CheckDlgButton(hDlg, RBA_9, auinit.sTeilung&S_9er);
		   break;
		   case RBA_PLAN:
				auinit.sAspekte^=S_PLAN;
				CheckDlgButton(hDlg, RBA_PLAN, auinit.sAspekte&S_PLAN);
		   break;
		   case RBA_ASTER:
				auinit.sAspekte^=S_ASTER;
				CheckDlgButton(hDlg, RBA_ASTER, auinit.sAspekte&S_ASTER);
		   break;
		   case RBA_HAUS:
				auinit.sAspekte^=S_HAUS;
				CheckDlgButton(hDlg, RBA_HAUS, auinit.sAspekte&S_HAUS);
		   break;
		   }/*endswitch*/
		}/*endif*/
		switch (wParam) {
			case PB_Ok:
				 sStartCalc(hDlg, SEL_S_TXT);
				 if (!sOpenIni(SEEK_SEL_OK))
					MessageBox(hDlg, "Probleme beim Schreiben des INI-Files! :(", 0, MB_OK);
			     else 
					EndDialog(hDlg, TRUE);
				 sEndCalc();
				 break;

			case PB_Default:
//				 sStartCalc(hDlg, SEL_EIN_TXT);
				 sOpenIni(SEEK_SEL_DEF);
				 CheckDlgButton(hDlg, RBA_KON, auinit.sAspekte&S_KON);
				 CheckDlgButton(hDlg, RBA_HSX, auinit.sAspekte&S_HAL);
				 CheckDlgButton(hDlg, RBA_SEX, auinit.sAspekte&S_SEX);
				 CheckDlgButton(hDlg, RBA_QUA, auinit.sAspekte&S_QUA);
				 CheckDlgButton(hDlg, RBA_TRI, auinit.sAspekte&S_TRI);
				 CheckDlgButton(hDlg, RBA_QUI, auinit.sAspekte&S_QUI);
				 CheckDlgButton(hDlg, RBA_OPO, auinit.sAspekte&S_OPO);
				 CheckDlgButton(hDlg, RBA_3, auinit.sTeilung&S_3er);
				 CheckDlgButton(hDlg, RBA_9, auinit.sTeilung&S_9er);
				 CheckDlgButton(hDlg, RBA_PLAN, auinit.sAspekte&S_PLAN);
				 CheckDlgButton(hDlg, RBA_ASTER, auinit.sAspekte&S_ASTER);
				 CheckDlgButton(hDlg, RBA_HAUS, auinit.sAspekte&S_HAUS);
//				 sEndCalc();
				 break;

            case PB_Abbruch:
				SendMessage(hDlg, WM_COMMAND, PB_Default, 0);
				EndDialog(hDlg, FALSE);
				break;

			case PB_Hilfe:
				MessageBox(hDlg, "Keine Hilfe :)", 0, MB_OK);
			    break;
		}  /* endswitch  */
		break;

	default:
		return (FALSE);
}  /* endswitch  */
return(TRUE);
}
/****************************************************************************/
/*	FUNCTION: DlgRadix   (HWND, unsigned, WORD, LONG)                    */
/*****************************************************************************/
LRESULT    CALLBACK DlgRadix(HWND hDlg, UINT message,
							 WPARAM wParam, LPARAM lParam )
{

RADIX* Radix=0;
switch (message) {
	case WM_INITRADIX:
		 SendMessage( GetDlgItem(hDlg, ED_MIN), EM_LIMITTEXT, 2, 0L);
		 SendMessage( GetDlgItem(hDlg, ED_STU), EM_LIMITTEXT, 2, 0L);
		 SendMessage( GetDlgItem(hDlg, ED_TAG), EM_LIMITTEXT, 2, 0L);
		 SendMessage( GetDlgItem(hDlg, ED_MON), EM_LIMITTEXT, 2, 0L);
		 SendMessage( GetDlgItem(hDlg, ED_JAH), EM_LIMITTEXT, 4, 0L);
		 SetScrollRange(GetDlgItem(hDlg, SC_MIN), SB_CTL, 0, 59, 0);
		 SetScrollRange(GetDlgItem(hDlg, SC_STU), SB_CTL, 0, 23, 0);
		 SetScrollRange(GetDlgItem(hDlg, SC_TAG), SB_CTL, 0, 31, 0);
		 SetScrollRange(GetDlgItem(hDlg, SC_MON), SB_CTL, 0, 12, 0);
		 SetScrollRange(GetDlgItem(hDlg, SC_JAH), SB_CTL, 1600, 2400, 0);
		 vSetRadixScrollIni((RADIX *)GetWindowLong(
						(HWND)GetWindowWord(hDlg, GWW_HWNDPARENT), 0), hDlg);
		 break;

	case WM_HSCROLL:
		switch(wParam){
		case SB_LINEDOWN:
			 sSetRadixScroll((RADIX *)GetWindowLong(
						(HWND)GetWindowWord(hDlg, GWW_HWNDPARENT), 0),
						hDlg, (HWND)HIWORD(lParam), 1);
		break;
		case SB_LINEUP:
			 sSetRadixScroll((RADIX *)GetWindowLong(
						(HWND)GetWindowWord(hDlg, GWW_HWNDPARENT), 0),
						hDlg, (HWND)HIWORD(lParam), -1);
		break;
		}/*endswitch*/
	break;

	case WM_COMMAND:
		switch (wParam) {
			case PB_NEU:
				Radix=(RADIX *)GetWindowLong((HWND)GetWindowWord(hDlg, GWW_HWNDPARENT), 0);
				PostMessage(hWndMain, WM_COMMAND, CM_U_PERSON, (LPARAM)Radix);
			break;
		}/*endswitch*/;
		break;

     default:
		 return (FALSE);
}  /* endswitch  */
return(TRUE);
}
