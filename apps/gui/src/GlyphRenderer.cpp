#include "GlyphRenderer.hpp"
#include <wx/fontenum.h>

// Static member initialization
bool GlyphRenderer::s_initialized = false;
wxFont GlyphRenderer::s_planetFont;
wxFont GlyphRenderer::s_signFont;
std::map<astrocore::PlanetId, wxString> GlyphRenderer::s_planetGlyphs;
std::map<int, wxString> GlyphRenderer::s_signGlyphs;
std::map<astrocore::AspectType, wxString> GlyphRenderer::s_aspectGlyphs;
std::map<astrocore::PlanetId, wxColour> GlyphRenderer::s_planetColors;
std::map<int, wxColour> GlyphRenderer::s_signColors;
std::map<astrocore::AspectType, wxColour> GlyphRenderer::s_aspectColors;

bool GlyphRenderer::Initialize(const wxString& planetFontPath, const wxString& signFontPath) {
    // Try to find an astrological font on the system
    wxArrayString fontFamilies = wxFontEnumerator::GetFacenames();
    bool foundAstroFont = false;
    
    // Look for common astrological fonts
    wxString astroFontName;
    for (const auto& fontName : fontFamilies) {
        if (fontName.Contains("Astro") || 
            fontName.Contains("Symbol") || 
            fontName.Contains("Zodiac") || 
            fontName.Contains("Astrological")) {
            astroFontName = fontName;
            foundAstroFont = true;
            break;
        }
    }
    
    // If we found an astrological font, use it
    if (foundAstroFont) {
        s_planetFont = wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, 
                             wxFONTWEIGHT_NORMAL, false, astroFontName);
        s_signFont = s_planetFont;
    } else {
        // Fall back to Symbol font which has some astrological glyphs
        s_planetFont = wxFont(12, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, 
                             wxFONTWEIGHT_NORMAL, false, "Symbol");
        s_signFont = s_planetFont;
    }
    
    // Initialize the glyph maps
    InitializeGlyphMaps();
    
    s_initialized = true;
    return true;
}

bool GlyphRenderer::IsInitialized() {
    return s_initialized;
}

