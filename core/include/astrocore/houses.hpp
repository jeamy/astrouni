#pragma once

#include <array>
#include <string>
#include "astrocore/api.hpp"

namespace astrocore {

// Supported house systems
enum class HouseSystem {
  Equal,
  Placidus,
  Koch,
  Campanus,
  Regiomontanus,
  Porphyry,
  PorphyryNeo,
  Whole,
  Topocentric,
  Meridian,
  Morinus,
  EqualMid,
  Alcabitius
};

// Result container for house computations (angles in degrees 0..360)
struct HouseCusps {
  double ascDeg {0.0};
  double mcDeg {0.0};
  std::array<double, 12> cuspDeg { {0} };
  bool valid {false};
  std::string warning; // non-empty when valid == false or partial
};

// Compute house cusps for given civil date/time, timezone, and observer lon/lat (degrees).
// Longitudes east-positive, latitudes north-positive.
HouseCusps compute_house_cusps(const Date& d,
                               const Time& t,
                               const TimeZone& tz,
                               double longitudeDeg,
                               double latitudeDeg,
                               HouseSystem system);

} // namespace astrocore
