#include "AspectGrid.hpp"
#include <wx/graphics.h>
#include <wx/dcbuffer.h>
#include <map>
#include <cmath>
#include <cstdio>

AspectGrid::AspectGrid(wxWindow* parent)
    : wxGrid(parent, wxID_ANY)
{
    // Add planets to the list (Sun through Pluto)
    for (int i = 0; i <= static_cast<int>(astrocore::PlanetId::Pluto); ++i) {
        m_planets.push_back(static_cast<astrocore::PlanetId>(i));
    }
    
    InitializeGrid();
}

void AspectGrid::InitializeGrid()
{
    // Create a square grid sized for planets with an extra header row/col at index 0
    int planetCount = static_cast<int>(m_planets.size());
    int gridSize = planetCount + 1; // rows/cols: 0=header, 1..planetCount=planets
    CreateGrid(gridSize, gridSize);
    
    // Hide row and column labels
    SetRowLabelSize(0);
    SetColLabelSize(0);
    
    // Set cell sizes to create square cells
    int cellSize = 30;
    for (int i = 0; i < gridSize; ++i) {
        SetRowSize(i, cellSize);
        SetColSize(i, cellSize);
    }
    
    // Disable cell editing
    EnableEditing(false);
    
    // Disable cell selection
    EnableDragGridSize(false);
    EnableDragColSize(false);
    EnableDragRowSize(false);
    
    // Set grid line color
    SetGridLineColour(wxColour(200, 200, 200));
    
    // Initialize the grid with planet symbols in the first row and column
    for (int i = 0; i < planetCount; ++i) {
        astrocore::PlanetId planet = m_planets[i];
        wxString planetName = GetPlanetName(planet);
        
        // Set the planet symbol in the first column (row header)
        SetCellValue(i + 1, 0, planetName);
        SetCellAlignment(i + 1, 0, wxALIGN_CENTER, wxALIGN_CENTER);
        SetCellBackgroundColour(i + 1, 0, wxColour(240, 240, 240));
        
        // Set the planet symbol in the first row (column header)
        SetCellValue(0, i + 1, planetName);
        SetCellAlignment(0, i + 1, wxALIGN_CENTER, wxALIGN_CENTER);
        SetCellBackgroundColour(0, i + 1, wxColour(240, 240, 240));
    }
    
    // Initialize GlyphRenderer if needed
    if (!GlyphRenderer::IsInitialized()) {
        GlyphRenderer::Initialize();
    }
}

void AspectGrid::UpdateGrid(const std::array<astrocore::PlanetPosition, 21>& positions, double orbDeg)
{
    std::fprintf(stderr, "[AspectGrid] UpdateGrid enter (visible=%d, rows=%d, cols=%d)\n",
                 m_visible ? 1 : 0, GetNumberRows(), GetNumberCols());
    if (!m_visible) {
        std::fprintf(stderr, "[AspectGrid] not visible, skip\n");
        return;
    }
    if (GetNumberRows() == 0 || GetNumberCols() == 0) {
        std::fprintf(stderr, "[AspectGrid] grid not initialized yet, InitializeGrid()\n");
        InitializeGrid();
    }
    
    // Clear existing aspect cells
    int planetCount = static_cast<int>(m_planets.size());
    if (planetCount <= 0) {
        std::fprintf(stderr, "[AspectGrid] no planets configured\n");
        return;
    }
    for (int row = 1; row <= planetCount; ++row) {
        for (int col = 1; col <= planetCount; ++col) {
            if (row > col) {  // Only show aspects in the lower triangle
                SetCellValue(row, col, "");
                SetCellBackgroundColour(row, col, *wxWHITE);
            }
        }
    }
    
    // Calculate and display aspects
    std::fprintf(stderr, "[AspectGrid] CalculateAspects start (orb=%.2f)\n", orbDeg);
    CalculateAspects(positions, orbDeg);
    std::fprintf(stderr, "[AspectGrid] CalculateAspects done\n");
    
    // Refresh the grid
    Refresh(false);
    std::fprintf(stderr, "[AspectGrid] UpdateGrid leave\n");
}
void AspectGrid::CalculateAspects(const std::array<astrocore::PlanetPosition, 21>& positions, double orbDeg)
{
    int planetCount = static_cast<int>(m_planets.size());
    
    // Calculate aspects between planets (grid indices 1..planetCount)
    for (int i = 1; i <= planetCount; ++i) {
        for (int j = 1; j < i; ++j) {
            // Map grid index to planet ID and position index
            astrocore::PlanetId p1 = m_planets[i - 1];
            astrocore::PlanetId p2 = m_planets[j - 1];
            int idx1 = static_cast<int>(p1);
            int idx2 = static_cast<int>(p2);
            if (idx1 < 0 || idx1 >= static_cast<int>(positions.size()) ||
                idx2 < 0 || idx2 >= static_cast<int>(positions.size())) {
                continue;
            }
            const astrocore::PlanetPosition& pos1 = positions[idx1];
            const astrocore::PlanetPosition& pos2 = positions[idx2];
            
            // Skip if either planet position is invalid
            if (!pos1.valid || !pos2.valid) {
                continue;
            }
            
            // Calculate aspect
            astrocore::AspectResult aspect = astrocore::detect_aspect(pos1.longitude, pos2.longitude, orbDeg);
            
            // Draw aspect symbol if an aspect is detected
            if (aspect.type != astrocore::AspectType::None) {
                DrawAspectSymbol(i, j, aspect.type, aspect.deltaDeg);
            }
        }
    }
}