void GlyphRenderer::InitializeGlyphMaps() {
    // Planet glyphs - using ASCII approximations for now
    // In a real implementation, these would be proper Unicode characters
    // or indices into an astrological font
    s_planetGlyphs[astrocore::PlanetId::Sun] = "O";
    s_planetGlyphs[astrocore::PlanetId::Moon] = ")";
    s_planetGlyphs[astrocore::PlanetId::Mercury] = "M";
    s_planetGlyphs[astrocore::PlanetId::Venus] = "V";
    s_planetGlyphs[astrocore::PlanetId::Mars] = "T";
    s_planetGlyphs[astrocore::PlanetId::Jupiter] = "J";
    s_planetGlyphs[astrocore::PlanetId::Saturn] = "S";
    s_planetGlyphs[astrocore::PlanetId::Uranus] = "U";
    s_planetGlyphs[astrocore::PlanetId::Neptune] = "N";
    s_planetGlyphs[astrocore::PlanetId::Pluto] = "P";
    s_planetGlyphs[astrocore::PlanetId::NorthNode] = "^";
    s_planetGlyphs[astrocore::PlanetId::SouthNode] = "v";
    s_planetGlyphs[astrocore::PlanetId::Chiron] = "K";
    s_planetGlyphs[astrocore::PlanetId::Ascendant] = "A";
    s_planetGlyphs[astrocore::PlanetId::Midheaven] = "M";
    
    // Sign glyphs
    s_signGlyphs[0] = "A"; // Aries
    s_signGlyphs[1] = "B"; // Taurus
    s_signGlyphs[2] = "C"; // Gemini
    s_signGlyphs[3] = "D"; // Cancer
    s_signGlyphs[4] = "E"; // Leo
    s_signGlyphs[5] = "F"; // Virgo
    s_signGlyphs[6] = "G"; // Libra
    s_signGlyphs[7] = "H"; // Scorpio
    s_signGlyphs[8] = "I"; // Sagittarius
    s_signGlyphs[9] = "J"; // Capricorn
    s_signGlyphs[10] = "K"; // Aquarius
    s_signGlyphs[11] = "L"; // Pisces
    
    // Aspect glyphs
    s_aspectGlyphs[astrocore::AspectType::Conjunction] = "Q";
    s_aspectGlyphs[astrocore::AspectType::Opposition] = "W";
    s_aspectGlyphs[astrocore::AspectType::Trine] = "E";
    s_aspectGlyphs[astrocore::AspectType::Square] = "R";
    s_aspectGlyphs[astrocore::AspectType::Sextile] = "T";
    
    // Planet colors
    s_planetColors[astrocore::PlanetId::Sun] = wxColour(255, 215, 0);      // Gold
    s_planetColors[astrocore::PlanetId::Moon] = wxColour(192, 192, 192);   // Silver
    s_planetColors[astrocore::PlanetId::Mercury] = wxColour(100, 149, 237); // Cornflower Blue
    s_planetColors[astrocore::PlanetId::Venus] = wxColour(0, 128, 0);      // Green
    s_planetColors[astrocore::PlanetId::Mars] = wxColour(255, 0, 0);       // Red
    s_planetColors[astrocore::PlanetId::Jupiter] = wxColour(128, 0, 128);  // Purple
    s_planetColors[astrocore::PlanetId::Saturn] = wxColour(139, 69, 19);   // Brown
    s_planetColors[astrocore::PlanetId::Uranus] = wxColour(0, 191, 255);   // Deep Sky Blue
    s_planetColors[astrocore::PlanetId::Neptune] = wxColour(65, 105, 225); // Royal Blue
    s_planetColors[astrocore::PlanetId::Pluto] = wxColour(139, 0, 0);      // Dark Red
    s_planetColors[astrocore::PlanetId::NorthNode] = wxColour(0, 100, 0);  // Dark Green
    s_planetColors[astrocore::PlanetId::SouthNode] = wxColour(139, 0, 139); // Dark Magenta
    s_planetColors[astrocore::PlanetId::Chiron] = wxColour(210, 105, 30);  // Chocolate
    s_planetColors[astrocore::PlanetId::Ascendant] = wxColour(255, 0, 0);  // Red
    s_planetColors[astrocore::PlanetId::Midheaven] = wxColour(0, 0, 255);  // Blue
    
    // Sign colors - element-based
    s_signColors[0] = wxColour(255, 0, 0);       // Aries - Fire
    s_signColors[1] = wxColour(0, 128, 0);       // Taurus - Earth
    s_signColors[2] = wxColour(255, 255, 0);     // Gemini - Air
    s_signColors[3] = wxColour(0, 0, 255);       // Cancer - Water
    s_signColors[4] = wxColour(255, 0, 0);       // Leo - Fire
    s_signColors[5] = wxColour(0, 128, 0);       // Virgo - Earth
    s_signColors[6] = wxColour(255, 255, 0);     // Libra - Air
    s_signColors[7] = wxColour(0, 0, 255);       // Scorpio - Water
    s_signColors[8] = wxColour(255, 0, 0);       // Sagittarius - Fire
    s_signColors[9] = wxColour(0, 128, 0);       // Capricorn - Earth
    s_signColors[10] = wxColour(255, 255, 0);    // Aquarius - Air
    s_signColors[11] = wxColour(0, 0, 255);      // Pisces - Water
    
    // Aspect colors
    s_aspectColors[astrocore::AspectType::Conjunction] = wxColour(0, 0, 0);       // Black
    s_aspectColors[astrocore::AspectType::Opposition] = wxColour(255, 0, 0);      // Red
    s_aspectColors[astrocore::AspectType::Trine] = wxColour(0, 128, 0);           // Green
    s_aspectColors[astrocore::AspectType::Square] = wxColour(255, 0, 0);          // Red
    s_aspectColors[astrocore::AspectType::Sextile] = wxColour(0, 0, 255);         // Blue
}

