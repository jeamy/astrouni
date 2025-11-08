#include "astro/swiss_ephemeris.h"
#include <swephexp.h>
#include <cstring>

namespace astro {

SwissEphemeris::SwissEphemeris() : initialized_(false) {}

SwissEphemeris::~SwissEphemeris() {
    if (initialized_) {
        swe_close();
    }
}

bool SwissEphemeris::initialize(const std::string& ephe_path) {
    swe_set_ephe_path(const_cast<char*>(ephe_path.c_str()));
    ephe_path_ = ephe_path;
    initialized_ = true;
    return true;
}

void SwissEphemeris::set_topocentric(double lon, double lat, double alt) {
    swe_set_topo(lon, lat, alt);
}

PlanetPosition SwissEphemeris::calculate_planet(Planet planet, 
                                               const JulianDate& jd,
                                               bool heliocentric) const {
    if (!initialized_) {
        throw EphemerisError("Swiss Ephemeris not initialized");
    }
    
    double xx[6];  // lng, lat, dist, speed_lng, speed_lat, speed_dist
    char serr[256];
    
    int32_t iflag = SEFLG_SWIEPH | SEFLG_SPEED;
    if (heliocentric) {
        iflag |= SEFLG_HELCTR;
    }
    
    int32_t ipl = to_swiss_planet(planet);
    double jd_ut = jd.to_julian();
    
    int ret = swe_calc_ut(jd_ut, ipl, iflag, xx, serr);
    
    if (ret < 0) {
        // Fallback: Versuche Moshier, wenn Swiss Ephemeris-Dateien fehlen
        int32_t iflag_fallback = SEFLG_MOSEPH | SEFLG_SPEED;
        ret = swe_calc_ut(jd_ut, ipl, iflag_fallback, xx, serr);
        if (ret < 0) {
            throw EphemerisError(std::string("Swiss Ephemeris error: ") + serr);
        }
    }
    
    PlanetPosition pos;
    pos.longitude = xx[0];
    pos.latitude = xx[1];
    pos.distance = xx[2];
    pos.speed_longitude = xx[3];
    pos.speed_latitude = xx[4];
    pos.speed_distance = xx[5];
    pos.is_retrograde = (xx[3] < 0.0);
    
    return pos;
}

std::vector<HouseCusp> SwissEphemeris::calculate_houses(HouseSystem system,
                                                       const JulianDate& jd,
                                                       const GeoLocation& location) const {
    if (!initialized_) {
        throw EphemerisError("Swiss Ephemeris not initialized");
    }
    
    double cusps[13];  // Index 0 unused, 1-12 = houses
    double ascmc[10];  // AC, MC, ARMC, Vertex, etc.
    
    char hsys = to_swiss_house_system(system);
    double jd_ut = jd.to_julian();
    
    int ret = swe_houses(jd_ut, location.latitude, location.longitude, hsys, cusps, ascmc);
    
    if (ret < 0) {
        throw EphemerisError("House calculation failed");
    }
    
    std::vector<HouseCusp> result;
    result.reserve(12);
    
    for (int i = 1; i <= 12; ++i) {
        result.emplace_back(i, cusps[i]);
    }
    
    return result;
}

double SwissEphemeris::calculate_delta_t(const JulianDate& jd) const {
    return swe_deltat(jd.to_julian());
}

bool SwissEphemeris::is_retrograde(Planet planet, const JulianDate& jd) const {
    auto pos = calculate_planet(planet, jd);
    return pos.is_retrograde;
}

std::string SwissEphemeris::get_planet_name(Planet planet) const {
    char name[256];
    int32_t ipl = to_swiss_planet(planet);
    swe_get_planet_name(ipl, name);
    return std::string(name);
}

int32_t SwissEphemeris::to_swiss_planet(Planet planet) const {
    switch (planet) {
        case Planet::Sun:      return SE_SUN;
        case Planet::Moon:     return SE_MOON;
        case Planet::Mercury:  return SE_MERCURY;
        case Planet::Venus:    return SE_VENUS;
        case Planet::Mars:     return SE_MARS;
        case Planet::Jupiter:  return SE_JUPITER;
        case Planet::Saturn:   return SE_SATURN;
        case Planet::Uranus:   return SE_URANUS;
        case Planet::Neptune:  return SE_NEPTUNE;
        case Planet::Pluto:    return SE_PLUTO;
        case Planet::MeanNode: return SE_MEAN_NODE;
        case Planet::TrueNode: return SE_TRUE_NODE;
        case Planet::Chiron:   return SE_CHIRON;
        default:
            throw EphemerisError("Unknown planet");
    }
}

char SwissEphemeris::to_swiss_house_system(HouseSystem system) const {
    return static_cast<char>(system);
}

} // namespace astro
