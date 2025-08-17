#include <gtest/gtest.h>
#include "astrocore/api.hpp"
#include <cmath>

using namespace astrocore;

TEST(JulianDayTest, J2000NoonUTC) {
  Date d{2000,1,1};
  Time t{12,0,0.0};
  TimeZone tz{0.0, 0.0};
  double jd = julian_day(d, t, tz);
  EXPECT_NEAR(jd, 2451545.0, 1e-9);
}

TEST(DeclinationAspectTest, ZeroAndBoundaryCases) {
  // Zero with positive small declination counts as Parallel
  {
    auto r = detect_declination_aspect(0.0, +0.2, 0.3);
    EXPECT_EQ(r.type, DeclAspectType::Parallel);
    EXPECT_NEAR(r.deltaDeg, +0.2, 1e-12);
  }
  // Zero with negative small declination also counts as Parallel (equator treated neutral)
  {
    auto r = detect_declination_aspect(0.0, -0.2, 0.3);
    EXPECT_EQ(r.type, DeclAspectType::Parallel);
    EXPECT_NEAR(r.deltaDeg, -0.2, 1e-12);
  }
  // Exact boundary for ContraParallel (|δ2|-|δ1| == 0 within orb 0)
  {
    auto r = detect_declination_aspect(+10.0, -10.0, 0.0);
    EXPECT_EQ(r.type, DeclAspectType::ContraParallel);
    EXPECT_NEAR(r.deltaDeg, 0.0, 1e-12);
    EXPECT_NEAR(r.orbDeg, 0.0, 1e-12);
  }
  // None when just outside orb
  {
    auto r = detect_declination_aspect(+1.0, -1.6, 0.5);
    EXPECT_EQ(r.type, DeclAspectType::None);
  }
}

TEST(DeclinationAspectTest, ParallelsAndContraParallels) {
  // Parallel: same hemisphere, within orb
  {
    auto r = detect_declination_aspect(+10.0, +10.5, 0.6);
    EXPECT_EQ(r.type, DeclAspectType::Parallel);
    EXPECT_NEAR(r.deltaDeg, +0.5, 1e-12);
    EXPECT_NEAR(r.orbDeg, 0.5, 1e-12);
  }
  // Parallel: southern hemisphere, small difference
  {
    auto r = detect_declination_aspect(-5.2, -5.1, 0.15);
    EXPECT_EQ(r.type, DeclAspectType::Parallel);
    EXPECT_NEAR(r.deltaDeg, +0.1, 1e-12);
  }
  // Contra-parallel: opposite hemispheres, magnitudes near equal
  {
    auto r = detect_declination_aspect(+12.0, -11.6, 0.5);
    EXPECT_EQ(r.type, DeclAspectType::ContraParallel);
    EXPECT_NEAR(r.deltaDeg, -0.4, 1e-12); // |dec2|-|dec1| = 11.6-12.0 = -0.4
  }
  // None: outside orb
  {
    auto r = detect_declination_aspect(+8.0, +9.2, 0.5);
    EXPECT_EQ(r.type, DeclAspectType::None);
  }
}

TEST(AspectSpeedTest, ApplyingSeparatingClassification) {
  // Near trine (120°): pre-exact (118°), applying when relative separation increasing (relRate>0)
  {
    double lon1 = 0.0, lon2 = 118.0; // d=118, delta=-2 (pre-exact)
    double s1 = 0.5, s2 = 1.2;       // speed2 - speed1 > 0 => relRate > 0 (since deltaWrap>0)
    auto r = detect_aspect_with_speeds(lon1, s1, lon2, s2, 3.0);
    ASSERT_NE(r.type, AspectType::None);
    EXPECT_EQ(r.type, AspectType::Trine);
    EXPECT_LT(r.deltaDeg, 0.0);
    EXPECT_TRUE(r.applying);
  }
  // Near trine (120°): post-exact (123°), separating when relative separation increasing (relRate>0)
  {
    double lon1 = 0.0, lon2 = 123.0; // d=123, delta=+3 (post-exact)
    double s1 = 1.0, s2 = 1.5;       // relRate > 0, so with delta>0 => separating
    auto r = detect_aspect_with_speeds(lon1, s1, lon2, s2, 5.0);
    ASSERT_NE(r.type, AspectType::None);
    EXPECT_EQ(r.type, AspectType::Trine);
    EXPECT_GT(r.deltaDeg, 0.0);
    EXPECT_FALSE(r.applying);
  }
}

