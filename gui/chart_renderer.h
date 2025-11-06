// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project

#pragma once

#include "astro/calculations.h"
#include "astro/i18n.h"
#include <wx/wx.h>
#include <wx/dcbuffer.h>

// Chart-Rendering mit professionellem Layout
class ChartRenderer {
public:
    ChartRenderer();
    
    // Zeichne komplettes Chart
    void DrawChart(wxDC& dc, const astro::AstroCalculator::ChartData& chart, 
                   int width, int height);
    
    // Farb-Schema
    struct Colors {
        wxColour background;
        wxColour circle_outer;
        wxColour circle_inner;
        wxColour houses;
        wxColour planets;
        wxColour aspects_major;  // Konjunktion, Opposition
        wxColour aspects_minor;  // Sextil, Trigon
        wxColour aspects_hard;   // Quadrat
        wxColour text;
        wxColour zodiac_signs;
    };
    
    void SetColors(const Colors& colors);
    
private:
    Colors colors_;
    
    // Zeichnen-Funktionen
    void DrawBackground(wxDC& dc, int cx, int cy, int radius);
    void DrawZodiacCircle(wxDC& dc, int cx, int cy, int radius);
    void DrawHouses(wxDC& dc, int cx, int cy, int radius, 
                   const std::vector<astro::HouseCusp>& houses);
    void DrawPlanets(wxDC& dc, int cx, int cy, int radius,
                    const std::vector<astro::PlanetPosition>& planets);
    void DrawAspects(wxDC& dc, int cx, int cy, int radius,
                    const std::vector<astro::Aspect>& aspects,
                    const std::vector<astro::PlanetPosition>& planets);
    
    // Hilfsfunktionen
    wxString GetPlanetSymbol(astro::Planet planet);
    wxString GetZodiacSymbol(int sign);
    wxColour GetAspectColor(astro::AspectType type);
    int GetAspectLineWidth(astro::AspectType type);
    
    // Kollisions-Vermeidung für Planeten-Labels
    struct LabelPosition {
        double angle;
        int x, y;
        bool used;
    };
    std::vector<LabelPosition> CalculateLabelPositions(
        const std::vector<astro::PlanetPosition>& planets, 
        int cx, int cy, int radius);
};
