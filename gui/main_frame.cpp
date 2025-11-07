// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project

#include "main_frame.h"
#include "astro/i18n.h"
#include <wx/filedlg.h>
#include <wx/msgdlg.h>

using namespace astro::i18n;

// Event Table (1:1 Legacy Mapping)
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    // Datei
    EVT_MENU(CM_U_NEW, MainFrame::OnNew)
    EVT_MENU(CM_U_FILEOPEN, MainFrame::OnOpen)
    EVT_MENU(CM_U_SAVE, MainFrame::OnSave)
    EVT_MENU(CM_U_SAVEU, MainFrame::OnSaveAs)
    EVT_MENU(CM_U_PRINT, MainFrame::OnPrint)
    EVT_MENU(CM_U_PINST, MainFrame::OnPrinterSetup)
    EVT_MENU(wxID_EXIT, MainFrame::OnExit)
    
    // Daten
    EVT_MENU(CM_U_PERSON, MainFrame::OnPersonDialog)
    EVT_MENU(CM_U_ORT, MainFrame::OnOrtDialog)
    
    // Horoskop
    EVT_MENU(CM_U_HOROTYP, MainFrame::OnHoroTyp)
    EVT_MENU(CM_U_HAUSER, MainFrame::OnHausSystem)
    EVT_MENU(CM_U_ORBEN, MainFrame::OnOrben)
    EVT_MENU(CM_U_FARBEN, MainFrame::OnFarben)
    EVT_MENU(CM_U_EINST, MainFrame::OnEinstellungen)
    
    // Hilfe
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(1024, 768)) {
    
    // Legacy Data Manager initialisieren
    data_manager_ = std::make_unique<LegacyDataManager>();
    data_manager_->LoadDefaultSettings();
    data_manager_->LoadUserSettings();
    data_manager_->LoadPersonDatabase();
    data_manager_->LoadPlaceDatabase();
    data_manager_->LoadEuropaPlaces();
    
    // Menü erstellen (1:1 Legacy-Struktur mit i18n)
    // Datei-Menü
    wxMenu* file_menu = new wxMenu;
    file_menu->Append(CM_U_NEW, wxString::FromUTF8("&") + wxString::FromUTF8(tr("menu.new").c_str()) + wxString::FromUTF8("\tCtrl-N"));
    file_menu->Append(CM_U_FILEOPEN, wxString::FromUTF8("&") + wxString::FromUTF8(tr("menu.open").c_str()) + wxString::FromUTF8("\tCtrl-O"));
    file_menu->Append(CM_U_SAVE, wxString::FromUTF8("&") + wxString::FromUTF8(tr("menu.save").c_str()) + wxString::FromUTF8("\tCtrl-S"));
    file_menu->Append(CM_U_SAVEU, wxString::FromUTF8(tr("menu.save_as").c_str()));
    file_menu->AppendSeparator();
    file_menu->Append(CM_U_PRINT, wxString::FromUTF8("&") + wxString::FromUTF8(tr("menu.print").c_str()) + wxString::FromUTF8("\tCtrl-P"));
    file_menu->Append(CM_U_PINST, wxString::FromUTF8(tr("menu.printer_setup").c_str()));
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, wxString::FromUTF8("&") + wxString::FromUTF8(tr("menu.exit").c_str()) + wxString::FromUTF8("\tAlt-F4"));
    
    // Daten-Menü
    wxMenu* data_menu = new wxMenu;
    data_menu->Append(CM_U_PERSON, wxString::FromUTF8("&") + wxString::FromUTF8(tr("menu.person").c_str()));
    data_menu->Append(CM_U_ORT, wxString::FromUTF8("&") + wxString::FromUTF8(tr("menu.place").c_str()));
    
    // Horoskop-Menü
    wxMenu* horo_menu = new wxMenu;
    horo_menu->Append(CM_U_HOROTYP, wxString::FromUTF8(tr("menu.horo_type").c_str()));
    horo_menu->Append(CM_U_HAUSER, wxString::FromUTF8("&") + wxString::FromUTF8(tr("menu.house_system").c_str()));
    horo_menu->AppendSeparator();
    horo_menu->Append(CM_U_ORBEN, wxString::FromUTF8("&") + wxString::FromUTF8(tr("menu.orbs").c_str()));
    horo_menu->Append(CM_U_FARBEN, wxString::FromUTF8("&") + wxString::FromUTF8(tr("menu.colors").c_str()));
    horo_menu->Append(CM_U_EINST, wxString::FromUTF8("&") + wxString::FromUTF8(tr("menu.settings").c_str()));
    
    // Hilfe-Menü
    wxMenu* help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT, wxString::FromUTF8("&") + wxString::FromUTF8(tr("menu.about").c_str()));
    
    // Menüleiste zusammenbauen
    menu_bar_ = new wxMenuBar;
    menu_bar_->Append(file_menu, wxString::FromUTF8("&") + wxString::FromUTF8(tr("menu.file").c_str()));
    menu_bar_->Append(data_menu, wxString::FromUTF8("&") + wxString::FromUTF8(tr("menu.data").c_str()));
    menu_bar_->Append(horo_menu, wxString::FromUTF8("&") + wxString::FromUTF8(tr("menu.horoscope").c_str()));
    menu_bar_->Append(help_menu, wxString::FromUTF8("&") + wxString::FromUTF8(tr("menu.help").c_str()));
    
    SetMenuBar(menu_bar_);
    
    // Statusleiste
    status_bar_ = CreateStatusBar(2);
    SetStatusText(wxString::FromUTF8(tr("status.ready").c_str()), 0);
    
    // Chart Panel erstellen
    chart_panel_ = new ChartPanel(this);
    // Layout: Panel füllt das Fenster
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(chart_panel_, 1, wxEXPAND);
    SetSizer(sizer);

    // Zentrieren
    Centre();
}

