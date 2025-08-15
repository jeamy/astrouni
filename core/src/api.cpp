#include "astrocore/api.hpp"
#include <cmath>
#include <algorithm>

namespace astrocore {

Version version() {
  return Version{0, 1, 0};
}

int64_t demo_compute(int64_t x) {
  // trivial placeholder
  return x * 2 + 1;
}

// Helper: Gregorian switch test (15 Oct 1582 and later)
static inline bool is_gregorian(int y, int m, int d) {
  if (y > 1582) return true;
  if (y < 1582) return false;
  if (m > 10) return true;
  if (m < 10) return false;
  return d >= 15;
}

double julian_day(const Date& date, const Time& time, const TimeZone& tz) {
  // Convert local civil time to UTC hours
  const double localHours = static_cast<double>(time.hour)
                          + static_cast<double>(time.minute) / 60.0
                          + time.second / 3600.0;
  const double utcHours = localHours - (tz.utcOffsetHours + tz.dstHours);

  int y = date.year;
  int m = date.month;
  int D = date.day;

  // Algorithm from Meeus, using proleptic Gregorian/Julian switch on 1582-10-15
  int a = (14 - m) / 12;
  int y2 = y + 4800 - a;
  int m2 = m + 12 * a - 3;

  long JDN;
  if (is_gregorian(y, m, D)) {
    JDN = D + (153 * m2 + 2) / 5 + 365L * y2 + y2 / 4 - y2 / 100 + y2 / 400 - 32045L;
  } else {
    JDN = D + (153 * m2 + 2) / 5 + 365L * y2 + y2 / 4 - 32083L;
  }

  // JD with time of day in UTC; JD is defined such that it rolls over at 12:00 UT
  const double JD = static_cast<double>(JDN) + (utcHours - 12.0) / 24.0;
  return JD;
}

void coord_transform(double& azimuth, double& altitude, double shift) {
  // Port of legacy vCoorXform with angles in radians
  const double sinAlt = std::sin(altitude);
  const double cosAlt = std::cos(altitude);
  const double sinAzi = std::sin(azimuth);
  const double cosAzi = std::cos(azimuth);
  const double sinShift = std::sin(shift);
  const double cosShift = std::cos(shift);

  double dx = cosAlt * sinAzi * cosShift;
  const double dy1 = sinAlt * sinShift;
  dx -= dy1;

  double dy = cosAlt * cosAzi;

  // dWinkel(dy, dx): angle from components; original passes (dy, dx)
  // Use atan2 with (y = dx, x = dy) to mirror that convention.
  const double newAzi = std::atan2(dx, dy);

  double newAlt = cosAlt * sinAzi * sinShift + sinAlt * cosShift;
  newAlt = std::asin(newAlt);

  azimuth = newAzi;
  altitude = newAlt;
}

double julian_century(double jd) {
  return (jd - 2451545.0) / 36525.0;
}

double mean_obliquity_rad(double jd) {
  // IAU 2006/2000A approximation (arcseconds)
  const double T = julian_century(jd);
  // Polynomial from Meeus (arcseconds)
  double epsArcsec = 84381.406
                   - 46.836769 * T
                   - 0.0001831 * T * T
                   + 0.00200340 * T * T * T
                   - 5.76e-7 * T * T * T * T
                   - 4.34e-8 * T * T * T * T * T;
  return (epsArcsec / 3600.0) * (M_PI / 180.0);
}

Equatorial ecliptic_to_equatorial(const Ecliptic& ecl, double eps) {
  const double sinE = std::sin(eps);
  const double cosE = std::cos(eps);
  const double sinB = std::sin(ecl.lat);
  const double cosB = std::cos(ecl.lat);
  const double sinL = std::sin(ecl.lon);
  const double cosL = std::cos(ecl.lon);

  const double y = sinL * cosE - (sinB / cosB) * sinE; // careful if cosB ~ 0
  const double x = cosL;
  double ra = std::atan2(y * cosB, x * cosB);
  if (ra < 0) ra += 2.0 * M_PI;
  const double dec = std::asin(sinB * cosE + cosB * sinE * sinL);
  return Equatorial{ra, dec};
}

static inline double wrap_two_pi(double a) {
  const double twopi = 2.0 * M_PI;
  a = std::fmod(a, twopi);
  if (a < 0) a += twopi;
  return a;
}

double gmst_rad(double jd) {
  // GMST from Meeus, simplified
  const double T = (std::floor(jd - 0.5) + 0.5 - 2451545.0) / 36525.0;
  const double S = 24110.54841 + 8640184.812866 * T + 0.093104 * T * T - 6.2e-6 * T * T * T; // seconds
  const double dayfrac = (jd + 0.5 - std::floor(jd + 0.5));
  const double seconds = S + dayfrac * 86400.0 * 1.00273790935;
  const double hours = std::fmod(seconds / 3600.0, 24.0);
  return wrap_two_pi(hours * (M_PI / 12.0));
}

double lst_rad(double jd, double longitudeRad) {
  return wrap_two_pi(gmst_rad(jd) + longitudeRad);
}

AspectResult detect_aspect(double lon1Deg, double lon2Deg, double orbDeg) {
  auto wrap360 = [](double a){ a = std::fmod(a, 360.0); if (a < 0) a += 360.0; return a; };
  double a1 = wrap360(lon1Deg);
  double a2 = wrap360(lon2Deg);
  double d = std::fabs(a1 - a2);
  if (d > 180.0) d = 360.0 - d;

  struct Target { AspectType t; double ang; } targets[] = {
    {AspectType::Conjunction, 0.0},
    {AspectType::Sextile, 60.0},
    {AspectType::Square, 90.0},
    {AspectType::Trine, 120.0},
    {AspectType::Opposition, 180.0},
  };
  for (auto& tgt : targets) {
    double delta = d - tgt.ang;
    if (std::fabs(delta) <= orbDeg) {
      return AspectResult{tgt.t, tgt.ang, delta};
    }
  }
  return AspectResult{AspectType::None, 0.0, 0.0};
}

void asc_mc_longitudes(double jd, double longitudeRad, double latitudeRad,
                       double& ascRad, double& mcRad) {
  const double eps = mean_obliquity_rad(jd);
  const double theta = lst_rad(jd, longitudeRad); // local sidereal time

  // MC longitude: tan(λ_MC) = sinθ / (cosθ cosε)
  {
    double y = std::sin(theta);
    double x = std::cos(theta) * std::cos(eps);
    mcRad = std::atan2(y, x);
    mcRad = wrap_two_pi(mcRad);
  }

  // Ascendant longitude:
  // tan(λ_ASC) = -cosθ / (sinθ cosε + tanφ sinε)
  {
    double y = -std::cos(theta);
    double x = std::sin(theta) * std::cos(eps) + std::tan(latitudeRad) * std::sin(eps);
    ascRad = std::atan2(y, x);
    ascRad = wrap_two_pi(ascRad);
  }
}

void equal_house_cusps(double ascRad, double cuspsRad[12]) {
  const double step = 2.0 * M_PI / 12.0;
  for (int i = 0; i < 12; ++i) {
    double a = ascRad + i * step;
    if (a >= 2.0 * M_PI || a < 0) a = std::fmod(a, 2.0 * M_PI);
    if (a < 0) a += 2.0 * M_PI;
    cuspsRad[i] = a;
  }
}

static inline double wrap_deg(double d) {
  d = std::fmod(d, 360.0);
  if (d < 0) d += 360.0;
  return d;
}

static inline double ra_to_lambda(double ra, double eps) {
  // λ = atan2( sin RA, cos RA * cos ε ) in radians, wrapped 0..2π
  double y = std::sin(ra);
  double x = std::cos(ra) * std::cos(eps);
  double lam = std::atan2(y, x);
  return wrap_two_pi(lam);
}

void placidus_house_cusps(double jd, double longitudeRad, double latitudeRad, double cuspsRad[12]) {
  // Iterative method based on semi-arc fractions.
  // References: Morinus notes on Placidus cusps (iterative "old way").
  const double eps = mean_obliquity_rad(jd);
  const double RAMC = lst_rad(jd, longitudeRad); // radians
  const double phi = latitudeRad;

  // Get Asc and MC
  double asc = 0.0, mc = 0.0;
  asc_mc_longitudes(jd, longitudeRad, latitudeRad, asc, mc);

  auto decl_from_ra = [&](double ra) {
    // For ecliptic intersection at given RA, declination δ = asin( sin ε * sin λ ), λ from RA
    double lam = ra_to_lambda(ra, eps);
    return std::asin(std::sin(eps) * std::sin(lam));
  };

  auto H_diurnal = [&](double dec) {
    double v = -std::tan(phi) * std::tan(dec);
    v = std::clamp(v, -1.0, 1.0);
    return std::acos(v);
  };
  auto H_nocturnal = [&](double dec) {
    double v =  std::tan(phi) * std::tan(dec);
    v = std::clamp(v, -1.0, 1.0);
    return std::acos(v);
  };

  auto iterate_ra = [&](double initRa, bool diurnal, double frac) {
    double ra = initRa;
    const int maxIter = 60;
    const double tol = 1e-12;
    for (int i = 0; i < maxIter; ++i) {
      double dec = decl_from_ra(ra);
      double H = diurnal ? H_diurnal(dec) : H_nocturnal(dec);
      double next = diurnal ? (RAMC + frac * H) : (RAMC - frac * H);
      // normalize RA to principal range near RAMC to aid convergence
      double delta = std::remainder(next - ra, 2.0 * M_PI);
      ra = ra + delta;
      if (std::fabs(delta) < tol) break;
    }
    return wrap_two_pi(ra);
  };

  // Initial guesses 30°/60° offsets as commonly suggested
  double ra12 = iterate_ra(wrap_two_pi(RAMC + M_PI / 6.0), true, 1.0 / 3.0);
  double ra11 = iterate_ra(wrap_two_pi(RAMC + M_PI / 3.0), true, 2.0 / 3.0);
  double ra2  = iterate_ra(wrap_two_pi(RAMC - M_PI / 6.0), false, 1.0 / 3.0);
  double ra3  = iterate_ra(wrap_two_pi(RAMC - M_PI / 3.0), false, 2.0 / 3.0);

  // Convert RA to ecliptic longitudes for cusps
  double c12 = ra_to_lambda(ra12, eps);
  double c11 = ra_to_lambda(ra11, eps);
  double c2  = ra_to_lambda(ra2,  eps);
  double c3  = ra_to_lambda(ra3,  eps);

  // Fill cusps: index 0..11 => houses 1..12
  cuspsRad[0] = asc;                 // 1
  cuspsRad[9] = mc;                  // 10
  cuspsRad[3] = wrap_two_pi(mc + M_PI); // 4
  cuspsRad[6] = wrap_two_pi(asc + M_PI); // 7

  cuspsRad[11] = c12;               // 12
  cuspsRad[10] = c11;               // 11
  cuspsRad[1]  = c2;                // 2
  cuspsRad[2]  = c3;                // 3

  cuspsRad[4]  = wrap_two_pi(c11 + M_PI); // 5 opposite 11
  cuspsRad[5]  = wrap_two_pi(c12 + M_PI); // 6 opposite 12
  cuspsRad[7]  = wrap_two_pi(c2  + M_PI); // 8 opposite 2
  cuspsRad[8]  = wrap_two_pi(c3  + M_PI); // 9 opposite 3
}

} // namespace astrocore
