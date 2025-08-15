#include "AspectGrid.hpp"
#include <wx/graphics.h>
#include <wx/dcbuffer.h>
#include <map>

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
    // Create a grid with planets as rows and columns plus extra space for headers
    int planetCount = static_cast<int>(m_planets.size());
    // Add 1 for header row/column and extra space for potential demo points
    int gridSize = planetCount + 10; // Extra space for demo points
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
        
        // Set the planet symbol in the first column
        SetCellValue(i, 0, planetName);
        SetCellAlignment(i, 0, wxALIGN_CENTER, wxALIGN_CENTER);
        SetCellBackgroundColour(i, 0, wxColour(240, 240, 240));
        
        // Set the planet symbol in the first row
        SetCellValue(0, i, planetName);
        SetCellAlignment(0, i, wxALIGN_CENTER, wxALIGN_CENTER);
        SetCellBackgroundColour(0, i, wxColour(240, 240, 240));
    }
    
    // Initialize GlyphRenderer if needed
    if (!GlyphRenderer::IsInitialized()) {
        GlyphRenderer::Initialize();
    }
}

void AspectGrid::UpdateGrid(const std::array<astrocore::PlanetPosition, 21>& positions, double orbDeg)
{
    if (!m_visible) {
        return;
    }
    
    // Clear existing aspect cells
    int planetCount = static_cast<int>(m_planets.size());
    for (int row = 1; row < planetCount; ++row) {
        for (int col = 1; col < planetCount; ++col) {
            if (row > col) {  // Only show aspects in the lower triangle
                SetCellValue(row, col, "");
                SetCellBackgroundColour(row, col, *wxWHITE);
            }
        }
    }
    
    // Calculate and display aspects
    CalculateAspects(positions, orbDeg);
    
    // Refresh the grid
    ForceRefresh();
}