// Datei-Menü Implementierung
void MainFrame::OnNew(wxCommandEvent& event) {
    // PersonDialog öffnen (1:1 Legacy)
    PersonDialog dialog(this);
    if (dialog.ShowModal() != wxID_OK) return;

    SetStatusText(wxString::FromUTF8(tr("status.ready").c_str()), 0);

    try {
        // Daten aus Dialog
        astro::RadixData radix = dialog.GetRadixData();

        // Ephemeris
        auto ephemeris = std::make_shared<astro::SwissEphemeris>();
        ephemeris->initialize("../swisseph/ephe");

        // Berechnen
        auto calculator = std::make_unique<astro::AstroCalculator>(ephemeris);
        auto chart = calculator->calculate_chart(
            radix,
            astro::HouseSystem::Placidus,
            astro::AstroCalculator::DEFAULT_PLANETS,
            astro::AstroCalculator::DEFAULT_ASPECTS,
            8.0
        );

        // Anzeigen
        chart_panel_->SetChart(chart);
        SetStatusText(wxString::FromUTF8(tr("status.chart_created").c_str()), 0);

    } catch (const std::exception& e) {
        wxMessageBox(wxString::FromUTF8(e.what()), wxString::FromUTF8("Fehler"), wxOK | wxICON_ERROR);
    }
}

void MainFrame::OnSaveAs(wxCommandEvent& event) {
    wxMessageBox("Speichern unter...", "Info", wxOK | wxICON_INFORMATION);
    // TODO: Implementieren
}

void MainFrame::OnPrint(wxCommandEvent& event) {
    wxMessageBox("Druck-Dialog wird implementiert...", "Info", wxOK | wxICON_INFORMATION);
    // TODO: Implementieren
}

void MainFrame::OnPrinterSetup(wxCommandEvent& event) {
    wxMessageBox("Drucker-Einrichtung...", "Info", wxOK | wxICON_INFORMATION);
    // TODO: Implementieren
}

// Daten-Menü Implementierung
void MainFrame::OnPersonDialog(wxCommandEvent& event) {
    // PersonDialog öffnen (1:1 Legacy DlgPErfassen)
    PersonDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK) {
        astro::RadixData radix = dialog.GetRadixData();
        wxString info = wxString::Format(
            wxString::FromUTF8("Person gespeichert:\n%s\n%s"),
            wxString::FromUTF8(radix.name.c_str()),
            wxString::FromUTF8(radix.birth_location.country_code.c_str())
        );
        SetStatusText(info, 0);
    }
}

