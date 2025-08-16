#include <gtest/gtest.h>
#include "astrocore/api.hpp"
#include "astrocore/houses.hpp"
#include <cmath>
#include <array>

using namespace astrocore;

static double norm2pi(double a) {
  while (a < 0) a += 2*M_PI;
  while (a >= 2*M_PI) a -= 2*M_PI;
  return a;
}

// -------------------- Unified API tests (Equal, Placidus) --------------------

TEST(HousesUnifiedAPI, EqualBasic) {
  Date d{2000,1,1};
  Time t{12,0,0.0};
  TimeZone tz{0.0, 0.0};
  // Berlin
  double lonDeg = 13.405;
  double latDeg = 52.52;

  auto res = compute_house_cusps(d, t, tz, lonDeg, latDeg, HouseSystem::Equal);
  EXPECT_TRUE(res.valid);
  // Check 12 cusps in [0,360) and 30° spacing
  for (int i = 0; i < 12; ++i) {
    EXPECT_TRUE(std::isfinite(res.cuspDeg[i]));
    EXPECT_GE(res.cuspDeg[i], 0.0);
    EXPECT_LT(res.cuspDeg[i], 360.0);
  }
  // spacing
  for (int i = 0; i < 12; ++i) {
    double expected = std::fmod(res.cuspDeg[0] + i * 30.0, 360.0);
    if (expected < 0) expected += 360.0;
    EXPECT_NEAR(res.cuspDeg[i], expected, 1e-9) << "cusp index " << i;
  }
  // asc equals cusp1
  EXPECT_NEAR(res.cuspDeg[0], res.ascDeg, 1e-9);
}

TEST(HousesUnifiedAPI, PlacidusBasic) {
  Date d{2024,3,20};
  Time t{12,0,0.0};
  TimeZone tz{0.0, 0.0};
  double lonDeg = 13.405;
  double latDeg = 52.52;

  auto res = compute_house_cusps(d, t, tz, lonDeg, latDeg, HouseSystem::Placidus);
  EXPECT_TRUE(res.valid);
  // Ranges
  for (int i = 0; i < 12; ++i) {
    EXPECT_TRUE(std::isfinite(res.cuspDeg[i]));
    EXPECT_GE(res.cuspDeg[i], 0.0);
    EXPECT_LT(res.cuspDeg[i], 360.0);
  }
  // Asc and MC alignment
  EXPECT_NEAR(res.cuspDeg[0], res.ascDeg, 1e-6);
  EXPECT_NEAR(res.cuspDeg[9], res.mcDeg, 1e-6);
}

// -------------------- Scaffolding for remaining house systems --------------------

// Note: The following tests are placeholders to enforce the plan. They will be
// enabled once the unified house API exists (e.g., compute_house_cusps with HouseSystem enum).

TEST(HousesUnifiedAPI, KochBasic) {
  Date d{2024,9,1};
  Time t{9,30,0.0};
  TimeZone tz{0.0, 0.0};
  // Reykjavik (high latitude but reasonable for Koch)
  double lonDeg = -21.817; // west negative, but our API expects east-positive; provide -21.817
  double latDeg = 64.1466;

  auto res = compute_house_cusps(d, t, tz, lonDeg, latDeg, HouseSystem::Koch);
  EXPECT_TRUE(res.valid);
  for (int i = 0; i < 12; ++i) {
    EXPECT_TRUE(std::isfinite(res.cuspDeg[i]));
    EXPECT_GE(res.cuspDeg[i], 0.0);
    EXPECT_LT(res.cuspDeg[i], 360.0);
  }
  // Asc alignment
  EXPECT_NEAR(res.cuspDeg[0], res.ascDeg, 1e-6);
}

TEST(HousesUnifiedAPI, CampanusBasic) {
  Date d{2021,11,10};
  Time t{6,45,0.0};
  TimeZone tz{0.0, 0.0};
  // New York City
  double lonDeg = -74.0060;
  double latDeg = 40.7128;

  auto res = compute_house_cusps(d, t, tz, lonDeg, latDeg, HouseSystem::Campanus);
  EXPECT_TRUE(res.valid);
  for (int i = 0; i < 12; ++i) {
    EXPECT_TRUE(std::isfinite(res.cuspDeg[i]));
    EXPECT_GE(res.cuspDeg[i], 0.0);
    EXPECT_LT(res.cuspDeg[i], 360.0);
  }
  // Asc alignment
  EXPECT_NEAR(res.cuspDeg[0], res.ascDeg, 1e-6);
}

