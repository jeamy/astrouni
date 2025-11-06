#pragma once

#include <wx/wx.h>
#include <wx/graphics.h>
#include <wx/sizer.h>

#include "astrocore/api.hpp"
#include "astrocore/planets.hpp"
#include "GlyphRenderer.hpp"
#include "AspectGridWindow.hpp"

class ChartPanel : public wxPanel {
public:
  ChartPanel(wxWindow* parent);

  // Export current chart view to PNG file path; returns true on success
  bool ExportPNG(const wxString& path, int width = -1, int height = -1);

  // House systems (currently Equal implemented; Placidus placeholder)
  enum class HouseSystem { Equal, Placidus };

  // Set parameters
  void SetDateTime(const astrocore::Date& d, const astrocore::Time& t, const astrocore::TimeZone& tz);
  void SetLocation(double longitudeDeg, double latitudeDeg);
  void SetHouseSystem(HouseSystem hs);
  void SetAspectOrbDeg(double orbDeg);
  void SetShowAxes(bool enabled);
  void SetShowLabels(bool enabled);
  void SetShowPlanets(bool enabled);
  void SetShowAspectLines(bool enabled);
  void SetShowAspectGrid(bool enabled);
  void SetSignLabelRadiusOffset(double offset);
  void ResetDefaults();

  // Get parameters
  const astrocore::Date& GetDate() const { return m_date; }
  const astrocore::Time& GetTime() const { return m_time; }
  const astrocore::TimeZone& GetTimeZone() const { return m_tz; }
  double GetLongitudeDeg() const { return m_longitudeDeg; }
  double GetLatitudeDeg() const { return m_latitudeDeg; }
  HouseSystem GetHouseSystem() const { return m_houseSystem; }
  double GetAspectOrbDeg() const { return m_aspectOrbDeg; }
  bool GetShowAxes() const { return m_showAxes; }
  bool GetShowLabels() const { return m_showLabels; }
  bool GetShowPlanets() const { return m_showPlanets; }
  bool GetShowAspectLines() const { return m_showAspectLines; }
  bool GetShowAspectGrid() const { return m_showAspectGrid; }
  double GetSignLabelRadiusOffset() const { return m_signLabelRadiusOffset; }

private:
  void OnPaint(wxPaintEvent&);
  void DrawChart(wxDC& dc, const wxSize& size);
  void DrawAspectLegend(wxDC& dc, const wxPoint& position);
  void DrawHouseNumbers(wxDC& dc, const wxPoint& center, int radius, const double cusps[12]);

  // parameters
  astrocore::Date m_date{2000,1,1};
  astrocore::Time m_time{12,0,0.0};
  astrocore::TimeZone m_tz{0.0, 0.0};
  double m_longitudeDeg{13.405};
  double m_latitudeDeg{52.52};
  HouseSystem m_houseSystem{HouseSystem::Equal};
  double m_aspectOrbDeg{3.0};
  bool m_showAxes{true};
  bool m_showLabels{true};
  bool m_showPlanets{true};
  bool m_showAspectLines{true};
  bool m_showAspectGrid{false};
  double m_signLabelRadiusOffset{44.0};
  
  // UI components
  AspectGridWindow* m_aspectGridWindow{nullptr};

  wxDECLARE_EVENT_TABLE();
};
