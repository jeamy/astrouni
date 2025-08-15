/*----------------------------------------------------------------------------*/
/* Autor:               JeaMY  Lee                                            */
/* Datum:               Samstag 27.01.1996                                   */
/*----------------------------------------------------------------------------*/

#include "astrouni.h"


HINSTANCE hInstance;
HWND      hWndMain;
extern    AU_INIT auinit;
extern    SYS Sys;
extern    WNDPROC lpLbProc;
extern    short sOrbenTyp;
extern    char MainMenuText[];
extern    sRadius;
extern    sMpX;
extern    sMpY;
extern    sDruber;

/**************************************************************/
BOOL InitApplication(void)
{
 WNDCLASS  wc;
 WNDCLASS  wcRadix;
 WNDCLASS  wcRadixLB;

    if ( sReadIni()<ERR_OK )
       return 0;

	wcRadix.style = CS_HREDRAW | CS_VREDRAW;
	wcRadix.lpfnWndProc = (long (FAR PASCAL*)())RadixWndProc;
	wcRadix.cbClsExtra = 0;
	wcRadix.cbWndExtra = 4;
	wcRadix.hInstance = hInstance;
	wcRadix.hIcon = LoadIcon(hInstance,"RadixIcon");
	wcRadix.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcRadix.hbrBackground = GetStockObject(WHITE_BRUSH);
	wcRadix.lpszMenuName =  "RADIXMENU";
	wcRadix.lpszClassName = "RADIXCLASS";
	if ( !RegisterClass(&wcRadix) )
	  return 0;

	wcRadix.style = CS_HREDRAW | CS_VREDRAW;
	wcRadix.lpfnWndProc = (long (FAR PASCAL*)())TransitWndProc;
	wcRadix.cbClsExtra = 0;
	wcRadix.cbWndExtra = 4;
	wcRadix.hInstance = hInstance;
	wcRadix.hIcon = LoadIcon(hInstance,"RadixIcon");
	wcRadix.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcRadix.hbrBackground = GetStockObject(WHITE_BRUSH);
	wcRadix.lpszMenuName =  "TRANSITMENU";
	wcRadix.lpszClassName = "TRANSITCLASS";
	if ( !RegisterClass(&wcRadix) )
	  return 0;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (long (FAR PASCAL*)())MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;	
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, "MainIcon");
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = "MAINMENU";
	wc.lpszClassName = "MAINCLASS";

 	return (RegisterClass(&wc));
}

/************************************************************************/
BOOL InitInstance(void)
{
HDC          hDC;
PAINTSTRUCT  PaintInfo;

	hWndMain =  CreateWindow(
				"MAINCLASS", 	              
				MainMenuText,
				WS_CAPTION|WS_SYSMENU|
				WS_MINIMIZEBOX|WS_MAXIMIZEBOX,
				CW_USEDEFAULT,
				CW_USEDEFAULT,	   	
				CW_USEDEFAULT,	      	
				CW_USEDEFAULT,
				NULL,
				NULL,	   		
				hInstance,
				NULL );

	if (!hWndMain)
       return (FALSE);
	ShowWindow(hWndMain, SW_SHOWMAXIMIZED );
	UpdateWindow(hWndMain);
	hDC=hdcPrinterIni(PRN_NORMAL, NULL);
	if(!hDC)
	  Sys.PrnCol=3;
	else
	  DeleteDC(hDC);
	BeginPaint(hWndMain, &PaintInfo);
    hDC = PaintInfo.hdc;
    SaveDC(hDC);
	sSetSys(hDC, 1);
	RestoreDC(hDC, -1);
    EndPaint(hWndMain, &PaintInfo);

	return (TRUE);			
}

