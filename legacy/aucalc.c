#include "astrouni.h"
#include "aucalc.h"


double dJulTag(short sTag, short sMonat, short sJahr, double dStunde, short sGreg)
{
  double dJd=0, dU, dU0, dU1, dU2;

  dU = (double)sJahr;
  if (sMonat < 3)
	dU -=1;
  dU0 = dU + 4712.0;
  dU1 = (double)sMonat + 1.0;
  if (dU1 < 4)
	dU1 += 12.0;
  dJd = floor(dU0*365.25) + floor(30.6*dU1+0.000001)+
		(double)sTag + dStunde/24.0 - 63.5;
  if (sGreg) {
	dU2 = floor(fabs(dU) / 100) - floor(fabs(dU) / 400);
	if (dU < 0.0)
	  dU2 = -dU2;
	dJd = dJd - dU2 + 2;
	if ((dU < 0.0) && (dU/100 == floor(dU/100)) &&
		(dU/400 != floor(dU/400)))
	  dJd -= 1;
  }
  return dJd;
}

void vGrad2GMS(double dDezGrad, short* sGrad, short* sMinuten, short* sSekunden)
{
 *sGrad=(short)floor(dDezGrad);
 dDezGrad=(dDezGrad-(double)*sGrad)*100;
 while(*sGrad>=30)
	*sGrad-=30;
 *sMinuten=(short)floor(dDezGrad);
 dDezGrad=(dDezGrad-(double)*sMinuten)*100;
 while(*sMinuten>=60) {
	(*sGrad)++;
    *sMinuten-=60;
 }/* endwhile*/
 *sSekunden=(short)floor(dDezGrad+RUNDEN);
 while(*sSekunden>=60) {
	(*sMinuten)++;
    if (*sMinuten>=60) {
		(*sGrad)++;
   	    *sMinuten-=60;
    }/* endif*/
	*sSekunden-=60;
  }/* endwhile*/
}

/* A similar modulus function: convert an integer to value from 1..12. */

/* Modulus function for floating point values, where we bring the given */
/* parameter to within the range of 0 to 360.                           */

double dMod(double dD)
{
  if (dD >= DEGMAX)        /* In most cases, our value is only slightly */
	dD -= DEGMAX;          /* out of range, so we can test for it and   */
  else
	if (dD < 0.0)        /* avoid the more complicated arithmetic.    */
	   dD += DEGMAX;
  if (dD >= 0 && dD < DEGMAX)
	 return dD;
  return (dD - floor(dD/DEGMAX)*DEGMAX);
}

double dModRad(double dD)
{
  while (dD >= PI2)    /* We assume our value is only slightly out of       */
	dD -= PI2;         /* range, so test and never do any complicated math. */
  while (dD < 0.0)
	dD += PI2;
  return dD;
}

double dWinkel( double dX, double dY)
{
  double dA;

  if (dX != 0.0) {
	if (dY != 0.0)
	  dA = atan(dY/dX);
	else
	  dA = dX < 0.0 ? PI : 0.0;
  } else
	dA = dY < 0.0 ? -PIHALB : PIHALB;
  if (dA < 0.0)
	dA += PI;
  if (dY < 0.0)
	dA += PI;
  return dA;
}

double dVorz( double dD)
{
  return dD == 0.0 ? 0.0 : (dD < 0.0 ? -1.0 : 1.0);
}

double dMinDist(double dD1, double dD2)
{
  double dI;

  dI = fabs(dD1-dD2);
  return dI < DEGHALB ? dI : DEGMAX - dI;
}


double dDez2Deg( double dD)
{
  return dVorz(dD)*(floor(fabs(dD))+(fabs(dD)-floor(fabs(dD)))*100.0/60.0);
}


/* This is the inverse of the above function. Given a true decimal value */
/* for a zodiac degree, adjust it so the degrees are in the integer part */
/* and the minute expressed as hundredths, e.g. 10.5 degrees -> 10.30    */

double dDeg2Dez(double dD)
{
  return dVorz(dD)*(floor(fabs(dD))+(fabs(dD)-floor(fabs(dD)))*60.0/100.0);
}

/*
******************************************************************************
** Assorted Calculations.
******************************************************************************
*/

