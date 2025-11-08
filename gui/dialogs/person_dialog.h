// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgPErfassen from astrofil.c

#pragma once

#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/timectrl.h>
#include <wx/spinctrl.h>
#include "astro/data_types.h"
#include "../database/legacy_data_manager.h"

// 1:1 Port von DlgPErfassen (E_PERSON Dialog)
class PersonDialog : public wxDialog {
public:
    PersonDialog(wxWindow* parent, LegacyDataManager* data_manager = nullptr);
    
    // Daten abrufen
    astro::RadixData GetRadixData() const;
    void SetRadixData(const astro::RadixData& data);
    
private:
    // Event Handlers (1:1 Legacy)
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnPersonAuswahl(wxCommandEvent& event);  // Person auswählen
    void OnOrtAuswahl(wxCommandEvent& event);     // Ort auswählen
    void OnBerechnen(wxCommandEvent& event);      // Berechnen
    
    // GUI Controls (1:1 Legacy Layout)
    // Person
    wxTextCtrl* name_ctrl_;
    wxTextCtrl* vorname_ctrl_;
    wxChoice* geschlecht_ctrl_;
    
    // Geburtsdatum & Zeit
    wxDatePickerCtrl* datum_ctrl_;
    wxTimePickerCtrl* zeit_ctrl_;
    wxCheckBox* sommerzeit_ctrl_;
    
    // Geburtsort
    wxTextCtrl* ort_ctrl_;
    wxTextCtrl* land_ctrl_;
    wxSpinCtrlDouble* breite_ctrl_;     // Breitengrad
    wxSpinCtrlDouble* laenge_ctrl_;     // Längengrad
    wxSpinCtrl* hoehe_ctrl_;            // Höhe in Metern
    
    // Buttons (1:1 Legacy)
    wxButton* person_btn_;              // Person auswählen
    wxButton* ort_btn_;                 // Ort auswählen
    wxButton* berechnen_btn_;           // Berechnen
    wxButton* ok_btn_;
    wxButton* cancel_btn_;
    
    // Daten-Manager (für Personen-/Orte-DB)
    LegacyDataManager* data_manager_;
    
    // Layout erstellen
    void CreateLayout();
    void InitializeControls();
    
    // Event IDs (1:1 Legacy Mapping)
    enum {
        ID_PERSON_AUSWAHL = wxID_HIGHEST + 100,
        ID_ORT_AUSWAHL,
        ID_BERECHNEN
    };
    
    wxDECLARE_EVENT_TABLE();
};
