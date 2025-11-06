// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project

#pragma once

#include "data_types.h"
#include "swiss_ephemeris.h"
#include <vector>
#include <memory>

namespace astro {

// Haupt-Berechnungs-Engine (portiert aus auwurzel.c)
class AstroCalculator {
public:
    // Standard-Konfigurationen (public für GUI-Zugriff)
    static const std::vector<Planet> DEFAULT_PLANETS;
    static const std::vector<AspectType> DEFAULT_ASPECTS;
    
    // Chart-Daten (Ergebnis einer Berechnung)
    struct ChartData {
        RadixData radix;
        std::vector<PlanetPosition> planets;
        std::vector<HouseCusp> houses;
        std::vector<Aspect> aspects;
    };
    
    AstroCalculator(std::shared_ptr<SwissEphemeris> ephemeris);
    
    // Chart berechnen
    ChartData calculate_chart(const RadixData& radix, 
                             HouseSystem house_system = HouseSystem::Placidus,
                             const std::vector<Planet>& planets = DEFAULT_PLANETS,
                             const std::vector<AspectType>& aspects = DEFAULT_ASPECTS,
                             double orb = 8.0) const;
    
    // Einzelne Berechnungen
    std::vector<PlanetPosition> calculate_all_planets(const JulianDate& jd,
                                                      const std::vector<Planet>& planets = DEFAULT_PLANETS) const;
    std::vector<HouseCusp> calculate_houses(const JulianDate& jd, 
                                           const GeoLocation& location,
                                           HouseSystem system = HouseSystem::Placidus) const;
    std::vector<Aspect> calculate_aspects(const std::vector<PlanetPosition>& planets,
                                         const std::vector<AspectType>& aspect_types = DEFAULT_ASPECTS,
                                         double orb = 8.0) const;
    
    // Hilfsfunktionen
    double calculate_angle(const PlanetPosition& p1, const PlanetPosition& p2) const;
    bool is_aspect(double angle, AspectType type, double orb, double& actual_orb) const;

private:
    std::shared_ptr<SwissEphemeris> ephemeris_;
};

} // namespace astro