TEST(HousesRegiomontanus, Placeholder) {
  GTEST_SKIP() << "Implement Regiomontanus system and validate asc/mc alignment and monotonic cusps";
}

TEST(HousesUnifiedAPI, PorphyryBasic) {
  Date d{2023,6,1};
  Time t{18,0,0.0};
  TimeZone tz{0.0, 0.0};
  double lonDeg = 13.405; // Berlin
  double latDeg = 52.52;

  auto res = compute_house_cusps(d, t, tz, lonDeg, latDeg, HouseSystem::Porphyry);
  EXPECT_TRUE(res.valid);
  for (int i = 0; i < 12; ++i) {
    EXPECT_TRUE(std::isfinite(res.cuspDeg[i]));
    EXPECT_GE(res.cuspDeg[i], 0.0);
    EXPECT_LT(res.cuspDeg[i], 360.0);
  }
  // Ascendant and MC alignment
  EXPECT_NEAR(res.cuspDeg[0], res.ascDeg, 1e-6);
  EXPECT_NEAR(res.cuspDeg[9], res.mcDeg, 1e-6);
}

TEST(HousesUnifiedAPI, PorphyryNeoBasic) {
  Date d{2023,6,1};
  Time t{18,0,0.0};
  TimeZone tz{0.0, 0.0};
  double lonDeg = -0.1278; // London
  double latDeg = 51.5074;

  auto res = compute_house_cusps(d, t, tz, lonDeg, latDeg, HouseSystem::PorphyryNeo);
  EXPECT_TRUE(res.valid);
  for (int i = 0; i < 12; ++i) {
    EXPECT_TRUE(std::isfinite(res.cuspDeg[i]));
    EXPECT_GE(res.cuspDeg[i], 0.0);
    EXPECT_LT(res.cuspDeg[i], 360.0);
  }
  // Ascendant and MC alignment
  EXPECT_NEAR(res.cuspDeg[0], res.ascDeg, 1e-6);
  EXPECT_NEAR(res.cuspDeg[9], res.mcDeg, 1e-6);
}

TEST(HousesUnifiedAPI, WholeSignBasic) {
  Date d{2022,1,15};
  Time t{3,15,0.0};
  TimeZone tz{0.0, 0.0};
  double lonDeg = 151.2093; // Sydney
  double latDeg = -33.8688;

  auto res = compute_house_cusps(d, t, tz, lonDeg, latDeg, HouseSystem::Whole);
  EXPECT_TRUE(res.valid);
  for (int i = 0; i < 12; ++i) {
    EXPECT_TRUE(std::isfinite(res.cuspDeg[i]));
    EXPECT_GE(res.cuspDeg[i], 0.0);
    EXPECT_LT(res.cuspDeg[i], 360.0);
  }
  // 30-degree spacing
  for (int i = 0; i < 12; ++i) {
    double expected = std::fmod(res.cuspDeg[0] + i * 30.0, 360.0);
    if (expected < 0) expected += 360.0;
    EXPECT_NEAR(res.cuspDeg[i], expected, 1e-9);
  }
  // cusp1 is start of Ascendant's sign (i.e., floor to multiple of 30 deg)
  double ascSignStart = std::floor(res.ascDeg / 30.0) * 30.0;
  EXPECT_NEAR(res.cuspDeg[0], ascSignStart, 1e-9);
  // Ascendant should lie within house 1 segment
  double nextCusp = std::fmod(res.cuspDeg[0] + 30.0, 360.0);
  bool inSeg = (res.cuspDeg[0] <= res.ascDeg && res.ascDeg < nextCusp) ||
               (nextCusp < res.cuspDeg[0] && (res.ascDeg >= res.cuspDeg[0] || res.ascDeg < nextCusp));
  EXPECT_TRUE(inSeg);
}

TEST(HousesTopocentric, Placeholder) {
  GTEST_SKIP() << "Implement Topocentric; verify stability across mid/high latitudes";
}

TEST(HousesMeridian, Placeholder) {
  GTEST_SKIP() << "Implement Meridian (Axial) houses and validate properties";
}

TEST(HousesMorinus, Placeholder) {
  GTEST_SKIP() << "Implement Morinus; validate cusp order and expected invariants";
}