/* Given a month, day, and year, convert it into a single Julian day value, */
/* i.e. the number of days passed since a fixed reference date.             */

long lTMJ2Jul(short sTag, short sMonat, short sJahr)
{
  short sGreg = TRUE;

  if (sJahr < 1582 || (sJahr == 1582 &&
	(sMonat < 10 || (sMonat == 10 && sTag < 15))))
	sGreg = FALSE;
  return (long)floor(dJulTag(sTag, sMonat, sJahr, 12.0, sGreg)+RUNDEN);
}


/* This is a subprocedure of CastChart(). Once we have the chart parameters, */
/* calculate a few important things related to the date, i.e. the Greenwich  */
/* time, the Julian day and fractional part of the day, the offset to the    */
/* sidereal, and a couple of other things.                                   */

/* Convert polar to rectangular coordinates. */

void vPolToRec(double dA, double dR, double * dX, double * dY)
{
  if (dA == 0.0)
	dA = KLEIN;
  *dX = dR*cos(dA);
  *dY = dR*sin(dA);
}


/* Convert rectangular to polar coordinates. */

void vRecToPol(double dX, double dY, double * dA, double * dR )
{
  if (dY == 0.0)
	dY = KLEIN;
  *dR = sqrt(dX*dX + dY*dY);
  *dA = dWinkel(dX, dY);
}


/* Convert rectangular to spherical coordinates. */

double dRecToSph(double dB, double dL, double dO)
{
  double dR, dQ, dG, dX, dY, dA;

  dA = dB; dR = 1.0;
  vPolToRec(dA, dR, &dX, &dY);
  dQ = dY;
  dR = dX;
  dA = dL;
  vPolToRec(dA, dR, &dX, &dY);
  dG = dX;
  dX = dY;
  dY = dQ;
  vRecToPol(dX, dY, &dA, &dR);
  dA += dO;
  vPolToRec(dA, dR, &dX, &dY);
  dQ = asin(dY);
  dY = dX; dX = dG;
  vRecToPol(dX, dY, &dA, &dR);
  if (dA < 0.0)
	dA += 2*PI;
  return dA;  /* We only ever care about and return one of the coordinates. */
}

/* This is another subprocedure of CastChart(). Calculate a few variables */
/* corresponding to the chart parameters that are used later on. The      */
/* astrological vertex (object number nineteen) is also calculated here.  */

void vCalcVar(RADIX* Radix )
{
  double dR, dR2, dB, dL, dO, dG, dX, dY, dA;
  double dOff, dLn, dT;

  Radix->dBreite = dDez2Deg(Radix->rFix.dBreite)/DEG2RAD;

  Radix->dJD = (double)lTMJ2Jul(Radix->rFix.sTag, Radix->rFix.sMonat,
									 Radix->rFix.sJahr);

  Radix->dT = (Radix->dJD + Radix->rFix.dTime/24.0 - 2415020.5) / 36525.0;
  dT=Radix->dT;
  /* Compute angle that the ecliptic is inclined to the Celestial Equator */
  Radix->OB = (23.452294-0.0130125*dT)/DEG2RAD;

  dLn = dMod((933060.0-6962911.0*dT+7.5*dT*dT)/3600.0);    /* Mean lunar node */
  dOff = (259205536.0*dT+2013816.0)/3600.0;         /* Mean Sun        */
  dOff = 17.23*sin(dLn/DEG2RAD)+1.27*sin(dOff/DEG2RAD)-(5025.64+1.11*dT)*dT;
  dOff = (dOff-84038.27)/3600.0;
// Radix->rFix.dSid = (us.fSidereal ? dOff : 0.0)    + us.rZodiacOffset;
  Radix->dSid = 0; //dOff;

/*
~  if (!us.fProgress || us.fSolarArc)
	  dT = (Radix->dJD + Radix->dTime/24.0 - 2415020.5) / 36525.0;
	else {
	  /* Determine actual time that a progressed chart(us.JDp)is to be cast for. */
/*
~	  dT = ((Radix->dJD + Radix->dTime/24.0 + (is.JDp - (Radix->dJD + Radix->dTime/24.0))
				  / us.rProgDay) -	2415020.5) / 36525.0;
	}*/

  Radix->RA = dMod((6.6460656+2400.0513*dT+2.58E-5*dT*dT+Radix->rFix.dTime)
  					*15.0-dDez2Deg(Radix->rFix.dLange))/DEG2RAD;
  dR2 = Radix->RA;
  dO = -Radix->OB;
  dB = Radix->dBreite;
  dA = dR2;
  dR = 1.0;
  vPolToRec(dA, dR, &dX, &dY);
  dX *= cos(dO);
  vRecToPol(dX, dY, &dA, &dR);
  Radix->MC = dMod(Radix->dSid + dA*DEG2RAD);  /* Midheaven */
  dL= dR2+PI;
  dB = PIHALB-fabs(dB);
  if (Radix->dBreite < 0.0)
    dB = -dB;
  dG = dRecToSph(dB, dL, dO);

  Radix->VER = dMod(Radix->dSid + DFromR(dG+PIHALB));    /* Vertex */
  
 }


