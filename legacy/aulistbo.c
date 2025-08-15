#include "astrouni.h"

extern  COLORREF  sColor[COLORS];
extern  LOGFONT   MainFontRec;//auwurzel.c
extern  FILE*     fOrte;
extern  char*     pszBuffer;
extern char  szLNANamen[][2];   //auwurzel.h
extern char  cAspektGly[][2];   
//extern char  szANamen[][2];      //auwurzel.h
extern char  szBNamen[][2];      //auwurzel.h
extern AU_INIT auinit;

LBCOLOR lbcolor;

/****************************************************************************/
/* LB/Ort Zeichnen           											    */
/****************************************************************************/
short sDrawLBOrt( DRAWITEMSTRUCT*Di )
{

char   szOrt[32];
char   szL[32];
char   szB[32];
char   szBuffer[BUF_SIZE];
ORTE   stOrt;

if (Di->itemData==(long)DRW_LB_MENU_ORTE){
    lbcolor.Pen=sColor[COL_LBREC_BK];
	lbcolor.Brush=sColor[COL_LBREC_BK];
    lbcolor.Text=sColor[COL_LBTXT_BK];
    sprintf(szBuffer, "%s %s %s %s %s", "Ort", "Länge", "Breite", "Land", "Zone" );
    sDrawLBState(Di, szBuffer, DRW_LB_NORMAL|DRW_LB_TXT_ORT);
    return ERR_OK;
}/*endif*/

 if (fseek(fOrte, Di->itemData, SEEK_SET))
    return ERR_FILE;

 fread(&stOrt, ORT_SIZE, 1, fOrte);
 if (ferror(fOrte))
    return ERR_FILE;

 fread(szOrt, 1, stOrt.slOrt, fOrte);
 if (ferror(fOrte))
    return ERR_FILE;
 szOrt[stOrt.slOrt]=0;

 vGrad2Str(-stOrt.dLange, szL, 1, 0);
 vGrad2Str(stOrt.dBreite, szB, 0, 0);
// OemToAnsi(szOrt, szOrt);
 sprintf(szBuffer, "%s %s %s %s %.1f", szOrt,  szL, szB, stOrt.szLand, stOrt.fZone );
// sprintf(szBuffer, "%s %.2f%s %.2f%s %s %.1f", szOrt,  fabs(stOrt.dBreite),
// stOrt.dBreite>0?"N":"S", fabs(stOrt.dLange), stOrt.dLange>0?"O":"W",
//	  stOrt.szLand, stOrt.fZone );

 vSetColor(Di);
 sDrawLBState(Di, szBuffer, DRW_LB_TXT_ORT);

return ERR_OK;
}

/*----------------------------------------------------------------------------*/
/* vSetColor            Listbox Sel/DeSel                                     */
/*----------------------------------------------------------------------------*/
void vSetColor(DRAWITEMSTRUCT* Di)
{
switch(Di->itemAction) {
case ODA_DRAWENTIRE:
     lbcolor.Pen=sColor[COL_LBREC_NORM];
     lbcolor.Brush=sColor[COL_LBREC_NORM];
     lbcolor.Text=sColor[COL_LBTXT_NORM];
break;
case ODA_SELECT:
	 if (Di->itemState&ODS_SELECTED) {
		lbcolor.Pen=sColor[COL_LBREC_SEL];
		lbcolor.Brush=sColor[COL_LBREC_SEL];
		lbcolor.Text=sColor[COL_LBTXT_SEL];
		if (sColor[COL_LBREC_SEL]==sColor[COL_LBREC_NORM])
		   lbcolor.Pen=!sColor[COL_LBREC_SEL];
	 }else{
	    lbcolor.Pen=sColor[COL_LBREC_NORM];
    	lbcolor.Brush=sColor[COL_LBREC_NORM];
	    lbcolor.Text=sColor[COL_LBTXT_NORM];
	 }/*endelse*/
     break;

/*case ODA_FOCUS:
	 if (Di->itemState&ODS_FOCUS){
	     lbcolor.Pen=RGB(0,0,0);//sColor[COL_LBREC_FOCUS];
	     lbcolor.Brush=sColor[COL_LBREC_FOCUS];
	     lbcolor.Text=sColor[COL_LBTXT_FOCUS];
	     sDrawLBState(Di, szBuffer, DRW_LB_TXT_ORT);
	 }else{
	    lbcolor.Pen=sColor[COL_LBREC_NORM];
    	lbcolor.Brush=sColor[COL_LBREC_NORM];
	    lbcolor.Text=sColor[COL_LBTXT_NORM];
   		sDrawLBState(Di, szBuffer, DRW_LB_TXT_ORT);
	 }/*endelse*/
//     break;
} /*endswitch*/

}

