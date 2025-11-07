// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgTransit from astrofil.c

#pragma once

#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/timectrl.h>
#include <wx/spinctrl.h>
#include "../database/legacy_data_manager.h"

// 1:1 Port von DlgTransit (TRANSIT Dialog)
class TransitDialog : public wxDialog {
public:
    TransitDialog(wxWindow* parent, LegacyDataManager* data_manager);
    
    // Transit-Daten abrufen/setzen
    struct TransitData {
        astro::DateTime start_datetime;
        astro::DateTime end_datetime;
        int interval_days;
        bool show_all_planets;
        bool show_aspects;
        bool show_ingress;
        bool show_exact_times;
    };
    
    TransitData GetTransitData() const;
    void SetTransitData(const TransitData& data);
    
private:
    // Event Handlers (1:1 Legacy)
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnBerechnen(wxCommandEvent& event);
    void OnPersonAuswahl(wxCommandEvent& event);
    void OnZeitraumChange(wxCommandEvent& event);
    void OnSpinChange(wxSpinEvent& event);
    
    // GUI Controls (1:1 Legacy Layout)
    // Person-Bereich
    wxStaticText* person_text_;
    wxButton* person_btn_;
    
    // Zeitraum-Bereich
    wxDatePickerCtrl* start_datum_ctrl_;
    wxTimePickerCtrl* start_zeit_ctrl_;
    wxDatePickerCtrl* end_datum_ctrl_;
    wxTimePickerCtrl* end_zeit_ctrl_;
    
    // Intervall
    wxSpinCtrl* intervall_ctrl_;
    wxStaticText* intervall_text_;
    
    // Optionen
    wxCheckBox* alle_planeten_ctrl_;
    wxCheckBox* aspekte_ctrl_;
    wxCheckBox* ingress_ctrl_;
    wxCheckBox* exakte_zeiten_ctrl_;
    
    // Vorschau
    wxStaticText* vorschau_text_;
    
    // Buttons
    wxButton* berechnen_btn_;
    wxButton* ok_btn_;
    wxButton* cancel_btn_;
    
    // Daten-Manager
    LegacyDataManager* data_manager_;
    
    // Aktuelle Person
    LegacyPerson current_person_;
    bool person_selected_;
    
    // Layout erstellen
    void CreateLayout();
    void InitializeControls();
    void UpdateVorschau();
    void UpdateIntervallText();
    
    // Event IDs
    enum {
        ID_PERSON_AUSWAHL = wxID_HIGHEST + 900,
        ID_BERECHNEN,
        ID_START_DATUM,
        ID_START_ZEIT,
        ID_END_DATUM,
        ID_END_ZEIT,
        ID_INTERVALL,
        ID_ALLE_PLANETEN,
        ID_ASPEKTE,
        ID_INGRESS,
        ID_EXAKTE_ZEITEN
    };
    
    wxDECLARE_EVENT_TABLE();
};
