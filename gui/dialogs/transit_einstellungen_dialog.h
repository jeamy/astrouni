// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgTransEin from astrofil.c

#pragma once

#include <wx/wx.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include <wx/choice.h>
#include "../database/legacy_data_manager.h"

// 1:1 Port von DlgTransEin (TRANS_EIN Dialog)
class TransitEinstellungenDialog : public wxDialog {
public:
    TransitEinstellungenDialog(wxWindow* parent, LegacyDataManager* data_manager);
    
    // Transit-Einstellungen abrufen/setzen
    struct TransitSettings {
        // Planeten-Auswahl
        bool show_sun;
        bool show_moon;
        bool show_mercury;
        bool show_venus;
        bool show_mars;
        bool show_jupiter;
        bool show_saturn;
        bool show_uranus;
        bool show_neptune;
        bool show_pluto;
        bool show_nodes;
        bool show_chiron;
        
        // Aspekt-Auswahl
        bool show_conjunction;      // 0°
        bool show_sextile;         // 60°
        bool show_square;          // 90°
        bool show_trine;           // 120°
        bool show_opposition;      // 180°
        bool show_semisextile;     // 30°
        bool show_semisquare;      // 45°
        bool show_sesquiquadrate;  // 135°
        bool show_quincunx;        // 150°
        
        // Orben für Transite
        double transit_orb_major;   // Hauptaspekte
        double transit_orb_minor;   // Nebenaspekte
        
        // Anzeige-Optionen
        bool show_exact_times;
        bool show_applying_separating;
        bool sort_by_date;
        bool group_by_planet;
    };
    
    TransitSettings GetTransitSettings() const;
    void SetTransitSettings(const TransitSettings& settings);
    
private:
    // Event Handlers (1:1 Legacy)
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnReset(wxCommandEvent& event);
    void OnAlleAuswahl(wxCommandEvent& event);
    void OnKeineAuswahl(wxCommandEvent& event);
    void OnPlanetChange(wxCommandEvent& event);
    void OnAspektChange(wxCommandEvent& event);
    
    // GUI Controls (1:1 Legacy Layout)
    // Planeten-Auswahl
    wxCheckBox* sun_cb_;
    wxCheckBox* moon_cb_;
    wxCheckBox* mercury_cb_;
    wxCheckBox* venus_cb_;
    wxCheckBox* mars_cb_;
    wxCheckBox* jupiter_cb_;
    wxCheckBox* saturn_cb_;
    wxCheckBox* uranus_cb_;
    wxCheckBox* neptune_cb_;
    wxCheckBox* pluto_cb_;
    wxCheckBox* nodes_cb_;
    wxCheckBox* chiron_cb_;
    
    // Aspekt-Auswahl
    wxCheckBox* conjunction_cb_;
    wxCheckBox* sextile_cb_;
    wxCheckBox* square_cb_;
    wxCheckBox* trine_cb_;
    wxCheckBox* opposition_cb_;
    wxCheckBox* semisextile_cb_;
    wxCheckBox* semisquare_cb_;
    wxCheckBox* sesquiquadrate_cb_;
    wxCheckBox* quincunx_cb_;
    
    // Orben
    wxSpinCtrlDouble* major_orb_ctrl_;
    wxSpinCtrlDouble* minor_orb_ctrl_;
    
    // Anzeige-Optionen
    wxCheckBox* exact_times_cb_;
    wxCheckBox* applying_separating_cb_;
    wxCheckBox* sort_by_date_cb_;
    wxCheckBox* group_by_planet_cb_;
    
    // Buttons
    wxButton* alle_planeten_btn_;
    wxButton* keine_planeten_btn_;
    wxButton* alle_aspekte_btn_;
    wxButton* keine_aspekte_btn_;
    wxButton* ok_btn_;
    wxButton* cancel_btn_;
    wxButton* reset_btn_;
    
    // Daten-Manager
    LegacyDataManager* data_manager_;
    
    // Layout erstellen
    void CreateLayout();
    void InitializeControls();
    void UpdateAuswahl();
    
    // Event IDs
    enum {
        ID_SUN = wxID_HIGHEST + 1000,
        ID_MOON,
        ID_MERCURY,
        ID_VENUS,
        ID_MARS,
        ID_JUPITER,
        ID_SATURN,
        ID_URANUS,
        ID_NEPTUNE,
        ID_PLUTO,
        ID_NODES,
        ID_CHIRON,
        ID_CONJUNCTION,
        ID_SEXTILE,
        ID_SQUARE,
        ID_TRINE,
        ID_OPPOSITION,
        ID_SEMISEXTILE,
        ID_SEMISQUARE,
        ID_SESQUIQUADRATE,
        ID_QUINCUNX,
        ID_MAJOR_ORB,
        ID_MINOR_ORB,
        ID_EXACT_TIMES,
        ID_APPLYING_SEPARATING,
        ID_SORT_BY_DATE,
        ID_GROUP_BY_PLANET,
        ID_ALLE_PLANETEN,
        ID_KEINE_PLANETEN,
        ID_ALLE_ASPEKTE,
        ID_KEINE_ASPEKTE,
        ID_RESET_TRANSIT
    };
    
    wxDECLARE_EVENT_TABLE();
};