/*
******************************************************************************
** House Cusp Calculations.
******************************************************************************
*/

double dSpitzeMid(RADIX* Radix)
{
  double dMC;

  dMC = atan(TAN(Radix->RA)/cos(Radix->OB));
  if (dMC < 0.0)
	dMC += PI;
  if (Radix->RA > PI)
	dMC += PI;
  return dMod(dMC*DEG2RAD+Radix->dSid);
}

double dSpitzeAsc(RADIX* Radix)
{
  double dAsc;

  dAsc = dWinkel(-sin(Radix->RA)*cos(Radix->OB)-
  		 TAN(Radix->dBreite)*sin(Radix->OB), cos(Radix->RA));
  return dMod(dAsc*DEG2RAD+Radix->dSid);
}

double dSpitzeEP(RADIX* Radix)
{
  double dEP;

  dEP = dWinkel(-sin(Radix->RA)*cos(Radix->OB), cos(Radix->RA));
  return dMod(dEP*DEG2RAD+Radix->dSid);
}


/* These are various different algorithms for calculating the house cusps: */

double dSpitzePlacidus(RADIX* Radix, double dDeg, double dFF, short sNeg)
{
  double dLO, dR1, dXS, dX;
  short i;

  dR1 = Radix->RA+dDeg/DEG2RAD;
  dX = sNeg ? 1.0 : -1.0;
  /* Looping 10 times is arbitrary, but it's what other programs do. */
  for (i = 1; i <= 10; i++) {

	/* This formula works except at 0 latitude (Radix->dBreite == 0.0). */

	dXS = dX*sin(dR1)*TAN(Radix->OB)*TAN(Radix->dBreite == 0.0 ? 0.0001 : Radix->dBreite);
	dXS = acos(dXS);
	if (dXS < 0.0)
	  dXS += PI;
	dR1 = Radix->RA + (sNeg ? PI-(dXS/dFF) : (dXS/dFF));
  }
  dLO = atan(TAN(dR1)/cos(Radix->OB));
  if (dLO < 0.0)
	dLO += PI;
  if (sin(dR1) < 0.0)
	dLO += PI;
  return dLO*DEG2RAD;
}

void vHausPlacidus(RADIX* Radix)
{
  short i;

  Radix->pdHaus[0] = dMod(Radix->ASC-Radix->dSid);
  Radix->pdHaus[3] = dMod(Radix->MC+DEGHALB-Radix->dSid);
  Radix->pdHaus[4] = dSpitzePlacidus(Radix, 30.0, 3.0, FALSE) + DEGHALB;
  Radix->pdHaus[5] = dSpitzePlacidus(Radix, 60.0, 1.5, FALSE) + DEGHALB;
  Radix->pdHaus[1] = dSpitzePlacidus(Radix, 120.0, 1.5, TRUE);
  Radix->pdHaus[2] = dSpitzePlacidus(Radix, 150.0, 3.0, TRUE);
  for (i = 0; i < 12; i++) {
	if (i < 6)
	  Radix->pdHaus[i] = dMod(Radix->pdHaus[i]+Radix->dSid);
	else
	  Radix->pdHaus[i] = dMod(Radix->pdHaus[i-6]+DEGHALB);
  }
}

