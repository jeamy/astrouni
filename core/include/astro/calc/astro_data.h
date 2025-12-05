#pragma once

#include "astro/calc/astro_calc.h"

namespace astro {

// Constants
// Constants
#define NUM_MOON_CORR 93
#define ENDMARK 99

// Struct definitions from aucalc.h/aupla1.c
struct eledata {
  double axis;   /* mean distance, in a.u. */
  double period; /* days for one revolution */
  double epoch;  /* relative juldate of epoch */
  double lg0, lg1, lg2,
      lg3; /* deg(epoch), degree/day, seconds/T^2, seconds/T^3 */
  double pe0, pe1, pe2,
      pe3;              /* deg(epoch), seconds/T, seconds/T^2, seconds/T^3 */
  double ex0, ex1, ex2; /* ecl(epoch), 1/T, 1/T^2 */
  double kn0, kn1, kn2,
      kn3;              /* node(epoch), seconds/T, seconds/T^2, seconds/T^3 */
  double in0, in1, in2; /* incl(epoch), 1/T, 1/T^2 */
};

struct AstroKor {
  int j, i;
  double lampl;  /* amplitude of disturbation in long, seconds of arc */
  double lphase; /* phase of disturbation in long, degrees */
  double rampl;  /* ampl. of disturbation in radius, 9th place of log */
  double rphase; /* phase of disturbation in radius, degrees */
  int k;         /* index into disturbing planet anomaly table sa[] */
};

struct sdat {
  double sd0; /* mean anomaly at epoch 1850 */
  double sd1; /* degrees/year */
};

struct m45dat {
  int i0, i1, i2, i3;
  double lng, lat, par;
};

struct m5dat {
  double lng;
  int i0, i1, i2, i3;
};

// Extern declarations
extern struct eledata pd[MAX_PLANET + 1];
extern struct AstroKor earthkor[];
extern struct AstroKor mercurykor[];
extern struct AstroKor venuskor[];
extern struct AstroKor marskor[];
extern struct sdat _sd[SDNUM];
extern struct m45dat m45[NUM_MOON_CORR];
extern struct m5dat m5[];
extern double ekld[4];

} // namespace astro
