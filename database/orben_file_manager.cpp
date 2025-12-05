#include "orben_file_manager.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>

// Konstanten aus dem Legacy-Code
constexpr int MAX_PLANET = 17;
constexpr int MAX_HAUS = 12;
constexpr int ASPEKTE = 7;

OrbenFileManager::OrbenFileManager() {
    // Initialisiere Seek-Positionen mit 0
    seek_positions_.lStart = 0;
    seek_positions_.lOrben = 0;
    seek_positions_.lTrans = 0;
    seek_positions_.lFarben = 0;
}

void OrbenFileManager::InitializeHardcodedDefaults(OrbenData& data) {
    // Hard-coded Defaults aus sOrben() im Legacy-Code
    // Radix Planeten: 2.5
    for (int i = 0; i < MAX_PLANET; ++i) {
        for (int j = 0; j < ASPEKTE; ++j) {
            data.radix_planet[i][j] = 2.5f;
        }
    }
    
    // Radix Häuser: 0.2
    for (int i = 0; i < MAX_HAUS; ++i) {
        for (int j = 0; j < ASPEKTE; ++j) {
            data.radix_haus[i][j] = 0.2f;
        }
    }
    
    // Transit Planeten: 0.8
    for (int i = 0; i < MAX_PLANET; ++i) {
        for (int j = 0; j < ASPEKTE; ++j) {
            data.transit_planet[i][j] = 0.8f;
        }
    }
    
    // Transit Häuser: 0.15
    for (int i = 0; i < MAX_HAUS; ++i) {
        for (int j = 0; j < ASPEKTE; ++j) {
            data.transit_haus[i][j] = 0.15f;
        }
    }
    
    // Synastrie Planeten: 2.0
    for (int i = 0; i < MAX_PLANET; ++i) {
        for (int j = 0; j < ASPEKTE; ++j) {
            data.synastrie_planet[i][j] = 2.0f;
        }
    }
    
    // Synastrie Häuser: 0.15
    for (int i = 0; i < MAX_HAUS; ++i) {
        for (int j = 0; j < ASPEKTE; ++j) {
            data.synastrie_haus[i][j] = 0.15f;
        }
    }
}

bool OrbenFileManager::LoadFromIniFile(OrbenData& data, const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Konnte " << filepath << " nicht öffnen. Verwende Defaults." << std::endl;
        LoadFromDefaultFile(data);
        return false;
    }
    
    // Lese Seek-Positionen vom Anfang der Datei
    if (!ReadSeekPositions(file)) {
        std::cerr << "Fehler beim Lesen der Seek-Positionen." << std::endl;
        file.close();
        LoadFromDefaultFile(data);
        return false;
    }
    
    // Prüfe ob Seek-Positionen plausibel sind (< 10 MB)
    if (seek_positions_.lOrben == 0 || seek_positions_.lOrben > 10000000) {
        std::cerr << "Ungültige Orben-Position (" << seek_positions_.lOrben << "). Datei nicht initialisiert. Verwende Defaults." << std::endl;
        file.close();
        LoadFromDefaultFile(data);
        return false;
    }
    
    file.seekg(seek_positions_.lOrben, std::ios::beg);
    
    // Lese Orben-Daten
    if (!ReadOrbenData(file, data)) {
        std::cerr << "Fehler beim Lesen der Orben-Daten." << std::endl;
        file.close();
        LoadFromDefaultFile(data);
        return false;
    }
    
    file.close();
    return true;
}

