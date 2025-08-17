#include "astrocore/planets.hpp"
#include "astrocore/ephemeris.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <optional>
#include <limits>

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

// Forward declaration for helper used below
static double normalize_degrees(double angle);

// Mean lunar elements (approximate, Meeus-like). Returned in degrees [0,360).
static double mean_lunar_node_longitude_deg(double jd) {
    // Mean longitude of the ascending node of the Moon
    const double T = (jd - J2000) / 36525.0; // Julian centuries
    double L = 125.04452 - 1934.136261 * T + 0.0020708 * T * T + (T * T * T) / 450000.0;
    return normalize_degrees(L);
}

static double mean_lunar_apogee_longitude_deg(double jd) {
    // Mean longitude of the lunar apogee ("Black Moon Lilith" ~ mean apogee)
    const double T = (jd - J2000) / 36525.0; // Julian centuries
    double L = 83.3532465 + 4069.0137287 * T - 0.0103200 * T * T - (T * T * T) / 800000.0;
    return normalize_degrees(L);
}


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

static double angular_diff_deg(double to, double from) {
    // Smallest signed difference to-from in degrees in (-180,180]
    double d = std::fmod(to - from, 360.0);
    if (d <= -180.0) d += 360.0;
    else if (d > 180.0) d -= 360.0;
    return d;
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

    // Apply delta-T if using ephemeris time (seconds -> days)
    double jd_eff = jd + (flags.ephemerisTime ? (delta_t(jd) / 86400.0) : 0.0);

    // Handle derived points independent of ephemeris (mean formulas)
    auto compute_derived = [&](PlanetId p, double when) -> PlanetPosition {
        PlanetPosition r; r.latitude = 0.0; r.distance = 0.0; r.valid = true;
        auto lon_fn = [&](double x)->double{
            if (p == PlanetId::NorthNode) return mean_lunar_node_longitude_deg(x);
            if (p == PlanetId::SouthNode) return normalize_degrees(mean_lunar_node_longitude_deg(x) + 180.0);
            // Lilith as mean lunar apogee
            return mean_lunar_apogee_longitude_deg(x);
        };
        r.longitude = lon_fn(when);
        if (flags.calculateSpeed) {
            const double dt = 0.5; // days
            double l1 = lon_fn(when - dt);
            double l2 = lon_fn(when + dt);
            r.speed = angular_diff_deg(l2, l1) / (2.0 * dt);
        }
        return r;
    };

    // Derived points: North/South Node and Lilith (mean apogee)
    if (planet == PlanetId::NorthNode || planet == PlanetId::SouthNode || planet == PlanetId::Lilith) {
        // Apply delta-T if requested
        double jd_eff_nodes = jd + (flags.ephemerisTime ? (delta_t(jd) / 86400.0) : 0.0);
        return compute_derived(planet, jd_eff_nodes);
    }

    // Ephemeris-backed computation
    if (EphemerisManager::isInitialized()) {
        auto tup = EphemerisManager::getPlanetPosition(static_cast<int>(planet), jd_eff);
        if (tup) {
            auto [lon, lat, dist] = *tup;
            result.longitude = normalize_degrees(lon);
            result.latitude = lat;
            result.distance = dist;

            if (flags.calculateSpeed) {
                const double dt = 0.5; // days
                auto left  = EphemerisManager::getPlanetPosition(static_cast<int>(planet), jd_eff - dt);
                auto right = EphemerisManager::getPlanetPosition(static_cast<int>(planet), jd_eff + dt);
                if (left && right) {
                    double lonL = normalize_degrees(std::get<0>(*left));
                    double lonR = normalize_degrees(std::get<0>(*right));
                    result.speed = angular_diff_deg(lonR, lonL) / (2.0 * dt);
                } else {
                    result.speed = 0.0;
                }
            }

            result.valid = true;
            return result;
        }
    }

    // Fallback placeholders if ephemeris not available
    switch (planet) {
        case PlanetId::Sun:
            result.longitude = 0.0;  // Aries 0°
            result.latitude = 0.0;
            result.distance = 1.0;
            result.speed = 0.9856;   // ~deg/day
            result.valid = true;
            break;
        case PlanetId::Moon:
            result.longitude = 90.0;  // Cancer 0°
            result.latitude = 5.0;
            result.distance = 0.002569;
            result.speed = 13.2;
            result.valid = true;
            break;
        default:
            result.longitude = 0.0;
            result.latitude = 0.0;
            result.distance = 0.0;
            result.speed = 0.0;
            result.valid = false;
            break;
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
    s_ephemerisInitialized = EphemerisManager::initialize(ephemerisPath);
    if (s_ephemerisInitialized) {
        s_ephemerisStartJD = EphemerisManager::getStartJD();
        s_ephemerisEndJD = EphemerisManager::getEndJD();
    } else {
        s_ephemerisStartJD = 0.0;
        s_ephemerisEndJD = 0.0;
    }
    return s_ephemerisInitialized;
}

bool is_ephemeris_available() {
    return EphemerisManager::isInitialized();
}

double get_ephemeris_start_jd() {
    return EphemerisManager::isInitialized() ? EphemerisManager::getStartJD() : 0.0;
}

double get_ephemeris_end_jd() {
    return EphemerisManager::isInitialized() ? EphemerisManager::getEndJD() : 0.0;
}

// ---- Lunar phases ----
static double moon_sun_phase_func(double jd) {
    auto m = calculate_planet_position(PlanetId::Moon, jd, {});
    auto s = calculate_planet_position(PlanetId::Sun, jd, {});
    if (!m.valid || !s.valid) return std::numeric_limits<double>::quiet_NaN();
    // Signed difference to target will be applied by caller
    return normalize_degrees(m.longitude - s.longitude);
}

std::optional<PhaseEvent> find_next_lunar_phase(double startJd, LunarPhase phase) {
    if (!EphemerisManager::isInitialized()) return std::nullopt;

    double target = 0.0;
    switch (phase) {
        case LunarPhase::NewMoon: target = 0.0; break;
        case LunarPhase::FirstQuarter: target = 90.0; break;
        case LunarPhase::FullMoon: target = 180.0; break;
        case LunarPhase::LastQuarter: target = 270.0; break;
    }

    auto f = [&](double jd){
        double elong = moon_sun_phase_func(jd);
        if (!std::isfinite(elong)) return std::numeric_limits<double>::quiet_NaN();
        // bring elongation relative to target into (-180,180]
        return angular_diff_deg(elong, target);
    };

    // Bracket search forward from startJd
    const double step = 1.0; // days
    const double bracketGuardDeg = 120.0; // ignore sign changes far from target (wrap-around at ±180°)
    double a = startJd;
    double fa = f(a);
    if (!std::isfinite(fa)) return std::nullopt;
    for (int i = 1; i <= 60; ++i) {
        double b = startJd + i * step;
        double fb = f(b);
        if (!std::isfinite(fb)) return std::nullopt;
        if ((fa <= 0.0 && fb >= 0.0) || (fa >= 0.0 && fb <= 0.0)) {
            // Guard against false brackets caused by jumping across the ±180° discontinuity.
            // In that case |f| is near 180° at both ends; reject and continue scanning.
            if (std::max(std::fabs(fa), std::fabs(fb)) > bracketGuardDeg) {
                a = b; fa = fb; // step forward
                continue;
            }
            // Bisection refine
            for (int it = 0; it < 40; ++it) {
                double c = 0.5 * (a + b);
                double fc = f(c);
                if (!std::isfinite(fc)) return std::nullopt;
                if ((fa <= 0.0 && fc >= 0.0) || (fa >= 0.0 && fc <= 0.0)) {
                    b = c; fb = fc;
                } else {
                    a = c; fa = fc;
                }
                if (std::fabs(fc) < 1e-6 || std::fabs(b - a) < 1e-6) {
                    PhaseEvent ev; ev.jd = c; ev.phase = phase; ev.valid = true; return ev;
                }
            }
            PhaseEvent ev; ev.jd = 0.5 * (a + b); ev.phase = phase; ev.valid = true; return ev;
        }
        a = b; fa = fb;
    }
    return std::nullopt;
}

// ---- Planetary stations ----
static double planet_speed_deg_per_day(PlanetId p, double jd) {
    const double dt = 0.5; // days
    auto left = calculate_planet_position(p, jd - dt, {});
    auto right = calculate_planet_position(p, jd + dt, {});
    if (!left.valid || !right.valid) return std::numeric_limits<double>::quiet_NaN();
    double d = angular_diff_deg(right.longitude, left.longitude);
    return d / (2.0 * dt);
}

std::optional<StationEvent> find_nearest_station(PlanetId planet, double approxJd) {
    if (!EphemerisManager::isInitialized()) return std::nullopt;

    // Bracket a sign change in speed around approxJd
    double a = approxJd, b = approxJd;
    double fa = planet_speed_deg_per_day(planet, a);
    if (!std::isfinite(fa)) return std::nullopt;
    double fb = fa;

    double step = 1.0; // start with 1 day
    for (int iter = 0; iter < 30; ++iter) {
        a = approxJd - step; fa = planet_speed_deg_per_day(planet, a);
        b = approxJd + step; fb = planet_speed_deg_per_day(planet, b);
        if (!std::isfinite(fa) || !std::isfinite(fb)) return std::nullopt;
        if ((fa <= 0.0 && fb >= 0.0) || (fa >= 0.0 && fb <= 0.0)) break;
        step *= 1.7;
    }

    // If no sign change found, give up
    if (!((fa <= 0.0 && fb >= 0.0) || (fa >= 0.0 && fb <= 0.0))) return std::nullopt;

    // Bisection on speed to find zero-crossing
    auto classify_turn = [&](double c) -> bool {
        // Determine if turning prograde->retrograde (true) or retrograde->direct (false)
        // by sampling speeds just before and after c.
        const double h = 1e-3; // days
        double sl = planet_speed_deg_per_day(planet, c - h);
        double sr = planet_speed_deg_per_day(planet, c + h);
        if (std::isfinite(sl) && std::isfinite(sr)) {
            return (sl > 0.0 && sr < 0.0);
        }
        // Fallback to bracket orientation if sampling failed
        return (fa > 0.0 && fb < 0.0);
    };
    for (int it = 0; it < 50; ++it) {
        double c = 0.5 * (a + b);
        double fc = planet_speed_deg_per_day(planet, c);
        if (!std::isfinite(fc)) return std::nullopt;
        if ((fa <= 0.0 && fc >= 0.0) || (fa >= 0.0 && fc <= 0.0)) { b = c; fb = fc; }
        else { a = c; fa = fc; }
        if (std::fabs(fc) < 1e-4 || std::fabs(b - a) < 1e-5) {
            StationEvent ev; ev.jd = c; ev.retrogradeStart = classify_turn(c); ev.valid = true; return ev;
        }
    }
    {
        double c = 0.5 * (a + b);
        StationEvent ev; ev.jd = c; ev.retrogradeStart = classify_turn(c); ev.valid = true; return ev;
    }
}

// ---- Eclipses (heuristic detection around syzygies) ----
std::optional<EclipseEvent> find_next_solar_eclipse(double startJd) {
    if (!EphemerisManager::isInitialized()) return std::nullopt;

    const double latAny = 1.5;     // deg: rough threshold for any (partial) solar eclipse
    const double latCentral = 0.3; // deg: rough threshold for central/umbral

    double jd = startJd;
    for (int k = 0; k < 40; ++k) { // search up to ~3 years worth of lunations
        auto ph = find_next_lunar_phase(jd, LunarPhase::NewMoon);
        if (!ph.has_value()) return std::nullopt;
        double t = ph->jd;
        auto m = calculate_planet_position(PlanetId::Moon, t, {});
        if (!m.valid) return std::nullopt;
        double abslat = std::fabs(m.latitude);
        if (abslat <= latAny) {
            EclipseEvent ev; ev.jd = t; ev.kind = EclipseKind::Solar; ev.central = (abslat <= latCentral); ev.valid = true; return ev;
        }
        jd = t + 1.0; // step past this lunation and continue
    }
    return std::nullopt;
}

std::optional<EclipseEvent> find_next_lunar_eclipse(double startJd) {
    if (!EphemerisManager::isInitialized()) return std::nullopt;

    const double latAny = 1.0;     // deg: lunar eclipse more tolerant
    const double latCentral = 0.3; // deg: rough umbral threshold

    double jd = startJd;
    for (int k = 0; k < 40; ++k) {
        auto ph = find_next_lunar_phase(jd, LunarPhase::FullMoon);
        if (!ph.has_value()) return std::nullopt;
        double t = ph->jd;
        auto m = calculate_planet_position(PlanetId::Moon, t, {});
        if (!m.valid) return std::nullopt;
        double abslat = std::fabs(m.latitude);
        if (abslat <= latAny) {
            EclipseEvent ev; ev.jd = t; ev.kind = EclipseKind::Lunar; ev.central = (abslat <= latCentral); ev.valid = true; return ev;
        }
        jd = t + 1.0;
    }
    return std::nullopt;
}

} // namespace astrocore
