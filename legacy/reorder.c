#include <windows.h>
#include "c:\progy\astrouni\astrouni.h"
#include "c:\progy\astrouni\auwurzel.h"
#include <dos.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <conio.h>
#include <malloc.h>
#include <io.h>
/*
typedef struct {
         long    lDefault;
   		 long    lPlanetI;
         char    cPlanetS;
         char    cPlanetE;
 		 long    lAsterI;
         char    cAsterS;
         char    cAsterE;
 		 long    lChironI;
         char    cChironS;
         char    cChironE;

}EPHEM;
#define  EP_SIZE sizeof(EPHEM)
*/
#define  START 23
#define  END   26
#define  EPHE  "astroeph.dat"


FILE *   fpR=0;
FILE *   fpW=0;
void    longreorder(unsigned char*, int);


int main(void)
{
 EPHEM e;
 short sA, sSize;
 long  icoord[5][3], chicoord[3], ascoord[4][3];
 char  szFName[32]; 

 e.lDefault=0;
 e.lPlanetI=0;
 e.cPlanetS=START;
 e.cPlanetE=END-1;
 e.lAsterI=0;
 e.cAsterS=START;
 e.cAsterE=END-1;
 e.lChironI=0;
 e.cChironS=START;
 e.cChironE=END-1;

 fpW=fopen(EPHE, "wb");
 if (!fpW) 
	return 0;

 sSize=fwrite(&e, EP_SIZE, 1, fpW);
 if (sSize!=1)
    return 0;

 e.lPlanetI=ftell(fpW);
 for (sA=START; sA<END; sA++) {
	 sprintf(szFName, "lrz5_%d", sA);
	 fpR=fopen( szFName, "rb" );
	 if (!fpR) 
		return 0;
     do {
	  	 sSize=fread(&icoord, sizeof(long), 3*5, fpR); //3koordinaten, 5planeten
         if (ferror(fpR))
            return 0;
         if (feof(fpR))
	     	break;
	 	 longreorder((unsigned char *)&icoord, 15*4);
		 sSize=fwrite(&icoord, sizeof(long), 15, fpW);
		 if (sSize!=15)
		    return 0;
     }while(1);
 } /* endif */
 fclose(fpR);

 e.lAsterI=ftell(fpW);
 for (sA=START; sA<END; sA++) {
	 sprintf(szFName, "cpjv_%d", sA);
	 fpR=fopen( szFName, "rb" );
	 if (!fpR)
		return 0;
     do{
	  	 sSize=fread(&ascoord, sizeof(long), 3*4, fpR);//3koordinaten, 4asteroiden
         if (ferror(fpR))
            return 0;
         if (feof(fpR))
	     	break;
	 	 longreorder((unsigned char *)&ascoord, 12*4);
		 sSize=fwrite(&ascoord, sizeof(long), 12, fpW);
		 if (sSize!=12)
		    return 0;
     }while(1);
 } /* endif */
 fclose(fpR);

 e.lChironI=ftell(fpW);
 for (sA=START; sA<END; sA++) {
	 sprintf(szFName, "chi_%d", sA);
	 fpR=fopen( szFName, "rb" );
	 if (!fpR)
		return 0;
     do{
	  	 sSize=fread(&chicoord, sizeof(long), 3, fpR);//3koordinaten
         if (ferror(fpR))
            return 0;
         if (feof(fpR))
	     	break;
	 	 longreorder((unsigned char *)&chicoord, 3*4);
		 sSize=fwrite(&chicoord, sizeof(long), 3, fpW);
		 if (sSize!=3)
		    return 0;
     }while(1);
 } /* endif */
 e.lDefault=ftell(fpW);
 sSize=fwrite(sSColor, sizeof(COLORREF), COLORS, fpW);
 if (sSize!=COLORS)
    return 0;
 sSize=fwrite(sPColor, sizeof(COLORREF), COLORS, fpW);
 if (sSize!=COLORS)
    return 0;
 sSize=fwrite(sMono, sizeof(COLORREF), COLORS, fpW);
 if (sSize!=COLORS)
    return 0;

 rewind(fpW);
 sSize=fwrite(&e, EP_SIZE, 1, fpW);
 if (sSize!=1)
    return 0;

 fclose(fpR);
 fclose(fpW);
 return 1;
}

void longreorder(unsigned char *p, int n)
{
  int i;
  unsigned char c0, c1, c2, c3;

  for (i = 0; i < n; i += 4, p += 4) {
    c0 = *p;
    c1 = *(p + 1);
    c2 = *(p + 2);
    c3 = *(p + 3);
    *p = c3;
    *(p + 1) = c2;
    *(p + 2) = c1;
    *(p + 3) = c0;
  }
}