void AspectGrid::DrawAspectSymbol(int row, int col, astrocore::AspectType aspect, double orb)
{
    // First check if row and column are within valid range
    if (row < 0 || row >= GetNumberRows() || col < 0 || col >= GetNumberCols()) {
        // Out of bounds, don't attempt to draw
        return;
    }

    // Set cell background color based on aspect type
    wxColour bgColor;
    switch (aspect) {
        case astrocore::AspectType::Conjunction:
            bgColor = wxColour(255, 200, 200);  // Light red
            break;
        case astrocore::AspectType::Opposition:
            bgColor = wxColour(255, 150, 150);  // Medium red
            break;
        case astrocore::AspectType::Trine:
            bgColor = wxColour(200, 255, 200);  // Light green
            break;
        case astrocore::AspectType::Square:
            bgColor = wxColour(200, 200, 255);  // Light blue
            break;
        case astrocore::AspectType::Sextile:
            bgColor = wxColour(255, 255, 200);  // Light yellow
            break;
        default:
            bgColor = *wxWHITE;
            break;
    }
    
    try {
        // Set cell background color
        SetCellBackgroundColour(row, col, bgColor);
        
        // Set cell value to aspect symbol
        wxString aspectSymbol;
        switch (aspect) {
            case astrocore::AspectType::Conjunction:
                aspectSymbol = "☌";  // Conjunction symbol
                break;
            case astrocore::AspectType::Opposition:
                aspectSymbol = "☍";  // Opposition symbol
                break;
            case astrocore::AspectType::Trine:
                aspectSymbol = "△";  // Trine symbol
                break;
            case astrocore::AspectType::Square:
                aspectSymbol = "□";  // Square symbol
                break;
            case astrocore::AspectType::Sextile:
                aspectSymbol = "⚹";  // Sextile symbol
                break;
            default:
                aspectSymbol = "";
                break;
        }
        
        // Add orb information (format value using ASCII-only format string to avoid
        // locale/encoding conversion issues, then append degree symbol via UTF-8)
        wxString orbText = wxString::Format("%.1f", std::abs(orb));
        // UTF-8 for degree sign: 0xC2 0xB0
        wxString degree = wxString::FromUTF8("\xC2\xB0");
        wxString cellValue = aspectSymbol + "\n" + orbText + degree;
        SetCellValue(row, col, cellValue);
        SetCellAlignment(row, col, wxALIGN_CENTER, wxALIGN_CENTER);
    } catch (const std::exception& e) {
        // Log error but don't crash
        wxLogError("Error drawing aspect symbol: %s", e.what());
    }
}

wxString AspectGrid::GetPlanetName(astrocore::PlanetId planet)
{
    // Map planet IDs to symbols or names
    static const std::map<astrocore::PlanetId, wxString> planetSymbols = {
        {astrocore::PlanetId::Sun, "☉"},
        {astrocore::PlanetId::Moon, "☽"},
        {astrocore::PlanetId::Mercury, "☿"},
        {astrocore::PlanetId::Venus, "♀"},
        {astrocore::PlanetId::Mars, "♂"},
        {astrocore::PlanetId::Jupiter, "♃"},
        {astrocore::PlanetId::Saturn, "♄"},
        {astrocore::PlanetId::Uranus, "♅"},
        {astrocore::PlanetId::Neptune, "♆"},
        {astrocore::PlanetId::Pluto, "♇"}
    };
    
    auto it = planetSymbols.find(planet);
    if (it != planetSymbols.end()) {
        return it->second;
    }
    
    // Fallback to planet ID as string
    return wxString::Format("%d", static_cast<int>(planet));
}

void AspectGrid::SetVisible(bool visible)
{
    m_visible = visible;
    Show(visible);
}