void vKoch(RADIX* Radix)
{
  double dA1, dA2, dA3, dKN, dD, dX;
  short i;

  dA1 = sin(Radix->RA)*TAN(Radix->dBreite)*TAN(Radix->OB);
  dA1 = asin(dA1);
  for (i = 0; i < 12; i++) {
	dD = dMod(60.0+30.0*(double)(i+1));
	dA2 = dD/DEGVIER-1.0;
	dKN = 1.0;
	if (dD >=DEGHALB) {
	  dKN = -1.0;
	  dA2 = dD/DEGVIER-3.0;
	}
	dA3 = dMod(Radix->RA*DEG2RAD+dD+dA2*(dA1*DEG2RAD))/DEG2RAD;
	dX = dWinkel(cos(dA3)*cos(Radix->OB)-dKN*TAN(Radix->dBreite)*sin(Radix->OB), sin(dA3));
	Radix->pdHaus[i] = dMod(dX*DEG2RAD+Radix->dSid);
  }
}

void vCampanus(RADIX* Radix)
{
  double dKO, dDN, dX;
  short i;

  for (i = 0; i < 12; i++) {
	dKO = (60.000001+30.0*(double)(i+1))/DEG2RAD;
	dDN = atan(TAN(dKO)*cos(Radix->dBreite));
	if (dDN < 0.0)
	  dDN += PI;
	if (sin(dKO) < 0.0)
	  dDN += PI;
	dX = dWinkel(cos(Radix->RA+dDN)*cos(Radix->OB)-sin(dDN)*TAN(Radix->dBreite)*sin(Radix->OB), sin(Radix->RA+dDN));
	Radix->pdHaus[i] = dMod(dX*DEG2RAD+Radix->dSid);
  }
}

void vMeridian(RADIX* Radix)
{
  double dD, dX;
  short i;

  for (i = 0; i < 12; i++) {
	dD = (60.0+30.0*(double)(i+1))/DEG2RAD;
	dX = dWinkel(cos(Radix->RA+dD)*cos(Radix->OB), sin(Radix->RA+dD));
	Radix->pdHaus[i]= dMod(dX*DEG2RAD+Radix->dSid);
  }
}

void vRegiomontanus(RADIX* Radix)
{
  double dD, dX;
  short i;

  for (i = 0; i < 12; i++) {
	dD = (60.0+30.0*(double)(i+1))/DEG2RAD;
	dX = dWinkel(cos(Radix->RA+dD)*cos(Radix->OB)-sin(dD)*TAN(Radix->dBreite)*sin(Radix->OB), sin(Radix->RA+dD));
	Radix->pdHaus[i] = dMod(dX*DEG2RAD+Radix->dSid);
  }
}

void vPorphyry(RADIX* Radix)
{
  double dX, dY;
  short i;

  dX = Radix->ASC-Radix->MC;
  if (dX < 0.0)
	dX += DEGMAX;
  dY = dX/3.0;
  for (i = 0; i < 2; i++)
	Radix->pdHaus[i+4] = dMod(DEGHALB+Radix->MC+(i+1)*dY);
  dX = dMod(DEGHALB+Radix->MC)-Radix->ASC;
  if (dX < 0.0)
	dX += DEGMAX;
  Radix->pdHaus[0]=Radix->ASC;
  dY = dX/3.0;
  for (i = 0; i < 3; i++)
	Radix->pdHaus[i+1] = dMod(Radix->ASC+(i+1)*dY);
  for (i = 0; i < 6; i++)
	Radix->pdHaus[i+6] = dMod(Radix->pdHaus[i]+DEGHALB);
}

void vMorinus(RADIX* Radix)
{
  double dD, dX;
  short i;

  for (i = 0; i < 12; i++) {
	dD = (60.0+30.0*(double)(i+1))/DEG2RAD;
	dX = dWinkel(cos(Radix->RA+dD), sin(Radix->RA+dD)*cos(Radix->OB));
	Radix->pdHaus[i] = dMod(dX*DEG2RAD+Radix->dSid);
  }
}

double dSpitzeTopocentric(RADIX* Radix, double dDeg)
{
  double dOA, dX, dLO;

  dOA = dModRad(Radix->RA+dDeg/DEG2RAD);
  dX = atan(TAN(Radix->dBreite)/cos(dOA));
  dLO = atan(cos(dX)*TAN(dOA)/cos(dX+Radix->OB));
  if (dLO < 0.0)
	dLO += PI;
  if (sin(dOA) < 0.0)
	dLO += PI;
  return dLO;
}