TEST(DetectAspectTest, MajorAspectsWithinOrb) {
  // Conjunction 0° within orb
  {
    auto r = detect_aspect(15.0, 15.0, 3.0);
    EXPECT_EQ(r.type, AspectType::Conjunction);
    EXPECT_NEAR(r.exactAngleDeg, 0.0, 1e-9);
    EXPECT_NEAR(r.deltaDeg, 0.0, 1e-9);
  }
  // Sextile 60° within orb
  {
    auto r = detect_aspect(10.0, 70.5, 2.0);
    EXPECT_EQ(r.type, AspectType::Sextile);
    EXPECT_NEAR(r.exactAngleDeg, 60.0, 1e-9);
    EXPECT_NEAR(r.deltaDeg, 0.5, 1e-9);
  }
  // Square 90° within orb
  {
    auto r = detect_aspect(200.0, 289.0, 2.0);
    EXPECT_EQ(r.type, AspectType::Square);
    EXPECT_NEAR(r.exactAngleDeg, 90.0, 1e-9);
    EXPECT_NEAR(r.deltaDeg, -1.0, 1e-9);
  }
  // Trine 120° within orb with wrap-around
  {
    auto r = detect_aspect(350.0, 110.0, 3.0);
    EXPECT_EQ(r.type, AspectType::Trine);
    EXPECT_NEAR(r.exactAngleDeg, 120.0, 1e-9);
    EXPECT_NEAR(r.deltaDeg, 0.0, 1e-9); // wrapped distance is exactly 120°
  }
  // Opposition 180° within orb
  {
    auto r = detect_aspect(0.0, 178.0, 3.0);
    EXPECT_EQ(r.type, AspectType::Opposition);
    EXPECT_NEAR(r.exactAngleDeg, 180.0, 1e-9);
    EXPECT_NEAR(r.deltaDeg, -2.0, 1e-9);
  }
}

TEST(DetectAspectTest, NoneOutsideOrb) {
  auto r = detect_aspect(0.0, 50.0, 3.0);
  EXPECT_EQ(r.type, AspectType::None);
}

TEST(CoordinateHelpersTest, EqualHouseCuspsMonotonic) {
  // Build simple ascendant at 30° (pi/6)
  double asc = M_PI / 6.0;
  double cusps[12];
  equal_house_cusps(asc, cusps);
  // Each cusp 30° apart
  for (int i = 0; i < 12; ++i) {
    double expected = asc + i * (M_PI / 6.0);
    // Wrap expected to [0,2pi)
    while (expected < 0) expected += 2*M_PI;
    while (expected >= 2*M_PI) expected -= 2*M_PI;
    EXPECT_NEAR(cusps[i], expected, 1e-12) << "cusp index " << i;
  }
}

TEST(AspectPatternTest, GrandTrineDetection) {
  // Longitudes approximately 0°, 120.5°, 239.8° -> three trines within 1° orb
  std::vector<double> longs = {0.0, 120.5, 239.8};
  double orb = 1.0;
  auto pats = detect_aspect_patterns(longs, orb);
  ASSERT_FALSE(pats.empty());
  bool found = false;
  for (const auto& p : pats) {
    if (p.type == AspectPatternType::GrandTrine) {
      ASSERT_EQ(p.indices.size(), 3u);
      EXPECT_EQ(p.indices[0], 0);
      EXPECT_EQ(p.indices[1], 1);
      EXPECT_EQ(p.indices[2], 2);
      found = true;
    }
  }
  EXPECT_TRUE(found);
}

TEST(AspectPatternTest, TSquareDetection) {
  // Longitudes approximately 0°, 90.5°, 179.6° -> opposition + two squares within 1° orb
  std::vector<double> longs = {0.0, 90.5, 179.6};
  double orb = 1.0;
  auto pats = detect_aspect_patterns(longs, orb);
  ASSERT_FALSE(pats.empty());
  bool found = false;
  for (const auto& p : pats) {
    if (p.type == AspectPatternType::TSquare) {
      ASSERT_EQ(p.indices.size(), 3u);
      EXPECT_EQ(p.indices[0], 0);
      EXPECT_EQ(p.indices[1], 1);
      EXPECT_EQ(p.indices[2], 2);
      found = true;
    }
  }
  EXPECT_TRUE(found);
}