void GlyphRenderer::DrawPlanetGlyph(wxGraphicsContext* gc, astrocore::PlanetId planet, 
                                   const wxPoint& position, double size, 
                                   const wxColour& color) {
    if (!s_initialized) {
        Initialize();
    }
    
    // Set up font and color
    wxFont font = s_planetFont;
    font.SetPointSize(size);
    gc->SetFont(font, color);
    
    // Get the glyph
    wxString glyph = s_planetGlyphs[planet];
    
    // Draw the glyph
    double textWidth, textHeight;
    gc->GetTextExtent(glyph, &textWidth, &textHeight);
    gc->DrawText(glyph, position.x - textWidth/2, position.y - textHeight/2);
}

void GlyphRenderer::DrawSignGlyph(wxGraphicsContext* gc, int signIndex, 
                                 const wxPoint& position, double size, 
                                 const wxColour& color) {
    if (!s_initialized) {
        Initialize();
    }
    
    // Set up font and color
    wxFont font = s_signFont;
    font.SetPointSize(size);
    gc->SetFont(font, color);
    
    // Get the glyph
    wxString glyph = s_signGlyphs[signIndex % 12];
    
    // Draw the glyph
    double textWidth, textHeight;
    gc->GetTextExtent(glyph, &textWidth, &textHeight);
    gc->DrawText(glyph, position.x - textWidth/2, position.y - textHeight/2);
}

void GlyphRenderer::DrawAspectGlyph(wxGraphicsContext* gc, astrocore::AspectType aspect, 
                                   const wxPoint& position, double size, 
                                   const wxColour& color) {
    if (!s_initialized) {
        Initialize();
    }
    
    // Set up font and color
    wxFont font = s_planetFont;
    font.SetPointSize(size);
    gc->SetFont(font, color);
    
    // Get the glyph
    wxString glyph = s_aspectGlyphs[aspect];
    
    // Draw the glyph
    double textWidth, textHeight;
    gc->GetTextExtent(glyph, &textWidth, &textHeight);
    gc->DrawText(glyph, position.x - textWidth/2, position.y - textHeight/2);
}

wxColour GlyphRenderer::GetPlanetColor(astrocore::PlanetId planet) {
    if (!s_initialized) {
        Initialize();
    }
    
    auto it = s_planetColors.find(planet);
    if (it != s_planetColors.end()) {
        return it->second;
    }
    return *wxBLACK;
}

wxColour GlyphRenderer::GetSignColor(int signIndex) {
    if (!s_initialized) {
        Initialize();
    }
    
    auto it = s_signColors.find(signIndex % 12);
    if (it != s_signColors.end()) {
        return it->second;
    }
    return *wxBLACK;
}

wxColour GlyphRenderer::GetAspectColor(astrocore::AspectType aspect) {
    if (!s_initialized) {
        Initialize();
    }
    
    auto it = s_aspectColors.find(aspect);
    if (it != s_aspectColors.end()) {
        return it->second;
    }
    return *wxBLACK;
}

// ChartGlyphHelper implementation

void ChartGlyphHelper::DrawPlanetOnWheel(wxGraphicsContext* gc, astrocore::PlanetId planet,
                                        double longitudeDeg, double latitudeDeg,
                                        const wxPoint& center, double radius,
                                        bool showLabel, double glyphSize) {
    // Calculate position on wheel
    double angle = (90.0 - longitudeDeg) * M_PI / 180.0; // Convert to radians, adjust for chart orientation
    double x = center.x + radius * cos(angle);
    double y = center.y - radius * sin(angle);
    
    // Draw the planet glyph
    wxColour color = GlyphRenderer::GetPlanetColor(planet);
    GlyphRenderer::DrawPlanetGlyph(gc, planet, wxPoint(x, y), glyphSize, color);
    
    // Draw label if requested
    if (showLabel) {
        std::string name = astrocore::get_planet_name(planet);
        wxString label = wxString::FromUTF8(name);
        
        // Set up text properties
        wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        gc->SetFont(font, color);
        
        // Calculate label position (slightly outside the glyph)
        double labelAngle = angle;
        double labelRadius = radius + 15;
        double labelX = center.x + labelRadius * cos(labelAngle);
        double labelY = center.y - labelRadius * sin(labelAngle);
        
        // Draw the label
        double textWidth, textHeight;
        gc->GetTextExtent(label, &textWidth, &textHeight);
        gc->DrawText(label, labelX - textWidth/2, labelY - textHeight/2);
    }
}