void vTopocentric(RADIX* Radix)
{
  double dTL, dP1, dP2, dLT;
  short i;

  Radix->pdHaus[3] = dModRad((Radix->MC+DEGHALB-Radix->dSid)/DEG2RAD);
  dTL = TAN(Radix->dBreite);
  dP1 = atan(dTL/3.0);
  dP2 = atan(dTL/1.5);
  dLT = Radix->dBreite;
  Radix->dBreite = dP1;
  Radix->pdHaus[4] = dSpitzeTopocentric(Radix, 30.0) + PI;
  Radix->dBreite = dP2;
  Radix->pdHaus[5] = dSpitzeTopocentric(Radix, 60.0) + PI;
  Radix->dBreite = dLT;
  Radix->pdHaus[0] = dSpitzeTopocentric(Radix, 90.0);
  Radix->dBreite = dP2;
  Radix->pdHaus[1] = dSpitzeTopocentric(Radix, 120.0);
  Radix->dBreite = dP1;
  Radix->pdHaus[2] = dSpitzeTopocentric(Radix, 150.0);
  Radix->dBreite = dLT;
  for (i = 0; i < 6; i++) {
	Radix->pdHaus[i] = dMod(Radix->pdHaus[i]*DEG2RAD+Radix->dSid);
	Radix->pdHaus[i+6] = dMod(Radix->pdHaus[i]+DEGHALB);
  }
}

void vEqual(RADIX* Radix)
{
  int i;

  for (i = 0; i < 12; i++) 
	Radix->pdHaus[i] = dMod(Radix->ASC+30.0*(double)i);
}

/* This house system is just like the Equal system except that we start */
/* our 12 equal segments from the Midheaven instead of the Ascendant.   */

void vEqualMid(RADIX* Radix)
{
  int i;

  for (i = 0; i < 12; i++) 
	  Radix->pdHaus[i] = dMod(Radix->MC-270.0+30.0*(double)i);
}

/* The "Whole" house system is like the Equal system with 30 degree houses, */
/* where the 1st house starts at zero degrees of the sign of the Ascendant. */

void vWhole(RADIX* Radix)
{
  short i;
  for (i = 0; i < 12; i++)
	Radix->pdHaus[i] = dMod(Radix->ASC+30*(double)i);
}


/* Compute the cusp positions using the Alcabitius house system. */

void vAlcabitius(RADIX* Radix)
{
  double dDecl, dSda, dSna, dD, dRA;
  short i;

  dDecl = asin(asin(Radix->OB) * sin(Radix->ASC/DEG2RAD));
  dD = -TAN(Radix->dBreite) * TAN(dDecl);
  dSda = acos(dD)*DEG2RAD;
  dSna = DEGHALB - dSda;
  dRA=Radix->RA*DEG2RAD;
  Radix->pdHaus[6] = dRA - dSna;
  Radix->pdHaus[7] = dRA - dSna*2.0/3.0;
  Radix->pdHaus[8] = dRA - dSna/3.0;
  Radix->pdHaus[9] = dRA;
  Radix->pdHaus[10] = dRA + dSda/3.0;
  Radix->pdHaus[11] = dRA + dSda*2.0/3.0;
  for (i = 6; i < 12; i++)
	Radix->pdHaus[i] = dMod(Radix->pdHaus[i]+Radix->dSid);
  for (i = 0; i < 6; i++)
	Radix->pdHaus[i] = dMod(Radix->pdHaus[i+6]+DEGHALB);
}


/* This is a new house system similar in philosophy to Porphyry houses.   */
/* Instead of just trisecting the difference in each quadrant, we do a    */
/* smooth sinusoidal distribution of the difference around all the cusps. */