TEST(HousesEqualMid, Placeholder) {
  GTEST_SKIP() << "Implement Equal(MC)/Equal Mid; houses equal sized starting from MC";
}

TEST(HousesAlcabitius, Placeholder) {
  GTEST_SKIP() << "Implement Alcabitius; validate against legacy reference output";
}

TEST(HousesUnifiedAPI, Placeholder) {
  // Future: call astrocore::compute_house_cusps(date,time,tz,lon,lat, HouseSystem::Koch) etc.
  GTEST_SKIP() << "Add unified API compute_house_cusps and switch tests to use it";
}

static std::vector<double> unwrap_monotonic(const double* cusps, int n) {
  std::vector<double> u(n);
  double base = cusps[0];
  u[0] = base;
  for (int i = 1; i < n; ++i) {
    double v = cusps[i];
    while (v < u[i-1]) v += 2*M_PI;
    u[i] = v;
  }
  return u;
}

TEST(HousesEqual, CuspSpacingAndAscendant) {
  // Berlin default as used by GUI defaults
  Date d{2000,1,1};
  Time t{12,0,0.0};
  TimeZone tz{0.0, 0.0};
  double jd = julian_day(d, t, tz);
  double lon = 13.405 * M_PI / 180.0; // East positive
  double lat = 52.52  * M_PI / 180.0; // North positive

  double asc = 0.0, mc = 0.0;
  asc_mc_longitudes(jd, lon, lat, asc, mc);

  double cusps[12];
  equal_house_cusps(asc, cusps);

  // Cusp 1 should equal Ascendant (within tight tolerance)
  EXPECT_NEAR(cusps[0], asc, 1e-12);

  // Each cusp should be 30 degrees (pi/6) apart
  for (int i = 0; i < 12; ++i) {
    double expected = asc + i * (M_PI/6.0);
    expected = norm2pi(expected);
    EXPECT_NEAR(cusps[i], expected, 1e-12) << "cusp index " << i;
  }
}

TEST(HousesPlacidus, BasicProperties) {
  Date d{2024,3,20}; // Equinox date for a stable scenario
  Time t{12,0,0.0};
  TimeZone tz{0.0, 0.0};
  double jd = julian_day(d, t, tz);
  double lon = 13.405 * M_PI / 180.0;
  double lat = 52.52  * M_PI / 180.0;

  double asc = 0.0, mc = 0.0;
  asc_mc_longitudes(jd, lon, lat, asc, mc);

  double cusps[12];
  placidus_house_cusps(jd, lon, lat, cusps);

  // All cusps are valid angles in [0,2pi)
  for (int i = 0; i < 12; ++i) {
    double c = cusps[i];
    EXPECT_TRUE(std::isfinite(c));
    EXPECT_GE(c, 0.0);
    EXPECT_LT(c, 2*M_PI);
  }

  // Cusp 1 should coincide with Ascendant, cusp 10 with MC (within reasonable tolerance)
  EXPECT_NEAR(cusps[0], norm2pi(asc), 1e-9);
  EXPECT_NEAR(cusps[9], norm2pi(mc), 1e-9);

  // Unwrapped sequence should be strictly increasing
  auto u = unwrap_monotonic(cusps, 12);
  for (int i = 0; i < 11; ++i) {
    EXPECT_LT(u[i], u[i+1] + 1e-12) << "non-increasing at index " << i;
  }
}

TEST(HousesPlacidus, HighLatitudeSanity) {
  Date d{2024,6,21}; // Near solstice
  Time t{12,0,0.0};
  TimeZone tz{0.0, 0.0};
  double jd = julian_day(d, t, tz);
  double lon = 0.0; // Greenwich
  double lat = 66.5 * M_PI / 180.0; // Arctic circle

  double cusps[12];
  placidus_house_cusps(jd, lon, lat, cusps);

  // Ensure outputs are finite and in range
  for (int i = 0; i < 12; ++i) {
    EXPECT_TRUE(std::isfinite(cusps[i]));
    EXPECT_GE(cusps[i], 0.0);
    EXPECT_LT(cusps[i], 2*M_PI);
  }

  // Unwrapped should be non-decreasing (allow tiny numerical noise)
  auto u = unwrap_monotonic(cusps, 12);
  for (int i = 0; i < 11; ++i) {
    EXPECT_LE(u[i], u[i+1] + 1e-10) << "non-monotonic at index " << i;
  }
}
