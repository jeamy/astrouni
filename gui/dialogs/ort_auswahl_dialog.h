// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgOrtAuswahl from astrofil.c

#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>
#include "../database/legacy_data_manager.h"

// 1:1 Port von DlgOrtAuswahl (ORT_AUSWAHL Dialog)
class OrtAuswahlDialog : public wxDialog {
public:
    OrtAuswahlDialog(wxWindow* parent, LegacyDataManager* data_manager);
    
    // Ausgewählten Ort abrufen
    LegacyPlace GetSelectedPlace() const;
    bool HasSelection() const { return selected_index_ >= 0; }
    
private:
    // Event Handlers (1:1 Legacy)
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnSuchen(wxCommandEvent& event);
    void OnOrtAuswahl(wxListEvent& event);
    void OnOrtDoppelklick(wxListEvent& event);
    void OnSuchTextChange(wxCommandEvent& event);
    
    // GUI Controls (1:1 Legacy Layout)
    wxTextCtrl* such_ctrl_;          // Suchfeld
    wxButton* suchen_btn_;           // Suchen-Button
    
    wxListCtrl* orte_liste_;         // Orte-Liste
    
    wxStaticText* info_text_;        // Info-Text
    
    // Buttons
    wxButton* ok_btn_;
    wxButton* cancel_btn_;
    
    // Daten-Manager
    LegacyDataManager* data_manager_;
    
    // Aktuell ausgewählter Index
    int selected_index_;
    
    // Alle verfügbaren Orte (Standard + Europa)
    std::vector<LegacyPlace> alle_orte_;
    std::vector<std::string> ort_namen_;
    
    // Layout erstellen
    void CreateLayout();
    void InitializeControls();
    void LoadAlleOrte();
    void UpdateOrteList(const std::vector<size_t>& indices = {});
    void FilterOrte(const wxString& suchtext);
    
    // Event IDs
    enum {
        ID_SUCHEN = wxID_HIGHEST + 800,
        ID_SUCH_TEXT,
        ID_ORTE_LISTE
    };
    
    wxDECLARE_EVENT_TABLE();
};
