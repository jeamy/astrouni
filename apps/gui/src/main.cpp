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
    fileMenu->Append(ID_EXPORT_PNG, "PNG exportieren...\tCtrl+E");
    wxLogMessage("OK File/Export id=%d", ID_EXPORT_PNG);
    wxLogMessage("Appending File/Separator");
    fileMenu->AppendSeparator();
    wxLogMessage("OK File/Separator");
    wxLogMessage("Appending File/Exit");
    fileMenu->Append(ID_EXIT_APP, "Beenden\tCtrl+Q");
    wxLogMessage("OK File/Exit id=%d", ID_EXIT_APP);

    // Chart menu
    auto* chartMenu = new wxMenu();
    auto* houseMenu = new wxMenu();
    wxLogMessage("Appending House/Equal");
    houseMenu->AppendRadioItem(ID_HOUSE_EQUAL, "Gleich");
    wxLogMessage("OK House/Equal id=%d", ID_HOUSE_EQUAL);
    wxLogMessage("Appending House/Placidus");
    houseMenu->AppendRadioItem(ID_HOUSE_PLACIDUS, "Placidus");
    wxLogMessage("OK House/Placidus id=%d", ID_HOUSE_PLACIDUS);
    wxLogMessage("Appending Chart/SubMenu House System");
    chartMenu->AppendSubMenu(houseMenu, "Haus-System");
    wxLogMessage("OK Chart/SubMenu House System");
    wxLogMessage("Appending Chart/Separator 1");
    chartMenu->AppendSeparator();
    wxLogMessage("OK Chart/Separator 1");
    wxLogMessage("Appending Chart/Aspect Orb");
    chartMenu->Append(ID_ASPECT_ORB, "Aspekt-Orb festlegen...\tCtrl+Shift+O");
    wxLogMessage("OK Chart/Aspect Orb id=%d", ID_ASPECT_ORB);
    wxLogMessage("Appending Chart/Separator 2");
    chartMenu->AppendSeparator();
    wxLogMessage("Appending Chart/Set DateTime");
    chartMenu->Append(ID_SET_DATETIME, "Datum/Uhrzeit/Zone festlegen...\tCtrl+T");
    wxLogMessage("OK Chart/Set DateTime id=%d", ID_SET_DATETIME);
    wxLogMessage("Appending Chart/Set Location");
    chartMenu->Append(ID_SET_LOCATION, "Ort festlegen...\tCtrl+L");
    wxLogMessage("OK Chart/Set Location id=%d", ID_SET_LOCATION);
    wxLogMessage("Appending Chart/Separator 3");
    chartMenu->AppendSeparator();
    wxLogMessage("Appending Chart/Reset Defaults");
    chartMenu->Append(ID_RESET_DEFAULTS, wxString::FromUTF8("Standardwerte zurücksetzen\tCtrl+R"));
    wxLogMessage("OK Chart/Reset Defaults id=%d", ID_RESET_DEFAULTS);

    // View menu
    auto* viewMenu = new wxMenu();
    wxLogMessage("Appending View/Axes");
    viewMenu->AppendCheckItem(ID_VIEW_AXES, "&Achsen anzeigen\tCtrl+A");
    wxLogMessage("OK View/Axes id=%d", ID_VIEW_AXES);
    wxLogMessage("Appending View/Labels");
    viewMenu->AppendCheckItem(ID_VIEW_LABELS, "&Beschriftungen anzeigen\tCtrl+L");
    wxLogMessage("OK View/Labels id=%d", ID_VIEW_LABELS);
    wxLogMessage("Appending View/Planets");
    viewMenu->AppendCheckItem(ID_VIEW_PLANETS, "&Planeten anzeigen\tCtrl+P");
    wxLogMessage("OK View/Planets id=%d", ID_VIEW_PLANETS);
    wxLogMessage("Appending View/Aspect Lines");
    viewMenu->AppendCheckItem(ID_VIEW_ASPECT_LINES, "Aspekt&linien anzeigen\tCtrl+Shift+A");
    wxLogMessage("OK View/Aspect Lines id=%d", ID_VIEW_ASPECT_LINES);
    wxLogMessage("Appending View/Aspect Grid");
    viewMenu->AppendCheckItem(ID_VIEW_ASPECT_GRID, "Aspekt&gitter anzeigen\tCtrl+G");
    wxLogMessage("OK View/Aspect Grid id=%d", ID_VIEW_ASPECT_GRID);

    // Help menu
    auto* helpMenu = new wxMenu();
    wxLogMessage("Appending Help/About");
    helpMenu->Append(ID_HELP_ABOUT, wxString::FromUTF8("Über...\tF1"));
    wxLogMessage("OK Help/About id=%d", ID_HELP_ABOUT);

    // Diagnostics: list menu items and labels
    auto dumpMenu = [](const char* name, wxMenu* m){
      wxLogMessage("Menu '%s' items:", name);
      for (auto* item : m->GetMenuItems()) {
        wxString lbl = item->GetItemLabelText();
        wxLogMessage("  id=%d, kind=%d, empty=%s, label='%s'", item->GetId(), (int)item->GetKind(), lbl.empty()?"yes":"no", lbl);
      }
    };
    dumpMenu("Datei", fileMenu);
    dumpMenu("Horoskop", chartMenu);
    dumpMenu("Haus", houseMenu);
    dumpMenu("Ansicht", viewMenu);
    dumpMenu("Hilfe", helpMenu);

    auto* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "Datei");
    menuBar->Append(chartMenu, "Horoskop");
    menuBar->Append(viewMenu, "Ansicht");
    menuBar->Append(helpMenu, "Hilfe");
    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText("Bereit");

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
    ID_EXPORT_PNG = wxID_HIGHEST + 1000,
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
    wxFileDialog dlg(this, "PNG exportieren", "", "chart.png",
                     "PNG-Dateien (*.png)|*.png", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dlg.ShowModal() == wxID_CANCEL) return;
    if (m_panel && m_panel->ExportPNG(dlg.GetPath())) {
      SetStatusText("Exportiert: " + dlg.GetPath());
    } else {
      wxMessageBox("Export fehlgeschlagen", "Fehler", wxOK | wxICON_ERROR, this);
    }
  }

  void OnHouseEqual(wxCommandEvent&) {
    if (!m_panel) return;
    m_panel->SetHouseSystem(ChartPanel::HouseSystem::Equal);
    SetStatusText("Haussystem: Gleich");
  }
  void OnHousePlacidus(wxCommandEvent&) {
    if (!m_panel) return;
    m_panel->SetHouseSystem(ChartPanel::HouseSystem::Placidus);
    SetStatusText("Haussystem: Placidus");
  }
  // Removed: Demo aspects toggle handler
  void OnToggleAxes(wxCommandEvent&) {
    if (!m_panel) return;
    bool checked = GetMenuBar()->IsChecked(ID_VIEW_AXES);
    m_panel->SetShowAxes(checked);
    SetStatusText(checked ? "Achsen: AN" : "Achsen: AUS");
  }
  void OnToggleLabels(wxCommandEvent&) {
    if (!m_panel) return;
    bool checked = GetMenuBar()->IsChecked(ID_VIEW_LABELS);
    m_panel->SetShowLabels(checked);
    SetStatusText(checked ? "Beschriftungen: AN" : "Beschriftungen: AUS");
  }
  void OnSetAspectOrb(wxCommandEvent&) {
    if (!m_panel) return;
    double orb = m_panel->GetAspectOrbDeg();
    wxTextEntryDialog dlg(this, "Aspekt-Orb (Grad):", "Aspekt-Orb", wxString::Format("%.2f", orb));
    if (dlg.ShowModal() == wxID_OK) {
      try {
        double v = std::stod(dlg.GetValue().ToStdString());
        if (v >= 0 && v <= 30) {
          m_panel->SetAspectOrbDeg(v);
          wxString val = wxString::Format("%.2f", v);
          wxString degree = wxString::FromUTF8("\xC2\xB0");
          SetStatusText(wxString("Aspekt-Orb: ") + val + degree);
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
        SetStatusText("Datum/Uhrzeit/Zone aktualisiert");
      } else {
        wxMessageBox("Ungültige Werte für Datum/Uhrzeit/Zeitzone.", "Eingabefehler", wxOK | wxICON_ERROR, this);
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
        SetStatusText(wxString::Format("Ort: Länge %.3f, Breite %.3f", nlon, nlat));
      } else {
        wxMessageBox("Ungültiger Ort. Länge muss -180..180, Breite -90..90 sein.", "Eingabefehler", wxOK | wxICON_ERROR, this);
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
    SetStatusText("Standardwerte zurückgesetzt");
  }
  void OnTogglePlanets(wxCommandEvent&) {
    if (!m_panel) return;
    bool checked = GetMenuBar()->IsChecked(ID_VIEW_PLANETS);
    m_panel->SetShowPlanets(checked);
    SetStatusText(checked ? "Planeten: AN" : "Planeten: AUS");
  }

  void OnToggleAspectLines(wxCommandEvent&) {
    if (!m_panel) return;
    bool checked = GetMenuBar()->IsChecked(ID_VIEW_ASPECT_LINES);
    m_panel->SetShowAspectLines(checked);
    SetStatusText(checked ? "Aspektlinien: AN" : "Aspektlinien: AUS");
  }

  void OnToggleAspectGrid(wxCommandEvent&) {
    if (!m_panel) return;
    bool checked = GetMenuBar()->IsChecked(ID_VIEW_ASPECT_GRID);
    m_panel->SetShowAspectGrid(checked);
    SetStatusText(checked ? "Aspektgitter: AN" : "Aspektgitter: AUS");
  }

  

  void OnAbout(wxCommandEvent&) {
    wxString msg;
    msg << "Astrologie GUI\n";
    msg << "Kern: astrocore-Bibliothek\n";
    msg << "Funktionen: AC/MC, Häuser (Equal/Placidus), Planeten, Aspekte\n";
    wxMessageBox(msg, "Info", wxOK | wxICON_INFORMATION, this);
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