void vPorphyryNeo(RADIX* Radix)
{
  double dD;
  short i;

  dD = (dMinDist(Radix->MC, Radix->ASC) - DEGVIER)/4.0;
  Radix->pdHaus[6] = dMod(Radix->ASC+DEGHALB);
  Radix->pdHaus[9] = Radix->MC;
  Radix->pdHaus[10]= dMod(Radix->pdHaus[9] + 30.0 + dD   + Radix->dSid);
  Radix->pdHaus[11]= dMod(Radix->pdHaus[10]+ 30.0 + dD*2 + Radix->dSid);
  Radix->pdHaus[8] = dMod(Radix->pdHaus[9] - 30.0 + dD   + Radix->dSid);
  Radix->pdHaus[7] = dMod(Radix->pdHaus[8] - 30.0 + dD*2 + Radix->dSid);
  for (i = 0; i < 6; i++)
	Radix->pdHaus[i] = dMod(Radix->pdHaus[i+6]-DEGHALB);
}


/* In "null" houses, the cusps are always fixed to start at their cor-    */
/* responding sign, i.e. the 1st house is always at 0 degrees Aries, etc. */

void vNull(RADIX* Radix)
{
  short i;

  for (i = 0; i < 12; i++)
	Radix->pdHaus[i] = i*30;
}


/* Calculate the house cusp positions, using the specified algorithm. */

short sCalcHauser(RADIX * Radix)
{

  short i;

  if (fabs(Radix->dBreite) > (DEGVIER-AXE)/DEG2RAD &&
		   Radix->cHausSys<TYP_EQUAL) {
//	MessageBox(0, "Häuserberechnung nach %s\n\
//	             für diese Breitengrade nicht gegeignet.\n\
//            	 Verwende Equal.", Radix->pszHausSys, NULL, MB_OK);
	  return ERR_HOUSE;
  }
   Radix->MC  = dSpitzeMid(Radix);          /* Calculate our Ascendant & Midheaven. */
   Radix->ASC = dSpitzeAsc(Radix);

  switch (Radix->cHausSys) {
  case  TYP_EQUAL:
		vEqual(Radix);
		break;
  case  TYP_TOPOZEN:
		vTopocentric(Radix);
		break;
  case  TYP_CAMPANUS:
		vCampanus(Radix);
		break;
  case  TYP_MERIDIAN:
		vMeridian(Radix);
		break;
  case  TYP_REGIOMON:
		vRegiomontanus(Radix);
		break;
  case  TYP_PORPHYRY:
		vPorphyry(Radix);
		break;
  case  TYP_MORINUS:
		vMorinus(Radix);
		break;
  case  TYP_ALCABIT:
		vAlcabitius(Radix);
		break;
  case  TYP_EQUALMID:
		vEqualMid(Radix);
		break;
  case  TYP_PORPHYRYN:
		vPorphyryNeo(Radix);
		break;
  case  TYP_WHOLE:
		vWhole(Radix);
		break;
  case  TYP_NULL:
		vNull(Radix);
		break;
  case  TYP_PLACIDUS:
		vHausPlacidus(Radix);
        break;
  default:
		vKoch(Radix);
        break;
  }

  return ERR_OK;
}



/*
******************************************************************************
** House Cusp Calculations.
******************************************************************************
*/

/* This is a subprocedure of ComputeInHouses(). Given a zodiac position,  */
/* return which of the twelve houses it falls in. Remember that a special */
/* check has to be done for the house that spans 0 degrees Aries.         */

