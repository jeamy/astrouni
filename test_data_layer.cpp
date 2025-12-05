#include "database/legacy_data_manager.h"
#include <iostream>
#include <vector>

int main() {
    astro::LegacyDataManager manager;
    std::vector<astro::LegacyRadix> persons;
    
    std::string filename = "legacy/astronam.dat"; // Pfad zur Legacy-Datei
    
    std::cout << "Reading " << filename << "..." << std::endl;
    
    if (manager.ReadAllPersons(filename, persons)) {
        std::cout << "Successfully read " << persons.size() << " persons." << std::endl;
        for (const auto& p : persons) {
            std::cout << "Name: " << p.vorname << " " << p.name 
                      << ", Ort: " << p.ort 
                      << ", Date: " << p.rFix.sTag << "." << p.rFix.sMonat << "." << p.rFix.sJahr 
                      << std::endl;
        }
    } else {
        std::cout << "Failed to read file." << std::endl;
    }

    std::cout << "\nReading legacy/astroger.dat..." << std::endl;
    std::vector<astro::LegacyOrte> orte;
    if (manager.ReadOrte("legacy/astroger.dat", orte)) {
        std::cout << "Successfully read " << orte.size() << " locations." << std::endl;
        if (!orte.empty()) {
            std::cout << "First location: " << orte[0].szLand << " - Lat: " << orte[0].dBreite << ", Lon: " << orte[0].dLange << std::endl;
        }
    } else {
        std::cout << "Failed to read locations." << std::endl;
    }
    
    return 0;
}