void AspectGrid::CalculateAspects(const std::array<astrocore::PlanetPosition, 21>& positions, double orbDeg)
{
    int planetCount = static_cast<int>(m_planets.size());
    
    // Calculate aspects between planets
    for (int i = 1; i < planetCount; ++i) {
        for (int j = 1; j < i; ++j) {
            // Get planet positions
            const astrocore::PlanetPosition& pos1 = positions[i];
            const astrocore::PlanetPosition& pos2 = positions[j];
            
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
    // Set cell background color based on aspect type
    wxColour bgColor;
    switch (aspect) {
        case astrocore::AspectType::Conjunction:
            bgColor = wxColour(255, 255, 200);  // Light yellow
            break;
        case astrocore::AspectType::Opposition:
            bgColor = wxColour(255, 200, 200);  // Light red
            break;
        case astrocore::AspectType::Trine:
            bgColor = wxColour(200, 255, 200);  // Light green
            break;
        case astrocore::AspectType::Square:
            bgColor = wxColour(200, 200, 255);  // Light blue
            break;
        case astrocore::AspectType::Sextile:
            bgColor = wxColour(200, 255, 255);  // Light cyan
            break;
        default:
            bgColor = *wxWHITE;
            break;
    }
    
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
    
    // Add orb information
    wxString cellValue = aspectSymbol + "\n" + wxString::Format("%.1f°", std::abs(orb));
    SetCellValue(row, col, cellValue);
    SetCellAlignment(row, col, wxALIGN_CENTER, wxALIGN_CENTER);
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

void AspectGrid::ShowDemoAspects(double asc, double mc, double orbDeg, const std::array<astrocore::PlanetPosition, 21>& positions)
{
    if (!m_visible) {
        return;
    }
    
    // Clear existing grid cells but don't recreate the grid
    int rows = GetNumberRows();
    int cols = GetNumberCols();
    
    // Clear all cell values and colors
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            SetCellValue(i, j, "");
            SetCellBackgroundColour(i, j, *wxWHITE);
        }
    }
    
    // Re-initialize planet symbols in the first row and column
    int planetCount = static_cast<int>(m_planets.size());
    for (int i = 0; i < planetCount && i < rows && i < cols; ++i) {
        astrocore::PlanetId planet = m_planets[i];
        wxString planetName = GetPlanetName(planet);
        
        // Set the planet symbol in the first column
        SetCellValue(i + 1, 0, planetName);
        SetCellAlignment(i + 1, 0, wxALIGN_CENTER, wxALIGN_CENTER);
        SetCellBackgroundColour(i + 1, 0, wxColour(240, 240, 240));
        
        // Set the planet symbol in the first row
        SetCellValue(0, i + 1, planetName);
        SetCellAlignment(0, i + 1, wxALIGN_CENTER, wxALIGN_CENTER);
        SetCellBackgroundColour(0, i + 1, wxColour(240, 240, 240));
    }
    
    // Create demo points at significant angles from Asc and MC
    std::vector<double> demoLongs = {
        asc,                                // Ascendant
        std::fmod(asc + 60.0, 360.0),      // Asc + 60° (sextile)
        std::fmod(asc + 120.0, 360.0),     // Asc + 120° (trine)
        mc,                                 // Midheaven
        std::fmod(mc + 180.0, 360.0)       // MC + 180° (opposition)
    };
    
    // Labels for the demo points
    std::vector<wxString> demoLabels = {
        "Asc",
        "Asc+60°",
        "Asc+120°",
        "MC",
        "MC+180°"
    };
    
    // Add a special row and column for demo points
    int demoRow = planetCount + 1; // +1 for header row
    
    // Make sure we have enough rows and columns
    int neededRows = demoRow + static_cast<int>(demoLongs.size());
    int neededCols = planetCount + 1; // +1 for header column
    
    // Resize grid if needed
    while (GetNumberRows() < neededRows) {
        AppendRows(1);
    }
    while (GetNumberCols() < neededCols) {
        AppendCols(1);
    }
    
    // Set header for demo points
    if (demoRow < GetNumberRows()) {
        SetCellValue(demoRow, 0, "Demo Points");
        SetCellBackgroundColour(demoRow, 0, wxColour(240, 240, 200));
        SetCellAlignment(demoRow, 0, wxALIGN_CENTER, wxALIGN_CENTER);
    }
    
    // Add demo point labels
    for (size_t i = 0; i < demoLongs.size(); ++i) {
        int row = demoRow + 1 + static_cast<int>(i);
        // Safety check to prevent out-of-bounds access
        if (row < GetNumberRows()) {
            SetCellValue(row, 0, demoLabels[i]);
            SetCellBackgroundColour(row, 0, wxColour(240, 240, 200));
            SetCellAlignment(row, 0, wxALIGN_CENTER, wxALIGN_CENTER);
        }
    }
    
    // Calculate aspects between demo points
    for (size_t i = 0; i < demoLongs.size(); ++i) {
        for (size_t j = 0; j < i; ++j) {
            // Calculate aspect
            astrocore::AspectResult aspect = astrocore::detect_aspect(demoLongs[i], demoLongs[j], orbDeg);
            
            // Draw aspect symbol if an aspect is detected
            if (aspect.type != astrocore::AspectType::None) {
                // Create a cell for this aspect
                int row = demoRow + 1 + static_cast<int>(i);
                int col = 1 + static_cast<int>(j);
                
                // Safety check to prevent out-of-bounds access
                if (row < GetNumberRows() && col < GetNumberCols()) {
                    // Draw the aspect symbol
                    DrawAspectSymbol(row, col, aspect.type, aspect.deltaDeg);
                }
            }
        }
    }
    
    // Calculate aspects between planets and demo points
    for (size_t i = 0; i < demoLongs.size(); ++i) {
        for (int j = 0; j < planetCount; ++j) {
            // Skip if planet position is invalid
            if (j >= static_cast<int>(m_planets.size())) {
                continue;
            }
            
            // Get planet ID and position
            astrocore::PlanetId planetId = m_planets[j];
            int planetIndex = static_cast<int>(planetId);
            
            // Skip if planet index is out of bounds or position is invalid
            if (planetIndex >= positions.size() || !positions[planetIndex].valid) {
                continue;
            }
            
            // Get planet longitude
            double planetLong = positions[planetIndex].longitude;
            
            // Calculate aspect
            astrocore::AspectResult aspect = astrocore::detect_aspect(demoLongs[i], planetLong, orbDeg);
            
            // Draw aspect symbol if an aspect is detected
            if (aspect.type != astrocore::AspectType::None) {
                // Create a cell for this aspect
                int row = demoRow + 1 + static_cast<int>(i);
                int col = j + 1; // +1 to account for header column
                
                // Safety check to prevent out-of-bounds access
                if (row < GetNumberRows() && col < GetNumberCols()) {
                    // Draw the aspect symbol
                    DrawAspectSymbol(row, col, aspect.type, aspect.deltaDeg);
                }
            }
        }
    }
    
    // Refresh the grid
    ForceRefresh();
}
