#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <vector>

namespace astrocore {

// Planet/body identifiers matching legacy code
enum class PlanetId {
    Sun = 0,
    Moon = 1,
    Mercury = 2,
    Venus = 3,
    Mars = 4,
    Jupiter = 5,
    Saturn = 6,
    Uranus = 7,
    Neptune = 8,
    Pluto = 9,
    NorthNode = 10,
    SouthNode = 11,
    Chiron = 12,
    Ceres = 13,
    Pallas = 14,
    Juno = 15,
    Vesta = 16,
    Lilith = 17,
    Earth = 18,
    Ascendant = 19,
    Midheaven = 20
};

// Calculation flags to control behavior
struct CalcFlags {
    bool heliocentric = false;    // Geocentric (false) or heliocentric (true)
    bool truePosition = false;    // Apparent (false) or true (true) positions
    bool meanEquinox = false;     // True (false) or mean (true) equinox of date
    bool ephemerisTime = false;   // Universal (false) or ephemeris (true) time
    bool calculateSpeed = true;   // Calculate speed (true) or not (false)
    bool calculateLatitude = true; // Calculate latitude (true) or not (false)
    bool calculateDistance = true; // Calculate distance (true) or not (false)
};

// Result structure for planet calculations
struct PlanetPosition {
    double longitude;    // Ecliptic longitude in degrees (0-360)
    double latitude;     // Ecliptic latitude in degrees
    double distance;     // Distance in AU
    double speed;        // Speed in degrees/day (negative = retrograde)
    bool valid;          // True if calculation succeeded
};

// Full chart calculation result
struct ChartPositions {
    double jd;                                // Julian day for the calculation
    double obliquity;                         // Obliquity of ecliptic in degrees
    std::array<PlanetPosition, 21> positions; // Positions for all bodies
    std::array<double, 12> houseCusps;        // House cusps in degrees (1-12)
    double ascendant;                         // Ascendant longitude in degrees
    double mc;                                // Midheaven longitude in degrees
};

// Planet calculation functions

// Calculate position for a single planet
PlanetPosition calculate_planet_position(PlanetId planet, double jd, const CalcFlags& flags = {});

// Calculate positions for all planets
std::vector<PlanetPosition> calculate_all_planets(double jd, const CalcFlags& flags = {});

// Calculate a complete chart (planets + houses)
ChartPositions calculate_chart(double jd, double longitudeDeg, double latitudeDeg, 
                              const CalcFlags& flags = {});

// Get the name of a planet/body
std::string get_planet_name(PlanetId planet);

// Get the glyph character for a planet/body (for font rendering)
char16_t get_planet_glyph(PlanetId planet);

// Ephemeris file handling
bool initialize_ephemeris(const std::string& ephemerisPath);
bool is_ephemeris_available();
double get_ephemeris_start_jd();
double get_ephemeris_end_jd();

} // namespace astrocore
