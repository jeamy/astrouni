// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project

#include "chart_renderer.h"
#include <cmath>
#include <algorithm>

ChartRenderer::ChartRenderer() {
    // Standard-Farben (professionell)
    colors_.background = wxColour(250, 250, 250);
    colors_.circle_outer = wxColour(40, 40, 40);
    colors_.circle_inner = wxColour(100, 100, 100);
    colors_.houses = wxColour(150, 150, 150);
    colors_.planets = wxColour(0, 80, 160);
    colors_.aspects_major = wxColour(200, 0, 0);
    colors_.aspects_minor = wxColour(0, 120, 200);
    colors_.aspects_hard = wxColour(180, 0, 0);
    colors_.text = wxColour(40, 40, 40);
    colors_.zodiac_signs = wxColour(120, 120, 120);
}

void ChartRenderer::SetColors(const Colors& colors) {
    colors_ = colors;
}

void ChartRenderer::DrawChart(wxDC& dc, const astro::AstroCalculator::ChartData& chart,
                             int width, int height) {
    // Hintergrund immer zeichnen
    dc.SetBrush(wxBrush(colors_.background));
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(0, 0, width, height);

    // Schutz: sehr kleine Größen ignorieren (beim ersten Paint kann Größe 0 sein)
    if (width <= 0 || height <= 0) {
        return;
    }

    int cx = width / 2;
    int cy = height / 2;
    int base = std::min(cx, cy);
    if (base <= 0) {
        return;
    }

    int radius = std::max(20, base - 60);
    int radius_aspects = std::max(5, radius - 80);

    // Tierkreis-Kreis mit Zeichen
    DrawZodiacCircle(dc, cx, cy, radius);
    
    // Häuser (zeichnet auch den inneren Kreis)
    int inner_radius = radius / 2;
    DrawHouses(dc, cx, cy, radius, chart.houses);
    
    // Aspekte INNERHALB des inneren Kreises
    DrawAspects(dc, cx, cy, inner_radius - 10, chart.aspects, chart.planets);
    
    // Planeten am äußeren Ring
    DrawPlanets(dc, cx, cy, radius, chart.planets);
}

void ChartRenderer::DrawZodiacCircle(wxDC& dc, int cx, int cy, int radius) {
    // Äußerer Kreis
    dc.SetPen(wxPen(colors_.circle_outer, 2));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawCircle(cx, cy, radius);
    
    // Innerer Kreis
    dc.SetPen(wxPen(colors_.circle_inner, 1));
    {
        int inner = std::max(5, radius - 40);
        dc.DrawCircle(cx, cy, inner);
    }
    
    // Tierkreiszeichen (12 Zeichen à 30°)
    dc.SetTextForeground(colors_.zodiac_signs);
    wxFont font(18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "AstroUniverse");
    if (!font.IsOk()) {
        font = wxFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    }
    dc.SetFont(font);
    
    for (int sign = 0; sign < 12; ++sign) {
        double angle = (90.0 - sign * 30.0 - 15.0) * M_PI / 180.0;  // Mitte des Zeichens
        int x = cx + (radius + 20) * cos(angle);
        int y = cy - (radius + 20) * sin(angle);
        
        wxString symbol = GetZodiacSymbol(sign);
        wxSize text_size = dc.GetTextExtent(symbol);
        dc.DrawText(symbol, x - text_size.x / 2, y - text_size.y / 2);
        
        // Trennlinien zwischen Zeichen
        double line_angle = (90.0 - sign * 30.0) * M_PI / 180.0;
        int x1 = cx + (radius - 40) * cos(line_angle);
        int y1 = cy - (radius - 40) * sin(line_angle);
        int x2 = cx + radius * cos(line_angle);
        int y2 = cy - radius * sin(line_angle);
        
        dc.SetPen(wxPen(colors_.zodiac_signs, 1, wxPENSTYLE_DOT));
        dc.DrawLine(x1, y1, x2, y2);
    }
}

