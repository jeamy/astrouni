#include "ChartPanel.hpp"
#include <wx/dcbuffer.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <cmath>
#include <numbers>
#include <vector>
#include "astrocore/api.hpp"

wxBEGIN_EVENT_TABLE(ChartPanel, wxPanel)
  EVT_PAINT(ChartPanel::OnPaint)
wxEND_EVENT_TABLE()

ChartPanel::ChartPanel(wxWindow* parent)
  : wxPanel(parent, wxID_ANY)
{
  SetBackgroundStyle(wxBG_STYLE_PAINT); // enable double-buffering with wxAutoBufferedPaintDC
  
  // Create the aspect grid window
  m_aspectGridWindow = new AspectGridWindow(parent, "Aspect Grid");
  
  // Initially hide the aspect grid window
  m_aspectGridWindow->ShowWindow(m_showAspectGrid);
}

void ChartPanel::OnPaint(wxPaintEvent&) {
  wxAutoBufferedPaintDC dc(this);
  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();
  DrawChart(dc, GetClientSize());
}

void ChartPanel::DrawChart(wxDC& dc, const wxSize& size) {
  using namespace astrocore;
  // Simple chart: circle, axes, equal-house cusps from computed Ascendant, label Asc/MC
  int w = size.GetWidth();
  int h = size.GetHeight();
  int cx = w / 2;
  int cy = h / 2;
  int radius = std::min(w, h) * 3 / 8;
  
  // Create graphics context for advanced drawing
  wxGraphicsContext* gc = nullptr;
  if (auto* windowDC = dynamic_cast<wxWindowDC*>(&dc)) {
    gc = wxGraphicsContext::Create(*windowDC);
  } else if (auto* memoryDC = dynamic_cast<wxMemoryDC*>(&dc)) {
    gc = wxGraphicsContext::Create(*memoryDC);
  } else if (auto* printerDC = dynamic_cast<wxPrinterDC*>(&dc)) {
    gc = wxGraphicsContext::Create(*printerDC);
  }
  if (!gc) {
    // Fall back to basic DC if graphics context creation fails
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawCircle(cx, cy, radius);
    return;
  }

  dc.SetPen(*wxBLACK_PEN);
  dc.SetBrush(*wxTRANSPARENT_BRUSH);
  dc.DrawCircle(cx, cy, radius);
  if (m_showAxes) {
    dc.DrawLine(cx - radius, cy, cx + radius, cy); // horizontal axis
    dc.DrawLine(cx, cy - radius, cx, cy + radius); // vertical axis
  }

  // Compute from current parameters
  double jd = julian_day(m_date, m_time, m_tz);
  const double lon = m_longitudeDeg * std::numbers::pi / 180.0;
  const double lat = m_latitudeDeg  * std::numbers::pi / 180.0;
  double asc = 0.0, mc = 0.0;
  asc_mc_longitudes(jd, lon, lat, asc, mc);

  double cusps[12];
  if (m_houseSystem == HouseSystem::Equal) {
    equal_house_cusps(asc, cusps);
  } else {
    placidus_house_cusps(jd, lon, lat, cusps);
  }

  // Draw house cusps as radial lines
  for (int i = 0; i < 12; ++i) {
    double a = cusps[i];
    int x2 = cx + static_cast<int>(std::cos(a) * radius);
    int y2 = cy + static_cast<int>(std::sin(a) * radius);
    dc.DrawLine(cx, cy, x2, y2);
  }

  // Label Asc and MC
  if (m_showLabels) {
    auto label_at = [&](const wxString& txt, double ang) {
      int rx = cx + static_cast<int>(std::cos(ang) * (radius + 8));
      int ry = cy + static_cast<int>(std::sin(ang) * (radius + 8));
      dc.DrawText(txt, rx, ry);
    };
    label_at("Asc", asc);
    label_at("MC", mc);
  }

  // Calculate planet positions
  CalcFlags flags;
  flags.calculateSpeed = true;
  flags.calculateLatitude = true;
  flags.calculateDistance = true;
  
  // Get planet positions
  ChartPositions chartData = calculate_chart(jd, m_longitudeDeg, m_latitudeDeg, flags);
  
  // Draw planets if enabled
  if (m_showPlanets) {
    // Initialize the glyph renderer if needed
    if (!GlyphRenderer::IsInitialized()) {
      GlyphRenderer::Initialize();
    }
    
    // Draw each planet
    for (int i = 0; i <= static_cast<int>(PlanetId::Pluto); ++i) {
      PlanetId planet = static_cast<PlanetId>(i);
      const PlanetPosition& pos = chartData.positions[i];
      
      if (pos.valid) {
        // Convert ecliptic longitude to chart angle (0° at right, moving counterclockwise)
        double angleDeg = 90.0 - pos.longitude;
        double angleRad = angleDeg * std::numbers::pi / 180.0;
        
        // Calculate position on chart
        double planetRadius = radius * 0.8; // Place planets inside the house cusps
        int x = cx + static_cast<int>(std::cos(angleRad) * planetRadius);
        int y = cy + static_cast<int>(std::sin(angleRad) * planetRadius);
        
        // Draw the planet glyph
        ChartGlyphHelper::DrawPlanetOnWheel(gc, planet, pos.longitude, pos.latitude,
                                          wxPoint(cx, cy), radius * 0.8, m_showLabels);
      }
    }
    
    // Draw aspect lines between planets if enabled
    if (m_showAspectLines) {
      ChartGlyphHelper::DrawAspectLines(gc, chartData.positions, wxPoint(cx, cy), radius, m_aspectOrbDeg);
    }
  }
  
  // Update aspect grid if visible
  if (m_aspectGridWindow && m_showAspectGrid) {
    AspectGrid* grid = m_aspectGridWindow->GetAspectGrid();
    if (grid) {
      // Update grid with planet positions
      grid->UpdateGrid(chartData.positions, m_aspectOrbDeg);
      
      // If demo aspects are enabled, add them to the grid
      if (m_demoAspects) {
        // Convert to degrees for the grid
        double ascDeg = asc * 180.0 / std::numbers::pi;
        double mcDeg = mc * 180.0 / std::numbers::pi;
        grid->ShowDemoAspects(ascDeg, mcDeg, m_aspectOrbDeg, chartData.positions);
      }
    }
  }
  
  // Demo aspects: draw lines between sample ecliptic points if within orb
  if (m_demoAspects) {
    std::vector<double> longs = {
      asc,
      std::fmod(asc + 60.0 * std::numbers::pi / 180.0, 2.0 * std::numbers::pi),
      std::fmod(asc + 120.0 * std::numbers::pi / 180.0, 2.0 * std::numbers::pi),
      mc,
      std::fmod(mc + M_PI, 2.0 * M_PI)
    };
    auto point_on_circle = [&](double ang) {
      int x = cx + static_cast<int>(std::cos(ang) * radius);
      int y = cy + static_cast<int>(std::sin(ang) * radius);
      return wxPoint(x, y);
    };
    auto set_pen_for_aspect = [&](astrocore::AspectType t) {
      switch (t) {
        case astrocore::AspectType::Conjunction: dc.SetPen(wxPen(*wxBLACK, 2)); break;
        case astrocore::AspectType::Opposition:  dc.SetPen(wxPen(*wxRED, 2)); break;
        case astrocore::AspectType::Trine:       dc.SetPen(wxPen(*wxGREEN, 2)); break;
        case astrocore::AspectType::Square:      dc.SetPen(wxPen(*wxBLUE, 2)); break;
        case astrocore::AspectType::Sextile:     dc.SetPen(wxPen(*wxCYAN, 2)); break;
        default: dc.SetPen(wxPen(*wxLIGHT_GREY, 1)); break;
      }
    };
    for (size_t i = 0; i < longs.size(); ++i) {
      for (size_t j = i + 1; j < longs.size(); ++j) {
        double d1 = longs[i] * 180.0 / std::numbers::pi;
        double d2 = longs[j] * 180.0 / std::numbers::pi;
        auto r = astrocore::detect_aspect(d1, d2, m_aspectOrbDeg);
        if (r.type != astrocore::AspectType::None) {
          set_pen_for_aspect(r.type);
          dc.DrawLine(point_on_circle(longs[i]), point_on_circle(longs[j]));
        }
      }
    }
  }
  
  // Clean up graphics context
  delete gc;
}

