// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of legacy auwurzel.c rendering functions

#include "legacy_renderer.h"
#include <cmath>
#include <algorithm>

namespace legacy {

LegacyRenderer::LegacyRenderer() 
    : colors_(LegacyColors::Screen()),
      sMpX_(0), sMpY_(0), sRadius_(0) {
}

void LegacyRenderer::DrawRadix(wxDC& dc, int width, int height,
                               const astro::AstroCalculator::ChartData& chart) {
    // Hintergrund
    dc.SetBackground(wxBrush(colors_.hinter));
    dc.Clear();
    
    // Mittelpunkt und Radius berechnen
    sMpX_ = width / 2;
    sMpY_ = height / 2;
    sRadius_ = std::min(sMpX_, sMpY_) - 40;
    
    if (sRadius_ < 60) return;
    
    // 1:1 Legacy-Reihenfolge:
    sRadix(dc, chart);      // Kreise und Gradeinteilung
    sHaus(dc, chart);       // Häusersystem
    sPlanet(dc, chart);     // Planeten
    sDrawAspekte(dc, chart.aspects, chart.planets, 
                 chart.houses.empty() ? 0.0 : chart.houses[0].longitude * PI / 180.0);
}

// 1:1 Port von sRadix() aus auwurzel.c Zeile 215-410
void LegacyRenderer::sRadix(wxDC& dc, const astro::AstroCalculator::ChartData& chart) {
    double dStzGrad = chart.houses.empty() ? PI : chart.houses[0].longitude * PI / 180.0;
    
    // Variablen wie im Legacy
    int sRI, sRG;
    int sRHZ;
    
    // Äußerster Kreis
    dc.SetPen(wxPen(colors_.radix, 1));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawCircle(sMpX_, sMpY_, sRadius_);
    
    // 2. Kreis (STZ)
    int sR = static_cast<int>(sRadius_ * KREIS_STZ);
    sRG = sR;
    dc.DrawCircle(sMpX_, sMpY_, sR);
    
    // 3. Kreis (10 Grad) - nicht gezeichnet im Legacy
    sR = static_cast<int>(sRadius_ * KREIS_10_GRAD);
    sRI = sR;
    sRHZ = sRadius_ - (sRadius_ - sRG) / 2;
    
    // 30 Grad Einteilung
    double dA = dStzGrad;
    for (int sA = 0; sA < 6; sA++) {
        int sX = static_cast<int>(cos(dA) * sRI);
        int sY = static_cast<int>(sin(dA) * sRI);
        dc.DrawLine(sMpX_ - sX, sMpY_ + sY, sMpX_ - static_cast<int>(cos(dA) * sRadius_), 
                    sMpY_ + static_cast<int>(sin(dA) * sRadius_));
        dc.DrawLine(sMpX_ + sX, sMpY_ - sY, sMpX_ + static_cast<int>(cos(dA) * sRadius_), 
                    sMpY_ - static_cast<int>(sin(dA) * sRadius_));
        
        // Tierkreiszeichen
        int sStz = (static_cast<int>(dStzGrad / (PI / 6.0)) + sA) % 12;
        wxString symbol = GetZodiacSymbol(sStz);
        
        wxFont font(28, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Astrouniverse");
        dc.SetFont(font);
        dc.SetTextForeground(colors_.txt);
        
        int sX1 = static_cast<int>(cos(dA + PI / 12.0) * sRHZ);
        int sY1 = static_cast<int>(sin(dA + PI / 12.0) * sRHZ);
        wxSize ts = dc.GetTextExtent(symbol);
        dc.DrawText(symbol, sMpX_ - sX1 - ts.x / 2, sMpY_ + sY1 - ts.y / 2);
        
        sStz = (sStz + 6) % 12;
        symbol = GetZodiacSymbol(sStz);
        dc.DrawText(symbol, sMpX_ + sX1 - ts.x / 2, sMpY_ - sY1 - ts.y / 2);
        
        dA += PI / 6.0;
    }
    
    // 5. Kreis (5 Grad)
    sRG = sR;
    sR = static_cast<int>(sRadius_ * KREIS_5_GRAD);
    sRI = sR;
    dc.DrawCircle(sMpX_, sMpY_, sR);
    
    // 6. Kreis (Aspekte)
    sR = static_cast<int>(sRadius_ * KREIS_ASP);
    dc.DrawCircle(sMpX_, sMpY_, sR);
    
    // 2 Grad Einteilung (5°-Ring)
    dA = dStzGrad;
    for (int sA = 0; sA < 90; sA++) {
        int sX = static_cast<int>(cos(dA) * sRG);
        int sY = static_cast<int>(sin(dA) * sRG);
        dc.DrawLine(sMpX_ - sX, sMpY_ + sY, 
                    sMpX_ - static_cast<int>(cos(dA) * sRI), 
                    sMpY_ + static_cast<int>(sin(dA) * sRI));
        dc.DrawLine(sMpX_ + sX, sMpY_ - sY, 
                    sMpX_ + static_cast<int>(cos(dA) * sRI), 
                    sMpY_ - static_cast<int>(sin(dA) * sRI));
        dA += PI / 90.0;
    }
}

// 1:1 Port von sHaus() aus auwurzel.c Zeile 415-471
void LegacyRenderer::sHaus(wxDC& dc, const astro::AstroCalculator::ChartData& chart) {
    if (chart.houses.empty()) return;
    
    int sRG = static_cast<int>(sRadius_ * KREIS_STZ);
    int sRH = static_cast<int>(sRadius_ * KREIS_ASP);
    int sRD = sRadius_ + 8;  // sDruber
    
    dc.SetPen(wxPen(colors_.hauser, 1));
    
    wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    dc.SetFont(font);
    dc.SetTextForeground(colors_.hauser);
    
    for (size_t sA = 0; sA < chart.houses.size() && sA < 12; sA++) {
        double dA = chart.houses[sA].longitude * PI / 180.0;
        
        // Hausnummer oder ASC/MC/IC/DSC
        wxString label;
        if (sA == 0) label = "ASC";
        else if (sA == 3) label = "IC";
        else if (sA == 6) label = "DSC";
        else if (sA == 9) label = "MC";
        else label = wxString::Format("%d", static_cast<int>(sA + 1));
        
        // Label-Position wie Legacy
        wxSize ts = dc.GetTextExtent(label);
        int sRHx = sRH + ts.x / 2;
        int sX = sMpX_ - static_cast<int>(cos(dA + 0.1) * sRHx);
        int sY = sMpY_ + static_cast<int>(sin(dA + 0.1) * sRHx);
        dc.DrawText(label, sX - ts.x / 2, sY - ts.y);
        
        // Linie
        sX = sMpX_ - static_cast<int>(cos(dA) * sRH);
        sY = sMpY_ + static_cast<int>(sin(dA) * sRH);
        
        if (sA == 0 || sA == 3 || sA == 6 || sA == 9) {
            // Kardinal: dicker bis sRD
            dc.SetPen(wxPen(colors_.asz, 3));
            int sX2 = sMpX_ - static_cast<int>(cos(dA) * sRD);
            int sY2 = sMpY_ + static_cast<int>(sin(dA) * sRD);
            dc.DrawLine(sX, sY, sX2, sY2);
            dc.SetPen(wxPen(colors_.hauser, 1));
        } else {
            // Normal: bis sRG
            int sX2 = sMpX_ - static_cast<int>(cos(dA) * sRG);
            int sY2 = sMpY_ + static_cast<int>(sin(dA) * sRG);
            dc.DrawLine(sX, sY, sX2, sY2);
        }
    }
}

// 1:1 Port von sPlanet() aus auwurzel.c Zeile 476-530
void LegacyRenderer::sPlanet(wxDC& dc, const astro::AstroCalculator::ChartData& chart) {
    if (chart.houses.empty()) return;
    double dStzGrad = chart.houses[0].longitude * PI / 180.0;
    
    vPlanetTicDraw(dc, chart.planets, dStzGrad);
    vPlanetDraw(dc, chart.planets, dStzGrad);
}

// 1:1 Port von vPlanetTicDraw() aus auwurzel.c Zeile 535-584
void LegacyRenderer::vPlanetTicDraw(wxDC& dc, const std::vector<astro::PlanetPosition>& planets, 
                                    double asc_rad) {
    int sRP = static_cast<int>(sRadius_ * KREIS_STZ);
    int sRPi = static_cast<int>(sRadius_ * KREIS_PLANET);
    int sRH = static_cast<int>(sRadius_ * KREIS_ASP);
    
    dc.SetPen(wxPen(colors_.planet_tic, 1));
    
    for (const auto& planet : planets) {
        double dA = planet.longitude * PI / 180.0;
        
        // Tick von STZ bis Planet
        int sX1 = sMpX_ - static_cast<int>(cos(dA) * sRP);
        int sY1 = sMpY_ + static_cast<int>(sin(dA) * sRP);
        int sX2 = sMpX_ - static_cast<int>(cos(dA) * sRPi);
        int sY2 = sMpY_ + static_cast<int>(sin(dA) * sRPi);
        dc.DrawLine(sX1, sY1, sX2, sY2);
        
        // Kleiner Tick am Aspektkreis
        sX1 = sMpX_ - static_cast<int>(cos(dA) * sRH);
        sY1 = sMpY_ + static_cast<int>(sin(dA) * sRH);
        sX2 = sMpX_ - static_cast<int>(cos(dA) * (sRH + 6));
        sY2 = sMpY_ + static_cast<int>(sin(dA) * (sRH + 6));
        dc.DrawLine(sX1, sY1, sX2, sY2);
    }
}

// 1:1 Port von vPlanetDraw() aus auwurzel.c Zeile 588-650
void LegacyRenderer::vPlanetDraw(wxDC& dc, const std::vector<astro::PlanetPosition>& planets,
                                 double asc_rad) {
    int sRPi = static_cast<int>(sRadius_ * KREIS_PLANET);
    
    wxFont symbol_font(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Astrouniverse");
    wxFont text_font(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    
    for (size_t sA = 0; sA < planets.size() && sA < 11; sA++) {
        const auto& planet = planets[sA];
        double dA = planet.longitude * PI / 180.0;
        
        // Planet-Symbol
        wxString symbol = GetPlanetSymbol(static_cast<astro::Planet>(sA));
        dc.SetFont(symbol_font);
        dc.SetTextForeground(colors_.plan);
        
        int sX = sMpX_ - static_cast<int>(cos(dA) * sRPi);
        int sY = sMpY_ + static_cast<int>(sin(dA) * sRPi);
        wxSize ts = dc.GetTextExtent(symbol);
        dc.DrawText(symbol, sX - ts.x / 2, sY - ts.y / 2);
        
        // Grad/Minuten
        dc.SetFont(text_font);
        int deg = static_cast<int>(planet.longitude) % 30;
        double frac = planet.longitude - static_cast<int>(planet.longitude);
        int min = static_cast<int>(frac * 60.0);
        if (min < 0) min = 0;
        if (min > 59) min = 59;
        
        wxString pos_text;
        if (planet.is_retrograde) {
            pos_text = wxString::Format("r%d°%02d'", deg, min);
        } else {
            pos_text = wxString::Format(" %d°%02d'", deg, min);
        }
        
        // Position radial
        int text_r = sRPi + 12;
        int text_x = sMpX_ - static_cast<int>(cos(dA) * text_r);
        int text_y = sMpY_ + static_cast<int>(sin(dA) * text_r);
        wxSize pts = dc.GetTextExtent(pos_text);
        dc.DrawText(pos_text, text_x - pts.x / 2, text_y - pts.y / 2);
    }
}

// 1:1 Port von sDrawAspekte() aus auwurzel.c Zeile 940-1000
void LegacyRenderer::sDrawAspekte(wxDC& dc, const std::vector<astro::Aspect>& aspects,
                                  const std::vector<astro::PlanetPosition>& planets,
                                  double asc_rad) {
    int sRH = static_cast<int>(sRadius_ * KREIS_ASP);
    
    for (const auto& aspect : aspects) {
        int sPa = static_cast<int>(aspect.planet1);
        int sPb = static_cast<int>(aspect.planet2);
        
        if (sPa >= static_cast<int>(planets.size()) || sPb >= static_cast<int>(planets.size())) continue;
        
        double dA1 = planets[sPa].longitude * PI / 180.0;
        double dA2 = planets[sPb].longitude * PI / 180.0;
        
        int sX1 = sMpX_ - static_cast<int>(cos(dA1) * sRH);
        int sY1 = sMpY_ + static_cast<int>(sin(dA1) * sRH);
        int sX2 = sMpX_ - static_cast<int>(cos(dA2) * sRH);
        int sY2 = sMpY_ + static_cast<int>(sin(dA2) * sRH);
        
        // Farbe nach Aspekttyp
        wxColour color;
        using astro::AspectType;
        switch (aspect.type) {
            case AspectType::Trine:
            case AspectType::Sextile:
                color = colors_.trigon;  // GREEN
                break;
            case AspectType::Conjunction:
            case AspectType::Opposition:
            case AspectType::Square:
                color = colors_.quadratur;  // RED
                break;
            default:
                color = colors_.radix;  // GRAY
        }
        
        dc.SetPen(wxPen(color, 1));
        dc.DrawLine(sX1, sY1, sX2, sY2);
    }
}

wxString LegacyRenderer::GetZodiacSymbol(int sign) {
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

wxString LegacyRenderer::GetPlanetSymbol(astro::Planet planet) {
    switch (planet) {
        case astro::Planet::Sun: return wxString::FromUTF8("\u2609");
        case astro::Planet::Moon: return wxString::FromUTF8("\u263D");
        case astro::Planet::Mercury: return wxString::FromUTF8("\u263F");
        case astro::Planet::Venus: return wxString::FromUTF8("\u2640");
        case astro::Planet::Mars: return wxString::FromUTF8("\u2642");
        case astro::Planet::Jupiter: return wxString::FromUTF8("\u2643");
        case astro::Planet::Saturn: return wxString::FromUTF8("\u2644");
        case astro::Planet::Uranus: return wxString::FromUTF8("\u2645");
        case astro::Planet::Neptune: return wxString::FromUTF8("\u2646");
        case astro::Planet::Pluto: return wxString::FromUTF8("\u2647");
        case astro::Planet::MeanNode: return wxString::FromUTF8("\u260A");
        default: return "?";
    }
}

} // namespace legacy