/**************************************************************/
HWND PaintRadixWnd(HWND hWndMain, RADIX * Radix )
{
HWND	hWnd;
RECT    rect;
short   sSub, sMenu, sX, sY, sZ;
#define LB 205

	sMenu=GetSystemMetrics(SM_CYMENU)+GetSystemMetrics(SM_CYCAPTION);
 	GetWindowRect(hWndMain, &rect);
	hWnd = CreateWindow(
			"RADIXCLASS",
			"AstroUniverse",
			WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_POPUP,
			4,
			sMenu+4,
			rect.right-25,
			rect.bottom-sMenu-12,
			hWndMain,
			NULL,
			hInstance,
			NULL );

/// Mittelpunkt, Radius und Aspectratio vom Radix definieren
	sSub=GetSystemMetrics(SM_CXVSCROLL);
    GetWindowRect(hWnd, &rect);
	sX=rect.bottom-rect.top-sMenu+2;
	sY=rect.right-sSub;
	if (auinit.sSelHoro==TYP_SYNASTRIE)
	   sZ=sX-40;
	else
	   sZ=sX-107;

	Radix->hWndLBG = CreateWindow("LISTBOX", "Häuser/Planetendaten",
			WS_CHILD | WS_BORDER |	WS_VSCROLL | LBS_NOTIFY |
			LBS_DISABLENOSCROLL | LBS_OWNERDRAWFIXED,
			sY-LB, 8, LB, sZ, hWnd,
			(HMENU)LB_POSITION,	hInstance, NULL );
	if (!Radix->hWndLBG)
      return (FALSE);
	Radix->hWndLBA = CreateWindow("LISTBOX", "Aspekte",
			WS_CHILD | WS_BORDER |	WS_VSCROLL |
			LBS_NOTIFY | LBS_DISABLENOSCROLL | LBS_OWNERDRAWFIXED,
			sY-LB,	8, LB, sZ, hWnd,
			(HMENU)LB_ASPEKTE, hInstance, NULL );
	if (!Radix->hWndLBA)
      return (FALSE);
	Radix->hWndLBD = CreateWindow("LISTBOX", "Daten",
			WS_CHILD | WS_VISIBLE | WS_BORDER |	WS_VSCROLL |
			LBS_NOTIFY | LBS_DISABLENOSCROLL | LBS_OWNERDRAWFIXED,
			sY-LB, 8, LB, sZ, hWnd,
			(HMENU)LB_DATEN, hInstance, NULL );
	if (!Radix->hWndLBD)
      return (FALSE);
	Radix->hWndLBLast=Radix->hWndLBD;

	if (!CreateWindow("BUTTON", "Daten", WS_CHILD | WS_GROUP | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
			sY-264, sX-30, 85,	20,	hWnd, (HMENU)PB_DATEN,
			hInstance, NULL ))
	  return (FALSE);

	if (!CreateWindow("BUTTON", "Positionen", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
            sY-174, sX-30, 85, 20, hWnd, (HMENU)PB_POSITION,
			hInstance, NULL ))
      return (FALSE);

	if (!CreateWindow("BUTTON", "Aspekte", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
			sY-84, sX-30, 85, 20, hWnd, (HMENU)PB_ASPEKTE,
			hInstance, NULL ))
	  return (FALSE);


	if (auinit.sSelHoro==TYP_RADIX||auinit.sSelHoro==TYP_TRANSIT){
       Radix->lpRadixDlg=(DLGPROC)MakeProcInstance((FARPROC)DlgRadix, hInstance);
	   Radix->hWndRadixDlg = CreateDialog(hInstance, "RADIXDLG", hWnd, Radix->lpRadixDlg);
	   SetWindowPos(Radix->hWndRadixDlg, NULL,
				sY-242, sZ+12, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	}/*endif*/


  return (hWnd);
}

/**************************************************************/
HWND PaintTransitWnd(HWND hWndMain, RADIX * Radix )
{
HWND	hWnd;
RECT    rect;
short   sMenu;
#define C_FACT 2

	sMenu=GetSystemMetrics(SM_CYMENU)+GetSystemMetrics(SM_CYCAPTION);
 	GetWindowRect(hWndMain, &rect);
	hWnd = CreateWindow(
			"TRANSITCLASS",
			"AstroUniverse",
			WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_POPUP,
			4,
			sMenu+4,
			rect.right-180,
			rect.bottom-sMenu-90,
			hWndMain,
			NULL,
			hInstance,
			NULL );

/// Mittelpunkt, Radius und Aspectratio vom Radix definieren
//	sSub=GetSystemMetrics(SM_CXVSCROLL);
    GetWindowRect(hWnd, &rect);
 //   sX=rect.bottom-rect.top-sMenu;

	Radix->hWndLBG = CreateWindow("LISTBOX", "Transitdaten",
			WS_CHILD | WS_VISIBLE | WS_BORDER |	WS_VSCROLL |
			LBS_NOTIFY | LBS_DISABLENOSCROLL | LBS_OWNERDRAWFIXED,
			8*C_FACT, 20*C_FACT, 220*C_FACT, 130*C_FACT, hWnd, (HMENU)LB_UT_TRANS,
			hInstance, NULL );
	if (!Radix->hWndLBG)
	  return (FALSE);

	if (!CreateWindow("BUTTON", "Grafik", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			144*C_FACT, 5*C_FACT, 36*C_FACT, 10*C_FACT, hWnd, (HMENU)PB_UT_GRAF, hInstance, NULL ))
	  return (FALSE);
	if (!CreateWindow("BUTTON", "Rückl.", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			185*C_FACT, 5*C_FACT, 36*C_FACT, 10*C_FACT, hWnd, (HMENU)PB_UT_RUCK, hInstance, NULL ))
      return (FALSE);
	if (!CreateWindow("STATIC", "Transite von", WS_CHILD | WS_VISIBLE | SS_LEFT,
			8*C_FACT, 6*C_FACT, 42*C_FACT, 10*C_FACT, hWnd, (HMENU)-1, hInstance, NULL ))
      return (FALSE);
	if (!CreateWindow("STATIC", "31.12.1997", WS_CHILD | WS_VISIBLE | SS_LEFT,
			51*C_FACT, 6*C_FACT, 36*C_FACT, 10*C_FACT, hWnd, (HMENU)TXT_UT_DATUM, hInstance,
			NULL ))
      return (FALSE);
	if (!CreateWindow("STATIC", "bis", WS_CHILD | WS_VISIBLE | SS_LEFT,
			91*C_FACT, 6*C_FACT, 10*C_FACT, 10*C_FACT, hWnd, (HMENU)-1, hInstance,
			NULL ))
      return (FALSE);
	if (!CreateWindow("STATIC", "31.12.1997", WS_CHILD | WS_VISIBLE | SS_LEFT,
			104*C_FACT, 6*C_FACT, 36*C_FACT, 10*C_FACT, hWnd, (HMENU)TXT_UT_BDATUM, hInstance,
			NULL ))
      return (FALSE);
/*	if (!CreateWindow("STATIC", "", WS_CHILD | WS_VISIBLE | SS_BLACKRECT,
			111*C_FACT, 22*C_FACT, 36*C_FACT, 1, hWnd, (HMENU)-1, hInstance,
			NULL ))
      return (FALSE);
	if (!CreateWindow("STATIC", "Beginn:", WS_CHILD | WS_VISIBLE | SS_LEFT,
			111*C_FACT, 25*C_FACT, 31*C_FACT, 10*C_FACT, hWnd, (HMENU)-1, hInstance,
			NULL ))
	  return (FALSE);
	if (!CreateWindow("STATIC", "31.12.1997", WS_CHILD | WS_VISIBLE | SS_LEFT,
			111*C_FACT, 40*C_FACT, 36*C_FACT, 10*C_FACT, hWnd, (HMENU)TXT_UT_TDATUM, hInstance,
			NULL ))
	  return (FALSE);
	if (!CreateWindow("STATIC", "", WS_CHILD | WS_VISIBLE | SS_BLACKRECT,
			111*C_FACT, 55*C_FACT, 36*C_FACT, 1, hWnd, (HMENU)-1, hInstance,
			NULL ))
      return (FALSE);
	if (!CreateWindow("STATIC", "Ende:", WS_CHILD | WS_VISIBLE | SS_LEFT,
			111*C_FACT, 57*C_FACT, 25*C_FACT, 10*C_FACT, hWnd, (HMENU)-1, hInstance,
			NULL ))
	  return (FALSE);
	if (!CreateWindow("STATIC", "31.12.1997", WS_CHILD | WS_VISIBLE | SS_LEFT,
			111*C_FACT, 72*C_FACT, 36*C_FACT, 10*C_FACT, hWnd, (HMENU)TXT_UT_TBDATUM, hInstance,
			NULL ))
      return (FALSE);
	if (!CreateWindow("STATIC", "", WS_CHILD | WS_VISIBLE | SS_BLACKRECT,
			111*C_FACT, 87*C_FACT, 36*C_FACT, 1, hWnd, (HMENU)-1, hInstance,
			NULL ))
	  return (FALSE);
*/

  return (hWnd);
}


/****************************************************************************/
/*	FUNCTION: MainWndProc(HWND, unsigned, WORD, LONG)                   */
/****************************************************************************/

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

switch (message)
{
 HWND hWndCli=0;
 RADIX * Radix=0;
 short   sRet=0;
 DLGPROC lpDlgProc=0;

	case WM_MOVE:
	if (lParam && !IsIconic(hWnd)) {
	  SetWindowPos(hWnd, NULL, 0, 0, 0,0,
					SWP_NOSIZE|SWP_NOZORDER);
	  lParam=MAKELPARAM(0,0);
	} /* endif */
	break;

	case WM_COMMAND:
		switch(wParam)
		{
/*			case CM_U_NEW:
			case CM_U_SAVE:
			case CM_U_SAVEU:
			case CM_U_PRINT:
			case CM_U_LAYOUT:
			case CM_U_FILEOPEN:
				CMUFileOpen(hWnd);
				break;
			case CM_U_PINST:
				sDlgPrint(NULL, PRN_SETUP);
				break;
*/

			case CM_U_EXIT:
				DestroyWindow(hWnd);
				break;

			case CM_U_PERSON:
				lpDlgProc=(DLGPROC)MakeProcInstance((FARPROC)DlgPErfassen, hInstance);
				DialogBoxParam(hInstance, "E_PERSON", hWnd, lpDlgProc, (DWORD)lParam);
				FreeProcInstance ( (FARPROC) lpDlgProc );
				break;

			case CM_U_ORT:
				 lpDlgProc=(DLGPROC)MakeProcInstance((FARPROC)DlgOErfassen, hInstance);
				 DialogBox(hInstance, "E_ORT", hWnd, lpDlgProc);
				 FreeProcInstance ( (FARPROC) lpDlgProc );
				break;

			case CM_U_HAUSER:
				lpDlgProc=(DLGPROC)MakeProcInstance((FARPROC)DlgHausAuswahl, hInstance);
				DialogBox(hInstance, "HAUS_AUSWAHL", hWnd, lpDlgProc);
				FreeProcInstance ( (FARPROC) lpDlgProc );
                break;
				
			case CM_U_ORBEN:
                sOrbenTyp=0;
				lpDlgProc=(DLGPROC)MakeProcInstance((FARPROC)DlgOrbenEin, hInstance);
				DialogBox(hInstance, "ORBEN_EIN", hWnd, lpDlgProc);
				FreeProcInstance ( (FARPROC) lpDlgProc );
				break;

			case CM_U_FARBEN:
				lpDlgProc=(DLGPROC)MakeProcInstance((FARPROC)DlgFarben, hInstance);
				DialogBox(hInstance, "FARBEN", hWnd, lpDlgProc);
				FreeProcInstance ( (FARPROC) lpDlgProc );
				break;

			case CM_U_EINST:
				lpDlgProc=(DLGPROC)MakeProcInstance((FARPROC)DlgAspekte, hInstance);
				DialogBox(hInstance, "EINSTELL", hWnd, lpDlgProc);
				FreeProcInstance ( (FARPROC) lpDlgProc );
				break;

			case CM_U_HOROTYP:
				lpDlgProc=(DLGPROC)MakeProcInstance((FARPROC)DlgHoroAuswahl, hInstance);
				sRet=DialogBox(hInstance, "HORO_TYP", hWnd, lpDlgProc);
				FreeProcInstance ( (FARPROC) lpDlgProc );
                if ( sRet<=0 )
				   break;
                Radix=InitRadix(hWnd);
				if ( Radix==(RADIX*)ERR_NOITEM)
				   break;
				switch (auinit.sRet){
				case ERR_TRANSIT:
				case ERR_SYNASTRIE:
				case ERR_RADIX:
			        while (Radix->lRadixCount--)
						ExitRadix((RADIX*)Radix->plRadix[Radix->lRadixCount]);
					ExitRadix(Radix);
				break;

				default:
					if (auinit.sRet==ERR_STRANSIT)
						hWndCli=PaintTransitWnd(hWnd, Radix);
					else
						hWndCli=PaintRadixWnd(hWnd, Radix);
    	            if (!hWndCli)
					   break;
					Radix->hWnd=hWndCli;
					SetWindowLong(hWndCli, 0, (DWORD)Radix);
					ShowWindow(hWndCli, SW_SHOWNORMAL);
					UpdateWindow(hWndCli);
                    SendMessage(hWndCli, WM_INITRADIX, 0, (LPARAM)Radix);
				} /* endswitch*/
			break;

			case CM_U_HELPINDEX:
			case CM_U_HELPKEY:
			case CM_U_HELPCOM:
			case CM_U_HELPPROC:
			case CM_U_HELP:
			case CM_U_HELPABOUT:
				MessageBox(hWnd, szAbout, AboutText, MB_OK);
			break;
			default:
				break;
		} /* endswitch */
		break;

	case WM_QUIT:
	case WM_DESTROY:
		EndApplication();
		PostQuitMessage(0);
		break;

	default:
		return (DefWindowProc(hWnd, message, wParam, lParam));

} /* endswitch */
return (NULL);
}

