#include "ChartPanel.hpp"
#include <wx/dcbuffer.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/app.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <numbers>
#include <vector>
#include <algorithm>
#include "astrocore/api.hpp"
#include "LegacyPortRenderer.hpp"

wxBEGIN_EVENT_TABLE(ChartPanel, wxPanel)
  EVT_PAINT(ChartPanel::OnPaint)
wxEND_EVENT_TABLE()

ChartPanel::ChartPanel(wxWindow* parent)
  : wxPanel(parent, wxID_ANY)
{
  std::fprintf(stderr, "[ChartPanel] ctor\n");
  SetBackgroundStyle(wxBG_STYLE_PAINT); // enable double-buffering with wxAutoBufferedPaintDC
  
  // Optionally skip creating the aspect grid window during tests
  const char* no_grid = std::getenv("ASTRO_TEST_NO_GRID");
  if (no_grid && no_grid[0] == '1') {
    std::fprintf(stderr, "[ChartPanel] Skipping AspectGridWindow creation due to ASTRO_TEST_NO_GRID=1\n");
    m_aspectGridWindow = nullptr;
  } else {
    // Create the aspect grid window
    m_aspectGridWindow = new AspectGridWindow(parent, "Aspect Grid");
    
    // Initially hide the aspect grid window
    m_aspectGridWindow->ShowWindow(m_showAspectGrid);
  }
}

void ChartPanel::OnPaint(wxPaintEvent&) {
  wxAutoBufferedPaintDC dc(this);
  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();
  DrawChart(dc, GetClientSize());
}