bool OrbenFileManager::SaveToIniFile(const OrbenData& data, const std::string& filepath) {
    // 1:1 Port von sWriteIni aus legacy/auhelper.c
    // Erstelle eine neue INI-Datei mit korrekter Struktur
    
    std::ofstream file(filepath, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Konnte " << filepath << " nicht zum Schreiben öffnen." << std::endl;
        return false;
    }
    
    // Initialisiere Seek-Positionen
    seek_positions_.lStart = sizeof(SeekPositions);
    
    // Reserviere Platz für Seek-Positionen (wird am Ende aktualisiert)
    file.write(reinterpret_cast<const char*>(&seek_positions_), sizeof(SeekPositions));
    
    // Schreibe Dummy-Daten für sSelHaus (5 shorts = 10 bytes)
    short dummy_sel[5] = {0, 0, 0, 0, 0};
    file.write(reinterpret_cast<const char*>(dummy_sel), sizeof(dummy_sel));
    
    // Merke Position für Orben-Daten
    seek_positions_.lOrben = file.tellp();
    
    // Schreibe Orben-Daten (1:1 wie sWriteOrb)
    if (!WriteOrbenData(file, data)) {
        std::cerr << "Fehler beim Schreiben der Orben-Daten." << std::endl;
        file.close();
        return false;
    }
    
    // Merke Position für Transit-Selektion
    seek_positions_.lTrans = file.tellp();
    
    // Schreibe Dummy Transit-Selektion (MAX_TRANS_SEL bytes)
    // MAX_TRANS_SEL = (MAX_PLANET*(MAX_PLANET+5)*ASPEKTE)+MAX_CHECK
    // = (17*(17+5)*7)+6 = 2618 + 6 = 2624
    const int MAX_TRANS_SEL = (17 * 22 * 7) + 6;
    std::vector<char> dummy_trans(MAX_TRANS_SEL, 1);
    file.write(dummy_trans.data(), MAX_TRANS_SEL);
    
    // Merke Position für Farben
    seek_positions_.lFarben = file.tellp();
    
    // Schreibe Dummy-Farben (2 * COLORS * sizeof(COLORREF))
    // COLORS = ca. 20, COLORREF = 4 bytes
    const int COLORS = 20;
    std::vector<int> dummy_colors(COLORS * 3, 0xC0C0C0); // Grau
    file.write(reinterpret_cast<const char*>(dummy_colors.data()), dummy_colors.size() * sizeof(int));
    
    // Gehe zurück zum Anfang und schreibe die Seek-Positionen
    file.seekp(0, std::ios::beg);
    file.write(reinterpret_cast<const char*>(&seek_positions_), sizeof(SeekPositions));
    
    file.close();
    
    std::cout << "Orben erfolgreich in " << filepath << " gespeichert." << std::endl;
    std::cout << "  lOrben: " << seek_positions_.lOrben << std::endl;
    std::cout << "  lTrans: " << seek_positions_.lTrans << std::endl;
    std::cout << "  lFarben: " << seek_positions_.lFarben << std::endl;
    
    return true;
}

bool OrbenFileManager::LoadFromDefaultFile(OrbenData& data, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Konnte " << filepath << " nicht öffnen. Verwende Hard-Coded Defaults." << std::endl;
        InitializeHardcodedDefaults(data);
        return false;
    }
    
    // Initialisiere mit Hard-Coded Defaults als Fallback
    InitializeHardcodedDefaults(data);
    
    // Aspekt-Namen in default.dat (in dieser Reihenfolge)
    const std::string aspect_sections[] = {
        "[KONJUNKTION]",
        "[HALBSEXTIL]", 
        "[SEXTIL]",
        "[QUADRAT]",
        "[TRIGON]",
        "[QUINCUNX]",
        "[OPPOSITION]"
    };
    
    // Planeten-Kürzel in default.dat
    const std::string planet_abbrev[] = {
        "SON", "MON", "MER", "VEN", "MAR", "JUP", "SAT",
        "URA", "NEP", "PLU", "NOD", "LIL", "CHI", "CER",
        "PAL", "JUN", "VES"
    };
    
    std::string line;
    int current_aspect = -1;
    int planet_row = 0;
    
    while (std::getline(file, line)) {
        // Entferne führende/trailing Whitespace
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        line = line.substr(start);
        
        // Überspringe Kommentare
        if (line[0] == ';') continue;
        
        // Prüfe auf Aspekt-Sektion
        for (int i = 0; i < 7; ++i) {
            if (line.find(aspect_sections[i]) != std::string::npos) {
                current_aspect = i;
                planet_row = 0;
                break;
            }
        }
        
        if (current_aspect == -1) continue;
        
        // Prüfe ob Zeile mit Planeten-Kürzel beginnt
        bool is_planet_line = false;
        int planet_index = -1;
        for (int i = 0; i < MAX_PLANET; ++i) {
            if (line.find(planet_abbrev[i]) == 0) {
                is_planet_line = true;
                planet_index = i;
                break;
            }
        }
        
        if (!is_planet_line) continue;
        
        // Parse die Orben-Werte aus der Zeile
        // Format: "SON  10  10  10  10  10  10  10  10  10 10   5   5   5   5   5   5   3   3   3   3   3"
        std::istringstream iss(line);
        std::string planet_name;
        iss >> planet_name; // Überspringe Planeten-Namen
        
        // Lese die Werte (diagonal: Planet mit sich selbst ist der planet_index-te Wert)
        float value;
        int col = 0;
        while (iss >> value && col < MAX_PLANET) {
            if (col == planet_index) {
                // Diagonal-Wert: Planet mit sich selbst
                // Konvertiere von Grad zu Halbgrad (wie im Legacy-Code: fOrb/2)
                data.radix_planet[planet_index][current_aspect] = value / 2.0f;
                data.transit_planet[planet_index][current_aspect] = value / 2.0f * 0.32f; // Transit: ~32% von Radix
                data.synastrie_planet[planet_index][current_aspect] = value / 2.0f * 0.8f; // Synastrie: ~80% von Radix
                break;
            }
            col++;
        }
    }
    
    file.close();
    return true;
}