void vCalcMissing(RADIX* Radix, RADIX* RadixV, short sTyp)
{
 short sA, sB, sRet;
 double dP, dH1, dH2;

 for (sA=0; sA<MAX_PLANET; sA++) {
	 dP=Radix->pdPlanet[sA];
     for(sB=0;sB<12;sB++) {
        dH1=RadixV->pdHaus[sB];
        dH2=RadixV->pdHaus[(sB+1)%12];
        if (dH2<dH1)
           dH2+=360;
        if (dP>dH1&&dP<dH2||(dH2>360&&dP<RadixV->pdHaus[(sB+1)%12]))
           break;
	 } /* endfor*/
	 Radix->pszInHaus[sA]=sB+1;
 } /* endfor*/

 for ( sA=0; sA<Radix->sAnzahlPlanet; sA++ )
      Radix->pszStzPlanet[sA]=(char)floor(Radix->pdPlanet[sA]/30);

  if ( sTyp==TYP_RADIX||sTyp==TYP_SYNASTRIE )
	 for ( sA=0; sA<MAX_HAUS; sA++ )
  		 Radix->pszStzHaus[sA]=(char)floor(Radix->pdHaus[sA]/30);

	 memset(Radix->sQualitaten, 0, 9);
	 for ( sA=0; sA<Radix->sAnzahlPlanet; sA++ ) {
    	 switch (Radix->pszStzPlanet[sA]) {
	     case iWIDDER:
    	 	  Radix->sQualitaten[QU_FEUER]++;
	     	  Radix->sQualitaten[QU_CARDINAL]++;
	     	  Radix->sQualitaten[QU_MANN]++;
		 break;
		 case iSTIER:
	    	  Radix->sQualitaten[QU_ERDE]++;
	     	  Radix->sQualitaten[QU_FIX]++;
	     	  Radix->sQualitaten[QU_FRAU]++;
		 break;
		 case iZWILLINGE:
	     	  Radix->sQualitaten[QU_LUFT]++;
	     	  Radix->sQualitaten[QU_BEWEGLICH]++;
	     	  Radix->sQualitaten[QU_MANN]++;
		 break;
		 case iKREBS:
	     	  Radix->sQualitaten[QU_WASSER]++;
	     	  Radix->sQualitaten[QU_CARDINAL]++;
	     	  Radix->sQualitaten[QU_FRAU]++;
		 break;
	     case iLOWE:
	     	  Radix->sQualitaten[QU_FEUER]++;
	     	  Radix->sQualitaten[QU_FIX]++;
	     	  Radix->sQualitaten[QU_MANN]++;
		 break;
	     case iJUNGFRAU:
	     	  Radix->sQualitaten[QU_ERDE]++;
	     	  Radix->sQualitaten[QU_BEWEGLICH]++;
	     	  Radix->sQualitaten[QU_FRAU]++;
		 break;
	     case iWAAGE:
	     	  Radix->sQualitaten[QU_LUFT]++;
	     	  Radix->sQualitaten[QU_CARDINAL]++;
	     	  Radix->sQualitaten[QU_MANN]++;
		 break;
	     case iSKORPION:
	  	 	  Radix->sQualitaten[QU_WASSER]++;
	     	  Radix->sQualitaten[QU_FIX]++;
	     	  Radix->sQualitaten[QU_FRAU]++;
		 break;
	     case iSCHUTZE:
	     	  Radix->sQualitaten[QU_FEUER]++;
	     	  Radix->sQualitaten[QU_BEWEGLICH]++;
	     	  Radix->sQualitaten[QU_MANN]++;
		 break;
	     case iSTEINBOCK:
	     	  Radix->sQualitaten[QU_ERDE]++;
	     	  Radix->sQualitaten[QU_CARDINAL]++;
	     	  Radix->sQualitaten[QU_FRAU]++;
		 break;
	     case iWASSERMANN:
	     	  Radix->sQualitaten[QU_LUFT]++;
	     	  Radix->sQualitaten[QU_FIX]++;
	     	  Radix->sQualitaten[QU_MANN]++;
		 break;
	     case iFISCHE:
	     	  Radix->sQualitaten[QU_WASSER]++;
	     	  Radix->sQualitaten[QU_BEWEGLICH]++;
	     	  Radix->sQualitaten[QU_FRAU]++;
		 break;
		 } /* endswitch */
	 } /* enfor */
// } /* endif */

 vRotateRadix(Radix, RadixV, sTyp);
}

