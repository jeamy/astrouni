#pragma once

#include "data_types.h"
#include <cmath>

namespace astro {
namespace math {

// Konstanten (aus aucalc.h)
constexpr double PI = 3.14159265358979323846;
constexpr double TWO_PI = 2.0 * PI;
constexpr double DEG2RAD = PI / 180.0;
constexpr double RAD2DEG = 180.0 / PI;

// Julianisches Datum berechnen (aus aucalc.c: dJulTag)
double calculate_julian_day(int32_t year, int32_t month, int32_t day,
                           int32_t hour, int32_t minute, double second);

// Julianisches Datum aus DateTime
JulianDate to_julian_date(const DateTime& dt);

// DateTime aus Julianischem Datum
DateTime from_julian_date(const JulianDate& jd);

// Modulo für Grad (0-360)
inline double mod_degrees(double degrees) {
    double result = std::fmod(degrees, 360.0);
    if (result < 0.0) {
        result += 360.0;
    }
    return result;
}

// Modulo für Radiant (0-2π)
inline double mod_radians(double radians) {
    double result = std::fmod(radians, TWO_PI);
    if (result < 0.0) {
        result += TWO_PI;
    }
    return result;
}

// Winkel zwischen zwei Positionen (kürzester Weg)
double angle_difference(double angle1, double angle2);

// Grad zu Grad/Minuten/Sekunden
struct DMS {
    int32_t degrees;
    int32_t minutes;
    double seconds;
    bool is_negative;
};

DMS degrees_to_dms(double degrees);

// Grad/Minuten/Sekunden zu Grad
double dms_to_degrees(const DMS& dms);

// Polar zu Rechteckig
struct RectangularCoords {
    double x;
    double y;
    double z;
};

RectangularCoords polar_to_rectangular(double longitude, double latitude, double radius);

// Rechteckig zu Polar
struct PolarCoords {
    double longitude;
    double latitude;
    double radius;
};

PolarCoords rectangular_to_polar(double x, double y, double z);

// Normalisiere Winkel auf Bereich [-180, +180]
inline double normalize_angle_180(double degrees) {
    double result = mod_degrees(degrees);
    if (result > 180.0) {
        result -= 360.0;
    }
    return result;
}

// Prüfe ob Winkel im Bereich [min, max] liegt (mit Wrap-Around)
bool is_angle_in_range(double angle, double min, double max);

} // namespace math
} // namespace astro
