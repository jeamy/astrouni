// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgAbort from astrofil.c

#pragma once

#include <wx/wx.h>
#include <wx/gauge.h>
#include <wx/timer.h>

// 1:1 Port von DlgAbort (ABORT Dialog)
class AbbruchDialog : public wxDialog {
public:
    AbbruchDialog(wxWindow* parent, const wxString& title = wxString::FromUTF8("Berechnung läuft..."));
    
    // Abbruch-Status
    bool IstAbgebrochen() const { return abgebrochen_; }
    void SetzeAbgebrochen() { abgebrochen_ = true; }
    
    // Fortschritt aktualisieren
    void UpdateFortschritt(const wxString& status, int prozent = -1);
    void SetzeMaximum(int maximum);
    
    // Auto-Close nach Fertigstellung
    void SetAutoClose(bool auto_close, int delay_ms = 1000);
    
private:
    // Event Handlers (1:1 Legacy)
    void OnAbbrechen(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnTimer(wxTimerEvent& event);
    
    // GUI Controls (1:1 Legacy Layout)
    wxStaticText* status_text_;
    wxGauge* fortschritt_gauge_;
    wxButton* abbrechen_btn_;
    
    // Timer für Auto-Close
    wxTimer* auto_close_timer_;
    
    // Status
    bool abgebrochen_;
    bool auto_close_;
    int auto_close_delay_;
    
    // Layout erstellen
    void CreateLayout();
    
    // Event IDs
    enum {
        ID_ABBRECHEN_ABORT = wxID_HIGHEST + 1300,
        ID_AUTO_CLOSE_TIMER
    };
    
    wxDECLARE_EVENT_TABLE();
};
