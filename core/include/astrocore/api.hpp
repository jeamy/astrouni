#pragma once

#include <string>
#include <cstdint>

namespace astrocore {

struct Version {
  int major;
  int minor;
  int patch;
};

// Returns the library version.
Version version();

// Placeholder for future calculation entry point. Will wrap legacy C code.
// For now, just returns a constant to prove wiring works.
int64_t demo_compute(int64_t x);

// Basic calendar/time structures
struct Date {
  int year;   // e.g., 2000
  int month;  // 1..12
  int day;    // 1..31
};

struct Time {
  int hour;     // 0..23
  int minute;   // 0..59
  double second; // 0..60
};

struct TimeZone {
  // Local time = UTC + utcOffsetHours + dstHours
  // Example: CET winter => utcOffsetHours = +1, dstHours = 0
  //          CEST summer => utcOffsetHours = +2, dstHours = 0 (or use utcOffsetHours=+1, dstHours=+1)
  double utcOffsetHours; // hours east of UTC are positive
  double dstHours;       // daylight saving offset in hours (usually 0 or +1)
};

// Compute the Julian Day number (fractional) for a local civil date/time and timezone.
// The time provided is local civil time; conversion to UTC is handled via utcOffsetHours and dstHours.
// Returns JD as a double.
double julian_day(const Date& d, const Time& t, const TimeZone& tz);

// Coordinate transform with pole shift (radians).
// Inputs/outputs are modified in place. All angles in radians.
void coord_transform(double& azimuth, double& altitude, double shift);

// --- Core astronomy helpers ---

// Julian centuries relative to J2000.0
double julian_century(double jd);

// Mean obliquity of the ecliptic in radians (IAU polynomial) for a given JD
double mean_obliquity_rad(double jd);

struct Ecliptic { double lon; double lat; };      // radians
struct Equatorial { double ra; double dec; };     // radians (ra in [0,2pi))
struct Horizontal { double az; double alt; };     // radians (az in [0,2pi), alt in [-pi/2, +pi/2])
struct Galactic { double lon; double lat; };      // radians (lon l in [0,2pi), lat b)

// Convert ecliptic -> equatorial using provided obliquity (radians)
Equatorial ecliptic_to_equatorial(const Ecliptic& ecl, double obliquityRad);

// Greenwich mean sidereal time and local sidereal time (radians; wrapped to [0,2pi))
double gmst_rad(double jd);
double lst_rad(double jd, double longitudeRad);

// --- Additional coordinate conversions ---
// Equatorial -> Horizontal for given observer latitude and local sidereal time (radians)
Horizontal equatorial_to_horizontal(const Equatorial& eq, double latitudeRad, double lstRad);
// Horizontal -> Equatorial for given observer latitude and local sidereal time (radians)
Equatorial horizontal_to_equatorial(const Horizontal& hor, double latitudeRad, double lstRad);

// Equatorial <-> Galactic (J2000) using standard rotation matrix
Galactic equatorial_to_galactic(const Equatorial& eq);
Equatorial galactic_to_equatorial(const Galactic& gal);

// Simple aspect detection among common major aspects
enum class AspectType { None, Conjunction, Opposition, Trine, Square, Sextile };
struct AspectResult {
  AspectType type;
  double exactAngleDeg;   // 0, 60, 90, 120, 180
  double deltaDeg;        // signed difference from exact, in degrees (-orb..+orb)
};

// Given two ecliptic longitudes (degrees, wrapped 0..360) and an orb in degrees,
// detect if they form a major aspect. Returns AspectResult with type None if not.
AspectResult detect_aspect(double lon1Deg, double lon2Deg, double orbDeg);

// Ascendant and Midheaven longitudes (radians, wrapped 0..2pi) for given JD and observer location
// longitudeRad: east positive, latitudeRad: geodetic latitude
void asc_mc_longitudes(double jd, double longitudeRad, double latitudeRad,
                       double& ascRad, double& mcRad);

// Equal-house cusps from Ascendant (radians). Fills 12-element array (house 1..12 at index 0..11).
void equal_house_cusps(double ascRad, double cuspsRad[12]);

// Placidus house cusps (radians). Placeholder implementation currently falls back to Equal.
// Inputs: jd (TT/UTC JD acceptable for our use), longitudeRad (east+), latitudeRad (geodetic)
void placidus_house_cusps(double jd, double longitudeRad, double latitudeRad, double cuspsRad[12]);

} // namespace astrocore
