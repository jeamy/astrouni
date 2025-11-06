#pragma once

#include "data_types.h"
#include "swiss_ephemeris.h"
#include <vector>
#include <memory>

namespace astro {

// Haupt-Berechnungs-Engine (portiert aus auwurzel.c)
class AstroCalculator {
public:
    explicit AstroCalculator(std::shared_ptr<SwissEphemeris> ephemeris);
    ~AstroCalculator() = default;
    
    // Chart-Berechnung (entspricht sCalcChart)
    struct ChartData {
        RadixData radix;
        std::vector<PlanetPosition> planets;
        std::vector<HouseCusp> houses;
        std::vector<Aspect> aspects;
        
        // Zusätzliche Punkte
        double ascendant;
        double midheaven;
        double vertex;
    };
    
    ChartData calculate_chart(const RadixData& radix, HouseSystem house_system);
    
    // Planeten berechnen (entspricht vCalcPlanets)
    std::vector<PlanetPosition> calculate_all_planets(const JulianDate& jd);
    
    // Häuser berechnen (entspricht sCalcHauser)
    std::vector<HouseCusp> calculate_houses(HouseSystem system, 
                                           const JulianDate& jd,
                                           const GeoLocation& location);
    
    // Aspekte berechnen (entspricht vCalcAsp)
    std::vector<Aspect> calculate_aspects(const std::vector<PlanetPosition>& planets,
                                         double max_orb = 8.0);
    
    // Winkel zwischen Planeten (entspricht vCalcWinkel)
    double calculate_angle(const PlanetPosition& planet1, 
                          const PlanetPosition& planet2);
    
    // Prüfe ob Aspekt vorliegt
    bool is_aspect(double angle, AspectType type, double max_orb, double& orb);
    
private:
    std::shared_ptr<SwissEphemeris> ephemeris_;
    
    // Standard-Planeten für Berechnungen
    static const std::vector<Planet> DEFAULT_PLANETS;
    
    // Standard-Aspekte
    static const std::vector<AspectType> DEFAULT_ASPECTS;
};

} // namespace astro
