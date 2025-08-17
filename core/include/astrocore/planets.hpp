#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <vector>
#include <optional>

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
    double longitude = 0.0;    // Ecliptic longitude in degrees (0-360)
    double latitude = 0.0;     // Ecliptic latitude in degrees
    double distance = 0.0;     // Distance in AU
    double speed = 0.0;        // Speed in degrees/day (negative = retrograde)
    bool valid = false;        // True if calculation succeeded
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

// Lunar phases
enum class LunarPhase { NewMoon, FirstQuarter, FullMoon, LastQuarter };

struct PhaseEvent {
  double jd {0.0};
  LunarPhase phase {LunarPhase::NewMoon};
  bool valid {false};
};

// Find the next occurrence of the requested lunar phase at or after startJd.
// Returns std::nullopt if ephemeris is unavailable or search fails/brackets not found.
std::optional<PhaseEvent> find_next_lunar_phase(double startJd, LunarPhase phase);

// Planetary stations (retrograde/direct turning points)
struct StationEvent {
  double jd {0.0};
  bool retrogradeStart {false}; // true: turning from prograde to retrograde; false: retrograde -> direct
  bool valid {false};
};

// Find the station nearest to approxJd for the given planet. Uses speed zero-crossing.
// Returns std::nullopt if ephemeris is unavailable or search fails.
std::optional<StationEvent> find_nearest_station(PlanetId planet, double approxJd);

// Eclipse calculations (simple geometry around syzygies)
enum class EclipseKind { Solar, Lunar };

struct EclipseEvent {
    double jd {0.0};
    EclipseKind kind {EclipseKind::Solar};
    bool central {false};   // Rough classification: central/umbral when true
    bool valid {false};
};

// Find next solar eclipse at or after startJd (searches New Moons)
std::optional<EclipseEvent> find_next_solar_eclipse(double startJd);

// Find next lunar eclipse at or after startJd (searches Full Moons)
std::optional<EclipseEvent> find_next_lunar_eclipse(double startJd);

} // namespace astrocore