bool OrbenFileManager::ReadSeekPositions(std::istream& file) {
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(&seek_positions_), sizeof(SeekPositions));
    return file.good();
}

bool OrbenFileManager::WriteSeekPositions(std::ostream& file) {
    file.seekp(0, std::ios::beg);
    file.write(reinterpret_cast<const char*>(&seek_positions_), sizeof(SeekPositions));
    return file.good();
}

bool OrbenFileManager::ReadOrbenData(std::istream& file, OrbenData& data) {
    // Lese die Daten im Legacy-Format:
    // 1. Radix Planeten (MAX_PLANET * MAX_PLANET * ASPEKTE floats)
    // 2. Radix Häuser (MAX_PLANET * MAX_HAUS * ASPEKTE floats)
    // 3. Transit Planeten
    // 4. Transit Häuser
    // 5. Synastrie Planeten
    // 6. Synastrie Häuser
    
    // Temporäre Arrays für das Legacy-Format (17x17x7)
    std::vector<float> temp_radix_planet(MAX_PLANET * MAX_PLANET * ASPEKTE);
    std::vector<float> temp_radix_haus(MAX_PLANET * MAX_HAUS * ASPEKTE);
    std::vector<float> temp_transit_planet(MAX_PLANET * MAX_PLANET * ASPEKTE);
    std::vector<float> temp_transit_haus(MAX_PLANET * MAX_HAUS * ASPEKTE);
    std::vector<float> temp_synastrie_planet(MAX_PLANET * MAX_PLANET * ASPEKTE);
    std::vector<float> temp_synastrie_haus(MAX_PLANET * MAX_HAUS * ASPEKTE);
    
    file.read(reinterpret_cast<char*>(temp_radix_planet.data()), temp_radix_planet.size() * sizeof(float));
    file.read(reinterpret_cast<char*>(temp_radix_haus.data()), temp_radix_haus.size() * sizeof(float));
    file.read(reinterpret_cast<char*>(temp_transit_planet.data()), temp_transit_planet.size() * sizeof(float));
    file.read(reinterpret_cast<char*>(temp_transit_haus.data()), temp_transit_haus.size() * sizeof(float));
    file.read(reinterpret_cast<char*>(temp_synastrie_planet.data()), temp_synastrie_planet.size() * sizeof(float));
    file.read(reinterpret_cast<char*>(temp_synastrie_haus.data()), temp_synastrie_haus.size() * sizeof(float));
    
    if (!file.good()) {
        return false;
    }
    
    // Konvertiere vom Legacy-Format (17x17x7) zu unserem Format (17x7)
    // Im Legacy-Code: pfOrbenPlanet[(sPlanetA*MAX_PLANET+sPlanetB) + sA*MAX_PLANET*MAX_PLANET]
    // Wir zeigen nur die Diagonal-Werte (Planet mit sich selbst)
    for (int planet = 0; planet < MAX_PLANET; ++planet) {
        for (int aspekt = 0; aspekt < ASPEKTE; ++aspekt) {
            int legacy_index = (planet * MAX_PLANET + planet) + aspekt * MAX_PLANET * MAX_PLANET;
            data.radix_planet[planet][aspekt] = temp_radix_planet[legacy_index];
            data.transit_planet[planet][aspekt] = temp_transit_planet[legacy_index];
            data.synastrie_planet[planet][aspekt] = temp_synastrie_planet[legacy_index];
        }
    }
    
    for (int haus = 0; haus < MAX_HAUS; ++haus) {
        for (int aspekt = 0; aspekt < ASPEKTE; ++aspekt) {
            int legacy_index = haus + aspekt * MAX_PLANET * MAX_HAUS;
            data.radix_haus[haus][aspekt] = temp_radix_haus[legacy_index];
            data.transit_haus[haus][aspekt] = temp_transit_haus[legacy_index];
            data.synastrie_haus[haus][aspekt] = temp_synastrie_haus[legacy_index];
        }
    }
    
    return true;
}

