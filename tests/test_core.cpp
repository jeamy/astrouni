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
