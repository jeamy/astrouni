#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdio>
#include "astrocore/planets.hpp"

namespace fs = std::filesystem;
using namespace astrocore;

static void write_text_file(const fs::path& p, const std::string& contents) {
  std::ofstream ofs(p);
  ASSERT_TRUE(ofs.is_open()) << "Failed to open file for write: " << p;
  ofs << contents;
}

static fs::path create_synthetic_ephemeris_extended(const fs::path& dir, double jd0, int days) {
  fs::create_directories(dir);
  const std::string names =
      "0|Sun|Su|Sun\n"
      "1|Moon|Mo|Moon\n"
      "2|Mercury|Me|Mercury\n";
  write_text_file(dir / "astronam.dat", names);

  auto fmt_line = [](double jd, double sunL, double moonL, double mercL) {
    char buf[512];
    std::snprintf(buf, sizeof(buf),
                  "%.6f %.6f 0 1 %.6f 0 1 %.6f 0 1\n",
                  jd, sunL, moonL, mercL);
    return std::string(buf);
  };

  std::string eph;
  for (int d = 0; d <= days; ++d) {
    double jd = jd0 + d;
    double sunL = std::fmod(0.0 + 1.0 * d, 360.0);
    if (sunL < 0) sunL += 360.0;
    double moonL = std::fmod(350.0 + 13.0 * d, 360.0);
    if (moonL < 0) moonL += 360.0;
    double mercL = std::fmod(10.0 + (d <= 1 ? d : (d == 2 ? 0.0 : -1.0 * (d - 2))), 360.0); // not used here
    eph += fmt_line(jd, sunL, moonL, mercL);
  }
  write_text_file(dir / "astroeph.dat", eph);

  return dir;
}

static double norm_deg(double a) {
  double x = std::fmod(a, 360.0);
  if (x < 0) x += 360.0;
  return x;
}

static double ang_diff(double to, double from) {
  double d = std::fmod(to - from, 360.0);
  if (d <= -180.0) d += 360.0;
  else if (d > 180.0) d -= 360.0;
  return d;
}

static fs::path create_synthetic_ephemeris(const fs::path& dir, double jd0) {
  fs::create_directories(dir);
  // Names file with minimal entries for Sun(0), Moon(1), Mercury(2)
  const std::string names =
      "0|Sun|Su|Sun\n"
      "1|Moon|Mo|Moon\n"
      "2|Mercury|Me|Mercury\n";
  write_text_file(dir / "astronam.dat", names);

  // Ephemeris file: 4 daily records
  // Sun: L = 0 + 1*d
  // Moon: L = 350 + 13*d (mod 360)
  // Mercury: L = 10, 11, 10, 9 (station at jd0+1.5)
  auto fmt_line = [](double jd, double sunL, double moonL, double mercL) {
    char buf[512];
    // order: jd, Sun(lon,lat,dist), Moon(lon,lat,dist), Mercury(lon,lat,dist)
    std::snprintf(buf, sizeof(buf),
                  "%.6f %.6f 0 1 %.6f 0 1 %.6f 0 1\n",
                  jd, sunL, moonL, mercL);
    return std::string(buf);
  };

  std::string eph;
  eph += fmt_line(jd0 + 0, 0.0, 350.0, 10.0);
  eph += fmt_line(jd0 + 1, 1.0,   3.0, 11.0);
  eph += fmt_line(jd0 + 2, 2.0,  16.0, 10.0);
  eph += fmt_line(jd0 + 3, 3.0,  29.0,  9.0);
  write_text_file(dir / "astroeph.dat", eph);

  return dir;
}

TEST(PlanetsEphemeris, LunarPhasesSynthetic) {
  const double jd0 = 2460000.0;
  fs::path tmp = fs::temp_directory_path() / "astro_ephem_synth_lunar";
  auto dir = create_synthetic_ephemeris(tmp, jd0);
  ASSERT_TRUE(initialize_ephemeris(dir.string()));
  ASSERT_TRUE(is_ephemeris_available());

  auto evOpt = find_next_lunar_phase(jd0, LunarPhase::NewMoon);
  ASSERT_TRUE(evOpt.has_value());
  auto ev = *evOpt;
  EXPECT_TRUE(ev.valid);
  // Expected new moon when elongation crosses 0: E = -10 + 12*d -> d = 10/12
  double expected = jd0 + (10.0/12.0);
  EXPECT_NEAR(ev.jd, expected, 2e-3); // ~3 minutes tolerance
}

TEST(PlanetsEphemeris, StationSyntheticMercury) {
  const double jd0 = 2460000.0;
  fs::path tmp = fs::temp_directory_path() / "astro_ephem_synth_station";
  auto dir = create_synthetic_ephemeris(tmp, jd0);
  ASSERT_TRUE(initialize_ephemeris(dir.string()));
  ASSERT_TRUE(is_ephemeris_available());

  // Approx near mid between day1 and day2 where slope flips
  auto stOpt = find_nearest_station(PlanetId::Mercury, jd0 + 1.5);
  ASSERT_TRUE(stOpt.has_value());
  auto st = *stOpt;
  EXPECT_TRUE(st.valid);
  EXPECT_NEAR(st.jd, jd0 + 1.0, 2e-3);
  EXPECT_TRUE(st.retrogradeStart); // + -> -
}

TEST(DerivedPoints, MeanNodesAndLilithBasics) {
  const double jd0 = 2460000.0;
  // Derived points don't require ephemeris
  CalcFlags flags; flags.calculateSpeed = true;

  auto nn = calculate_planet_position(PlanetId::NorthNode, jd0, flags);
  auto sn = calculate_planet_position(PlanetId::SouthNode, jd0, flags);
  auto li = calculate_planet_position(PlanetId::Lilith, jd0, flags);

  ASSERT_TRUE(nn.valid);
  ASSERT_TRUE(sn.valid);
  ASSERT_TRUE(li.valid);

  // South node opposite North node
  double diff = std::fabs(std::fabs(ang_diff(sn.longitude, nn.longitude)) - 180.0);
  EXPECT_LT(diff, 1e-6);

  // Mean node regresses slowly
  EXPECT_LT(nn.speed, 0.0);
  EXPECT_GT(nn.speed, -0.3);
  EXPECT_LT(nn.speed, -0.01);

  // Mean apogee progrades slowly
  EXPECT_GT(li.speed, 0.0);
  EXPECT_GT(li.speed, 0.05);
  EXPECT_LT(li.speed, 0.3);
}

TEST(Eclipses, SyntheticSolarAndLunar) {
  const double jd0 = 2460000.0;
  fs::path tmp = fs::temp_directory_path() / "astro_ephem_synth_eclipse";
  auto dir = create_synthetic_ephemeris_extended(tmp, jd0, 30);
  ASSERT_TRUE(initialize_ephemeris(dir.string()));
  ASSERT_TRUE(is_ephemeris_available());

  // Solar eclipse at nearest New Moon (lat=0 in synthetic data => central)
  auto se = find_next_solar_eclipse(jd0);
  ASSERT_TRUE(se.has_value());
  EXPECT_TRUE(se->valid);
  double expected_new = jd0 + (10.0/12.0);
  EXPECT_NEAR(se->jd, expected_new, 2e-3);
  EXPECT_TRUE(se->central);

  // Lunar eclipse at nearest Full Moon
  auto le = find_next_lunar_eclipse(jd0);
  ASSERT_TRUE(le.has_value());
  EXPECT_TRUE(le->valid);
  double expected_full = jd0 + (190.0/12.0);
  EXPECT_NEAR(le->jd, expected_full, 2e-3);
  EXPECT_TRUE(le->central);
}