void ChartRenderer::DrawHouses(wxDC& dc, int cx, int cy, int radius,
                              const std::vector<astro::HouseCusp>& houses) {
    dc.SetTextForeground(colors_.text);
    
    wxFont font = dc.GetFont();
    font.SetPointSize(10);
    dc.SetFont(font);
    
    // Mittelkreis (halber Radius)
    int inner_radius = radius / 2;
    dc.SetPen(wxPen(colors_.circle_inner, 1));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawCircle(cx, cy, inner_radius);
    
    for (const auto& cusp : houses) {
        double angle = (90.0 - cusp.longitude) * M_PI / 180.0;
        
        // Linie vom Mittelkreis zum äußeren Kreis (durchgezogen)
        int x1 = cx + inner_radius * cos(angle);
        int y1 = cy - inner_radius * sin(angle);
        int x2 = cx + radius * cos(angle);
        int y2 = cy - radius * sin(angle);
        
        // Dickere Linie für AC (Haus 1) und MC (Haus 10)
        if (cusp.house_number == 1 || cusp.house_number == 10) {
            dc.SetPen(wxPen(colors_.houses, 3, wxPENSTYLE_SOLID));
        } else {
            dc.SetPen(wxPen(colors_.houses, 2, wxPENSTYLE_SOLID));
        }
        
        dc.DrawLine(x1, y1, x2, y2);
        
        // Hausnummer
        wxString house_num = wxString::Format("%d", cusp.house_number);
        int tx = cx + (radius - 20) * cos(angle);
        int ty = cy - (radius - 20) * sin(angle);
        wxSize text_size = dc.GetTextExtent(house_num);
        dc.DrawText(house_num, tx - text_size.x / 2, ty - text_size.y / 2);
    }
}

void ChartRenderer::DrawPlanets(wxDC& dc, int cx, int cy, int radius,
                               const std::vector<astro::PlanetPosition>& planets) {
    dc.SetTextForeground(colors_.planets);
    
    wxFont font(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "AstroUniverse");
    if (!font.IsOk()) {
        font = wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    }
    dc.SetFont(font);
    
    // Planeten direkt am äußeren Ring (zwischen innerem Kreis und äußerem Kreis)
    int planet_radius = radius - 20;
    
    for (size_t i = 0; i < planets.size() && i < astro::AstroCalculator::DEFAULT_PLANETS.size(); ++i) {
        const auto& pos = planets[i];
        astro::Planet planet = astro::AstroCalculator::DEFAULT_PLANETS[i];
        
        // Planet-Symbol
        wxString symbol = GetPlanetSymbol(planet);
        
        // Farbe je nach Planet
        if (planet == astro::Planet::Sun) {
            dc.SetTextForeground(wxColour(255, 140, 0));  // Orange
        } else if (planet == astro::Planet::Moon) {
            dc.SetTextForeground(wxColour(100, 100, 200));  // Hellblau
        } else if (planet == astro::Planet::Mars) {
            dc.SetTextForeground(wxColour(200, 0, 0));  // Rot
        } else if (planet == astro::Planet::Venus) {
            dc.SetTextForeground(wxColour(0, 150, 100));  // Grün
        } else {
            dc.SetTextForeground(colors_.planets);
        }
        
        // Position direkt am Ring (exakte Längengrad-Position)
        double angle = (90.0 - pos.longitude) * M_PI / 180.0;
        int x = cx + planet_radius * cos(angle);
        int y = cy - planet_radius * sin(angle);
        
        wxSize text_size = dc.GetTextExtent(symbol);
        dc.DrawText(symbol, x - text_size.x / 2, y - text_size.y / 2);
        
        // Retrograde-Marker (kleiner, direkt daneben)
        if (pos.is_retrograde) {
            wxFont small_font = dc.GetFont();
            small_font.SetPointSize(10);
            dc.SetFont(small_font);
            dc.SetTextForeground(*wxRED);
            dc.DrawText("R", x + text_size.x / 2 + 2, y - text_size.y / 2);
            dc.SetFont(font);
        }
    }
}

void ChartRenderer::DrawAspects(wxDC& dc, int cx, int cy, int radius,
                               const std::vector<astro::Aspect>& aspects,
                               const std::vector<astro::PlanetPosition>& planets) {
    for (const auto& aspect : aspects) {
        // Finde Planeten-Positionen
        int idx1 = static_cast<int>(aspect.planet1);
        int idx2 = static_cast<int>(aspect.planet2);
        
        if (idx1 >= planets.size() || idx2 >= planets.size()) continue;
        
        double angle1 = (90.0 - planets[idx1].longitude) * M_PI / 180.0;
        double angle2 = (90.0 - planets[idx2].longitude) * M_PI / 180.0;
        
        int x1 = cx + radius * cos(angle1);
        int y1 = cy - radius * sin(angle1);
        int x2 = cx + radius * cos(angle2);
        int y2 = cy - radius * sin(angle2);
        
        // Farbe und Linienstärke je nach Aspekt
        wxColour color = GetAspectColor(aspect.type);
        int width = GetAspectLineWidth(aspect.type);
        
        dc.SetPen(wxPen(color, width, wxPENSTYLE_SOLID));
        dc.DrawLine(x1, y1, x2, y2);
    }
}

