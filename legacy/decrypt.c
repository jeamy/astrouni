
/*----------------------------------------------------------------------------*/
/* formal Parameters: char * szText Buffer to decrypt                         */
/*                    short  sLen   length of text                            */
/*----------------------------------------------------------------------------*/

short SJDecrypt ( char * szText, short sLen )
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
