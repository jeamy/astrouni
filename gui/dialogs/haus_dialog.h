// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgHausAuswahl from astrofil.c

#pragma once

#include <wx/wx.h>
#include <wx/radiobut.h>
#include "astro/data_types.h"

// 1:1 Port von DlgHausAuswahl (HAUS_AUSWAHL Dialog)
class HausDialog : public wxDialog {
public:
    HausDialog(wxWindow* parent);
    
    // Häusersystem abrufen/setzen
    astro::HouseSystem GetHouseSystem() const;
    void SetHouseSystem(astro::HouseSystem system);
    
private:
    // Event Handlers (1:1 Legacy)
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnHausSystemChange(wxCommandEvent& event);
    
    // GUI Controls (1:1 Legacy Layout)
    wxRadioButton* placidus_rb_;
    wxRadioButton* koch_rb_;
    wxRadioButton* porphyry_rb_;
    wxRadioButton* regiomontanus_rb_;
    wxRadioButton* campanus_rb_;
    wxRadioButton* equal_rb_;
    wxRadioButton* whole_sign_rb_;
    wxRadioButton* alcabitius_rb_;
    wxRadioButton* morinus_rb_;
    wxRadioButton* topocentric_rb_;
    
    // Beschreibung
    wxStaticText* beschreibung_text_;
    
    // Buttons
    wxButton* ok_btn_;
    wxButton* cancel_btn_;
    
    // Layout erstellen
    void CreateLayout();
    void InitializeControls();
    void UpdateBeschreibung();
    
    // Aktuell ausgewähltes System
    astro::HouseSystem current_system_;
    
    // Event IDs
    enum {
        ID_PLACIDUS = wxID_HIGHEST + 300,
        ID_KOCH,
        ID_PORPHYRY,
        ID_REGIOMONTANUS,
        ID_CAMPANUS,
        ID_EQUAL,
        ID_WHOLE_SIGN,
        ID_ALCABITIUS,
        ID_MORINUS,
        ID_TOPOCENTRIC
    };
    
    wxDECLARE_EVENT_TABLE();
};
