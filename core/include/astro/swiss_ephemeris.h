// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project

#pragma once

#include "data_types.h"
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

namespace astro {

// Swiss Ephemeris Exception
class EphemerisError : public std::runtime_error {
public:
    explicit EphemerisError(const std::string& msg) : std::runtime_error(msg) {}
};

// Swiss Ephemeris Wrapper
class SwissEphemeris {
public:
    SwissEphemeris();
    ~SwissEphemeris();
    
    // Nicht kopierbar
    SwissEphemeris(const SwissEphemeris&) = delete;
    SwissEphemeris& operator=(const SwissEphemeris&) = delete;
    
    // Initialisierung
    bool initialize(const std::string& ephe_path);
    void set_topocentric(double lon, double lat, double alt);
    
    // Planeten-Berechnungen
    PlanetPosition calculate_planet(Planet planet, const JulianDate& jd, 
                                   bool heliocentric = false) const;
    
    // Häuser-Berechnungen
    std::vector<HouseCusp> calculate_houses(HouseSystem system, 
                                           const JulianDate& jd,
                                           const GeoLocation& location) const;
    
    // Hilfsfunktionen
    double calculate_delta_t(const JulianDate& jd) const;
    bool is_retrograde(Planet planet, const JulianDate& jd) const;
    std::string get_planet_name(Planet planet) const;
    
    // Status
    bool is_initialized() const { return initialized_; }
    const std::string& get_ephe_path() const { return ephe_path_; }
    
private:
    std::string ephe_path_;
    bool initialized_;
    
    // Konvertierungs-Helfer
    int32_t to_swiss_planet(Planet planet) const;
    char to_swiss_house_system(HouseSystem system) const;
};

} // namespace astro
