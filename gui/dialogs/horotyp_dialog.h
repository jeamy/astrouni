// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgHoroAuswahl from astrofil.c

#pragma once

#include <wx/wx.h>
#include <wx/radiobut.h>
#include "../database/legacy_data_manager.h"

// Horoskop-Typen (1:1 Legacy)
enum class HoroType {
    RADIX = 0,      // Standard-Radix
    TRANSIT = 1,    // Transit
    SYNASTRIE = 2,  // Synastrie (Partnervergleich)
    COMPOSIT = 3,   // Composit
    SOLAR = 4,      // Solar-Return
    LUNAR = 5,      // Lunar-Return
    PROGRESSION = 6 // Progression
};

// 1:1 Port von DlgHoroAuswahl (HORO_TYP Dialog)
class HoroTypDialog : public wxDialog {
public:
    HoroTypDialog(wxWindow* parent, LegacyDataManager* data_manager);
    
    // Horoskop-Typ abrufen/setzen
    HoroType GetHoroType() const;
    void SetHoroType(HoroType type);
    
private:
    // Event Handlers (1:1 Legacy)
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnTypeChange(wxCommandEvent& event);
    
    // GUI Controls (1:1 Legacy Layout)
    wxRadioButton* radix_rb_;
    wxRadioButton* transit_rb_;
    wxRadioButton* synastrie_rb_;
    wxRadioButton* composit_rb_;
    wxRadioButton* solar_rb_;
    wxRadioButton* lunar_rb_;
    wxRadioButton* progression_rb_;
    
    // Beschreibung
    wxStaticText* beschreibung_text_;
    
    // Buttons
    wxButton* ok_btn_;
    wxButton* cancel_btn_;
    
    // Daten-Manager
    LegacyDataManager* data_manager_;
    
    // Aktueller Typ
    HoroType current_type_;
    
    // Layout erstellen
    void CreateLayout();
    void InitializeControls();
    void UpdateBeschreibung();
    
    // Event IDs
    enum {
        ID_RADIX = wxID_HIGHEST + 700,
        ID_TRANSIT,
        ID_SYNASTRIE,
        ID_COMPOSIT,
        ID_SOLAR,
        ID_LUNAR,
        ID_PROGRESSION
    };
    
    wxDECLARE_EVENT_TABLE();
};