/*
******************************************************************************
** Planetary Position Calculations.
******************************************************************************
*/
void vCalcPlanets(RADIX * Radix)
{
  short i, sFlag;
  double dPlanet, dPlanetAlt, dPlanetRet, dSpace, dJD_ad=0;

  /* We can compute the positions of Sun through Pluto, Chiron, the four */
  /* asteroids, Lilith, and the (true or mean) North Node using Placalc. */
  /* The other objects must be done elsewhere.                           */

 //JUL_OFFSET 2433282.0  /* offset of Astrodienst relative Julian date */
//	dJD_ad = Radix->dT*36525.0+2415020.0-JUL_OFFSET+deltat(dJD_ad);
  dJD_ad = Radix->dT*36525.0-18262.0+dDeltat(dJD_ad);
//	sFlag = helio ? CALC_BIT_HELIO|CALC_BIT_SPEED : CALC_BIT_SPEED;
  sFlag=16;
  for (i = 0; i < Radix->sAnzahlPlanet; i++) {
	if (sCalc(i, dJD_ad, sFlag, &dPlanet, &dSpace, &dPlanetAlt, &dPlanetRet)==OK) {
	  /* Note that this can't compute charts with central planets other */
	  /* than the Sun or Earth or relative velocities in current state. */

	   Radix->pdPlanet[i]    = dMod(dPlanet + Radix->dSid);
	   Radix->psPlanetTyp[i] = dPlanetRet<0 ? Radix->psPlanetTyp[i]|P_TYP_RUCK:
											  Radix->psPlanetTyp[i];
//	   Radix->pdPlanetAlt[i] = dPlanetAlt;
//	   Radix->pdPlanetRet[i] = dPlanetRet/DEG2RAD;

	  /* Compute x,y,z coordinates from azimuth, altitude, and distance. */

//	  vSphToRec(dSpace, Radix->pdPlanet[i], Radix->pdPlanetAlt[i],
//		&spacex[i], &spacey[i], &spacez[i]);

	} /* endif*/
  } /* endfor*/
}

/*
******************************************************************************
** Chart Calculation.
******************************************************************************
*/
/* Compute the positions of the planets at a certain time using the Placalc */
/* accurate formulas and ephemeris. This will superseed the Matrix routine  */
/* values and is only called with the -b switch is in effect. Not all       */
/* objects or modes are available using this, but some additional values    */
/* such as Moon and Node velocities not available without -b are. (This is  */
/* the one place in Astrolog which calls the Placalc package functions.)    */
/*------------------------------------------------------------------------*/
/* This is probably the main routine in all of Astrolog. It generates a   */
/* chart, calculating the positions of all the celestial bodies and house */
/* cusps, based on the current chart information, and saves them for use  */
/* by any of the display routines.                                        */
/*------------------------------------------------------------------------*/

short sCalcChart(RADIX * Radix, RADIX* RadixV, short sTyp)
{
  int i, sRet;

  /* Hack: Time zone +/-24 means to have the time of day be in Local Mean */
  /* Time (LMT). This is done by making the time zone value reflect the   */
  /* logical offset from GMT as indicated by the chart's longitude value. */

//  if (fabs(Radix->rFix.fZone) == 24.0)
//	Radix->rFix.fZone = dDeg2Dez(dDez2Deg(Radix->rFix.dLange)/15.0);

  vCalcVar(Radix);

  if ( sTyp==TYP_RADIX||sTyp==TYP_SYNASTRIE ) {
	 if ( sCalcHauser(Radix)<ERR_OK)
  		return ERR_HOUSE;
  } /* endif */

  vCalcPlanets(Radix);

  vCalcWinkel(Radix, RadixV, sTyp);

  vCalcAsp(Radix, sTyp);

  vCalcMissing(Radix, RadixV, sTyp);


	/* Fill in "planet" positions corresponding to house cusps. */
/*
	Radix->pdPlanet[oVtx] = Radix->dVer;
	Radix->pdPlanet[oEP] = dSpitzeEP(Radix);
	for (i = 0; i < 12; i++)
	  Radix->pdPlanet[cuspLo + i] = Radix->pdHaus[i];
	if (!us.fHouseAngle) {              //Aspecte nur fr Haupth„ueser
	  Radix->pdPlanet[oAsc] = Radix->ASC;
	  Radix->pdPlanet[oMC]  = Radix->MC;
	  Radix->pdPlanet[oDes] = dMod(Radix->ASC + DEGHALB);
	  Radix->pdPlanet[oNad] = dMod(Radix->MC + DEGHALB);
	}
	for (i = oFor; i <= cuspHi; i++)
	  Radix->pdPlanetRet[i] = DEGMAX/DEG2RAD;
	  */

  /* If -3 decan chart switch in effect, edit planet positions accordingly. */
/*
  if (us.fDecan) {
	for (i = 1; i <= cObj; i++)
	  Radix->pdPlanet[i] = Decan(Radix->pdPlanet[i]);
	vInHaus();
  }*/

  return ERR_OK;
}


