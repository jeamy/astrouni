#include <wx/wx.h>
#include <wx/log.h>
#include <sstream>
#include "astrocore/api.hpp"
#include "ChartPanel.hpp"
#include "Dialogs.hpp"

class AstroFrame : public wxFrame {
public:
  AstroFrame()
  : wxFrame(nullptr, wxID_ANY, "Astro GUI (wxWidgets)", wxDefaultPosition, wxSize(800, 600))
  {
    // Log to stderr so we can see diagnostics in terminal
    wxLog::SetActiveTarget(new wxLogStderr());
    m_panel = new ChartPanel(this);

    // Basic menu
    auto* fileMenu = new wxMenu();
    wxLogMessage("Appending File/Export");
    fileMenu->Append(ID_EXPORT_PNG, "Export PNG...\tCtrl+E");
    wxLogMessage("Appending File/Separator");
    fileMenu->AppendSeparator();
    wxLogMessage("Appending File/Exit");
    fileMenu->Append(ID_EXIT_APP, "E&xit\tCtrl+Q");

    // Chart menu
    auto* chartMenu = new wxMenu();
    auto* houseMenu = new wxMenu();
    wxLogMessage("Appending House/Equal");
    houseMenu->AppendRadioItem(ID_HOUSE_EQUAL, "Equal");
    wxLogMessage("Appending House/Placidus");
    houseMenu->AppendRadioItem(ID_HOUSE_PLACIDUS, "Placidus");
    wxLogMessage("Appending Chart/SubMenu House System");
    chartMenu->AppendSubMenu(houseMenu, "House System");
    wxLogMessage("Appending Chart/Separator 1");
    chartMenu->AppendSeparator();
    wxLogMessage("Appending Chart/Aspect Orb");
    chartMenu->Append(ID_ASPECT_ORB, "Set Aspect Orb...\tCtrl+Shift+O");
    wxLogMessage("Appending Chart/Separator 2");
    chartMenu->AppendSeparator();
    wxLogMessage("Appending Chart/Set DateTime");
    chartMenu->Append(ID_SET_DATETIME, "Set Date/Time/Zone...\tCtrl+T");
    wxLogMessage("Appending Chart/Set Location");
    chartMenu->Append(ID_SET_LOCATION, "Set Location...\tCtrl+L");
    wxLogMessage("Appending Chart/Separator 3");
    chartMenu->AppendSeparator();
    wxLogMessage("Appending Chart/Reset Defaults");
    chartMenu->Append(ID_RESET_DEFAULTS, "Reset Defaults\tCtrl+R");

    // View menu
    auto* viewMenu = new wxMenu();
    viewMenu->AppendCheckItem(ID_VIEW_AXES, "Show &Axes\tCtrl+A");
    viewMenu->AppendCheckItem(ID_VIEW_LABELS, "Show &Labels\tCtrl+L");
    viewMenu->AppendCheckItem(ID_VIEW_PLANETS, "Show &Planets\tCtrl+P");
    viewMenu->AppendCheckItem(ID_VIEW_ASPECT_LINES, "Show Aspect &Lines\tCtrl+Shift+A");
    viewMenu->AppendCheckItem(ID_VIEW_ASPECT_GRID, "Show Aspect &Grid\tCtrl+G");
    viewMenu->AppendSeparator();

    // Help menu
    auto* helpMenu = new wxMenu();
    helpMenu->Append(ID_HELP_ABOUT, "About...\tF1");

    // Diagnostics: list menu items and labels
    auto dumpMenu = [](const char* name, wxMenu* m){
      wxLogMessage("Menu '%s' items:", name);
      for (auto* item : m->GetMenuItems()) {
        wxString lbl = item->GetItemLabelText();
        wxLogMessage("  id=%d, kind=%d, empty=%s, label='%s'", item->GetId(), (int)item->GetKind(), lbl.empty()?"yes":"no", lbl);
      }
    };
    dumpMenu("File", fileMenu);
    dumpMenu("Chart", chartMenu);
    dumpMenu("House", houseMenu);
    dumpMenu("View", viewMenu);
    dumpMenu("Help", helpMenu);

    auto* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "File");
    menuBar->Append(chartMenu, "Chart");
    menuBar->Append(viewMenu, "View");
    menuBar->Append(helpMenu, "Help");
    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText("Ready");

    Bind(wxEVT_MENU, &AstroFrame::OnExportPNG, this, ID_EXPORT_PNG);
    Bind(wxEVT_MENU, [this](wxCommandEvent&){ Close(true); }, ID_EXIT_APP);

