#include "astro/calculations.h"
#include "astro/math_utils.h"
#include <algorithm>
#include <cmath>

namespace astro {

// Standard-Planeten (10 klassische + Mondknoten)
const std::vector<Planet> AstroCalculator::DEFAULT_PLANETS = {
    Planet::Sun, Planet::Moon, Planet::Mercury, Planet::Venus, Planet::Mars,
    Planet::Jupiter, Planet::Saturn, Planet::Uranus, Planet::Neptune, Planet::Pluto,
    Planet::MeanNode
};

// Standard-Aspekte
const std::vector<AspectType> AstroCalculator::DEFAULT_ASPECTS = {
    AspectType::Conjunction,
    AspectType::Opposition,
    AspectType::Trine,
    AspectType::Square,
    AspectType::Sextile,
    AspectType::Quincunx
};

AstroCalculator::AstroCalculator(std::shared_ptr<SwissEphemeris> ephemeris)
    : ephemeris_(ephemeris) {
    if (!ephemeris_) {
        throw std::invalid_argument("Ephemeris pointer is null");
    }
}

AstroCalculator::ChartData AstroCalculator::calculate_chart(const RadixData& radix, 
                                                            HouseSystem house_system) {
    if (!radix.is_valid()) {
        throw std::invalid_argument("Invalid radix data");
    }
    
    ChartData chart;
    chart.radix = radix;
    
    // Planeten berechnen
    chart.planets = calculate_all_planets(radix.julian_date);
    
    // Häuser berechnen
    chart.houses = calculate_houses(house_system, radix.julian_date, radix.birth_location);
    
    // Aspekte berechnen
    chart.aspects = calculate_aspects(chart.planets);
    
    // Ascendant und Midheaven (aus Häusern)
    if (!chart.houses.empty()) {
        chart.ascendant = chart.houses[0].longitude;  // 1. Haus = Ascendant
        
        // Midheaven ist normalerweise 10. Haus
        if (chart.houses.size() >= 10) {
            chart.midheaven = chart.houses[9].longitude;
        }
    }
    
    return chart;
}

std::vector<PlanetPosition> AstroCalculator::calculate_all_planets(const JulianDate& jd) {
    std::vector<PlanetPosition> positions;
    positions.reserve(DEFAULT_PLANETS.size());
    
    for (Planet planet : DEFAULT_PLANETS) {
        try {
            PlanetPosition pos = ephemeris_->calculate_planet(planet, jd);
            positions.push_back(pos);
        } catch (const EphemerisError& e) {
            // Log error, aber weiter machen
            // TODO: Logging implementieren
        }
    }
    
    return positions;
}

std::vector<HouseCusp> AstroCalculator::calculate_houses(HouseSystem system,
                                                        const JulianDate& jd,
                                                        const GeoLocation& location) {
    return ephemeris_->calculate_houses(system, jd, location);
}

std::vector<Aspect> AstroCalculator::calculate_aspects(
    const std::vector<PlanetPosition>& planets,
    double max_orb) {
    
    std::vector<Aspect> aspects;
    
    // Alle Planeten-Paare durchgehen
    for (size_t i = 0; i < planets.size(); ++i) {
        for (size_t j = i + 1; j < planets.size(); ++j) {
            double angle = calculate_angle(planets[i], planets[j]);
            
            // Prüfe alle Aspekt-Typen
            for (AspectType type : DEFAULT_ASPECTS) {
                double orb;
                if (is_aspect(angle, type, max_orb, orb)) {
                    Aspect aspect;
                    aspect.planet1 = DEFAULT_PLANETS[i];
                    aspect.planet2 = DEFAULT_PLANETS[j];
                    aspect.type = type;
                    aspect.orb = orb;
                    
                    // Prüfe ob zunehmend oder abnehmend
                    // (vereinfacht: wenn Planet1 schneller als Planet2)
                    aspect.is_applying = (planets[i].speed_longitude > planets[j].speed_longitude);
                    
                    aspects.push_back(aspect);
                }
            }
        }
    }
    
    // Sortiere nach Orbis (kleinste zuerst)
    std::sort(aspects.begin(), aspects.end(), 
              [](const Aspect& a, const Aspect& b) {
                  return std::abs(a.orb) < std::abs(b.orb);
              });
    
    return aspects;
}

double AstroCalculator::calculate_angle(const PlanetPosition& planet1,
                                       const PlanetPosition& planet2) {
    return math::angle_difference(planet1.longitude, planet2.longitude);
}

bool AstroCalculator::is_aspect(double angle, AspectType type, double max_orb, double& orb) {
    double aspect_angle = static_cast<double>(type);
    
    // Berechne Differenz zum exakten Aspekt
    double diff = std::abs(math::angle_difference(angle, aspect_angle));
    
    // Prüfe ob innerhalb Orbis
    if (diff <= max_orb) {
        orb = diff;
        return true;
    }
    
    return false;
}

} // namespace astro