void MainFrame::OnOrtDialog(wxCommandEvent& event) {
    // OrtDialog öffnen (1:1 Legacy DlgOErfassen)
    OrtDialog dialog(this);
    if (dialog.ShowModal() == wxID_OK) {
        astro::GeoLocation location = dialog.GetGeoLocation();
        wxString info = wxString::Format(
            wxString::FromUTF8("Ort gespeichert:\n%s\nBreite: %.4f°, Länge: %.4f°"),
            wxString::FromUTF8(location.country_code.c_str()),
            location.latitude,
            location.longitude
        );
        SetStatusText(info, 0);
    }
}

// Horoskop-Menü Implementierung
void MainFrame::OnHoroTyp(wxCommandEvent& event) {
    // HoroTyp-Dialog öffnen (1:1 Legacy DlgHoroAuswahl)
    wxArrayString choices;
    choices.Add(wxString::FromUTF8("Radix"));
    choices.Add(wxString::FromUTF8("Transit"));
    choices.Add(wxString::FromUTF8("Synastrie"));
    choices.Add(wxString::FromUTF8("Composit"));
    choices.Add(wxString::FromUTF8("Solar"));
    choices.Add(wxString::FromUTF8("Lunar"));
    
    wxSingleChoiceDialog dialog(this, 
        wxString::FromUTF8("Wählen Sie den Horoskop-Typ:"),
        wxString::FromUTF8("Horoskop-Typ Auswahl"),
        choices);
    
    dialog.SetSelection(0); // Radix als Standard
    
    if (dialog.ShowModal() == wxID_OK) {
        wxString selected = dialog.GetStringSelection();
        SetStatusText(wxString::FromUTF8("Horoskop-Typ: ") + selected, 0);
        
        // TODO: Horoskop-Typ in Einstellungen speichern und verwenden
        if (selected == wxString::FromUTF8("Radix")) {
            // Standard-Radix-Modus
        } else if (selected == wxString::FromUTF8("Transit")) {
            // Transit-Modus aktivieren
        } else if (selected == wxString::FromUTF8("Synastrie")) {
            // Synastrie-Modus aktivieren (2 Personen)
        }
        // Weitere Typen...
    }
}

void MainFrame::OnHausSystem(wxCommandEvent& event) {
    // HausDialog öffnen (1:1 Legacy DlgHausAuswahl)
    HausDialog dialog(this);
    dialog.SetHouseSystem(astro::HouseSystem::Placidus); // Aktuelles System setzen
    
    if (dialog.ShowModal() == wxID_OK) {
        astro::HouseSystem system = dialog.GetHouseSystem();
        wxString system_name;
        
        switch (system) {
            case astro::HouseSystem::Placidus: system_name = wxString::FromUTF8("Placidus"); break;
            case astro::HouseSystem::Koch: system_name = wxString::FromUTF8("Koch"); break;
            case astro::HouseSystem::Porphyry: system_name = wxString::FromUTF8("Porphyry"); break;
            case astro::HouseSystem::Regiomontanus: system_name = wxString::FromUTF8("Regiomontanus"); break;
            case astro::HouseSystem::Campanus: system_name = wxString::FromUTF8("Campanus"); break;
            case astro::HouseSystem::Equal: system_name = wxString::FromUTF8("Equal House"); break;
            case astro::HouseSystem::WholeSign: system_name = wxString::FromUTF8("Whole Sign"); break;
            case astro::HouseSystem::Alcabitius: system_name = wxString::FromUTF8("Alcabitius"); break;
            case astro::HouseSystem::Morinus: system_name = wxString::FromUTF8("Morinus"); break;
            case astro::HouseSystem::Topocentric: system_name = wxString::FromUTF8("Topocentric"); break;
        }
        
        SetStatusText(wxString::FromUTF8("Häusersystem: ") + system_name, 0);
    }
}

