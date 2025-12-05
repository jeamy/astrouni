#include "astro/colors.h"
#include <fstream>
#include <sstream>
#include <iostream>

AstroColors& AstroColors::instance() {
    static AstroColors instance;
    return instance;
}

AstroColors::AstroColors() {
    InitializeDefaults();
}

void AstroColors::InitializeDefaults() {
    // Default-Farben aus astroorb.dat (falls Datei nicht geladen werden kann)
    colors_["CS_HINTER"] = wxColour(192, 192, 192);  // Hintergrund
    colors_["CS_FEUER"] = wxColour(255, 0, 0);       // Feuer (Rot)
    colors_["CS_WASSER"] = wxColour(0, 0, 128);      // Wasser (Dunkelblau)
    colors_["CS_ERDE"] = wxColour(128, 64, 0);       // Erde (Braun)
    colors_["CS_LUFT"] = wxColour(255, 255, 0);      // Luft (Gelb)
    colors_["CS_RADIX"] = wxColour(128, 128, 128);   // Radixkreise
    colors_["CS_ASZ"] = wxColour(255, 255, 255);     // Aszendent
    colors_["CS_HAUSER"] = wxColour(192, 192, 192);  // Häuser
    colors_["CS_PLANET"] = wxColour(0, 0, 255);      // Planeten (Blau)
    colors_["CS_PLANET_T"] = wxColour(0, 128, 128);  // Transit-Planeten
}

void AstroColors::LoadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Konnte " << filepath << " nicht öffnen. Verwende Default-Farben." << std::endl;
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Überspringe Kommentare und leere Zeilen
        if (line.empty() || line[0] == ';' || line[0] == '[') {
            continue;
        }
        
        // Parse Zeilen im Format: CS_NAME  R,G,B
        size_t space_pos = line.find_first_of(" \t");
        if (space_pos == std::string::npos) {
            continue;
        }
        
        std::string name = line.substr(0, space_pos);
        std::string rgb_part = line.substr(space_pos + 1);
        
        // Entferne führende Whitespace
        size_t start = rgb_part.find_first_not_of(" \t");
        if (start != std::string::npos) {
            rgb_part = rgb_part.substr(start);
        }
        
        wxColour color = ParseRGB(rgb_part);
        if (color.IsOk()) {
            colors_[name] = color;
        }
    }
    
    file.close();
    std::cout << "Farben aus " << filepath << " geladen (" << colors_.size() << " Farben)." << std::endl;
}

wxColour AstroColors::ParseRGB(const std::string& rgb_string) {
    // Parse "R,G,B" Format
    std::istringstream iss(rgb_string);
    int r, g, b;
    char comma1, comma2;
    
    if (iss >> r >> comma1 >> g >> comma2 >> b) {
        if (comma1 == ',' && comma2 == ',') {
            return wxColour(r, g, b);
        }
    }
    
    return wxColour();  // Invalid
}

wxColour AstroColors::GetColor(const std::string& name) const {
    auto it = colors_.find(name);
    if (it != colors_.end()) {
        return it->second;
    }
    return wxColour(0, 0, 0);  // Schwarz als Fallback
}
