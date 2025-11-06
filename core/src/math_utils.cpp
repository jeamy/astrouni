#include "astro/math_utils.h"
#include <cmath>

namespace astro {
namespace math {

// Julianisches Datum berechnen (Algorithmus aus aucalc.c)
double calculate_julian_day(int32_t year, int32_t month, int32_t day,
                           int32_t hour, int32_t minute, double second) {
    // Gregorianischer Kalender
    int32_t a = (14 - month) / 12;
    int32_t y = year + 4800 - a;
    int32_t m = month + 12 * a - 3;
    
    int32_t jdn = day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
    
    // Zeit-Anteil
    double time_fraction = (hour - 12.0) / 24.0 + minute / 1440.0 + second / 86400.0;
    
    return static_cast<double>(jdn) + time_fraction;
}

JulianDate to_julian_date(const DateTime& dt) {
    double jd = calculate_julian_day(dt.year, dt.month, dt.day, 
                                     dt.hour, dt.minute, dt.second);
    return JulianDate(jd);
}

DateTime from_julian_date(const JulianDate& jd) {
    // Inverse Berechnung
    double z = std::floor(jd.jd + 0.5);
    double f = (jd.jd + 0.5) - z;
    
    int32_t a;
    if (z < 2299161) {
        a = static_cast<int32_t>(z);
    } else {
        int32_t alpha = static_cast<int32_t>((z - 1867216.25) / 36524.25);
        a = static_cast<int32_t>(z + 1 + alpha - alpha / 4);
    }
    
    int32_t b = a + 1524;
    int32_t c = static_cast<int32_t>((b - 122.1) / 365.25);
    int32_t d = static_cast<int32_t>(365.25 * c);
    int32_t e = static_cast<int32_t>((b - d) / 30.6001);
    
    DateTime dt;
    dt.day = b - d - static_cast<int32_t>(30.6001 * e);
    dt.month = e < 14 ? e - 1 : e - 13;
    dt.year = dt.month > 2 ? c - 4716 : c - 4715;
    
    // Zeit-Anteil
    double time_fraction = f + 0.5;
    if (time_fraction >= 1.0) {
        time_fraction -= 1.0;
        dt.day += 1;
    }
    
    dt.hour = static_cast<int32_t>(time_fraction * 24.0);
    double min_frac = (time_fraction * 24.0 - dt.hour) * 60.0;
    dt.minute = static_cast<int32_t>(min_frac);
    dt.second = (min_frac - dt.minute) * 60.0;
    
    return dt;
}

double angle_difference(double angle1, double angle2) {
    double diff = angle2 - angle1;
    diff = mod_degrees(diff + 180.0) - 180.0;
    return diff;
}

DMS degrees_to_dms(double degrees) {
    DMS result;
    result.is_negative = (degrees < 0.0);
    
    double abs_degrees = std::abs(degrees);
    result.degrees = static_cast<int32_t>(abs_degrees);
    
    double minutes_frac = (abs_degrees - result.degrees) * 60.0;
    result.minutes = static_cast<int32_t>(minutes_frac);
    
    result.seconds = (minutes_frac - result.minutes) * 60.0;
    
    return result;
}

double dms_to_degrees(const DMS& dms) {
    double degrees = dms.degrees + dms.minutes / 60.0 + dms.seconds / 3600.0;
    return dms.is_negative ? -degrees : degrees;
}

RectangularCoords polar_to_rectangular(double longitude, double latitude, double radius) {
    double lon_rad = longitude * DEG2RAD;
    double lat_rad = latitude * DEG2RAD;
    
    double cos_lat = std::cos(lat_rad);
    
    RectangularCoords result;
    result.x = radius * cos_lat * std::cos(lon_rad);
    result.y = radius * cos_lat * std::sin(lon_rad);
    result.z = radius * std::sin(lat_rad);
    
    return result;
}

PolarCoords rectangular_to_polar(double x, double y, double z) {
    PolarCoords result;
    
    result.radius = std::sqrt(x * x + y * y + z * z);
    
    if (result.radius > 0.0) {
        result.latitude = std::asin(z / result.radius) * RAD2DEG;
        result.longitude = std::atan2(y, x) * RAD2DEG;
        result.longitude = mod_degrees(result.longitude);
    } else {
        result.latitude = 0.0;
        result.longitude = 0.0;
    }
    
    return result;
}

bool is_angle_in_range(double angle, double min, double max) {
    angle = mod_degrees(angle);
    min = mod_degrees(min);
    max = mod_degrees(max);
    
    if (min <= max) {
        return angle >= min && angle <= max;
    } else {
        // Wrap-Around Fall
        return angle >= min || angle <= max;
    }
}

} // namespace math

// JulianDate Konstruktor Implementation
JulianDate::JulianDate(const DateTime& dt) {
    jd = math::calculate_julian_day(dt.year, dt.month, dt.day, 
                                    dt.hour, dt.minute, dt.second);
}

} // namespace astro
