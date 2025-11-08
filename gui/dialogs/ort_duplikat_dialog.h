// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgODProc from astrofil.c

#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>
#include "../database/legacy_data_manager.h"

// 1:1 Port von DlgODProc (OD Dialog)
class OrtDuplikatDialog : public wxDialog {
public:
    OrtDuplikatDialog(wxWindow* parent, LegacyDataManager* data_manager, 
                     const LegacyPlace& neuer_ort);
    
    enum DuplikatAktion {
        AKTION_ABBRECHEN,
        AKTION_ERSETZEN,
        AKTION_HINZUFUEGEN,
        AKTION_IGNORIEREN
    };
    
    DuplikatAktion GetAktion() const { return aktion_; }
    LegacyPlace GetAusgewaehlterOrt() const;
    
private:
    // Event Handlers (1:1 Legacy)
    void OnErsetzen(wxCommandEvent& event);
    void OnHinzufuegen(wxCommandEvent& event);
    void OnIgnorieren(wxCommandEvent& event);
    void OnAbbrechen(wxCommandEvent& event);
    void OnOrtAuswahl(wxListEvent& event);
    
    // GUI Controls (1:1 Legacy Layout)
    wxStaticText* info_text_;
    wxListCtrl* duplikate_liste_;
    wxStaticText* neuer_ort_text_;
    
    wxButton* ersetzen_btn_;
    wxButton* hinzufuegen_btn_;
    wxButton* ignorieren_btn_;
    wxButton* abbrechen_btn_;
    
    // Daten
    LegacyDataManager* data_manager_;
    LegacyPlace neuer_ort_;
    std::vector<LegacyPlace> duplikate_;
    DuplikatAktion aktion_;
    int ausgewaehlter_index_;
    
    // Layout erstellen
    void CreateLayout();
    void InitializeControls();
    void FindeDuplikate();
    void UpdateButtons();
    
    // Event IDs
    enum {
        ID_ERSETZEN = wxID_HIGHEST + 1700,
        ID_HINZUFUEGEN,
        ID_IGNORIEREN,
        ID_DUPLIKATE_LISTE
    };
    
    wxDECLARE_EVENT_TABLE();
};