void MainFrame::OnOrben(wxCommandEvent& event) {
    // OrbenDialog öffnen (1:1 Legacy DlgOrbenEin)
    OrbenDialog dialog(this, data_manager_.get());
    if (dialog.ShowModal() == wxID_OK) {
        SetStatusText(wxString::FromUTF8("Orben-Einstellungen gespeichert"), 0);
    }
}

void MainFrame::OnFarben(wxCommandEvent& event) {
    // FarbenDialog öffnen (1:1 Legacy DlgFarben)
    FarbenDialog dialog(this, data_manager_.get());
    if (dialog.ShowModal() == wxID_OK) {
        SetStatusText(wxString::FromUTF8("Farben-Einstellungen gespeichert"), 0);
    }
}

void MainFrame::OnEinstellungen(wxCommandEvent& event) {
    // EinstellungenDialog öffnen (1:1 Legacy DlgAspekte)
    // TODO: EinstellungenDialog implementieren - zeige aktuell verfügbare Einstellungen
    wxString settings_info = wxString::Format(
        wxString::FromUTF8("Aktuelle Einstellungen:\n\n"
                          "Häusersystem: %s\n"
                          "Aspekte anzeigen: %s\n"
                          "Häuser anzeigen: %s\n"
                          "Grade anzeigen: %s\n"
                          "Retrograde anzeigen: %s\n\n"
                          "Orben:\n"
                          "• Sonne-Mond: %.1f°\n"
                          "• Sonne-Planeten: %.1f°\n"
                          "• Mond-Planeten: %.1f°\n"
                          "• Planeten-Planeten: %.1f°\n"
                          "• Nebenaspekte: %.1f°"),
        "Placidus",
        data_manager_->GetSettings().show_aspects ? "Ja" : "Nein",
        data_manager_->GetSettings().show_houses ? "Ja" : "Nein", 
        data_manager_->GetSettings().show_degrees ? "Ja" : "Nein",
        data_manager_->GetSettings().show_retrograde ? "Ja" : "Nein",
        data_manager_->GetSettings().orbs.sun_moon,
        data_manager_->GetSettings().orbs.sun_planets,
        data_manager_->GetSettings().orbs.moon_planets,
        data_manager_->GetSettings().orbs.planets_planets,
        data_manager_->GetSettings().orbs.minor_aspects
    );
    
    wxMessageBox(settings_info, wxString::FromUTF8("Einstellungen"), wxOK | wxICON_INFORMATION);
}

void MainFrame::OnOpen(wxCommandEvent& event) {
    wxFileDialog dialog(this, wxString::FromUTF8(tr("dialog.open_chart").c_str()), "", "",
                       "AstroUniverse Daten (*.dat)|*.dat",
                       wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    
    if (dialog.ShowModal() == wxID_OK) {
        wxString path = dialog.GetPath();
        SetStatusText(wxString::FromUTF8(tr("status.file_opened").c_str()) + " " + path, 0);
        // TODO: .dat File laden
    }
}

void MainFrame::OnSave(wxCommandEvent& event) {
    wxFileDialog dialog(this, wxString::FromUTF8(tr("dialog.save_chart").c_str()), "", "",
                       "AstroUniverse Daten (*.dat)|*.dat",
                       wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    
    if (dialog.ShowModal() == wxID_OK) {
        wxString path = dialog.GetPath();
        SetStatusText(wxString::FromUTF8(tr("status.file_saved").c_str()) + " " + path, 0);
        // TODO: Chart speichern
    }
}

void MainFrame::OnExit(wxCommandEvent& event) {
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& event) {
    wxString about_text = 
        "AstroUniverse - Astrologie-Software\n\n"
        "Version 0.1.0 (Phase 3 - GUI)\n"
        "Copyright (C) 2025 AstroUniverse Project\n\n"
        "Lizenz: GPL v2\n"
        "Swiss Ephemeris: GPL v2 / LGPL\n\n"
        "Language: " + wxString(I18n::instance().get_language() == Language::German ? "Deutsch" : "English");
    
    wxMessageBox(about_text, tr("menu.about"), wxOK | wxICON_INFORMATION);
}