    // Chart bindings
    Bind(wxEVT_MENU, &AstroFrame::OnHouseEqual, this, ID_HOUSE_EQUAL);
    Bind(wxEVT_MENU, &AstroFrame::OnHousePlacidus, this, ID_HOUSE_PLACIDUS);
    Bind(wxEVT_MENU, &AstroFrame::OnSetAspectOrb, this, ID_ASPECT_ORB);
    Bind(wxEVT_MENU, &AstroFrame::OnSetDateTime, this, ID_SET_DATETIME);
    Bind(wxEVT_MENU, &AstroFrame::OnSetLocation, this, ID_SET_LOCATION);
    Bind(wxEVT_MENU, &AstroFrame::OnResetDefaults, this, ID_RESET_DEFAULTS);

    // View bindings
    Bind(wxEVT_MENU, &AstroFrame::OnToggleAxes, this, ID_VIEW_AXES);
    Bind(wxEVT_MENU, &AstroFrame::OnToggleLabels, this, ID_VIEW_LABELS);
    Bind(wxEVT_MENU, &AstroFrame::OnTogglePlanets, this, ID_VIEW_PLANETS);
    Bind(wxEVT_MENU, &AstroFrame::OnToggleAspectLines, this, ID_VIEW_ASPECT_LINES);
    Bind(wxEVT_MENU, &AstroFrame::OnToggleAspectGrid, this, ID_VIEW_ASPECT_GRID);

    // Help bindings
    Bind(wxEVT_MENU, &AstroFrame::OnAbout, this, ID_HELP_ABOUT);

    // Defaults (set after menubar is attached)
    if (m_panel) {
      bool axes = m_panel->GetShowAxes();
      bool labels = m_panel->GetShowLabels();
      bool planets = m_panel->GetShowPlanets();
      bool aspectLines = m_panel->GetShowAspectLines();
      bool aspectGrid = m_panel->GetShowAspectGrid();
      viewMenu->Check(ID_VIEW_AXES, axes);
      viewMenu->Check(ID_VIEW_LABELS, labels);
      viewMenu->Check(ID_VIEW_PLANETS, planets);
      viewMenu->Check(ID_VIEW_ASPECT_LINES, aspectLines);
      viewMenu->Check(ID_VIEW_ASPECT_GRID, aspectGrid);
      // no demo aspects checkbox anymore
    }
    // Default house system selection reflects ChartPanel default (Equal)
    houseMenu->Check(ID_HOUSE_EQUAL, true);
  }
