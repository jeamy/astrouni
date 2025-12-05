#pragma once

#include "astro/data_types.h"
#include <cmath>
#include <vector>

namespace astro {

// Constants from legacy/aucalc.h
#define EPOCH1900 -18262.0
#define EPOCH1850 -36524.0
#define DEGMAX 360.0
#define DEGHALB 180.0
#define DEGVIER 90.0
#define DEG2RAD (DEGHALB / M_PI)
#define RAD2DEG (M_PI / DEGHALB)
#define SDNUM 20

// Planet indices
enum PlanetIndex {
  P_SONNE = 0,
  P_ERDE = 0, // Earth and Sun share index 0 (Earth in helio, Sun in geo)
  P_MOND,
  P_MERKUR,
  P_VENUS,
  P_MARS,
  P_JUPITER,
  P_SATURN,
  P_URANUS,
  P_NEPTUN,
  P_PLUTO,
  P_CHIRON,
  P_LILITH,
  P_NKNOTEN,
  // ... others
  MAX_PLANET = 20
};

// House Systems
enum HouseSystem {
  TYP_PLACIDUS = 0,
  TYP_KOCH,
  TYP_EQUAL,
  TYP_CAMPANUS,
  TYP_MERIDIAN,
  TYP_REGIOMON,
  TYP_PORPHYRY,
  TYP_MORINUS,
  TYP_TOPOCENTRIC,
  TYP_ALCABIT,
  TYP_EQUALMID,
  TYP_PORPHYRYN,
  TYP_WHOLE,
  TYP_NULL
};

class AstroCalculator {
public:
  AstroCalculator();

  // Main entry point for chart calculation
  // Corresponds to sCalcChart in legacy code
  bool CalcChart(AstroRadix &radix);

private:
  // Internal state (mirrors legacy global variables)
  struct elements {
    double tj; /* centuries from epoch */
    double lg; /* mean longitude in degrees of arc*/
    double pe; /* longitude of the perihelion in degrees of arc*/
    double ex; /* excentricity in degrees of arc*/
    double kn; /* longitude of node in degrees of arc*/
    double in; /* inclination of the orbit in degrees of arc*/
    double ma; /* mean anomaly in degrees of arc*/
  } el[MAX_PLANET];
  double meanekl;
  double ekl;
  double nut;
  double sa[SDNUM];

  // Helper functions
  double dJulTag(short sTag, short sMonat, short sJahr, double dStunde,
                 short sGreg);
  long lTMJ2Jul(short sTag, short sMonat, short sJahr);
  void vCalcVar(AstroRadix &radix);
  void vCalcPlanets(AstroRadix &radix);
  short sCalcHauser(AstroRadix &radix);
  void vCalcMissing(AstroRadix &radix);

  // House Systems
  void vHausPlacidus(AstroRadix &radix);
  void vKoch(AstroRadix &radix);
  double dSpitzePlacidus(AstroRadix &radix, double dDeg, double dFF,
                         short sNeg);
  double dSpitzeMid(AstroRadix &radix);
  double dSpitzeAsc(AstroRadix &radix);

  // Planetary calculation helpers (from aupla.c)
  // We might need to port the Placalc logic or use a simplified version if the
  // full ephemeris is not available/needed immediately. For now, we'll aim for
  // the legacy logic.
  short sCalc(short planet, double jd_ad, short flag, double *alng,
              double *arad, double *alat, double *alngspeed);
  int hel(int planet, double t, double *al, double *ar, double *az, double *alp,
          double *arp, double *azp);
  int outer_hel(int planet, double jd_ad, double *al, double *ar, double *az,
                double *alp, double *arp, double *azp);
  int moon(double *al, double *ar, double *az);
  void helup(double jd_ad);
  void disturb(struct AstroKor *k, double *al, double *ar, double lk, double rk,
               double man);
  void togeo(double lngearth, double radearth, double lng, double rad,
             double zet, double *alnggeo, double *aradgeo);
  double dDeltat(double jd_ad);
  double fnu(double t, double ex, double err);

  // Math helpers
  double dMod(double dD);
  double dModRad(double dD);
  double dWinkel(double dX, double dY);
  double dDez2Deg(double dD);
  double dDeg2Dez(double dD);
  void vPolToRec(double dA, double dR, double *dX, double *dY);
  void vRecToPol(double dX, double dY, double *dA, double *dR);
  double dRecToSph(double dB, double dL, double dO);
};

} // namespace astro
