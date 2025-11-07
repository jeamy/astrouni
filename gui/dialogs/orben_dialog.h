// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgOrbenEin from astrofil.c

#pragma once

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include "../database/legacy_data_manager.h"

// 1:1 Port von DlgOrbenEin (ORBEN_EIN Dialog)
class OrbenDialog : public wxDialog {
public:
    OrbenDialog(wxWindow* parent, LegacyDataManager* data_manager);
    
    // Orben abrufen/setzen
    LegacySettings::Orbs GetOrbs() const;
    void SetOrbs(const LegacySettings::Orbs& orbs);
    
private:
    // Event Handlers (1:1 Legacy)
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnReset(wxCommandEvent& event);
    void OnOrbChange(wxSpinDoubleEvent& event);
    
    // GUI Controls (1:1 Legacy Layout)
    wxSpinCtrlDouble* sun_moon_ctrl_;        // Sonne-Mond
    wxSpinCtrlDouble* sun_planets_ctrl_;     // Sonne-Planeten
    wxSpinCtrlDouble* moon_planets_ctrl_;    // Mond-Planeten
    wxSpinCtrlDouble* planets_planets_ctrl_; // Planeten-Planeten
    wxSpinCtrlDouble* minor_aspects_ctrl_;   // Nebenaspekte
    
    // Beschreibung
    wxStaticText* beschreibung_text_;
    
    // Buttons
    wxButton* ok_btn_;
    wxButton* cancel_btn_;
    wxButton* reset_btn_;
    
    // Daten-Manager
    LegacyDataManager* data_manager_;
    
    // Layout erstellen
    void CreateLayout();
    void InitializeControls();
    void UpdateBeschreibung();
    
    // Event IDs
    enum {
        ID_SUN_MOON = wxID_HIGHEST + 400,
        ID_SUN_PLANETS,
        ID_MOON_PLANETS,
        ID_PLANETS_PLANETS,
        ID_MINOR_ASPECTS,
        ID_RESET
    };
    
    wxDECLARE_EVENT_TABLE();
};
