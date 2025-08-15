/* Like above but return a fractional Julian time given the extra info. */

double dTMJzoz2Jul(short sTag, short sMonat, short sJahr,
					   double dZeit, double dSom, double dZone)
{
  return (double)lTMJ2Jul( sTag, sMonat, sJahr) +
	(dDecToDeg(dZeit) + dDecToDeg(dZone) - dDecToDeg(dSom)) / 24.0;
}

/* Do a coordinate transformation: Given a longitude and latitude value,    */
/* return the new longitude and latitude values that the same location      */
/* would have, were the equator tilted by a specified number of degrees.    */
/* In other words, do a pole shift! This is used to convert among ecliptic, */
/* equatorial, and local coordinates, each of which have zero declination   */
/* in different planes. In other words, take into account the Earth's axis. */

void vCoorXform( double * dAzi, double * dAlt, double dShift)
{
  double dx=0, dy, da1=0, dl1;
  double dSinalt, dCosalt, dSinazi, dSinshift, dCosshift;

  dSinalt = sin(*dAlt);
  dCosalt = cos(*dAlt);
  dSinazi = sin(*dAzi);
  dSinshift = sin(dShift);
  dCosshift = cos(dShift);

  dx = dCosalt * dSinazi * dCosshift;
  dy = dSinalt * dSinshift;
  dx -= dy;
  da1 = dCosalt;
  dy = dCosalt * cos(*dAzi);
  dl1 = dWinkel(dy, dx);
  da1 = da1 * dSinazi * dSinshift + dSinalt * dCosshift;
  da1 = asin(da1);
  *dAzi = dl1;
  *dAlt = da1;
}


long lTMJ2Jul(short sTag, short sMonat, short sJahr)
{
 /*       wenn NICHT PLACALC!
  long lIm, lJ;

  lIm = 12*((long)sJahr+4800)+(long)sMonat-3;
  lJ = (2*(lIm%12) + 7 + 365*lIm)/12;
  lJ += (long)sTag + lIm/48 - 32083;
  if (lJ > 2299171)                   /* Take care of dates in
	lJ += lIm/4800 - lIm/1200 + 38;     /* Gregorian calendar.
  return lJ;
*/

  short sGreg = TRUE;

  if (sJahr < 1582 || (sJahr == 1582 &&
	(sMonat < 10 || (sMonat == 10 && sTag < 15))))
	sGreg = FALSE;
  return (long)floor(dJulTag(sTag, sMonat, sJahr, 12.0, sGreg)+RUNDEN);
}