bool OrbenFileManager::WriteOrbenData(std::ostream& file, const OrbenData& data) {
    // Konvertiere von unserem Format (17x7) zum Legacy-Format (17x17x7)
    std::vector<float> temp_radix_planet(MAX_PLANET * MAX_PLANET * ASPEKTE);
    std::vector<float> temp_radix_haus(MAX_PLANET * MAX_HAUS * ASPEKTE);
    std::vector<float> temp_transit_planet(MAX_PLANET * MAX_PLANET * ASPEKTE);
    std::vector<float> temp_transit_haus(MAX_PLANET * MAX_HAUS * ASPEKTE);
    std::vector<float> temp_synastrie_planet(MAX_PLANET * MAX_PLANET * ASPEKTE);
    std::vector<float> temp_synastrie_haus(MAX_PLANET * MAX_HAUS * ASPEKTE);
    
    // Initialisiere mit Defaults
    std::fill(temp_radix_planet.begin(), temp_radix_planet.end(), 2.5f);
    std::fill(temp_radix_haus.begin(), temp_radix_haus.end(), 0.2f);
    std::fill(temp_transit_planet.begin(), temp_transit_planet.end(), 0.8f);
    std::fill(temp_transit_haus.begin(), temp_transit_haus.end(), 0.15f);
    std::fill(temp_synastrie_planet.begin(), temp_synastrie_planet.end(), 2.0f);
    std::fill(temp_synastrie_haus.begin(), temp_synastrie_haus.end(), 0.15f);
    
    // Setze die Diagonal-Werte (Planet mit sich selbst)
    for (int planet = 0; planet < MAX_PLANET; ++planet) {
        for (int aspekt = 0; aspekt < ASPEKTE; ++aspekt) {
            int legacy_index = (planet * MAX_PLANET + planet) + aspekt * MAX_PLANET * MAX_PLANET;
            temp_radix_planet[legacy_index] = data.radix_planet[planet][aspekt];
            temp_transit_planet[legacy_index] = data.transit_planet[planet][aspekt];
            temp_synastrie_planet[legacy_index] = data.synastrie_planet[planet][aspekt];
        }
    }
    
    for (int haus = 0; haus < MAX_HAUS; ++haus) {
        for (int aspekt = 0; aspekt < ASPEKTE; ++aspekt) {
            int legacy_index = haus + aspekt * MAX_PLANET * MAX_HAUS;
            temp_radix_haus[legacy_index] = data.radix_haus[haus][aspekt];
            temp_transit_haus[legacy_index] = data.transit_haus[haus][aspekt];
            temp_synastrie_haus[legacy_index] = data.synastrie_haus[haus][aspekt];
        }
    }
    
    // Schreibe im Legacy-Format
    file.write(reinterpret_cast<const char*>(temp_radix_planet.data()), temp_radix_planet.size() * sizeof(float));
    file.write(reinterpret_cast<const char*>(temp_radix_haus.data()), temp_radix_haus.size() * sizeof(float));
    file.write(reinterpret_cast<const char*>(temp_transit_planet.data()), temp_transit_planet.size() * sizeof(float));
    file.write(reinterpret_cast<const char*>(temp_transit_haus.data()), temp_transit_haus.size() * sizeof(float));
    file.write(reinterpret_cast<const char*>(temp_synastrie_planet.data()), temp_synastrie_planet.size() * sizeof(float));
    file.write(reinterpret_cast<const char*>(temp_synastrie_haus.data()), temp_synastrie_haus.size() * sizeof(float));
    
    return file.good();
}
