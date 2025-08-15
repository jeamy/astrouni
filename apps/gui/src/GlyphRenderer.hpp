#pragma once

#include <wx/wx.h>
#include <wx/graphics.h>
#include <map>
#include <string>
#include "astrocore/planets.hpp"
#include "astrocore/api.hpp"

// Class for rendering astrological glyphs (planets, signs, aspects)
class GlyphRenderer {
public:
    // Initialize the renderer with the given font paths
    // Returns true if initialization was successful
    static bool Initialize(const wxString& planetFontPath = wxEmptyString,
                          const wxString& signFontPath = wxEmptyString);

    // Check if the renderer is initialized
    static bool IsInitialized();

    // Draw a planet glyph at the specified position
    static void DrawPlanetGlyph(wxGraphicsContext* gc, astrocore::PlanetId planet, 
                               const wxPoint& position, double size = 16.0,
                               const wxColour& color = *wxBLACK);

    // Draw a zodiac sign glyph at the specified position
    static void DrawSignGlyph(wxGraphicsContext* gc, int signIndex, // 0-11 for Aries-Pisces
                             const wxPoint& position, double size = 16.0,
                             const wxColour& color = *wxBLACK);

    // Draw an aspect glyph at the specified position
    static void DrawAspectGlyph(wxGraphicsContext* gc, astrocore::AspectType aspect,
                               const wxPoint& position, double size = 16.0,
                               const wxColour& color = *wxBLACK);

    // Get the color associated with a planet
    static wxColour GetPlanetColor(astrocore::PlanetId planet);

    // Get the color associated with a zodiac sign
    static wxColour GetSignColor(int signIndex);

    // Get the color associated with an aspect
    static wxColour GetAspectColor(astrocore::AspectType aspect);

private:
    static bool s_initialized;
    static wxFont s_planetFont;
    static wxFont s_signFont;
    
    // Maps for glyph characters
    static std::map<astrocore::PlanetId, wxString> s_planetGlyphs;
    static std::map<int, wxString> s_signGlyphs;
    static std::map<astrocore::AspectType, wxString> s_aspectGlyphs;
    
    // Maps for colors
    static std::map<astrocore::PlanetId, wxColour> s_planetColors;
    static std::map<int, wxColour> s_signColors;
    static std::map<astrocore::AspectType, wxColour> s_aspectColors;

    // Initialize the glyph maps
    static void InitializeGlyphMaps();
};

// Helper class for chart drawing with glyphs
class ChartGlyphHelper {
public:
    // Draw a planet on the chart wheel
    static void DrawPlanetOnWheel(wxGraphicsContext* gc, astrocore::PlanetId planet,
                                 double longitudeDeg, double latitudeDeg,
                                 const wxPoint& center, double radius,
                                 bool showLabel = true, double glyphSize = 16.0);

    // Draw a zodiac sign on the chart wheel
    static void DrawSignOnWheel(wxGraphicsContext* gc, int signIndex,
                               const wxPoint& center, double radius,
                               bool showLabel = true, double glyphSize = 16.0);

    // Draw aspect lines between planets
    static void DrawAspectLines(wxGraphicsContext* gc, 
                               const std::array<astrocore::PlanetPosition, 21>& positions,
                               const wxPoint& center, double radius,
                               double orbDeg = 3.0);
};