/****************************************************************************/
/*	FUNCTION: RadixWndProc(HWND, unsigned, WORD, LONG)                      */
/****************************************************************************/

LRESULT CALLBACK RadixWndProc(HWND hWnd, UINT message,
									 WPARAM wParam, LPARAM lParam)
{
RADIX * Radix=0;
RADIX * plRadix=0;
DRAWITEMSTRUCT* Di=0;
//DLGPROC lpDlgProc=0;
char szBuffer[BUF_SIZE];
RECT    rect;

switch (message)
{                    
	case WM_INITRADIX:
		 Radix=(RADIX*)lParam;
		 lpLbProc=(WNDPROC)SetWindowLong(Radix->hWndLBA,
				 		GWL_WNDPROC, (LPARAM)SubLB);
		 lpLbProc=(WNDPROC)SetWindowLong(Radix->hWndLBG,
				 		GWL_WNDPROC, (LPARAM)SubLB);
		 lpLbProc=(WNDPROC)SetWindowLong(Radix->hWndLBD,
				 		GWL_WNDPROC, (LPARAM)SubLB);
         if (Radix->sLB&LB_A) 
 		    if (Radix->sLB&LB_S)
			   SetWindowText(GetDlgItem(hWnd, PB_ASPEKTE), ASPEKTE_TXT);
            else
				   SetWindowText(GetDlgItem(hWnd, PB_ASPEKTE), TASPEKTE_TXT);
         else 
   	        SetWindowText(GetDlgItem(hWnd, PB_ASPEKTE), ASPEKTE_TXT);
		 if (Radix->sLB&LB_G)
			if (Radix->sLB&LB_S)
			   SetWindowText(GetDlgItem(hWnd, PB_POSITION), S2POS_TXT);
			else
			   SetWindowText(GetDlgItem(hWnd, PB_POSITION), TPOS_TXT);
		else
       	    SetWindowText(GetDlgItem(hWnd, PB_POSITION), POS_TXT);
		SendMessage(Radix->hWndRadixDlg, WM_INITRADIX, 0, (LPARAM)Radix);
	break;

/*    case WM_ACTIVATE:
         if (wParam)
            SendMessage(hWnd, WM_SETREDRAW, 1, 0);
         else
            SendMessage(hWnd, WM_SETREDRAW, 0, 0);
    break;*/

	case WM_RBUTTONDOWN:
//	case WM_LBUTTONDOWN:
		 Radix=(RADIX *)GetWindowLong(hWnd, 0);
         Radix->cMouse^=MAUS_ZOOM;
         Radix->lParam=lParam;
         Radix->wParam=wParam;
		 GetWindowRect(hWnd, &rect);
		 rect.left=rect.top=0;
		 rect.right=sMpX+sRadius+sDruber;
		 rect.bottom=sMpY+sRadius+sDruber;
		 sSetXY((short*)&rect.right, (short*)&rect.bottom);
		 InvalidateRect(hWnd, &rect, 1);
//		 InvalidateRect(hWnd, NULL, 1);
	break;
	case WM_PAINT:
		 Radix=(RADIX *)GetWindowLong(hWnd, 0);
		 if (Radix->lPaintFlag&DRW_RADIX)
			 sDrawRad(Radix);
		 break;

    case WM_DRAWITEM:
         Di=(DRAWITEMSTRUCT*)lParam;
     	 Radix=(RADIX *)GetWindowLong(hWnd, 0);
         if (Di->hwndItem==Radix->hWndLBG) {
   	        if (Radix->sLB&LB_G) {
				plRadix=(RADIX*)Radix->plRadix[0];
                plRadix->hWndLBG=Radix->hWndLBG;
		        sDrawText(plRadix, Radix, Di, szBuffer, DRW_LB_BUFFER);
            }else{
		        sDrawText(Radix, Radix, Di, szBuffer, DRW_LB_BUFFER);
	        }/*endif*/
	        sDrawLBState(Di, szBuffer, DRW_LB_TXT_HAUS);
            break;
         }/*endif*/
         if (Di->hwndItem==Radix->hWndLBD){
            sDrawText(Radix, Radix, Di, szBuffer, DRW_LBD_BUFFER);
 	        sDrawLBState(Di, szBuffer, DRW_LB_TXT_DATEN);
            break;
         }/*endif*/
         if (Di->hwndItem==Radix->hWndLBA){
   	        if (Radix->sLB&LB_A) {
				plRadix=(RADIX*)Radix->plRadix[0];
                plRadix->hWndLBA=Radix->hWndLBA;
		        sDrawText(plRadix, Radix, Di, szBuffer, DRW_LBA_BUFFER);
            } else {
	 	        sDrawText(Radix, Radix, Di, szBuffer, DRW_LBA_BUFFER);
	        }/*endif*/
 	        sDrawLBState(Di, szBuffer, DRW_LB_TXT_ASPEKT);
            break;
         }/*endif*/
    break;

    case WM_COMMAND:
		Radix=(RADIX *)GetWindowLong(hWnd, 0);
		switch(wParam)
		{
            case PB_DATEN:
  	            if (Radix->hWndLBLast==Radix->hWndLBD)
                   break;
	   		  	ShowWindow(Radix->hWndLBLast, SW_HIDE);
    	    	ShowWindow(Radix->hWndLBD, SW_SHOW);
           	    Radix->hWndLBLast=Radix->hWndLBD;
            break;
            case PB_ASPEKTE:
				if (Radix->hWndLBLast==Radix->hWndLBA&&(Radix->sLB&LB_S))
                   break;
  	            if (Radix->hWndLBLast==Radix->hWndLBA&&Radix->lRadixCount)
         	        Radix->sLB^=LB_A;
				SendMessage(Radix->hWndLBA,	LB_RESETCONTENT, 0 , 0L);
				if (Radix->sLB&LB_A) {
					plRadix=(RADIX*)Radix->plRadix[0];
					plRadix->hWndLBA=Radix->hWndLBA;
					plRadix->sLB=Radix->sLB;
					sDrawText(plRadix, Radix, 0, 0, DRW_LBA_INDEX);
					if (Radix->sLB&LB_S)
					   SetWindowText(GetDlgItem(hWnd, PB_ASPEKTE), ASPEKTE_TXT);
					else
					   SetWindowText(GetDlgItem(hWnd, PB_ASPEKTE), TASPEKTE_TXT);
	            } else {
					sDrawText(Radix, Radix, 0, 0, DRW_LBA_INDEX);
        	        SetWindowText(GetDlgItem(hWnd, PB_ASPEKTE), ASPEKTE_TXT);
				} /*endelse*/
  	            if (Radix->hWndLBLast!=Radix->hWndLBA) {
	 	   		   ShowWindow(Radix->hWndLBLast, SW_HIDE);
		   	       ShowWindow(Radix->hWndLBA, SW_SHOW);
	          	   Radix->hWndLBLast=Radix->hWndLBA;
                } /* endelse*/
				if (!(Radix->cMouse&MAUS_ZOOM)){
					Radix->cMouse^=MAUS_ASPEKT;
    	            vInvalidateAsp(hWnd);
                } /* endif */
            break;
            case PB_POSITION:
  	            if (Radix->hWndLBLast==Radix->hWndLBG&&!Radix->lRadixCount)
                   break;
  	            if (Radix->hWndLBLast==Radix->hWndLBG&&Radix->lRadixCount)
         	        Radix->sLB^=LB_G;
				SendMessage(Radix->hWndLBG,	LB_RESETCONTENT, 0 , 0L);
    	        if (Radix->sLB&LB_G) {
					plRadix=(RADIX*)Radix->plRadix[0];
    	            plRadix->hWndLBG=Radix->hWndLBG;
					plRadix->sLB=Radix->sLB;
					sDrawText(plRadix, Radix, 0, 0, DRW_LB_INDEX);
					if (Radix->sLB&LB_S)
					   SetWindowText(GetDlgItem(hWnd, PB_POSITION), S2POS_TXT);
                    else
					   SetWindowText(GetDlgItem(hWnd, PB_POSITION), TPOS_TXT);
	            } else {
					sDrawText(Radix, Radix, 0, 0, DRW_LB_INDEX);
					if (Radix->sLB&LB_S)
					   SetWindowText(GetDlgItem(hWnd, PB_POSITION), S1POS_TXT);
                    else
  					   SetWindowText(GetDlgItem(hWnd, PB_POSITION), POS_TXT);
        	    } /*endelse*/
  	            if (Radix->hWndLBLast!=Radix->hWndLBG) {
		   		  	ShowWindow(Radix->hWndLBLast, SW_HIDE);
	                ShowWindow(Radix->hWndLBG, SW_SHOW);
	           	    Radix->hWndLBLast=Radix->hWndLBG;
        	    } /*endelse*/
            break;
			case CM_UR_EXIT:
				DestroyWindow(hWnd);
				break;

			case CM_UR_SAVE:
				PostMessage(hWndMain, WM_COMMAND, CM_U_PERSON, (LPARAM)Radix);
//				if (sWriteDat(Radix, hWnd)<=0)
//				   MessageBox(hWnd, "Fehler beim Sichern! :(", 0, MB_OK);
               break;

			case CM_UR_PRINT:
				Radix->lPaintFlag^=DRW_RADIX;
				sDlgPrint(Radix, PRN_NORMAL);
				Radix->lPaintFlag|=DRW_RADIX;
				sSetScrCol();
				break;

            case CM_UR_HOROTYP:
	            auinit.Radix.cMemory=CALC_ON_MEM;
                if (RadixCopy(&auinit.Radix, Radix, ALLOC_MEM)<=0)
                  break;
 	            PostMessage(hWndMain, WM_COMMAND, CM_U_HOROTYP, 0);
                break;

			case CM_UR_HELP:
				MessageBox(hWnd, "kommt schon noch! :)", "NoNixDa", MB_OK);
				break;

    	    default:
                break;
		} /* endswitch */
		break;

    case WM_QUIT:
	case WM_DESTROY:
		Radix=(RADIX *)GetWindowLong(hWnd, 0);
		if (auinit.sSelHoro==TYP_RADIX||auinit.sSelHoro==TYP_TRANSIT){
			DestroyWindow(Radix->hWndRadixDlg);
			FreeProcInstance((FARPROC)Radix->lpRadixDlg);
		}/*endif*/
		while (Radix->lRadixCount--)
			ExitRadix((RADIX*)Radix->plRadix[Radix->lRadixCount]);
		ExitRadix(Radix);
		break;

	default:
		return (DefWindowProc(hWnd, message, wParam, lParam));

} /* endswitch */			
return (NULL);
}

