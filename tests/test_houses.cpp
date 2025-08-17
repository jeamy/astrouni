#include <gtest/gtest.h>
#include "astrocore/api.hpp"
#include "astrocore/houses.hpp"
#include <cmath>
#include <array>
#include <regex>

using namespace astrocore;

// Forward declaration for helper defined later in this file
static void expect_range_and_finite(const HouseCusps& res);

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

TEST(HousesUnifiedAPI, RegiomontanusBasic) {
  Date d{2022,5,5};
  Time t{14,15,0.0};
  TimeZone tz{0.0, 0.0};
  // Chicago
  double lonDeg = -87.6298;
  double latDeg = 41.8781;

  auto res = compute_house_cusps(d, t, tz, lonDeg, latDeg, HouseSystem::Regiomontanus);
  EXPECT_TRUE(res.valid);
  for (int i = 0; i < 12; ++i) {
    EXPECT_TRUE(std::isfinite(res.cuspDeg[i]));
    EXPECT_GE(res.cuspDeg[i], 0.0);
    EXPECT_LT(res.cuspDeg[i], 360.0);
  }
  // Asc alignment
  EXPECT_NEAR(res.cuspDeg[0], res.ascDeg, 1e-6);
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

TEST(HousesUnifiedAPI, TopocentricBasic) {
  Date d{2023,3,21};
  Time t{10,0,0.0};
  TimeZone tz{0.0, 0.0};
  // Madrid
  double lonDeg = -3.7038;
  double latDeg = 40.4168;

  auto res = compute_house_cusps(d, t, tz, lonDeg, latDeg, HouseSystem::Topocentric);
  EXPECT_TRUE(res.valid);
  for (int i = 0; i < 12; ++i) {
    EXPECT_TRUE(std::isfinite(res.cuspDeg[i]));
    EXPECT_GE(res.cuspDeg[i], 0.0);
    EXPECT_LT(res.cuspDeg[i], 360.0);
  }
  EXPECT_NEAR(res.cuspDeg[0], res.ascDeg, 1e-6);
}

TEST(HousesUnifiedAPI, MeridianBasic) {
  Date d{2022,9,23};
  Time t{12,0,0.0};
  TimeZone tz{0.0, 0.0};
  // Rome
  double lonDeg = 12.4964;
  double latDeg = 41.9028;

  auto res = compute_house_cusps(d, t, tz, lonDeg, latDeg, HouseSystem::Meridian);
  EXPECT_TRUE(res.valid);
  for (int i = 0; i < 12; ++i) {
    EXPECT_TRUE(std::isfinite(res.cuspDeg[i]));
    EXPECT_GE(res.cuspDeg[i], 0.0);
    EXPECT_LT(res.cuspDeg[i], 360.0);
  }
  EXPECT_NEAR(res.cuspDeg[0], res.ascDeg, 1e-6);
}

TEST(HousesUnifiedAPI, MorinusBasic) {
  Date d{2021,12,1};
  Time t{8,30,0.0};
  TimeZone tz{0.0, 0.0};
  // Tokyo
  double lonDeg = 139.6917;
  double latDeg = 35.6895;

  auto res = compute_house_cusps(d, t, tz, lonDeg, latDeg, HouseSystem::Morinus);
  EXPECT_TRUE(res.valid);
  for (int i = 0; i < 12; ++i) {
    EXPECT_TRUE(std::isfinite(res.cuspDeg[i]));
    EXPECT_GE(res.cuspDeg[i], 0.0);
    EXPECT_LT(res.cuspDeg[i], 360.0);
  }
  EXPECT_NEAR(res.cuspDeg[0], res.ascDeg, 1e-6);
}

TEST(HousesUnifiedAPI, EqualMidBasic) {
  Date d{2020,7,1};
  Time t{16,0,0.0};
  TimeZone tz{0.0, 0.0};
  // Cape Town
  double lonDeg = 18.4241;
  double latDeg = -33.9249;

  auto res = compute_house_cusps(d, t, tz, lonDeg, latDeg, HouseSystem::EqualMid);
  EXPECT_TRUE(res.valid);
  for (int i = 0; i < 12; ++i) {
    EXPECT_TRUE(std::isfinite(res.cuspDeg[i]));
    EXPECT_GE(res.cuspDeg[i], 0.0);
    EXPECT_LT(res.cuspDeg[i], 360.0);
  }
  // Equal from MC: cusp10 equals MC, 30-degree spacing
  EXPECT_NEAR(res.cuspDeg[9], res.mcDeg, 1e-6);
  for (int i = 0; i < 12; ++i) {
    double expected = std::fmod(res.cuspDeg[9] - 270.0 + i * 30.0, 360.0);
    if (expected < 0) expected += 360.0;
    EXPECT_NEAR(res.cuspDeg[i], expected, 1e-6);
  }
}

TEST(HousesHighLatitudeWarnings, WarningMessageContentRegexPlacidus) {
  Date d{2024,3,20};
  Time t{12,0,0.0};
  TimeZone tz{0.0, 0.0};
  double lonDeg = 0.0; // Greenwich

  double jd = julian_day(d, t, tz);
  double limitDeg = 90.0 - (mean_obliquity_rad(jd) * 180.0 / M_PI);
  double latDeg = limitDeg + 3.0; // above threshold
  if (latDeg > 89.0) latDeg = 89.0;

  auto res = compute_house_cusps(d, t, tz, lonDeg, latDeg, HouseSystem::Placidus);
  expect_range_and_finite(res);
  ASSERT_FALSE(res.warning.empty());

  // Expect warning to mention high-latitude caution, the system name, polar circles, and advisory
  std::regex rx("^High-latitude caution:\\s*Placidus.*polar circles.*Consider Equal/Whole/Meridian/Porphyry-based systems\\.$");
  EXPECT_TRUE(std::regex_search(res.warning, rx)) << "warning='" << res.warning << "'";
}

// -------------------- High-latitude warnings --------------------

TEST(HousesHighLatitudeWarnings, TimeArcSystemsAboveLimit) {
  Date d{2024,3,20};
  Time t{12,0,0.0};
  TimeZone tz{0.0, 0.0};
  double lonDeg = 0.0; // Greenwich

  double jd = julian_day(d, t, tz);
  double limitDeg = 90.0 - (mean_obliquity_rad(jd) * 180.0 / M_PI);
  double latDeg = limitDeg + 3.0; // above threshold
  if (latDeg > 89.0) latDeg = 89.0; // avoid pathological pole

  std::vector<HouseSystem> warnSystems = {
    HouseSystem::Placidus,
    HouseSystem::Koch,
    HouseSystem::Topocentric
  };

  for (auto sys : warnSystems) {
    auto res = compute_house_cusps(d, t, tz, lonDeg, latDeg, sys);
    expect_range_and_finite(res);
    EXPECT_FALSE(res.warning.empty()) << "sys=" << static_cast<int>(sys) << ", lat=" << latDeg;
  }
}

TEST(HousesHighLatitudeWarnings, NoWarningsBelowLimitAndRobustSystems) {
  Date d{2024,3,20};
  Time t{12,0,0.0};
  TimeZone tz{0.0, 0.0};
  double lonDeg = 0.0;

  double jd = julian_day(d, t, tz);
  double limitDeg = 90.0 - (mean_obliquity_rad(jd) * 180.0 / M_PI);
  double latBelow = limitDeg - 3.0; // below threshold
  if (latBelow < 0.0) latBelow = 0.0;

  // Time/semi-arc systems: no warning below threshold
  for (auto sys : {HouseSystem::Placidus, HouseSystem::Koch, HouseSystem::Topocentric}) {
    auto res = compute_house_cusps(d, t, tz, lonDeg, latBelow, sys);
    expect_range_and_finite(res);
    EXPECT_TRUE(res.warning.empty()) << "sys=" << static_cast<int>(sys) << ", lat=" << latBelow;
  }

  // Robust systems: no warnings even above threshold
  double latAbove = limitDeg + 3.0;
  if (latAbove > 89.0) latAbove = 89.0;
  std::vector<HouseSystem> robustSystems = {
    HouseSystem::Equal,
    HouseSystem::Whole,
    HouseSystem::Meridian,
    HouseSystem::Porphyry,
    HouseSystem::PorphyryNeo,
    HouseSystem::Regiomontanus,
    HouseSystem::Campanus,
    HouseSystem::Morinus,
    HouseSystem::EqualMid,
    HouseSystem::Alcabitius
  };
  for (auto sys : robustSystems) {
    auto res = compute_house_cusps(d, t, tz, lonDeg, latAbove, sys);
    expect_range_and_finite(res);
    EXPECT_TRUE(res.warning.empty()) << "sys=" << static_cast<int>(sys) << ", lat=" << latAbove;
  }
}

TEST(HousesUnifiedAPI, AlcabitiusBasic) {
  Date d{2024,3,20};
  Time t{12,0,0.0};
  TimeZone tz{0.0, 0.0};
  // Berlin
  double lonDeg = 13.405;
  double latDeg = 52.52;

  auto res = compute_house_cusps(d, t, tz, lonDeg, latDeg, HouseSystem::Alcabitius);
  EXPECT_TRUE(res.valid);
  for (int i = 0; i < 12; ++i) {
    EXPECT_TRUE(std::isfinite(res.cuspDeg[i]));
    EXPECT_GE(res.cuspDeg[i], 0.0);
    EXPECT_LT(res.cuspDeg[i], 360.0);
  }
  // Alignment checks
  EXPECT_NEAR(res.cuspDeg[0], res.ascDeg, 1e-6);
  EXPECT_NEAR(res.cuspDeg[9], res.mcDeg, 1e-6);
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

// -------------------- Parameterized, multi-latitude validations --------------------

static const double kLatitudesDeg[] = { 0.0, 45.0, -45.0, 60.0, -60.0, 66.5, -66.5, 70.0, -70.0 };

static double wrap_deg(double d) {
  while (d < 0.0) d += 360.0;
  while (d >= 360.0) d -= 360.0;
  return d;
}

static double mindeg(double a, double b) {
  double d = std::fabs(a - b);
  return d <= 180.0 ? d : 360.0 - d;
}

static void expect_range_and_finite(const HouseCusps& res) {
  EXPECT_TRUE(res.valid);
  for (int i = 0; i < 12; ++i) {
    EXPECT_TRUE(std::isfinite(res.cuspDeg[i]));
    EXPECT_GE(res.cuspDeg[i], 0.0);
    EXPECT_LT(res.cuspDeg[i], 360.0);
  }
  EXPECT_TRUE(std::isfinite(res.ascDeg));
  EXPECT_TRUE(std::isfinite(res.mcDeg));
}

TEST(HousesParameterized, AscAlignmentMultiLat) {
  Date d{2024,3,20};
  Time t{12,0,0.0};
  TimeZone tz{0.0, 0.0};
  double lonDeg = 0.0; // Greenwich

  std::vector<HouseSystem> systems = {
    HouseSystem::Equal,
    HouseSystem::Placidus,
    HouseSystem::Koch,
    HouseSystem::Campanus,
    HouseSystem::Regiomontanus,
    HouseSystem::Porphyry,
    HouseSystem::PorphyryNeo,
    HouseSystem::Topocentric,
    HouseSystem::Meridian,
    HouseSystem::Morinus,
    HouseSystem::Alcabitius
  };

  for (double latDeg : kLatitudesDeg) {
    for (auto sys : systems) {
      auto res = compute_house_cusps(d, t, tz, lonDeg, latDeg, sys);
      expect_range_and_finite(res);
      EXPECT_NEAR(res.cuspDeg[0], res.ascDeg, 1e-6) << "lat=" << latDeg << ", sys=" << static_cast<int>(sys);
    }
  }
}

TEST(HousesParameterized, MCAlignmentMultiLat) {
  Date d{2024,3,20};
  Time t{12,0,0.0};
  TimeZone tz{0.0, 0.0};
  double lonDeg = 0.0;

  std::vector<HouseSystem> systems = {
    HouseSystem::Placidus,
    HouseSystem::Porphyry,
    HouseSystem::PorphyryNeo,
    HouseSystem::EqualMid,
    HouseSystem::Alcabitius
  };

  for (double latDeg : kLatitudesDeg) {
    for (auto sys : systems) {
      auto res = compute_house_cusps(d, t, tz, lonDeg, latDeg, sys);
      expect_range_and_finite(res);
      EXPECT_NEAR(res.cuspDeg[9], res.mcDeg, 1e-6) << "lat=" << latDeg << ", sys=" << static_cast<int>(sys);
    }
  }
}

TEST(HousesParameterized, OppositePairsExactForSymmetricSystems) {
  Date d{2024,3,20};
  Time t{12,0,0.0};
  TimeZone tz{0.0, 0.0};
  double lonDeg = 0.0;

  std::vector<HouseSystem> systems = {
    HouseSystem::Equal,
    HouseSystem::Porphyry,
    HouseSystem::PorphyryNeo,
    HouseSystem::Whole,
    HouseSystem::Topocentric,
    HouseSystem::EqualMid,
    HouseSystem::Alcabitius
  };

  for (double latDeg : kLatitudesDeg) {
    for (auto sys : systems) {
      auto res = compute_house_cusps(d, t, tz, lonDeg, latDeg, sys);
      expect_range_and_finite(res);
      for (int i = 0; i < 6; ++i) {
        double expected = wrap_deg(res.cuspDeg[i] + 180.0);
        EXPECT_LE(mindeg(res.cuspDeg[i+6], expected), 1e-6)
          << "pair=" << i << ", lat=" << latDeg << ", sys=" << static_cast<int>(sys);
      }
    }
  }
}

TEST(HousesParameterized, WholeSignSpacingAndContainmentMultiLat) {
  Date d{2024,3,20};
  Time t{12,0,0.0};
  TimeZone tz{0.0, 0.0};
  double lonDeg = 0.0;

  for (double latDeg : kLatitudesDeg) {
    auto res = compute_house_cusps(d, t, tz, lonDeg, latDeg, HouseSystem::Whole);
    expect_range_and_finite(res);
    // 30-degree spacing
    for (int i = 0; i < 12; ++i) {
      double expected = std::fmod(res.cuspDeg[0] + i * 30.0, 360.0);
      if (expected < 0) expected += 360.0;
      EXPECT_NEAR(res.cuspDeg[i], expected, 1e-6) << "idx=" << i << ", lat=" << latDeg;
    }
    // cusp1 is start of Ascendant's sign
    double ascSignStart = std::floor(res.ascDeg / 30.0) * 30.0;
    EXPECT_NEAR(res.cuspDeg[0], ascSignStart, 1e-6) << "lat=" << latDeg;
    // Ascendant lies within house 1 segment
    double nextCusp = std::fmod(res.cuspDeg[0] + 30.0, 360.0);
    if (nextCusp < 0) nextCusp += 360.0;
    bool inSeg = (res.cuspDeg[0] <= res.ascDeg && res.ascDeg < nextCusp) ||
                 (nextCusp < res.cuspDeg[0] && (res.ascDeg >= res.cuspDeg[0] || res.ascDeg < nextCusp));
    EXPECT_TRUE(inSeg) << "lat=" << latDeg;
  }
}
