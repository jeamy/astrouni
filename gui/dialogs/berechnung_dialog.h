// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgCalc from astrofil.c

#pragma once

#include <wx/wx.h>
#include <wx/gauge.h>
#include "../database/legacy_data_manager.h"

// 1:1 Port von DlgCalc (CALC Dialog)
class BerechnungDialog : public wxDialog {
public:
    BerechnungDialog(wxWindow* parent, const wxString& title = wxString::FromUTF8("Berechnung"));
    
    // Berechnung starten/stoppen
    void StartBerechnung(const wxString& beschreibung, int max_schritte = 100);
    void UpdateFortschritt(int schritt, const wxString& status = "");
    void BeendeBerechnung(bool erfolgreich = true);
    
    // Abbruch-Status
    bool IstAbgebrochen() const { return abgebrochen_; }
    
private:
    // Event Handlers (1:1 Legacy)
    void OnAbbrechen(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    
    // GUI Controls (1:1 Legacy Layout)
    wxStaticText* beschreibung_text_;
    wxGauge* fortschritt_gauge_;
    wxStaticText* status_text_;
    wxStaticText* prozent_text_;
    
    wxButton* abbrechen_btn_;
    
    // Status
    bool berechnung_laeuft_;
    bool abgebrochen_;
    int max_schritte_;
    int aktueller_schritt_;
    
    // Layout erstellen
    void CreateLayout();
    void UpdateAnzeige();
    
    // Event IDs
    enum {
        ID_ABBRECHEN = wxID_HIGHEST + 1100
    };
    
    wxDECLARE_EVENT_TABLE();
};
