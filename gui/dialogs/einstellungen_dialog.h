// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgAspekte from astrofil.c

#pragma once

#include <wx/wx.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include "../database/legacy_data_manager.h"

// 1:1 Port von DlgAspekte (EINSTELL Dialog)
class EinstellungenDialog : public wxDialog {
public:
    EinstellungenDialog(wxWindow* parent, LegacyDataManager* data_manager);
    
    // Einstellungen abrufen/setzen
    LegacySettings GetSettings() const;
    void SetSettings(const LegacySettings& settings);
    
private:
    // Event Handlers (1:1 Legacy)
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnReset(wxCommandEvent& event);
    void OnSettingChange(wxCommandEvent& event);
    
    // GUI Controls (1:1 Legacy Layout)
    // Anzeige-Optionen
    wxCheckBox* show_aspects_ctrl_;
    wxCheckBox* show_houses_ctrl_;
    wxCheckBox* show_degrees_ctrl_;
    wxCheckBox* show_retrograde_ctrl_;
    
    // Aspekt-Einstellungen
    wxCheckBox* show_major_aspects_ctrl_;
    wxCheckBox* show_minor_aspects_ctrl_;
    wxCheckBox* show_aspect_lines_ctrl_;
    wxCheckBox* show_aspect_symbols_ctrl_;
    
    // Planeten-Einstellungen
    wxCheckBox* show_all_planets_ctrl_;
    wxCheckBox* show_asteroids_ctrl_;
    wxCheckBox* show_fixed_stars_ctrl_;
    
    // Häuser-Einstellungen
    wxCheckBox* show_house_numbers_ctrl_;
    wxCheckBox* show_house_cusps_ctrl_;
    wxCheckBox* show_house_rulers_ctrl_;
    
    // Darstellungs-Optionen
    wxCheckBox* show_planet_symbols_ctrl_;
    wxCheckBox* show_zodiac_symbols_ctrl_;
    wxCheckBox* show_element_colors_ctrl_;
    
    // Buttons
    wxButton* ok_btn_;
    wxButton* cancel_btn_;
    wxButton* reset_btn_;
    
    // Daten-Manager
    LegacyDataManager* data_manager_;
    
    // Layout erstellen
    void CreateLayout();
    void InitializeControls();
    void UpdateControls();
    
    // Event IDs
    enum {
        ID_SHOW_ASPECTS = wxID_HIGHEST + 600,
        ID_SHOW_HOUSES,
        ID_SHOW_DEGREES,
        ID_SHOW_RETROGRADE,
        ID_SHOW_MAJOR_ASPECTS,
        ID_SHOW_MINOR_ASPECTS,
        ID_SHOW_ASPECT_LINES,
        ID_SHOW_ASPECT_SYMBOLS,
        ID_SHOW_ALL_PLANETS,
        ID_SHOW_ASTEROIDS,
        ID_SHOW_FIXED_STARS,
        ID_SHOW_HOUSE_NUMBERS,
        ID_SHOW_HOUSE_CUSPS,
        ID_SHOW_HOUSE_RULERS,
        ID_SHOW_PLANET_SYMBOLS,
        ID_SHOW_ZODIAC_SYMBOLS,
        ID_SHOW_ELEMENT_COLORS,
        ID_RESET_SETTINGS
    };
    
    wxDECLARE_EVENT_TABLE();
};