void ChartPanel::DrawChart(wxDC& dc, const wxSize& size) {
  using namespace astrocore;
  std::fprintf(stderr, "[ChartPanel] DrawChart start\n");
  // Simple chart: circle, axes, equal-house cusps from computed Ascendant, label Asc/MC
  int w = size.GetWidth();
  int h = size.GetHeight();
  int cx = w / 2;
  int cy = h / 2;
  int minwh = std::min(w, h);
  int margin = std::clamp(minwh / 10, 48, 96); // dynamic frame margin
  int radius = std::max(50, minwh / 2 - margin);
  
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
    std::fprintf(stderr, "[ChartPanel] No wxGraphicsContext, fallback drawing\n");
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawCircle(cx, cy, radius);
    std::fprintf(stderr, "[ChartPanel] DrawChart end (fallback)\n");
    return;
  }

  dc.SetPen(*wxBLACK_PEN);
  dc.SetBrush(*wxTRANSPARENT_BRUSH);
  dc.DrawCircle(cx, cy, radius);
  if (m_showAxes) {
    dc.DrawLine(cx - radius, cy, cx + radius, cy); // horizontal axis
    dc.DrawLine(cx, cy - radius, cx, cy + radius); // vertical axis
  }

  // Zodiac sign ring (always visible)
  if (!GlyphRenderer::IsInitialized()) {
    std::fprintf(stderr, "[ChartPanel] GlyphRenderer::Initialize() for sign ring\n");
    GlyphRenderer::Initialize();
  }
  // Use legacy-inspired renderer via adapter for ring band and ticks
  LegacyPort::DrawSignRing(gc, wxPoint(cx, cy), radius);

  // Dynamic glyph sizes
  double glyphSizeSign = std::clamp(radius * 0.06, 14.0, 26.0);
  double glyphSizePlanet = std::clamp(radius * 0.055, 12.0, 24.0);

  // Draw the 12 sign glyphs around the wheel
  for (int sign = 0; sign < 12; ++sign) {
    ChartGlyphHelper::DrawSignOnWheel(gc, sign, wxPoint(cx, cy), radius, m_showLabels, glyphSizeSign, m_signLabelRadiusOffset);
  }

  // Compute from current parameters
  std::fprintf(stderr, "[ChartPanel] computing jd/asc/mc\n");
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
  std::fprintf(stderr, "[ChartPanel] calculate_chart...\n");
  ChartPositions chartData = calculate_chart(jd, m_longitudeDeg, m_latitudeDeg, flags);
  std::fprintf(stderr, "[ChartPanel] calculate_chart done\n");
  
  // Draw planets if enabled
  if (m_showPlanets) {
    // Initialize the glyph renderer if needed
    if (!GlyphRenderer::IsInitialized()) {
      std::fprintf(stderr, "[ChartPanel] GlyphRenderer::Initialize()\n");
      GlyphRenderer::Initialize();
    }
    
    // Draw each planet
    std::fprintf(stderr, "[ChartPanel] drawing planets...\n");
    for (int i = 0; i <= static_cast<int>(PlanetId::Pluto); ++i) {
      PlanetId planet = static_cast<PlanetId>(i);
      const PlanetPosition& pos = chartData.positions[i];
      
      if (pos.valid) {
        // Convert ecliptic longitude to chart angle (0° at right, moving counterclockwise)
        double angleDeg = 90.0 - pos.longitude;
        double angleRad = angleDeg * std::numbers::pi / 180.0;
        
        // Calculate position on chart
        double planetRadius = radius * 0.86; // Place planets inside the house cusps with more space
        int x = cx + static_cast<int>(std::cos(angleRad) * planetRadius);
        int y = cy + static_cast<int>(std::sin(angleRad) * planetRadius);
        
        // Draw the planet glyph
        ChartGlyphHelper::DrawPlanetOnWheel(gc, planet, pos.longitude, pos.latitude,
                                          wxPoint(cx, cy), radius * 0.86, m_showLabels, glyphSizePlanet);
      }
    }
    
    // Draw aspect lines between planets if enabled
    if (m_showAspectLines) {
      std::fprintf(stderr, "[ChartPanel] drawing aspect lines (orb=%.2f)\n", m_aspectOrbDeg);
      ChartGlyphHelper::DrawAspectLines(gc, chartData.positions, wxPoint(cx, cy), radius, m_aspectOrbDeg);
    }
  }
  
  // Update aspect grid if visible (schedule after paint to avoid re-entrancy)
  if (m_aspectGridWindow && m_showAspectGrid) {
    std::fprintf(stderr, "[ChartPanel] scheduling AspectGrid update\n");
    auto positionsCopy = chartData.positions; // copy for async use
    double orbCopy = m_aspectOrbDeg;
    AspectGridWindow* wnd = m_aspectGridWindow;
    if (wnd) {
      wnd->CallAfter([wnd, positionsCopy, orbCopy]() {
        if (!wnd) return;
        AspectGrid* grid = wnd->GetAspectGrid();
        if (grid && grid->IsShownOnScreen()) {
          std::fprintf(stderr, "[ChartPanel] calling AspectGrid::UpdateGrid now\n");
          grid->CallAfter([grid, positionsCopy, orbCopy]() {
            if (!grid) return;
            grid->UpdateGrid(positionsCopy, orbCopy);
          });
        }
      });
    }
  }
  
  // Clean up graphics context
  delete gc;
  std::fprintf(stderr, "[ChartPanel] DrawChart end\n");
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
  std::fprintf(stderr, "[ChartPanel] SetShowAspectGrid(%d)\n", enabled ? 1 : 0);
  if (m_aspectGridWindow) {
    m_aspectGridWindow->ShowWindow(enabled);
  }
  Refresh();
}

void ChartPanel::SetSignLabelRadiusOffset(double offset) {
  m_signLabelRadiusOffset = offset;
  Refresh();
}

// Sign ring is always visible; setter removed

void ChartPanel::ResetDefaults() {
  m_date = {2000,1,1};
  m_time = {12,0,0.0};
  m_tz = {0.0, 0.0};
  m_longitudeDeg = 13.405; // Berlin default
  m_latitudeDeg = 52.52;
  m_houseSystem = HouseSystem::Equal;
  m_aspectOrbDeg = 3.0;
  m_showAxes = true;
  m_showLabels = true;
  m_showPlanets = true;
  m_showAspectLines = true;
  m_showAspectGrid = false;
  m_signLabelRadiusOffset = 44.0;
  
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
