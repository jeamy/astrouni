#pragma once

#include <wx/wx.h>
#include <wx/grid.h>
#include <vector>
#include "astrocore/planets.hpp"
#include "GlyphRenderer.hpp"

class AspectGrid : public wxGrid {
public:
    AspectGrid(wxWindow* parent);
    virtual ~AspectGrid() = default;

    // Update the grid with new planet positions
    void UpdateGrid(const std::array<astrocore::PlanetPosition, 21>& positions, double orbDeg);

    // Set the visibility of the grid
    void SetVisible(bool visible);

    // Get the visibility of the grid
    bool IsVisible() const { return m_visible; }

private:
    // Initialize the grid with headers and formatting
    void InitializeGrid();
    
    // Calculate aspects between planets
    void CalculateAspects(const std::array<astrocore::PlanetPosition, 21>& positions, double orbDeg);
    
    // Draw an aspect symbol in a cell
    void DrawAspectSymbol(int row, int col, astrocore::AspectType aspect, double orb);
    
    // Get the name or symbol for a planet
    wxString GetPlanetName(astrocore::PlanetId planet);
    
    // List of planets to display in the grid
    std::vector<astrocore::PlanetId> m_planets;
    
    // Visibility flag
    bool m_visible{true};
};