/****************************************************************************/
/* LB/CB_Zeichnen           											    */
/****************************************************************************/
short sDrawLBState( DRAWITEMSTRUCT*Di, char * szBuffer, short sAction )
{
HFONT  hFont;
HBRUSH hBrush;
HPEN   hPen;

SaveDC(Di->hDC);
MainFontRec.lfHeight = 15;
MainFontRec.lfWidth = (short)((double)MainFontRec.lfHeight/2.5);
hFont=SelectObject(Di->hDC, CreateFontIndirect(&MainFontRec));
hPen=SelectObject(Di->hDC, CreatePen(PS_SOLID, 1, lbcolor.Pen));
hBrush=SelectObject(Di->hDC, CreateSolidBrush(lbcolor.Brush));
SetTextColor(Di->hDC, lbcolor.Text);
Rectangle(Di->hDC, Di->rcItem.left, Di->rcItem.top,
    	  Di->rcItem.right, Di->rcItem.bottom);
SetBkMode(Di->hDC, TRANSPARENT);
sTextOut(Di, szBuffer, sAction);
DeleteObject(SelectObject(Di->hDC, hBrush));
DeleteObject(SelectObject(Di->hDC, hPen));
DeleteObject(SelectObject(Di->hDC, hFont));
RestoreDC(Di->hDC, -1);

return ERR_OK;
}
/****************************************************************************/
/* Text in LB/CB formatieren											    */
/****************************************************************************/
short sTextOut( DRAWITEMSTRUCT*Di, char * szBuffer, short sAction )
{

short t[7];
short sX, sD, sC, sB, sA;
short yAdd=LOWORD(GetTextExtent(Di->hDC, "m", 1));
char  *psz=szBuffer;
char  szOrt[BUF_SIZE];
RECT  rect;
long  dIndex=Di->itemData;
long  lNum;
RADIX * Radix=(RADIX *)GetWindowLong((HWND)GetWindowWord(Di->hwndItem, GWW_HWNDPARENT), 0);
RADIX* plRadix;
char   szD[2]={0,0}, szH[2]={0,0};

rect.left=Di->rcItem.left;
rect.top=Di->rcItem.top;
rect.bottom=Di->rcItem.bottom;

switch(sAction) {
case DRW_LB_TXT_TRANSIT:
	  sA=(short)(Radix->plMulti[dIndex+2]&0xFF);
	  sB=HIBYTE(Radix->plMulti[dIndex+2]&0xFF00);
	  sC=HIWORD(Radix->plMulti[dIndex+2]&0xFF0000);
	  SetTextAlign(Di->hDC, TA_LEFT);
//TPlanet
	  rect.right=rect.left+yAdd*2;
	  SetTextColor(Di->hDC, sColor[COL_PLAN_T]);
	  plRadix=(RADIX*)Radix->plRadix[Radix->plMulti[dIndex]];
	  if ((plRadix->psPlanetTyp[sA]&P_TYP_RUCK))
		 sprintf(szOrt,"%sÒ", szLNANamen[sA]);
      else
	     strcpy(szOrt, szLNANamen[sA]);
	  ExtTextOut(Di->hDC, Di->rcItem.left+1, Di->rcItem.top+1, ETO_CLIPPED,
				&rect, szOrt, strlen(szOrt), NULL);
	  sD=rect.left+yAdd*2;
	  rect.right+=yAdd*2;
	  SetTextColor(Di->hDC, lbcolor.Text);
	  ExtTextOut(Di->hDC, sD+1, Di->rcItem.top+1, ETO_CLIPPED,
				&rect, "in", 2, NULL);
// Zeichen?
	  lNum=Radix->plMulti[dIndex];
	  sD+=yAdd;
	  rect.right+=yAdd*3;
      sX=0;
	  do{
		plRadix=(RADIX*)Radix->plRadix[lNum];
		szH[0]=szBNamen[plRadix->pszStzPlanet[sA]][0];
//		szH[0]=szANamen[plRadix->pszStzPlanet[sA]][0];
		if (szD[0]!=szH[0]){
		   SetTextColor(Di->hDC, sColor[((WORD)szH[0]-197)%4+COL_EL]);
		   sX++;
		   ExtTextOut(Di->hDC, sD+sX*yAdd+1, Di->rcItem.top+1, ETO_CLIPPED,
					&rect, szH, 1, NULL);
		   szD[0]=szH[0];
		}/*endif*/
	  }while(++lNum<=Radix->plMulti[dIndex+1]); /*endwhile*/

//Aspekt
	  SetTextColor(Di->hDC, sColor[CKONJUNKTION+sC]);
	  rect.right+=yAdd*4;
	  sD+=yAdd*4;
	  szOrt[0]=cAspektGly[sC][0];
	  ExtTextOut(Di->hDC, sD+1, Di->rcItem.top+1, ETO_CLIPPED,
				&rect, szOrt, 1, NULL);

//Radix-Haus/Planet
	  if (Radix->plMulti[dIndex+2]&0x01000000){
		 sprintf(szOrt, "Ñ%d", sB+1);
	  }else{
		 if ((Radix->psPlanetTyp[sB]&P_TYP_RUCK))
			sprintf(szOrt,"%sÒ", szLNANamen[sB]);
	     else
			strcpy(szOrt, szLNANamen[sB]);
	  }/*endelse*/
	  SetTextColor(Di->hDC, sColor[COL_PLAN]);
	  rect.right+=yAdd*2;
	  sD+=yAdd*2;
	  ExtTextOut(Di->hDC, sD+1, Di->rcItem.top+1, ETO_CLIPPED,
	  				&rect, szOrt, strlen(szOrt), NULL);
	  if (auinit.sModus){
		  rect.right+=yAdd*3;
		  sD+=yAdd*3;
	  }else {
		  rect.right+=yAdd*2;
		  sD+=yAdd*2;
	  }/*endelse*/
	  SetTextColor(Di->hDC, lbcolor.Text);
	  ExtTextOut(Di->hDC, sD+1, Di->rcItem.top+1, ETO_CLIPPED,
				&rect, "in", 2, NULL);
//Radix-Sternzeichen
	  if (Radix->plMulti[dIndex+2]&0x01000000)
		 sprintf(szOrt, "%s", szBNamen[Radix->pszStzHaus[sB]]);
//		 sprintf(szOrt, "%s", szANamen[Radix->pszStzHaus[sB]]);
	  else
		 sprintf(szOrt, "%s", szBNamen[Radix->pszStzPlanet[sB]]);
//		 sprintf(szOrt, "%s", szANamen[Radix->pszStzPlanet[sB]]);
	  SetTextColor(Di->hDC, sColor[((WORD)szOrt[0]-197)%4+COL_EL]);
	  rect.right+=yAdd*2;
	  sD+=yAdd*2;
	  ExtTextOut(Di->hDC, sD+1, Di->rcItem.top+1, ETO_CLIPPED,
					&rect, szOrt, strlen(szOrt), NULL);
	  strcpy(szOrt, "von  ");
	  plRadix=(RADIX*)Radix->plRadix[Radix->plMulti[dIndex]];
	  strcat(szOrt, plRadix->szDatum);
	  strcat(szOrt, "  bis  ");
	  if (Radix->plMulti[dIndex+1]!=-1){
		 plRadix=(RADIX*)Radix->plRadix[Radix->plMulti[dIndex+1]];
		 strcat(szOrt, plRadix->szDatum);
	  }else
		 strcat(szOrt, "N/A");
	  if (auinit.sModus)
		  sD+=yAdd*2;
	  else
		  sD+=yAdd*2+yAdd/2;
	  rect.right=Di->rcItem.right;
	  SetTextColor(Di->hDC, lbcolor.Text);
	  ExtTextOut(Di->hDC, sD, Di->rcItem.top+1, ETO_CLIPPED,
					&rect, szOrt, strlen(szOrt), NULL);
break;

case DRW_LB_TXT_ORT:
     t[0]=0;
	 t[1]=yAdd*20;
	 t[2]=t[1]+yAdd*5;
     t[3]=t[2]+yAdd*4;
     t[4]=t[3]+yAdd*4;
     t[5]=Di->rcItem.right;

     SetTextAlign(Di->hDC, TA_LEFT);
     szOrt[32]=0;
	 psz=pszGetString(szOrt, psz);
	 rect.right=t[1]-yAdd*4;
     ExtTextOut(Di->hDC, Di->rcItem.left+1, Di->rcItem.top+1, ETO_CLIPPED,
	 	    	 &rect, szOrt, strlen(szOrt), NULL);
     if(!psz++)
       break;
     SetTextAlign(Di->hDC, TA_RIGHT);
     sA=1;
	 do{
		psz=pszGetString(szOrt, psz);
	    rect.right=t[sA+1]-2;
        ExtTextOut(Di->hDC, Di->rcItem.left+t[sA]+1, Di->rcItem.top+1, ETO_CLIPPED,
    	 	    	&rect, szOrt, strlen(szOrt), NULL);
	} while(psz++&&++sA<5);
	break;

case DRW_LB_TXT_HAUS:
     t[0]=0;
	 t[1]=yAdd*2;
     t[2]=t[1]+yAdd*6;
     t[3]=Di->rcItem.right;
     SetTextAlign(Di->hDC, TA_LEFT);
     sA=0;
     do{
	    psz=pszGetString(szOrt, psz);
        if (!(dIndex&0x1000)&&!sA){
   	       t[1]=yAdd*3;
	       t[2]=t[1]+(short)((double)yAdd*6.5);
		} /* endelse*/
        if (!psz)
		   rect.right=t[3]-2;
        else
		   rect.right=t[sA+1]-2;
        if (!sA&&psz){
		   if (!(HIWORD(dIndex)&P_TYP_NORM))
			  SetTextColor(Di->hDC, sColor[COL_PLAN_T]);
		   else
			  SetTextColor(Di->hDC, sColor[COL_PLAN]);
		} else {
			if ( sA==2 ) 
			   SetTextColor(Di->hDC, sColor[((WORD)szOrt[0]-197)%4+COL_EL]);
	        else
		  	   SetTextColor(Di->hDC, lbcolor.Text);
        }/*endelse*/
        ExtTextOut(Di->hDC, Di->rcItem.left+t[sA]+1, Di->rcItem.top+1, ETO_CLIPPED,
    	 	    	&rect, szOrt, strlen(szOrt), NULL);
     } while(psz++&&++sA<3);
	 break;

 case DRW_LB_TXT_DATEN:
     t[0]=0;
	 t[1]=yAdd*9;
     t[2]=Di->rcItem.right;

	 SetTextAlign(Di->hDC, TA_LEFT);
     psz=pszGetString(szOrt, psz);
     if (!psz)
 	    rect.right=t[2]-2;
     else
	    rect.right=t[1]-2;
     ExtTextOut(Di->hDC, Di->rcItem.left+1, Di->rcItem.top+1, ETO_CLIPPED,
    	 	      &rect, szOrt, strlen(szOrt), NULL);
     if(!psz++)
       break;
     SetTextAlign(Di->hDC, TA_RIGHT);
     sA=1;
     do{
	    psz=pszGetString(szOrt, psz);
 	    rect.right=t[sA+1]-2;
        ExtTextOut(Di->hDC, Di->rcItem.left+t[sA]+1, Di->rcItem.top+1, ETO_CLIPPED,
    	 	    	&rect, szOrt, strlen(szOrt), NULL);
     } while(psz++&&++sA<2);
	 break;
case DRW_LB_TXT_ASPEKT:
     t[0]=0;
     t[1]=yAdd*2;			
     t[2]=t[1]+yAdd*2;      
     t[3]=t[2]+yAdd*2;
     t[4]=t[3]+yAdd*2;
     t[5]=t[4]+yAdd*2;
     t[6]=t[5]+yAdd*2;
     t[7]=Di->rcItem.right;

     SetTextAlign(Di->hDC, TA_LEFT);
	 sA=0;
     do{
        if ((dIndex&0x0F000000)&&sA==4){
		   t[4]=t[3]+yAdd;
		   t[5]=t[4]+yAdd*3;
	       t[6]=t[5]+yAdd*2;
		} /* endelse*/
	    psz=pszGetString(szOrt, psz);
 	    rect.right=t[sA+1]-2;
        switch(sA){
        case 0:
		    if (!psz) {
 	    	   rect.right=t[7]-2;
	  	  	   SetTextColor(Di->hDC, lbcolor.Text);
               break;
		    } /*endif*/
	        if ((dIndex&0xF0000000)==DRW_LBA_PLANETT||
				(dIndex&0x0F000000)==DRW_LBA_HAUST)
		   	  SetTextColor(Di->hDC, sColor[COL_PLAN_T]);
            else
		   	  SetTextColor(Di->hDC, sColor[COL_PLAN]);
        break;
		case 4:
			SetTextColor(Di->hDC, sColor[COL_PLAN]);
        break;
        case 1:
        case 5:
   	  	    SetTextColor(Di->hDC, lbcolor.Text);
        break;
        case 2:
        case 6:
			SetTextColor(Di->hDC, sColor[((WORD)szOrt[0]-197)%4+COL_EL]);
		break;
        case 3:
			SetTextColor(Di->hDC, sColor[CKONJUNKTION+HIWORD(dIndex&0xFF0000)]);
		break;   }/* endwitch*/
        ExtTextOut(Di->hDC, Di->rcItem.left+t[sA]+1, Di->rcItem.top+1, ETO_CLIPPED,
    	 	    	&rect, szOrt, strlen(szOrt), NULL);
     } while(psz++&&++sA<8);
  } /*endswitch*/

return ERR_OK;
}

/****************************************************************************/
/* Text in LB/CB formatieren											    */
/****************************************************************************/
char * pszGetString( char * szOut, char* szIn )
{
char * psz1;

while (*szIn==0x20)
   szIn++;

 psz1=strchr(szIn, 0x20 );
 if (!psz1) {
    strcpy(szOut, szIn);
    return NULL;
 }
 *psz1=0;
 strcpy(szOut, szIn);

 return psz1;
}

/****************************************************************************/
/* NamenAuswählenLb auffüllen										    */
/****************************************************************************/

short sFillNameLB (HWND hDlg, short sID, short sIndex)
{
HWND hWndL=GetDlgItem(hDlg, sID);
short sA=0;

do {
	SendMessage(hWndL, LB_ADDSTRING, 0,
			 (LPARAM)&pszBuffer[sA*BUF_SIZE]);
 } while (++sA<sIndex);
 SendMessage(hWndL, LB_SETCURSEL, 0 , 0L);

 return ERR_OK;
}