private:
  enum {
    ID_EXPORT_PNG = wxID_HIGHEST + 1,
    ID_EXIT_APP,
    ID_HOUSE_EQUAL,
    ID_HOUSE_PLACIDUS,
    ID_ASPECT_ORB,
    ID_SET_DATETIME,
    ID_SET_LOCATION,
    ID_VIEW_AXES,
    ID_VIEW_LABELS,
    ID_VIEW_PLANETS,
    ID_VIEW_ASPECT_LINES,
    ID_VIEW_ASPECT_GRID,
    ID_HELP_ABOUT,
    ID_RESET_DEFAULTS,
  };

  void OnExportPNG(wxCommandEvent&)
  {
    wxFileDialog dlg(this, "Export PNG", "", "chart.png",
                     "PNG files (*.png)|*.png", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dlg.ShowModal() == wxID_CANCEL) return;
    if (m_panel && m_panel->ExportPNG(dlg.GetPath())) {
      SetStatusText("Exported: " + dlg.GetPath());
    } else {
      wxMessageBox("Export failed", "Error", wxOK | wxICON_ERROR, this);
    }
  }

  void OnHouseEqual(wxCommandEvent&) {
    if (!m_panel) return;
    m_panel->SetHouseSystem(ChartPanel::HouseSystem::Equal);
    SetStatusText("House system: Equal");
  }
  void OnHousePlacidus(wxCommandEvent&) {
    if (!m_panel) return;
    m_panel->SetHouseSystem(ChartPanel::HouseSystem::Placidus);
    SetStatusText("House system: Placidus");
  }
  // Removed: Demo aspects toggle handler
  void OnToggleAxes(wxCommandEvent&) {
    if (!m_panel) return;
    bool checked = GetMenuBar()->IsChecked(ID_VIEW_AXES);
    m_panel->SetShowAxes(checked);
    SetStatusText(checked ? "Axes: ON" : "Axes: OFF");
  }
  void OnToggleLabels(wxCommandEvent&) {
    if (!m_panel) return;
    bool checked = GetMenuBar()->IsChecked(ID_VIEW_LABELS);
    m_panel->SetShowLabels(checked);
    SetStatusText(checked ? "Labels: ON" : "Labels: OFF");
  }
  void OnSetAspectOrb(wxCommandEvent&) {
    if (!m_panel) return;
    double orb = m_panel->GetAspectOrbDeg();
    wxTextEntryDialog dlg(this, "Aspect orb (degrees):", "Aspect Orb", wxString::Format("%.2f", orb));
    if (dlg.ShowModal() == wxID_OK) {
      try {
        double v = std::stod(dlg.GetValue().ToStdString());
        if (v >= 0 && v <= 30) {
          m_panel->SetAspectOrbDeg(v);
          wxString val = wxString::Format("%.2f", v);
          wxString degree = wxString::FromUTF8("\xC2\xB0");
          SetStatusText(wxString("Aspect orb: ") + val + degree);
        }
      } catch (...) {}
    }
  }
  void OnSetDateTime(wxCommandEvent&) {
    if (!m_panel) return;
    auto d = m_panel->GetDate();
    auto t = m_panel->GetTime();
    auto tz = m_panel->GetTimeZone();
    DateTimeDialog dlg(this, d, t, tz);
    if (dlg.ShowModal() == wxID_OK) {
      astrocore::Date nd; astrocore::Time nt; astrocore::TimeZone ntz;
      if (dlg.GetResult(nd, nt, ntz)) {
        m_panel->SetDateTime(nd, nt, ntz);
        SetStatusText("Date/Time/Zone updated");
      } else {
        wxMessageBox("Invalid date/time/zone values.", "Input Error", wxOK | wxICON_ERROR, this);
      }
    }
  }
  void OnSetLocation(wxCommandEvent&) {
    if (!m_panel) return;
    double lon = m_panel->GetLongitudeDeg();
    double lat = m_panel->GetLatitudeDeg();
    LocationDialog dlg(this, lon, lat);
    if (dlg.ShowModal() == wxID_OK) {
      double nlon, nlat;
      if (dlg.GetResult(nlon, nlat)) {
        m_panel->SetLocation(nlon, nlat);
        SetStatusText(wxString::Format("Location: lon %.3f, lat %.3f", nlon, nlat));
      } else {
        wxMessageBox("Invalid location. Longitude must be -180..180, Latitude -90..90.", "Input Error", wxOK | wxICON_ERROR, this);
      }
    }
  }
  void OnResetDefaults(wxCommandEvent&) {
    if (!m_panel) return;
    m_panel->ResetDefaults();
    // Reflect defaults in menu checks
    if (auto* mb = GetMenuBar()) {
      mb->Check(ID_VIEW_AXES, true);
      mb->Check(ID_VIEW_LABELS, true);
      mb->Check(ID_VIEW_PLANETS, true);
      mb->Check(ID_VIEW_ASPECT_LINES, true);
      mb->Check(ID_VIEW_ASPECT_GRID, false);
      mb->Check(ID_HOUSE_EQUAL, true);
    }
    SetStatusText("Defaults restored");
  }
  void OnTogglePlanets(wxCommandEvent&) {
    if (!m_panel) return;
    bool checked = GetMenuBar()->IsChecked(ID_VIEW_PLANETS);
    m_panel->SetShowPlanets(checked);
    SetStatusText(checked ? "Planets: ON" : "Planets: OFF");
  }

  void OnToggleAspectLines(wxCommandEvent&) {
    if (!m_panel) return;
    bool checked = GetMenuBar()->IsChecked(ID_VIEW_ASPECT_LINES);
    m_panel->SetShowAspectLines(checked);
    SetStatusText(checked ? "Aspect Lines: ON" : "Aspect Lines: OFF");
  }

  void OnToggleAspectGrid(wxCommandEvent&) {
    if (!m_panel) return;
    bool checked = GetMenuBar()->IsChecked(ID_VIEW_ASPECT_GRID);
    m_panel->SetShowAspectGrid(checked);
    SetStatusText(checked ? "Aspect Grid: ON" : "Aspect Grid: OFF");
  }

  void OnAbout(wxCommandEvent&) {
    wxString msg;
    msg << "Astrology GUI\n";
    msg << "Core: astrocore library\n";
    msg << "Features: Asc/MC, Equal/Placidus houses, planets, aspects\n";
    wxMessageBox(msg, "About", wxOK | wxICON_INFORMATION, this);
  }
  ChartPanel* m_panel {nullptr};
};

class AstroApp : public wxApp {
public:
  bool OnInit() override {
    if (!wxApp::OnInit())
      return false;
    auto* frame = new AstroFrame();
    frame->Show(true);
    return true;
  }
};

wxIMPLEMENT_APP(AstroApp);