wxString ChartRenderer::GetPlanetSymbol(astro::Planet planet) {
    // Unicode-Symbole für Planeten (AstroUniverse Font)
    switch (planet) {
        case astro::Planet::Sun: return wxString::FromUTF8("\u2609");      // ☉
        case astro::Planet::Moon: return wxString::FromUTF8("\u263D");     // ☽
        case astro::Planet::Mercury: return wxString::FromUTF8("\u263F");  // ☿
        case astro::Planet::Venus: return wxString::FromUTF8("\u2640");    // ♀
        case astro::Planet::Mars: return wxString::FromUTF8("\u2642");     // ♂
        case astro::Planet::Jupiter: return wxString::FromUTF8("\u2643");  // ♃
        case astro::Planet::Saturn: return wxString::FromUTF8("\u2644");   // ♄
        case astro::Planet::Uranus: return wxString::FromUTF8("\u2645");   // ♅
        case astro::Planet::Neptune: return wxString::FromUTF8("\u2646");  // ♆
        case astro::Planet::Pluto: return wxString::FromUTF8("\u2647");    // ♇
        case astro::Planet::MeanNode: return wxString::FromUTF8("\u260A"); // ☊
        default: return "?";
    }
}

wxString ChartRenderer::GetZodiacSymbol(int sign) {
    // Unicode-Symbole für Tierkreiszeichen (AstroUniverse Font)
    const wxString symbols[] = {
        wxString::FromUTF8("\u2648"),  // ♈ Widder
        wxString::FromUTF8("\u2649"),  // ♉ Stier
        wxString::FromUTF8("\u264A"),  // ♊ Zwillinge
        wxString::FromUTF8("\u264B"),  // ♋ Krebs
        wxString::FromUTF8("\u264C"),  // ♌ Löwe
        wxString::FromUTF8("\u264D"),  // ♍ Jungfrau
        wxString::FromUTF8("\u264E"),  // ♎ Waage
        wxString::FromUTF8("\u264F"),  // ♏ Skorpion
        wxString::FromUTF8("\u2650"),  // ♐ Schütze
        wxString::FromUTF8("\u2651"),  // ♑ Steinbock
        wxString::FromUTF8("\u2652"),  // ♒ Wassermann
        wxString::FromUTF8("\u2653")   // ♓ Fische
    };
    return (sign >= 0 && sign < 12) ? symbols[sign] : "?";
}

wxColour ChartRenderer::GetAspectColor(astro::AspectType type) {
    using astro::AspectType;
    switch (type) {
        case AspectType::Conjunction:
        case AspectType::Opposition:
            return colors_.aspects_major;
        case AspectType::Square:
            return colors_.aspects_hard;
        case AspectType::Trine:
        case AspectType::Sextile:
            return colors_.aspects_minor;
        default:
            return wxColour(150, 150, 150);
    }
}

int ChartRenderer::GetAspectLineWidth(astro::AspectType type) {
    using astro::AspectType;
    switch (type) {
        case AspectType::Conjunction:
        case AspectType::Opposition:
            return 2;
        case AspectType::Square:
        case AspectType::Trine:
            return 2;
        default:
            return 1;
    }
}

std::vector<ChartRenderer::LabelPosition> ChartRenderer::CalculateLabelPositions(
    const std::vector<astro::PlanetPosition>& planets, int cx, int cy, int radius) {
    
    std::vector<LabelPosition> positions;
    const double min_distance = 20.0;  // Minimaler Abstand zwischen Labels
    
    for (const auto& planet : planets) {
        double angle = (90.0 - planet.longitude) * M_PI / 180.0;
        
        LabelPosition pos;
        pos.angle = angle;
        pos.x = cx + radius * cos(angle);
        pos.y = cy - radius * sin(angle);
        pos.used = false;
        
        // Prüfe Kollision mit existierenden Labels
        bool collision = false;
        for (const auto& existing : positions) {
            double dx = pos.x - existing.x;
            double dy = pos.y - existing.y;
            double dist = sqrt(dx * dx + dy * dy);
            
            if (dist < min_distance) {
                collision = true;
                // Verschiebe leicht nach außen
                pos.x = cx + (radius + 15) * cos(angle);
                pos.y = cy - (radius + 15) * sin(angle);
                break;
            }
        }
        
        positions.push_back(pos);
    }
    
    return positions;
}
