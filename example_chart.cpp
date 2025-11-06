#include "astro/swiss_ephemeris.h"
#include "astro/calculations.h"
#include "astro/math_utils.h"
#include <iostream>
#include <iomanip>
#include <memory>

using namespace astro;

void print_planet_position(Planet planet, const PlanetPosition& pos, 
                          const std::string& name) {
    std::cout << std::left << std::setw(12) << name
              << std::right << std::fixed << std::setprecision(2)
              << std::setw(8) << pos.longitude << "°"
              << "  Lat: " << std::setw(6) << pos.latitude << "°"
              << "  Dist: " << std::setw(8) << pos.distance << " AU"
              << "  Speed: " << std::setw(6) << pos.speed_longitude << "°/day";
    
    if (pos.is_retrograde) {
        std::cout << "  [R]";
    }
    
    std::cout << std::endl;
}

void print_house_cusps(const std::vector<HouseCusp>& houses) {
    std::cout << "\nHäuser (Placidus):\n";
    std::cout << "─────────────────────────────\n";
    
    for (const auto& cusp : houses) {
        std::cout << "Haus " << std::setw(2) << cusp.house_number
                  << ": " << std::fixed << std::setprecision(2)
                  << std::setw(8) << cusp.longitude << "°" << std::endl;
    }
}

void print_aspects(const std::vector<Aspect>& aspects) {
    std::cout << "\nAspekte:\n";
    std::cout << "─────────────────────────────────────────────\n";
    
    const char* aspect_names[] = {
        "Konjunktion", "Opposition", "Trigon", "Quadrat", 
        "Sextil", "Quincunx", "Halbsextil", "Halbquadrat", "Anderthalbquadrat"
    };
    
    for (const auto& aspect : aspects) {
        int type_index = static_cast<int>(aspect.type) / 30;
        if (type_index >= 0 && type_index < 9) {
            std::cout << "  " << aspect_names[type_index]
                      << " (Orbis: " << std::fixed << std::setprecision(2) 
                      << aspect.orb << "°)";
            
            if (aspect.is_applying) {
                std::cout << " [zunehmend]";
            }
            
            std::cout << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    try {
        std::cout << "═══════════════════════════════════════════\n";
        std::cout << "  AstroUniverse - Chart Calculation Demo\n";
        std::cout << "═══════════════════════════════════════════\n\n";
        
        // Swiss Ephemeris initialisieren
        auto ephemeris = std::make_shared<SwissEphemeris>();
        
        // Default-Pfad relativ zum Build-Verzeichnis
        std::string ephe_path = "../swisseph/ephe";
        if (argc > 1) {
            ephe_path = argv[1];
        }
        
        std::cout << "Initialisiere Swiss Ephemeris...\n";
        std::cout << "Ephemeriden-Pfad: " << ephe_path << "\n\n";
        
        if (!ephemeris->initialize(ephe_path)) {
            std::cerr << "Fehler: Konnte Swiss Ephemeris nicht initialisieren!\n";
            return 1;
        }
        
        // Calculator erstellen
        auto calculator = std::make_unique<AstroCalculator>(ephemeris);
        
        // Beispiel-Radix: 1. Januar 2000, 12:00 UT, Berlin
        RadixData radix;
        radix.name = "Beispiel Person";
        radix.birth_datetime = DateTime(2000, 1, 1, 12, 0, 0.0);
        radix.birth_location = GeoLocation(52.52, 13.40);  // Berlin
        radix.julian_date = JulianDate(radix.birth_datetime);
        
        std::cout << "Berechne Chart für:\n";
        std::cout << "  Name: " << radix.name << "\n";
        std::cout << "  Datum: " << radix.birth_datetime.day << "."
                  << radix.birth_datetime.month << "."
                  << radix.birth_datetime.year << "\n";
        std::cout << "  Zeit: " << radix.birth_datetime.hour << ":"
                  << std::setfill('0') << std::setw(2) << radix.birth_datetime.minute 
                  << " UT\n";
        std::cout << "  Ort: Berlin (52.52°N, 13.40°E)\n";
        std::cout << "  JD: " << std::fixed << std::setprecision(5) 
                  << radix.julian_date.to_julian() << "\n\n";
        
        // Chart berechnen
        std::cout << "Berechne Planeten-Positionen...\n";
        auto chart = calculator->calculate_chart(radix, HouseSystem::Placidus);
        
        // Planeten ausgeben
        std::cout << "\nPlaneten-Positionen:\n";
        std::cout << "═══════════════════════════════════════════════════════════════════\n";
        
        const char* planet_names[] = {
            "Sonne", "Mond", "Merkur", "Venus", "Mars",
            "Jupiter", "Saturn", "Uranus", "Neptun", "Pluto", "Mondknoten"
        };
        
        for (size_t i = 0; i < chart.planets.size() && i < 11; ++i) {
            print_planet_position(static_cast<Planet>(i), chart.planets[i], planet_names[i]);
        }
        
        // Häuser ausgeben
        print_house_cusps(chart.houses);
        
        // Ascendant und Midheaven
        std::cout << "\nWichtige Punkte:\n";
        std::cout << "─────────────────────────────\n";
        std::cout << "Ascendant (AC): " << std::fixed << std::setprecision(2)
                  << chart.ascendant << "°\n";
        std::cout << "Midheaven (MC): " << chart.midheaven << "°\n";
        
        // Aspekte ausgeben (nur die ersten 10)
        if (!chart.aspects.empty()) {
            std::cout << "\nTop 10 Aspekte (nach Orbis sortiert):\n";
            std::cout << "─────────────────────────────────────────────\n";
            
            size_t max_aspects = std::min(chart.aspects.size(), size_t(10));
            for (size_t i = 0; i < max_aspects; ++i) {
                const auto& aspect = chart.aspects[i];
                int p1 = static_cast<int>(aspect.planet1);
                int p2 = static_cast<int>(aspect.planet2);
                
                if (p1 < 11 && p2 < 11) {
                    std::cout << planet_names[p1] << " - " << planet_names[p2] << ": ";
                    print_aspects({aspect});
                }
            }
        }
        
        std::cout << "\n═══════════════════════════════════════════\n";
        std::cout << "Berechnung erfolgreich abgeschlossen!\n";
        std::cout << "═══════════════════════════════════════════\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "FEHLER: " << e.what() << std::endl;
        return 1;
    }
}
