// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of legacy auwurzel.c rendering functions

#pragma once

#include "astro/calculations.h"
#include <wx/dc.h>
#include <vector>

namespace legacy {

// Legacy-Konstanten (exakt wie astrouni.h)
constexpr double KREIS_STZ = 0.882;
constexpr double KREIS_10_GRAD = 0.824;
constexpr double KREIS_3_GRAD = 0.765;
constexpr double KREIS_5_GRAD = 0.735;
constexpr double KREIS_PLANET = 0.677;
constexpr double KREIS_ASP = 0.300;
constexpr double PI = 3.14159265358979323846;

// Legacy-Farben (exakt wie astrouni.h)
struct LegacyColors {
    wxColour hinter;      // COL_HINTER - Hintergrund
    wxColour radix;       // COL_RADIX - Radix-Kreis
    wxColour asz;         // COL_ASZ - Aszendent
    wxColour hauser;      // COL_HAUSER - Häuser
    wxColour planet_tic;  // COL_PLANET_TIC - Planeten-Tick
    wxColour planet_tict; // COL_PLANET_TICT - Transit-Tick
    wxColour plan;        // COL_PLAN - Planet
    wxColour plan_t;      // COL_PLAN_T - Transit-Planet
    wxColour txt;         // COL_TXT - Text
    wxColour feuer;       // COL_FEUER - Feuer
    wxColour erde;        // COL_ERDE - Erde
    wxColour luft;        // COL_LUFT - Luft
    wxColour wasser;      // COL_WASSER - Wasser
    wxColour konjunktion; // Konjunktion
    wxColour sextil;      // Sextil
    wxColour quadratur;   // Quadratur
    wxColour trigon;      // Trigon
    wxColour opposition;  // Opposition
    
    static LegacyColors Screen() {
        return {
            wxColour(192, 192, 192),  // WHITE (Hintergrund)
            wxColour(128, 128, 128),  // GRAY (Radix)
            wxColour(255, 255, 255),  // LWHITE (Aszendent)
            wxColour(128, 128, 128),  // GRAY (Häuser)
            wxColour(128, 0, 128),    // MAGENTA (Planet-Tick)
            wxColour(255, 0, 255),    // LMAGENTA (Transit-Tick)
            wxColour(0, 0, 255),      // LBLUE (Planet)
            wxColour(0, 128, 128),    // CYAN (Transit-Planet)
            wxColour(0, 0, 0),        // BLACK (Text)
            wxColour(255, 0, 0),      // LRED (Feuer)
            wxColour(128, 64, 0),     // BROWN (Erde)
            wxColour(255, 255, 0),    // YELLOW (Luft)
            wxColour(0, 0, 128),      // BLUE (Wasser)
            wxColour(128, 0, 0),      // RED (Konjunktion)
            wxColour(0, 128, 0),      // GREEN (Sextil)
            wxColour(128, 0, 0),      // RED (Quadratur)
            wxColour(0, 128, 0),      // GREEN (Trigon)
            wxColour(128, 0, 128)     // MAGENTA (Opposition)
        };
    }
};

// 1:1 Port of legacy rendering
class LegacyRenderer {
public:
    LegacyRenderer();
    
    // Hauptfunktion: Radix zeichnen (entspricht sRadix + sHaus + sPlanet)
    void DrawRadix(wxDC& dc, int width, int height,
                   const astro::AstroCalculator::ChartData& chart);
    
private:
    LegacyColors colors_;
    int sMpX_, sMpY_;  // Mittelpunkt
    int sRadius_;      // Hauptradius
    
    // Legacy-Funktionen 1:1
    void sRadix(wxDC& dc, const astro::AstroCalculator::ChartData& chart);
    void sHaus(wxDC& dc, const astro::AstroCalculator::ChartData& chart);
    void sPlanet(wxDC& dc, const astro::AstroCalculator::ChartData& chart);
    void vPlanetTicDraw(wxDC& dc, const std::vector<astro::PlanetPosition>& planets, 
                        double asc_rad);
    void vPlanetDraw(wxDC& dc, const std::vector<astro::PlanetPosition>& planets,
                     double asc_rad);
    void sDrawAspekte(wxDC& dc, const std::vector<astro::Aspect>& aspects,
                      const std::vector<astro::PlanetPosition>& planets,
                      double asc_rad);
    
    // Helper
    wxString GetZodiacSymbol(int sign);
    wxString GetPlanetSymbol(astro::Planet planet);
};

} // namespace legacy
