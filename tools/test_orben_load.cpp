#include "../database/orben_file_manager.h"
#include <iostream>
#include <iomanip>

const char* planet_names[] = {
    "Sonne", "Mond", "Merkur", "Venus", "Mars", "Jupiter", "Saturn",
    "Uranus", "Neptun", "Pluto", "Mondknoten", "Chiron", "Lilith",
    "Aszendent", "MC", "Pars Fortunae", "Vesta"
};

const char* aspect_names[] = {
    "Konjunktion", "Halbsextil", "Sextil", "Quadrat", "Trigon", "Quincunx", "Opposition"
};

int main() {
    OrbenData data;
    OrbenFileManager mgr;
    
    std::cout << "=== Test: Lade Default-Orben aus default.dat ===" << std::endl;
    
    if (mgr.LoadFromDefaultFile(data, "data/default.dat")) {
        std::cout << "Erfolgreich geladen!" << std::endl;
    } else {
        std::cout << "Fehler beim Laden!" << std::endl;
        return 1;
    }
    
    std::cout << "\n=== Radix Planeten Orben ===" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    
    for (int aspect = 0; aspect < 7; ++aspect) {
        std::cout << "\n" << aspect_names[aspect] << ":" << std::endl;
        for (int planet = 0; planet < 17; ++planet) {
            std::cout << "  " << std::setw(15) << std::left << planet_names[planet]
                      << ": " << std::setw(6) << std::right << data.radix_planet[planet][aspect] << "°" << std::endl;
        }
    }
    
    return 0;
}