// Setters
void ChartPanel::SetDateTime(const astrocore::Date& d, const astrocore::Time& t, const astrocore::TimeZone& tz) {
  m_date = d; m_time = t; m_tz = tz; Refresh();
}
void ChartPanel::SetLocation(double longitudeDeg, double latitudeDeg) {
  m_longitudeDeg = longitudeDeg; m_latitudeDeg = latitudeDeg; Refresh();
}
void ChartPanel::SetHouseSystem(HouseSystem hs) {
  m_houseSystem = hs; Refresh();
}
void ChartPanel::SetAspectOrbDeg(double orbDeg) {
  m_aspectOrbDeg = orbDeg; Refresh();
}
void ChartPanel::SetDemoAspects(bool enabled) {
  m_demoAspects = enabled;
  Refresh();
  
  // If aspect grid is visible, update it with the new demo aspects setting
  if (m_aspectGridWindow && m_showAspectGrid) {
    AspectGrid* grid = m_aspectGridWindow->GetAspectGrid();
    if (grid) {
      // Force a recalculation of the chart data to update the grid
      double jd = astrocore::julian_day(m_date, m_time, m_tz);
      astrocore::ChartPositions chartData;
      chartData.jd = jd;
      
      // Calculate Asc/MC
      double longitudeRad = m_longitudeDeg * M_PI / 180.0;
      double latitudeRad = m_latitudeDeg * M_PI / 180.0;
      double ascRad, mcRad;
      astrocore::asc_mc_longitudes(jd, longitudeRad, latitudeRad, ascRad, mcRad);
      
      // Convert to degrees for the grid
      double asc = ascRad * 180.0 / M_PI;
      double mc = mcRad * 180.0 / M_PI;
      
      // Update the grid with current positions
      grid->UpdateGrid(chartData.positions, m_aspectOrbDeg);
      
      // If demo aspects are enabled, add them to the grid
      if (enabled) {
        grid->ShowDemoAspects(asc, mc, m_aspectOrbDeg, chartData.positions);
      }
    }
  }
}

