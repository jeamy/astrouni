// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgOErfassen from astrofil.c

#pragma once

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/listctrl.h>
#include "astro/data_types.h"
#include "../database/legacy_data_manager.h"

// 1:1 Port von DlgOErfassen (E_ORT Dialog)
class OrtDialog : public wxDialog {
public:
    OrtDialog(wxWindow* parent, LegacyDataManager* data_manager = nullptr);
    
    // Daten abrufen
    astro::GeoLocation GetGeoLocation() const;
    void SetGeoLocation(const astro::GeoLocation& location);
    void SetGeoLocation(const LegacyPlace& place);
    
private:
    // Event Handlers (1:1 Legacy)
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnNeu(wxCommandEvent& event);           // Neuer Ort
    void OnLoeschen(wxCommandEvent& event);      // Ort löschen
    void OnSuchen(wxCommandEvent& event);        // Ort suchen
    void OnOrtAuswahl(wxListEvent& event);       // Ort aus Liste auswählen
    
    // GUI Controls (1:1 Legacy Layout)
    // Ort-Eingabe
    wxTextCtrl* ort_ctrl_;
    wxTextCtrl* land_ctrl_;
    wxTextCtrl* region_ctrl_;
    
    // Koordinaten
    wxSpinCtrlDouble* breite_ctrl_;     // Breitengrad
    wxSpinCtrlDouble* laenge_ctrl_;     // Längengrad
    wxSpinCtrl* hoehe_ctrl_;            // Höhe in Metern
    wxTextCtrl* zeitzone_ctrl_;         // Zeitzone
    
    // Orte-Liste (1:1 Legacy)
    wxListCtrl* orte_liste_;
    
    // Buttons (1:1 Legacy)
    wxButton* neu_btn_;
    wxButton* loeschen_btn_;
    wxButton* suchen_btn_;
    wxButton* ok_btn_;
    wxButton* cancel_btn_;
    
    // Layout erstellen
    void CreateLayout();
    void InitializeControls();
    void LoadOrteFromDatabase();
    void UpdateOrteList();
    
    // Daten-Manager und lokale Kopie der Orte
    LegacyDataManager* data_manager_;
    std::vector<LegacyPlace> orte_database_;
    
    // Event IDs (1:1 Legacy Mapping)
    enum {
        ID_NEU = wxID_HIGHEST + 200,
        ID_LOESCHEN,
        ID_SUCHEN,
        ID_ORTE_LISTE
    };
    
    wxDECLARE_EVENT_TABLE();
};