/****************************************************************************/
/*	FUNCTION: TransitWndProc(HWND, unsigned, WORD, LONG)                      */
/****************************************************************************/

LRESULT CALLBACK TransitWndProc(HWND hWnd, UINT message,
									 WPARAM wParam, LPARAM lParam)
{
RADIX * Radix=0;
RADIX * plRadix=0;
DRAWITEMSTRUCT* Di=0;
HWND hWndCli=0;

switch (message)
{

	case WM_INITRADIX:
		 Radix=(RADIX*)lParam;
		 lpLbProc=(WNDPROC)SetWindowLong(Radix->hWndLBG,
				 		GWL_WNDPROC, (LPARAM)SubLB);
		 sDrawTRad(Radix);
	break;
// case WM_PAINT:
// Radix=(RADIX *)GetWindowLong(hWnd, 0);
// sDrawTRad(Radix);
// break;

	case WM_DRAWITEM:
		 Di=(DRAWITEMSTRUCT*)lParam;
     	 Radix=(RADIX *)GetWindowLong(hWnd, 0);
		 vSetColor(Di);
		 sDrawLBState(Di, NULL, DRW_LB_TXT_TRANSIT);
	break;

    case WM_COMMAND:
		Radix=(RADIX *)GetWindowLong(hWnd, 0);
		switch(wParam)
		{
			case PB_UT_GRAF:
				if ((plRadix=RadixCopy(plRadix, Radix, MTRANS))<=0)
				   break;
				hWndCli=PaintRadixWnd(hWndMain, plRadix);
    	        if (!hWndCli)
				   break;
				plRadix->hWnd=hWndCli;
				SetWindowLong(hWndCli, 0, (DWORD)plRadix);
				ShowWindow(hWndCli, SW_SHOWNORMAL);
				UpdateWindow(hWndCli);
				SendMessage(hWndCli, WM_INITRADIX, 0, (LPARAM)plRadix);
			break;
			case PB_UT_RUCK:
                sRucklauf(hWnd);
			break;
			case CM_UT_EXIT:
				DestroyWindow(hWnd);
				break;

			case CM_UT_SAVE:
				PostMessage(hWndMain, WM_COMMAND, CM_U_PERSON, (LPARAM)Radix);
			   break;

			case CM_UT_PRINT:
				sDlgPrint(Radix, PRN_MULTITRANS);
				sSetScrCol();
				break;

            case CM_UT_HOROTYP:
	            auinit.Radix.cMemory=CALC_ON_MEM;
				if (RadixCopy(&auinit.Radix, Radix, ALLOC_MEM)<=0)
                  break;
				SendMessage(hWndMain, WM_COMMAND, CM_U_HOROTYP, 0);
				break;

			case CM_UT_HELP:
				MessageBox(hWnd, "kommt schon noch! :)", "NoNixDa", MB_OK);
				break;

    	    default:
                break;
       	} /* endswitch */
        break;

    case WM_QUIT:
	case WM_DESTROY:
		Radix=(RADIX *)GetWindowLong(hWnd, 0);
        while (Radix->lRadixCount--)
			ExitRadix((RADIX*)Radix->plRadix[Radix->lRadixCount]);
		ExitRadix(Radix);

		break;

    default:
		return (DefWindowProc(hWnd, message, wParam, lParam));

} /* endswitch */			
return (NULL);

}

/****************************************************************************/
#pragma argsused
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance,
						 LPSTR lpCmdLine, int nCmdShow)
{
 MSG msg;
 			
 hInstance=hInst;

 if (!hPrevInstance)
   if (!InitApplication())	
	 return (FALSE);

 if (!InitInstance())
   return (FALSE);


 while (GetMessage(&msg, NULL, NULL, NULL))	
 {
   TranslateMessage(&msg);	
   DispatchMessage(&msg);	
 }
 return 0;	
}
