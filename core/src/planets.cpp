#include "astrocore/planets.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <fstream>

namespace astrocore {

// Constants
constexpr double DEG_TO_RAD = M_PI / 180.0;
constexpr double RAD_TO_DEG = 180.0 / M_PI;
constexpr double J2000 = 2451545.0;
constexpr double JUL_OFFSET = 2433282.0;  // Astrodienst relative Julian date offset

// Ephemeris file paths and availability flags
static std::string s_ephemerisPath;
static bool s_ephemerisInitialized = false;
static double s_ephemerisStartJD = 0.0;
static double s_ephemerisEndJD = 0.0;

// Planet names
static const std::unordered_map<PlanetId, std::string> s_planetNames = {
    {PlanetId::Sun, "Sun"},
    {PlanetId::Moon, "Moon"},
    {PlanetId::Mercury, "Mercury"},
    {PlanetId::Venus, "Venus"},
    {PlanetId::Mars, "Mars"},
    {PlanetId::Jupiter, "Jupiter"},
    {PlanetId::Saturn, "Saturn"},
    {PlanetId::Uranus, "Uranus"},
    {PlanetId::Neptune, "Neptune"},
    {PlanetId::Pluto, "Pluto"},
    {PlanetId::NorthNode, "North Node"},
    {PlanetId::SouthNode, "South Node"},
    {PlanetId::Chiron, "Chiron"},
    {PlanetId::Ceres, "Ceres"},
    {PlanetId::Pallas, "Pallas"},
    {PlanetId::Juno, "Juno"},
    {PlanetId::Vesta, "Vesta"},
    {PlanetId::Lilith, "Lilith"},
    {PlanetId::Earth, "Earth"},
    {PlanetId::Ascendant, "Ascendant"},
    {PlanetId::Midheaven, "Midheaven"}
};

// Planet glyphs (Unicode code points)
static const std::unordered_map<PlanetId, char16_t> s_planetGlyphs = {
    {PlanetId::Sun, u'\u2609'},
    {PlanetId::Moon, u'\u263D'},
    {PlanetId::Mercury, u'\u263F'},
    {PlanetId::Venus, u'\u2640'},
    {PlanetId::Mars, u'\u2642'},
    {PlanetId::Jupiter, u'\u2643'},
    {PlanetId::Saturn, u'\u2644'},
    {PlanetId::Uranus, u'\u2645'},
    {PlanetId::Neptune, u'\u2646'},
    {PlanetId::Pluto, u'\u2647'},
    {PlanetId::NorthNode, u'\u260A'},
    {PlanetId::SouthNode, u'\u260B'},
    {PlanetId::Chiron, u'\u26B7'},
    {PlanetId::Ceres, u'\u26B3'},
    {PlanetId::Pallas, u'\u26B4'},
    {PlanetId::Juno, u'\u26B5'},
    {PlanetId::Vesta, u'\u26B6'},
    {PlanetId::Lilith, u'\u26E7'},
    {PlanetId::Earth, u'\u2295'},
    {PlanetId::Ascendant, u'\u2315'},
    {PlanetId::Midheaven, u'\u2339'}
};

// Helper function to normalize angle to 0-360 range
static double normalize_degrees(double angle) {
    angle = std::fmod(angle, 360.0);
    if (angle < 0.0) angle += 360.0;
    return angle;
}

// Helper function to calculate delta-T (difference between TT and UT)
static double delta_t(double jd) {
    // Simple approximation for delta-T
    const double t = (jd - 2451545.0) / 36525.0;  // Julian centuries since J2000
    return 64.184 + 59.0 * t;  // Simplified formula in seconds
}

// Calculate Ascendant and MC longitudes
static void asc_mc_longitudes(double jd, double longitudeRad, double latitudeRad, double& ascRad, double& mcRad) {
    // Simplified calculation for testing
    // In a real implementation, this would use proper astronomical formulas
    
    // Local sidereal time approximation
    double t = (jd - 2451545.0) / 36525.0;
    double lst = 280.46061837 + 360.98564736629 * (jd - 2451545.0) + 0.000387933 * t * t - t * t * t / 38710000.0;
    lst = std::fmod(lst + longitudeRad * RAD_TO_DEG, 360.0) * DEG_TO_RAD;
    
    // MC calculation
    mcRad = lst;
    
    // Ascendant calculation (simplified)
    double tanLat = std::tan(latitudeRad);
    double cosLst = std::cos(lst);
    double sinLst = std::sin(lst);
    
    // Simplified formula for ascendant
    ascRad = std::atan2(sinLst, cosLst * std::cos(23.4392911 * DEG_TO_RAD) - tanLat * std::sin(23.4392911 * DEG_TO_RAD));
    
    // Normalize to 0-2π range
    if (ascRad < 0) ascRad += 2.0 * M_PI;
}

// Calculate Placidus house cusps
static void placidus_house_cusps(double jd, double longitudeRad, double latitudeRad, double* cuspsRad) {
    // Simplified Placidus implementation for testing
    double ascRad, mcRad;
    asc_mc_longitudes(jd, longitudeRad, latitudeRad, ascRad, mcRad);
    
    // Set MC and Ascendant
    cuspsRad[9] = mcRad;  // 10th house cusp is MC
    cuspsRad[0] = ascRad; // 1st house cusp is Ascendant
    
    // Opposite points for IC and Descendant
    cuspsRad[3] = std::fmod(ascRad + M_PI, 2.0 * M_PI); // 4th house cusp is Descendant
    cuspsRad[6] = std::fmod(mcRad + M_PI, 2.0 * M_PI);  // 7th house cusp is IC
    
    // Simple approximation for other cusps
    // In a real implementation, this would use proper Placidus formulas
    cuspsRad[1] = std::fmod(ascRad + M_PI / 6.0, 2.0 * M_PI);
    cuspsRad[2] = std::fmod(ascRad + M_PI / 3.0, 2.0 * M_PI);
    cuspsRad[4] = std::fmod(cuspsRad[3] + M_PI / 6.0, 2.0 * M_PI);
    cuspsRad[5] = std::fmod(cuspsRad[3] + M_PI / 3.0, 2.0 * M_PI);
    cuspsRad[7] = std::fmod(cuspsRad[6] + M_PI / 6.0, 2.0 * M_PI);
    cuspsRad[8] = std::fmod(cuspsRad[6] + M_PI / 3.0, 2.0 * M_PI);
    cuspsRad[10] = std::fmod(cuspsRad[9] + M_PI / 6.0, 2.0 * M_PI);
    cuspsRad[11] = std::fmod(cuspsRad[9] + M_PI / 3.0, 2.0 * M_PI);
}

// Calculate Equal house cusps
static void equal_house_cusps(double ascRad, double* cuspsRad) {
    // Equal house system - each house is exactly 30 degrees
    cuspsRad[0] = ascRad;  // 1st house cusp is Ascendant
    
    for (int i = 1; i < 12; ++i) {
        cuspsRad[i] = std::fmod(ascRad + i * M_PI / 6.0, 2.0 * M_PI);
    }
}

// Calculate position for a single planet
PlanetPosition calculate_planet_position(PlanetId planet, double jd, const CalcFlags& flags) {
    PlanetPosition result;
    result.valid = false;
    
    // Convert JD to Astrodienst relative JD
    double jd_ad = jd - JUL_OFFSET;
    
    // Apply delta-T if using ephemeris time
    if (flags.ephemerisTime) {
        jd_ad += delta_t(jd) / 86400.0;
    }
    
    // TODO: Implement actual planet calculation based on legacy code
    // This is a placeholder that returns fixed values for testing
    
    switch (planet) {
        case PlanetId::Sun:
            result.longitude = 0.0;  // Aries 0°
            result.latitude = 0.0;
            result.distance = 1.0;
            result.speed = 1.0;
            result.valid = true;
            break;
            
        case PlanetId::Moon:
            result.longitude = 90.0;  // Cancer 0°
            result.latitude = 5.0;
            result.distance = 0.002569;
            result.speed = 13.2;
            result.valid = true;
            break;
            
        // Add other planets with placeholder values
        default:
            result.longitude = 0.0;
            result.latitude = 0.0;
            result.distance = 0.0;
            result.speed = 0.0;
            result.valid = false;
    }
    
    return result;
}

// Calculate positions for all planets
std::vector<PlanetPosition> calculate_all_planets(double jd, const CalcFlags& flags) {
    std::vector<PlanetPosition> results;
    
    // Calculate each planet
    for (int i = 0; i <= static_cast<int>(PlanetId::Midheaven); ++i) {
        PlanetId planet = static_cast<PlanetId>(i);
        results.push_back(calculate_planet_position(planet, jd, flags));
    }
    
    return results;
}

// Calculate a complete chart (planets + houses)
ChartPositions calculate_chart(double jd, double longitudeDeg, double latitudeDeg, 
                              const CalcFlags& flags) {
    ChartPositions chart;
    chart.jd = jd;
    
    // Calculate obliquity of ecliptic
    const double t = (jd - J2000) / 36525.0;
    chart.obliquity = 23.4392911 - 0.0130042 * t - 0.00000164 * t * t + 0.000000503 * t * t * t;
    
    // Calculate Ascendant and MC
    double longitudeRad = longitudeDeg * DEG_TO_RAD;
    double latitudeRad = latitudeDeg * DEG_TO_RAD;
    double ascRad, mcRad;
    asc_mc_longitudes(jd, longitudeRad, latitudeRad, ascRad, mcRad);
    chart.ascendant = ascRad * RAD_TO_DEG;
    chart.mc = mcRad * RAD_TO_DEG;
    
    // Calculate house cusps
    double cuspsRad[12];
    if (true) {  // TODO: Add house system selection
        placidus_house_cusps(jd, longitudeRad, latitudeRad, cuspsRad);
    } else {
        equal_house_cusps(ascRad, cuspsRad);
    }
    
    // Convert house cusps to degrees
    for (int i = 0; i < 12; ++i) {
        chart.houseCusps[i] = cuspsRad[i] * RAD_TO_DEG;
    }
    
    // Calculate planet positions
    for (int i = 0; i <= static_cast<int>(PlanetId::Midheaven); ++i) {
        PlanetId planet = static_cast<PlanetId>(i);
        chart.positions[i] = calculate_planet_position(planet, jd, flags);
    }
    
    // Special case for Ascendant and Midheaven
    chart.positions[static_cast<int>(PlanetId::Ascendant)].longitude = chart.ascendant;
    chart.positions[static_cast<int>(PlanetId::Ascendant)].valid = true;
    chart.positions[static_cast<int>(PlanetId::Midheaven)].longitude = chart.mc;
    chart.positions[static_cast<int>(PlanetId::Midheaven)].valid = true;
    
    return chart;
}

// Get the name of a planet/body
std::string get_planet_name(PlanetId planet) {
    auto it = s_planetNames.find(planet);
    if (it != s_planetNames.end()) {
        return it->second;
    }
    return "Unknown";
}

// Get the glyph character for a planet/body (for font rendering)
char16_t get_planet_glyph(PlanetId planet) {
    auto it = s_planetGlyphs.find(planet);
    if (it != s_planetGlyphs.end()) {
        return it->second;
    }
    return u'?';
}

// Ephemeris file handling
bool initialize_ephemeris(const std::string& ephemerisPath) {
    s_ephemerisPath = ephemerisPath;
    
    // TODO: Implement actual ephemeris file loading and validation
    // For now, just set some default values
    s_ephemerisStartJD = 2305447.5;  // 1600 CE
    s_ephemerisEndJD = 2524627.5;    // 2200 CE
    s_ephemerisInitialized = true;
    
    return true;
}

bool is_ephemeris_available() {
    return s_ephemerisInitialized;
}

double get_ephemeris_start_jd() {
    return s_ephemerisStartJD;
}

double get_ephemeris_end_jd() {
    return s_ephemerisEndJD;
}

} // namespace astrocore
