// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgPHProc from astrofil.c

#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>
#include "../database/legacy_data_manager.h"

// 1:1 Port von DlgPHProc (PH Dialog)
class PersonHilfeDialog : public wxDialog {
public:
    PersonHilfeDialog(wxWindow* parent, LegacyDataManager* data_manager);
    
    // Ausgewählte Person abrufen
    LegacyPerson GetSelectedPerson() const;
    bool HasSelection() const;
    
private:
    // Event Handlers (1:1 Legacy)
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnPersonDoppelklick(wxListEvent& event);
    void OnPersonAuswahl(wxListEvent& event);
    void OnSuchen(wxCommandEvent& event);
    void OnSuchTextChange(wxCommandEvent& event);
    
    // GUI Controls (1:1 Legacy Layout)
    wxTextCtrl* such_ctrl_;
    wxButton* suchen_btn_;
    wxListCtrl* personen_liste_;
    wxStaticText* anzahl_text_;
    
    wxButton* ok_btn_;
    wxButton* cancel_btn_;
    
    // Daten-Manager
    LegacyDataManager* data_manager_;
    
    // Ausgewählte Person
    LegacyPerson selected_person_;
    bool has_selection_;
    
    // Layout erstellen
    void CreateLayout();
    void InitializeControls();
    void LoadPersonenListe();
    void FilterPersonenListe(const wxString& filter);
    void UpdateAnzahlText();
    
    // Event IDs
    enum {
        ID_SUCH_CTRL = wxID_HIGHEST + 1600,
        ID_SUCHEN_BTN,
        ID_PERSONEN_LISTE
    };
    
    wxDECLARE_EVENT_TABLE();
};