void ChartGlyphHelper::DrawSignOnWheel(wxGraphicsContext* gc, int signIndex,
                                      const wxPoint& center, double radius,
                                      bool showLabel, double glyphSize) {
    // Calculate position on wheel (center of sign)
    double signLongitude = signIndex * 30.0 + 15.0;
    double angle = (90.0 - signLongitude) * M_PI / 180.0;
    double x = center.x + (radius + 20) * cos(angle);
    double y = center.y - (radius + 20) * sin(angle);
    
    // Draw the sign glyph
    wxColour color = GlyphRenderer::GetSignColor(signIndex);
    GlyphRenderer::DrawSignGlyph(gc, signIndex, wxPoint(x, y), glyphSize, color);
    
    // Draw label if requested
    if (showLabel) {
        // Sign names
        static const wxString signNames[] = {
            "Aries", "Taurus", "Gemini", "Cancer", "Leo", "Virgo",
            "Libra", "Scorpio", "Sagittarius", "Capricorn", "Aquarius", "Pisces"
        };
        
        wxString label = signNames[signIndex % 12];
        
        // Set up text properties
        wxFont font(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        gc->SetFont(font, color);
        
        // Calculate label position (outside the glyph)
        double labelAngle = angle;
        double labelRadius = radius + 35;
        double labelX = center.x + labelRadius * cos(labelAngle);
        double labelY = center.y - labelRadius * sin(labelAngle);
        
        // Draw the label
        double textWidth, textHeight;
        gc->GetTextExtent(label, &textWidth, &textHeight);
        gc->DrawText(label, labelX - textWidth/2, labelY - textHeight/2);
    }
}

void ChartGlyphHelper::DrawAspectLines(wxGraphicsContext* gc, 
                                      const std::array<astrocore::PlanetPosition, 21>& positions,
                                      const wxPoint& center, double radius,
                                      double orbDeg) {
    // We'll use the detect_aspect function from astrocore instead of defining aspects here
    
    // Check each planet pair for aspects
    for (size_t i = 0; i < positions.size(); ++i) {
        // Skip invalid positions
        if (!positions[i].valid) continue;
        
        for (size_t j = i + 1; j < positions.size(); ++j) {
            // Skip invalid positions
            if (!positions[j].valid) continue;
            
            // Get planet longitudes
            double angle1 = positions[i].longitude;
            double angle2 = positions[j].longitude;
            
            // Detect aspect between planets
            astrocore::AspectResult aspectResult = astrocore::detect_aspect(angle1, angle2, orbDeg);
            
            // If there's an aspect, draw the line
            if (aspectResult.type != astrocore::AspectType::None) {
                // Calculate positions on the wheel
                double a1 = (90.0 - angle1) * M_PI / 180.0;
                double a2 = (90.0 - angle2) * M_PI / 180.0;
                
                double x1 = center.x + radius * 0.8 * cos(a1);
                double y1 = center.y - radius * 0.8 * sin(a1);
                double x2 = center.x + radius * 0.8 * cos(a2);
                double y2 = center.y - radius * 0.8 * sin(a2);
                
                // Set line properties based on aspect
                wxColour color = GlyphRenderer::GetAspectColor(aspectResult.type);
                wxPen pen(color, 1);
                
                switch (aspectResult.type) {
                    case astrocore::AspectType::Conjunction:
                        pen.SetWidth(2);
                        break;
                    case astrocore::AspectType::Opposition:
                        pen.SetStyle(wxPENSTYLE_LONG_DASH);
                        break;
                    case astrocore::AspectType::Trine:
                        pen.SetWidth(2);
                        break;
                    case astrocore::AspectType::Square:
                        pen.SetStyle(wxPENSTYLE_DOT);
                        break;
                    case astrocore::AspectType::Sextile:
                        pen.SetStyle(wxPENSTYLE_DOT_DASH);
                        break;
                    default:
                        pen.SetStyle(wxPENSTYLE_SHORT_DASH);
                        break;
                }
                
                gc->SetPen(pen);
                gc->StrokeLine(x1, y1, x2, y2);
            }
        }
    }
}
