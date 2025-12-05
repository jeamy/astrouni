#include "../database/orben_file_manager.h"
#include <iostream>

int main() {
    OrbenData data;
    OrbenFileManager mgr;
    
    std::cout << "=== Test: Speichern und Laden von Orben-Daten ===" << std::endl;
    
    // Lade Defaults
    std::cout << "\n1. Lade Defaults aus default.dat..." << std::endl;
    mgr.LoadFromDefaultFile(data, "data/default.dat");
    
    // Ändere einige Werte
    std::cout << "\n2. Ändere einige Werte..." << std::endl;
    data.radix_planet[0][0] = 99.9f;  // Sonne Konjunktion
    data.transit_planet[1][1] = 88.8f; // Mond Halbsextil
    data.synastrie_planet[2][2] = 77.7f; // Merkur Sextil
    
    std::cout << "   Radix Sonne Konjunktion: " << data.radix_planet[0][0] << std::endl;
    std::cout << "   Transit Mond Halbsextil: " << data.transit_planet[1][1] << std::endl;
    std::cout << "   Synastrie Merkur Sextil: " << data.synastrie_planet[2][2] << std::endl;
    
    // Speichere in neue Datei
    std::cout << "\n3. Speichere in data/test_astroini.dat..." << std::endl;
    if (mgr.SaveToIniFile(data, "data/test_astroini.dat")) {
        std::cout << "   Erfolgreich gespeichert!" << std::endl;
    } else {
        std::cout << "   Fehler beim Speichern!" << std::endl;
        return 1;
    }
    
    // Lade wieder
    std::cout << "\n4. Lade wieder aus data/test_astroini.dat..." << std::endl;
    OrbenData data2;
    if (mgr.LoadFromIniFile(data2, "data/test_astroini.dat")) {
        std::cout << "   Erfolgreich geladen!" << std::endl;
    } else {
        std::cout << "   Fehler beim Laden!" << std::endl;
        return 1;
    }
    
    // Vergleiche Werte
    std::cout << "\n5. Vergleiche Werte..." << std::endl;
    std::cout << "   Radix Sonne Konjunktion: " << data2.radix_planet[0][0] << " (erwartet: 99.9)" << std::endl;
    std::cout << "   Transit Mond Halbsextil: " << data2.transit_planet[1][1] << " (erwartet: 88.8)" << std::endl;
    std::cout << "   Synastrie Merkur Sextil: " << data2.synastrie_planet[2][2] << " (erwartet: 77.7)" << std::endl;
    
    bool success = (data2.radix_planet[0][0] == 99.9f &&
                    data2.transit_planet[1][1] == 88.8f &&
                    data2.synastrie_planet[2][2] == 77.7f);
    
    if (success) {
        std::cout << "\n✓ Test erfolgreich! Speichern und Laden funktioniert korrekt." << std::endl;
    } else {
        std::cout << "\n✗ Test fehlgeschlagen! Werte stimmen nicht überein." << std::endl;
        return 1;
    }
    
    return 0;
}
