#include "astrocore/houses.hpp"
#include <cmath>
#include <algorithm>

namespace astrocore {

static inline double wrap_deg(double d) {
  d = std::fmod(d, 360.0);
  if (d < 0) d += 360.0;
  return d;
}

// (Campanus definition moved below helper functions)

// Forward declaration (definition below helpers)
static void campanus_house_cusps(double jd, double longitudeRad, double latitudeRad, double cuspsRad[12]);

// (campanus_house_cusps is defined below helper definitions)

static inline double rad2deg(double r) { return r * (180.0 / M_PI); }
static inline double deg2rad(double d) { return d * (M_PI / 180.0); }

static inline double wrap_rad(double r) {
  r = std::fmod(r, 2*M_PI);
  if (r < 0) r += 2*M_PI;
  return r;
}

// atan2 wrapped to [0, 2pi)
static inline double angle_wrap(double y, double x) {
  double a = std::atan2(y, x);
  return a < 0 ? a + 2*M_PI : a;
}

// Meridian (Axial) house system translated from legacy vMeridian()
static void meridian_house_cusps(double jd, double longitudeRad, double cuspsRad[12]) {
  const double OB = mean_obliquity_rad(jd);
  const double RA = lst_rad(jd, longitudeRad);
  for (int i = 0; i < 12; ++i) {
    double D = deg2rad(60.0 + 30.0 * static_cast<double>(i + 1));
    double x = std::cos(RA + D) * std::cos(OB);
    double y = std::sin(RA + D);
    cuspsRad[i] = angle_wrap(y, x);
  }
}

// Morinus house system translated from legacy vMorinus()
static void morinus_house_cusps(double jd, double longitudeRad, double cuspsRad[12]) {
  const double OB = mean_obliquity_rad(jd);
  const double RA = lst_rad(jd, longitudeRad);
  for (int i = 0; i < 12; ++i) {
    double D = deg2rad(60.0 + 30.0 * static_cast<double>(i + 1));
    double x = std::cos(RA + D);
    double y = std::sin(RA + D) * std::cos(OB);
    cuspsRad[i] = angle_wrap(y, x);
  }
}

// Equal Mid (Equal from MC) translated from legacy vEqualMid()
static void equal_mid_house_cusps(double mcRad, double cuspsRad[12]) {
  // Start 270 degrees behind MC, then 30-degree steps
  double base = wrap_rad(mcRad - deg2rad(270.0));
  for (int i = 0; i < 12; ++i) {
    cuspsRad[i] = wrap_rad(base + deg2rad(30.0 * i));
  }
}

// Helper for Topocentric (translated from dSpitzeTopocentric)
static double topo_spitze_longitude(double RA, double OB, double lat, double deltaRad) {
  double OA = wrap_rad(RA + deltaRad);
  double X = std::atan(std::tan(lat) / std::cos(OA));
  double LO = std::atan(std::cos(X) * std::tan(OA) / std::cos(X + OB));
  if (LO < 0.0) LO += M_PI;
  if (std::sin(OA) < 0.0) LO += M_PI;
  return LO; // radians
}

// Topocentric house system translated from legacy vTopocentric()
static void topocentric_house_cusps(double jd, double longitudeRad, double latitudeRad, double mcRad, double cuspsRad[12]) {
  const double OB = mean_obliquity_rad(jd);
  const double RA = lst_rad(jd, longitudeRad);

  // Precompute latitude adjustments
  double tLat = std::tan(latitudeRad);
  double p1 = std::atan(tLat / 3.0);
  double p2 = std::atan(tLat / 1.5);

  // Compute primary set (indices 0..5) in local frame
  double prim[6];
  prim[3] = wrap_rad(mcRad + M_PI); // IC opposite MC
  prim[4] = wrap_rad(topo_spitze_longitude(RA, OB, p1, deg2rad(30.0)) + M_PI);
  prim[5] = wrap_rad(topo_spitze_longitude(RA, OB, p2, deg2rad(60.0)) + M_PI);
  prim[0] = topo_spitze_longitude(RA, OB, latitudeRad, deg2rad(90.0));
  prim[1] = topo_spitze_longitude(RA, OB, p2, deg2rad(120.0));
  prim[2] = topo_spitze_longitude(RA, OB, p1, deg2rad(150.0));

  // Finalize by adding LST (sidereal) like legacy + wrap
  for (int i = 0; i < 6; ++i) {
    if (i == 3) {
      cuspsRad[i] = prim[i]; // already absolute (mc+pi)
    } else {
      cuspsRad[i] = wrap_rad(prim[i] + RA);
    }
    cuspsRad[i + 6] = wrap_rad(cuspsRad[i] + M_PI);
  }
}

// Koch system translated from legacy vKoch() using LST (RA), obliquity, latitude
static void koch_house_cusps(double jd, double longitudeRad, double latitudeRad, double cuspsRad[12]) {
  const double OB = mean_obliquity_rad(jd);
  const double RA = lst_rad(jd, longitudeRad); // local sidereal time (radians)

  const double a1 = std::asin(std::sin(RA) * std::tan(latitudeRad) * std::tan(OB)); // radians

  for (int i = 0; i < 12; ++i) {
    double dD = 60.0 + 30.0 * static_cast<double>(i + 1); // degrees
    dD = wrap_deg(dD);
    double a2 = dD / 90.0 - 1.0;
    double kn = 1.0;
    if (dD >= 180.0) {
      kn = -1.0;
      a2 = dD / 90.0 - 3.0;
    }
    double a3 = wrap_rad(deg2rad(rad2deg(RA) + dD + a2 * rad2deg(a1))); // radians
    double x = std::cos(a3) * std::cos(OB) - kn * std::tan(latitudeRad) * std::sin(OB);
    double y = std::sin(a3);
    double ang = angle_wrap(y, x); // radians
    cuspsRad[i] = ang; // zodiac longitude in radians
  }
}

// Campanus house cusps translated from legacy vCampanus()
static void campanus_house_cusps(double jd, double longitudeRad, double latitudeRad, double cuspsRad[12]) {
  const double OB = mean_obliquity_rad(jd);
  const double RA = lst_rad(jd, longitudeRad);
  for (int i = 0; i < 12; ++i) {
    double KO = deg2rad(60.0 + 30.0 * static_cast<double>(i + 1));
    double DN = std::atan(std::tan(KO) * std::cos(latitudeRad));
    if (DN < 0.0) DN += M_PI;
    if (std::sin(KO) < 0.0) DN += M_PI;
    double x = std::cos(RA + DN) * std::cos(OB) - std::sin(DN) * std::tan(latitudeRad) * std::sin(OB);
    double y = std::sin(RA + DN);
    double ang = angle_wrap(y, x);
    cuspsRad[i] = ang;
  }
}

// Regiomontanus house cusps translated from legacy vRegiomontanus()
static void regiomontanus_house_cusps(double jd, double longitudeRad, double latitudeRad, double cuspsRad[12]) {
  const double OB = mean_obliquity_rad(jd);
  const double RA = lst_rad(jd, longitudeRad);
  for (int i = 0; i < 12; ++i) {
    double D = deg2rad(60.0 + 30.0 * static_cast<double>(i + 1));
    double x = std::cos(RA + D) * std::cos(OB) - std::sin(D) * std::tan(latitudeRad) * std::sin(OB);
    double y = std::sin(RA + D);
    double ang = angle_wrap(y, x);
    cuspsRad[i] = ang;
  }
}

// Porphyry house cusps using Asc and MC
static void porphyry_house_cusps(double ascRad, double mcRad, double cuspsRad[12]) {
  double ascDeg = rad2deg(ascRad);
  double mcDeg = rad2deg(mcRad);

  double dX = ascDeg - mcDeg;
  if (dX < 0.0) dX += 360.0;
  double dY = dX / 3.0;
  // Houses 5 and 6 (indices 4 and 5)
  for (int i = 0; i < 2; ++i) {
    cuspsRad[i + 4] = deg2rad(wrap_deg(180.0 + mcDeg + (i + 1) * dY));
  }
  // Houses 1..3
  double dX2 = wrap_deg(180.0 + mcDeg) - ascDeg;
  if (dX2 < 0.0) dX2 += 360.0;
  double dY2 = dX2 / 3.0;
  cuspsRad[0] = deg2rad(wrap_deg(ascDeg));
  for (int i = 0; i < 3; ++i) {
    cuspsRad[i + 1] = deg2rad(wrap_deg(ascDeg + (i + 1) * dY2));
  }
  // Opposite houses
  for (int i = 0; i < 6; ++i) {
    cuspsRad[i + 6] = deg2rad(wrap_deg(rad2deg(cuspsRad[i]) + 180.0));
  }
}

// Porphyry Neo as in legacy vPorphyryNeo()
static double min_distance_deg(double a, double b) { // in degrees 0..360
  double d = std::fabs(a - b);
  return d <= 180.0 ? d : 360.0 - d;
}

static void porphyry_neo_house_cusps(double ascRad, double mcRad, double cuspsRad[12]) {
  double ascDeg = wrap_deg(rad2deg(ascRad));
  double mcDeg = wrap_deg(rad2deg(mcRad));
  double dD = (min_distance_deg(mcDeg, ascDeg) - 90.0) / 4.0;

  cuspsRad[6] = deg2rad(wrap_deg(ascDeg + 180.0));
  cuspsRad[9] = deg2rad(mcDeg);
  cuspsRad[10] = deg2rad(wrap_deg(mcDeg + 30.0 + dD));
  cuspsRad[11] = deg2rad(wrap_deg(rad2deg(cuspsRad[10]) + 30.0 + 2.0 * dD));
  cuspsRad[8]  = deg2rad(wrap_deg(mcDeg - 30.0 + dD));
  cuspsRad[7]  = deg2rad(wrap_deg(rad2deg(cuspsRad[8]) - 30.0 + 2.0 * dD));
  for (int i = 0; i < 6; ++i) {
    cuspsRad[i] = deg2rad(wrap_deg(rad2deg(cuspsRad[i + 6]) - 180.0));
  }
}

// Whole sign house cusps from Ascendant sign start
static void whole_sign_house_cusps(double ascRad, double cuspsRad[12]) {
  double ascDeg = wrap_deg(rad2deg(ascRad));
  double signStart = std::floor(ascDeg / 30.0) * 30.0;
  for (int i = 0; i < 12; ++i) {
    cuspsRad[i] = deg2rad(wrap_deg(signStart + 30.0 * i));
  }
}

HouseCusps compute_house_cusps(const Date& d,
                               const Time& t,
                               const TimeZone& tz,
                               double longitudeDeg,
                               double latitudeDeg,
                               HouseSystem system) {
  HouseCusps out{};

  // Convert inputs and compute JD/asc/mc
  const double jd = julian_day(d, t, tz);
  const double lonRad = deg2rad(longitudeDeg);
  const double latRad = deg2rad(latitudeDeg);

  double asc = 0.0, mc = 0.0;
  asc_mc_longitudes(jd, lonRad, latRad, asc, mc);

  out.ascDeg = wrap_deg(rad2deg(asc));
  out.mcDeg  = wrap_deg(rad2deg(mc));

  double cuspsRad[12]{};

  switch (system) {
    case HouseSystem::Equal: {
      equal_house_cusps(asc, cuspsRad);
      for (int i = 0; i < 12; ++i) out.cuspDeg[i] = wrap_deg(rad2deg(cuspsRad[i]));
      out.valid = true;
      break;
    }
    case HouseSystem::Placidus: {
      placidus_house_cusps(jd, lonRad, latRad, cuspsRad);
      for (int i = 0; i < 12; ++i) out.cuspDeg[i] = wrap_deg(rad2deg(cuspsRad[i]));
      out.valid = true; // Placidus may fail at extreme latitudes; handle later with warnings
      break;
    }
    case HouseSystem::Koch: {
      koch_house_cusps(jd, lonRad, latRad, cuspsRad);
      // Convert to degrees first
      double degs[12];
      for (int i = 0; i < 12; ++i) degs[i] = wrap_deg(rad2deg(cuspsRad[i]));
      // Some formulations yield cusp[0] near the Descendant (Asc+180). If cusp[6] is
      // closer to Ascendant than cusp[0], rotate by 6 to make cusp1 align with Asc.
      auto dist0 = min_distance_deg(degs[0], out.ascDeg);
      auto dist6 = min_distance_deg(degs[6], out.ascDeg);
      if (dist6 < dist0) {
        for (int i = 0; i < 12; ++i) out.cuspDeg[i] = degs[(i + 6) % 12];
      } else {
        for (int i = 0; i < 12; ++i) out.cuspDeg[i] = degs[i];
      }
      out.valid = true;
      break;
    }
    case HouseSystem::Porphyry: {
      porphyry_house_cusps(asc, mc, cuspsRad);
      for (int i = 0; i < 12; ++i) out.cuspDeg[i] = wrap_deg(rad2deg(cuspsRad[i]));
      out.valid = true;
      break;
    }
    case HouseSystem::Campanus: {
      campanus_house_cusps(jd, lonRad, latRad, cuspsRad);
      double degs[12];
      for (int i = 0; i < 12; ++i) degs[i] = wrap_deg(rad2deg(cuspsRad[i]));
      auto dist0 = min_distance_deg(degs[0], out.ascDeg);
      auto dist6 = min_distance_deg(degs[6], out.ascDeg);
      if (dist6 < dist0) {
        for (int i = 0; i < 12; ++i) out.cuspDeg[i] = degs[(i + 6) % 12];
      } else {
        for (int i = 0; i < 12; ++i) out.cuspDeg[i] = degs[i];
      }
      out.valid = true;
      break;
    }
    case HouseSystem::PorphyryNeo: {
      porphyry_neo_house_cusps(asc, mc, cuspsRad);
      for (int i = 0; i < 12; ++i) out.cuspDeg[i] = wrap_deg(rad2deg(cuspsRad[i]));
      out.valid = true;
      break;
    }
    case HouseSystem::Whole: {
      whole_sign_house_cusps(asc, cuspsRad);
      for (int i = 0; i < 12; ++i) out.cuspDeg[i] = wrap_deg(rad2deg(cuspsRad[i]));
      out.valid = true;
      break;
    }
    case HouseSystem::Regiomontanus: {
      regiomontanus_house_cusps(jd, lonRad, latRad, cuspsRad);
      double degs[12];
      for (int i = 0; i < 12; ++i) degs[i] = wrap_deg(rad2deg(cuspsRad[i]));
      auto dist0 = min_distance_deg(degs[0], out.ascDeg);
      auto dist6 = min_distance_deg(degs[6], out.ascDeg);
      if (dist6 < dist0) {
        for (int i = 0; i < 12; ++i) out.cuspDeg[i] = degs[(i + 6) % 12];
      } else {
        for (int i = 0; i < 12; ++i) out.cuspDeg[i] = degs[i];
      }
      out.valid = true;
      break;
    }
    case HouseSystem::Topocentric: {
      topocentric_house_cusps(jd, lonRad, latRad, mc, cuspsRad);
      double degs[12];
      for (int i = 0; i < 12; ++i) degs[i] = wrap_deg(rad2deg(cuspsRad[i]));
      auto dist0 = min_distance_deg(degs[0], out.ascDeg);
      auto dist6 = min_distance_deg(degs[6], out.ascDeg);
      if (dist6 < dist0) {
        for (int i = 0; i < 12; ++i) out.cuspDeg[i] = degs[(i + 6) % 12];
      } else {
        for (int i = 0; i < 12; ++i) out.cuspDeg[i] = degs[i];
      }
      out.valid = true;
      break;
    }
    case HouseSystem::Meridian: {
      meridian_house_cusps(jd, lonRad, cuspsRad);
      double degs[12];
      for (int i = 0; i < 12; ++i) degs[i] = wrap_deg(rad2deg(cuspsRad[i]));
      auto dist0 = min_distance_deg(degs[0], out.ascDeg);
      auto dist6 = min_distance_deg(degs[6], out.ascDeg);
      if (dist6 < dist0) {
        for (int i = 0; i < 12; ++i) out.cuspDeg[i] = degs[(i + 6) % 12];
      } else {
        for (int i = 0; i < 12; ++i) out.cuspDeg[i] = degs[i];
      }
      out.valid = true;
      break;
    }
    case HouseSystem::Morinus: {
      morinus_house_cusps(jd, lonRad, cuspsRad);
      double degs[12];
      for (int i = 0; i < 12; ++i) degs[i] = wrap_deg(rad2deg(cuspsRad[i]));
      auto dist0 = min_distance_deg(degs[0], out.ascDeg);
      auto dist6 = min_distance_deg(degs[6], out.ascDeg);
      if (dist6 < dist0) {
        for (int i = 0; i < 12; ++i) out.cuspDeg[i] = degs[(i + 6) % 12];
      } else {
        for (int i = 0; i < 12; ++i) out.cuspDeg[i] = degs[i];
      }
      out.valid = true;
      break;
    }
    case HouseSystem::EqualMid: {
      equal_mid_house_cusps(mc, cuspsRad);
      for (int i = 0; i < 12; ++i) out.cuspDeg[i] = wrap_deg(rad2deg(cuspsRad[i]));
      out.valid = true;
      break;
    }
    case HouseSystem::Alcabitius: {
      out.valid = false;
      out.warning = "House system not implemented yet";
      break;
    }
  }

  return out;
}

} // namespace astrocore
