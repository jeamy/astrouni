#pragma once

#include <wx/colour.h>
#include <string>
#include <map>

// 1:1 Port der Farb-Definitionen aus astroorb.dat
class AstroColors {
public:
    static AstroColors& instance();
    
    // Lade Farben aus astroorb.dat
    void LoadFromFile(const std::string& filepath = "data/astroorb.dat");
    
    // Getter für Farben
    wxColour GetColor(const std::string& name) const;
    
    // Standard-Farben (falls Datei nicht geladen werden kann)
    wxColour GetPlanetColor() const { return GetColor("CS_PLANET"); }
    wxColour GetAspectColor() const { return GetColor("CS_RADIX"); }
    wxColour GetBackgroundColor() const { return GetColor("CS_HINTER"); }
    wxColour GetFireColor() const { return GetColor("CS_FEUER"); }
    wxColour GetWaterColor() const { return GetColor("CS_WASSER"); }
    wxColour GetEarthColor() const { return GetColor("CS_ERDE"); }
    wxColour GetAirColor() const { return GetColor("CS_LUFT"); }
    
private:
    AstroColors();
    std::map<std::string, wxColour> colors_;
    
    void InitializeDefaults();
    wxColour ParseRGB(const std::string& rgb_string);
};