void ChartPanel::SetShowAxes(bool enabled) {
  m_showAxes = enabled; Refresh();
}

void ChartPanel::SetShowLabels(bool enabled) {
  m_showLabels = enabled; Refresh();
}

void ChartPanel::SetShowPlanets(bool enabled) {
  m_showPlanets = enabled; Refresh();
}

void ChartPanel::SetShowAspectLines(bool enabled) {
  m_showAspectLines = enabled;
  Refresh();
}

void ChartPanel::SetShowAspectGrid(bool enabled) {
  m_showAspectGrid = enabled;
  if (m_aspectGridWindow) {
    m_aspectGridWindow->ShowWindow(enabled);
  }
  Refresh();
}

void ChartPanel::ResetDefaults() {
  m_date = {2000,1,1};
  m_time = {12,0,0.0};
  m_tz = {0.0, 0.0};
  m_longitudeDeg = 13.405; // Berlin default
  m_latitudeDeg = 52.52;
  m_houseSystem = HouseSystem::Equal;
  m_aspectOrbDeg = 3.0;
  m_demoAspects = false;
  m_showAxes = true;
  m_showLabels = true;
  m_showPlanets = true;
  m_showAspectLines = true;
  m_showAspectGrid = false;
  
  if (m_aspectGridWindow) {
    m_aspectGridWindow->ShowWindow(m_showAspectGrid);
  }
  Refresh();
}

bool ChartPanel::ExportPNG(const wxString& path, int width, int height) {
  wxSize size = GetClientSize();
  if (width > 0 && height > 0) {
    size = wxSize(width, height);
  }

  wxBitmap bmp(size.GetWidth(), size.GetHeight(), 32);
  if (!bmp.IsOk()) return false;

  wxMemoryDC mdc(bmp);
  mdc.SetBackground(*wxWHITE_BRUSH);
  mdc.Clear();
  DrawChart(mdc, size);
  mdc.SelectObject(wxNullBitmap);

  wxImage img = bmp.ConvertToImage();
  if (!img.IsOk()) return false;

  return img.SaveFile(path, wxBITMAP_TYPE_PNG);
}
