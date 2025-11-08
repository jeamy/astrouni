// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgRuck from autransi.c

#pragma once

#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/timectrl.h>
#include <wx/listctrl.h>
#include "../database/legacy_data_manager.h"

// 1:1 Port von DlgRuck (RUCK Dialog)
class RuecklaeufigDialog : public wxDialog {
public:
    RuecklaeufigDialog(wxWindow* parent, LegacyDataManager* data_manager);
    
    // Rückläufigkeits-Daten
    struct RuecklaeufigData {
        astro::DateTime start_datetime;
        astro::DateTime end_datetime;
        
        // Planeten-Auswahl für Rückläufigkeit
        bool check_mercury;
        bool check_venus;
        bool check_mars;
        bool check_jupiter;
        bool check_saturn;
        bool check_uranus;
        bool check_neptune;
        bool check_pluto;
        
        // Anzeige-Optionen
        bool show_exact_times;
        bool show_durations;
        bool highlight_current;
    };
    
    RuecklaeufigData GetRuecklaeufigData() const;
    void SetRuecklaeufigData(const RuecklaeufigData& data);
    
private:
    // Event Handlers (1:1 Legacy)
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnBerechnen(wxCommandEvent& event);
    void OnAlleAuswahl(wxCommandEvent& event);
    void OnKeineAuswahl(wxCommandEvent& event);
    void OnZeitraumChange(wxCommandEvent& event);
    
    // GUI Controls (1:1 Legacy Layout)
    // Zeitraum-Bereich
    wxDatePickerCtrl* start_datum_ctrl_;
    wxTimePickerCtrl* start_zeit_ctrl_;
    wxDatePickerCtrl* end_datum_ctrl_;
    wxTimePickerCtrl* end_zeit_ctrl_;
    
    // Planeten-Auswahl
    wxCheckBox* mercury_cb_;
    wxCheckBox* venus_cb_;
    wxCheckBox* mars_cb_;
    wxCheckBox* jupiter_cb_;
    wxCheckBox* saturn_cb_;
    wxCheckBox* uranus_cb_;
    wxCheckBox* neptune_cb_;
    wxCheckBox* pluto_cb_;
    
    // Optionen
    wxCheckBox* exact_times_cb_;
    wxCheckBox* durations_cb_;
    wxCheckBox* highlight_current_cb_;
    
    // Ergebnis-Liste
    wxListCtrl* ergebnis_liste_;
    
    // Buttons
    wxButton* alle_btn_;
    wxButton* keine_btn_;
    wxButton* berechnen_btn_;
    wxButton* ok_btn_;
    wxButton* cancel_btn_;
    
    // Daten-Manager
    LegacyDataManager* data_manager_;
    
    // Layout erstellen
    void CreateLayout();
    void InitializeControls();
    void UpdateVorschau();
    void BerechneRuecklaeufigkeit();
    
    // Event IDs
    enum {
        ID_START_DATUM = wxID_HIGHEST + 1200,
        ID_START_ZEIT,
        ID_END_DATUM,
        ID_END_ZEIT,
        ID_MERCURY,
        ID_VENUS,
        ID_MARS,
        ID_JUPITER,
        ID_SATURN,
        ID_URANUS,
        ID_NEPTUNE,
        ID_PLUTO,
        ID_EXACT_TIMES,
        ID_DURATIONS,
        ID_HIGHLIGHT_CURRENT,
        ID_ALLE_PLANETEN,
        ID_KEINE_PLANETEN,
        ID_BERECHNEN_RUCK,
        ID_ERGEBNIS_LISTE
    };
    
    wxDECLARE_EVENT_TABLE();
};
