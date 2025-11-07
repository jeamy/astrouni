// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgOErfassen from astrofil.c

#include "ort_dialog.h"
#include "astro/i18n.h"
#include <wx/statbox.h>
#include <wx/sizer.h>

using namespace astro::i18n;

// Event Table (1:1 Legacy)
wxBEGIN_EVENT_TABLE(OrtDialog, wxDialog)
    EVT_BUTTON(wxID_OK, OrtDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, OrtDialog::OnCancel)
    EVT_BUTTON(ID_NEU, OrtDialog::OnNeu)
    EVT_BUTTON(ID_LOESCHEN, OrtDialog::OnLoeschen)
    EVT_BUTTON(ID_SUCHEN, OrtDialog::OnSuchen)
    EVT_LIST_ITEM_SELECTED(ID_ORTE_LISTE, OrtDialog::OnOrtAuswahl)
wxEND_EVENT_TABLE()

OrtDialog::OrtDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, wxString::FromUTF8("Orte Erfassen"), 
               wxDefaultPosition, wxSize(500, 600),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) {
    
    CreateLayout();
    InitializeControls();
    LoadOrteFromDatabase();
    Centre();
}

void OrtDialog::CreateLayout() {
    // Haupt-Sizer
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Ort-Eingabe Gruppe (1:1 Legacy)
    wxStaticBoxSizer* ort_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Ort-Daten"));
    
    // Ort
    wxBoxSizer* ort_sizer = new wxBoxSizer(wxHORIZONTAL);
    ort_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Ort:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    ort_ctrl_ = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(150, -1));
    ort_sizer->Add(ort_ctrl_, 1, wxEXPAND);
    ort_box->Add(ort_sizer, 0, wxEXPAND | wxALL, 5);
    
    // Land
    wxBoxSizer* land_sizer = new wxBoxSizer(wxHORIZONTAL);
    land_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Land:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    land_ctrl_ = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(150, -1));
    land_sizer->Add(land_ctrl_, 1, wxEXPAND);
    ort_box->Add(land_sizer, 0, wxEXPAND | wxALL, 5);
    
    // Region
    wxBoxSizer* region_sizer = new wxBoxSizer(wxHORIZONTAL);
    region_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Region:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    region_ctrl_ = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(150, -1));
    region_sizer->Add(region_ctrl_, 1, wxEXPAND);
    ort_box->Add(region_sizer, 0, wxEXPAND | wxALL, 5);
    
    // Koordinaten (1:1 Legacy)
    wxBoxSizer* coord_sizer = new wxBoxSizer(wxHORIZONTAL);
    
    // Breitengrad
    coord_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Breite:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    breite_ctrl_ = new wxSpinCtrlDouble(this, wxID_ANY, "", wxDefaultPosition, wxSize(80, -1));
    breite_ctrl_->SetRange(-90.0, 90.0);
    breite_ctrl_->SetDigits(4);
    breite_ctrl_->SetIncrement(0.0001);
    coord_sizer->Add(breite_ctrl_, 0, wxRIGHT, 10);
    
    // Längengrad
    coord_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Länge:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    laenge_ctrl_ = new wxSpinCtrlDouble(this, wxID_ANY, "", wxDefaultPosition, wxSize(80, -1));
    laenge_ctrl_->SetRange(-180.0, 180.0);
    laenge_ctrl_->SetDigits(4);
    laenge_ctrl_->SetIncrement(0.0001);
    coord_sizer->Add(laenge_ctrl_, 0, wxRIGHT, 10);
    
    // Höhe
    coord_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Höhe (m):")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    hoehe_ctrl_ = new wxSpinCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(60, -1));
    hoehe_ctrl_->SetRange(-500, 9000);
    hoehe_ctrl_->SetValue(0);
    coord_sizer->Add(hoehe_ctrl_, 0);
    
    ort_box->Add(coord_sizer, 0, wxEXPAND | wxALL, 5);
    
    // Zeitzone
    wxBoxSizer* tz_sizer = new wxBoxSizer(wxHORIZONTAL);
    tz_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Zeitzone:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    zeitzone_ctrl_ = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(100, -1));
    tz_sizer->Add(zeitzone_ctrl_, 0, wxEXPAND);
    ort_box->Add(tz_sizer, 0, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(ort_box, 0, wxEXPAND | wxALL, 10);
    
    // Orte-Liste (1:1 Legacy)
    wxStaticBoxSizer* liste_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Gespeicherte Orte"));
    
    orte_liste_ = new wxListCtrl(this, ID_ORTE_LISTE, wxDefaultPosition, wxSize(-1, 200), 
                                wxLC_REPORT | wxLC_SINGLE_SEL);
    orte_liste_->AppendColumn(wxString::FromUTF8("Ort"), wxLIST_FORMAT_LEFT, 120);
    orte_liste_->AppendColumn(wxString::FromUTF8("Land"), wxLIST_FORMAT_LEFT, 100);
    orte_liste_->AppendColumn(wxString::FromUTF8("Breite"), wxLIST_FORMAT_RIGHT, 80);
    orte_liste_->AppendColumn(wxString::FromUTF8("Länge"), wxLIST_FORMAT_RIGHT, 80);
    orte_liste_->AppendColumn(wxString::FromUTF8("Höhe"), wxLIST_FORMAT_RIGHT, 60);
    
    liste_box->Add(orte_liste_, 1, wxEXPAND | wxALL, 5);
    
    // Liste-Buttons (1:1 Legacy)
    wxBoxSizer* liste_btn_sizer = new wxBoxSizer(wxHORIZONTAL);
    neu_btn_ = new wxButton(this, ID_NEU, wxString::FromUTF8("Neu"));
    loeschen_btn_ = new wxButton(this, ID_LOESCHEN, wxString::FromUTF8("Löschen"));
    suchen_btn_ = new wxButton(this, ID_SUCHEN, wxString::FromUTF8("Suchen"));
    
    liste_btn_sizer->Add(neu_btn_, 0, wxRIGHT, 5);
    liste_btn_sizer->Add(loeschen_btn_, 0, wxRIGHT, 5);
    liste_btn_sizer->Add(suchen_btn_, 0);
    liste_btn_sizer->AddStretchSpacer();
    
    liste_box->Add(liste_btn_sizer, 0, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(liste_box, 1, wxEXPAND | wxALL, 10);
    
    // Dialog-Buttons (1:1 Legacy)
    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    
    ok_btn_ = new wxButton(this, wxID_OK, wxString::FromUTF8("OK"));
    cancel_btn_ = new wxButton(this, wxID_CANCEL, wxString::FromUTF8("Abbrechen"));
    
    button_sizer->AddStretchSpacer();
    button_sizer->Add(ok_btn_, 0, wxRIGHT, 5);
    button_sizer->Add(cancel_btn_, 0);
    
    main_sizer->Add(button_sizer, 0, wxEXPAND | wxALL, 10);
    
    SetSizer(main_sizer);
}

void OrtDialog::InitializeControls() {
    // Default-Werte setzen (1:1 Legacy)
    ort_ctrl_->SetValue(wxString::FromUTF8("Berlin"));
    land_ctrl_->SetValue(wxString::FromUTF8("Deutschland"));
    region_ctrl_->SetValue(wxString::FromUTF8("Berlin"));
    breite_ctrl_->SetValue(52.5200);  // Berlin
    laenge_ctrl_->SetValue(13.4050);  // Berlin
    hoehe_ctrl_->SetValue(34);        // Berlin Höhe
    zeitzone_ctrl_->SetValue(wxString::FromUTF8("Europe/Berlin"));
    
    // Fokus auf Ort setzen
    ort_ctrl_->SetFocus();
}

void OrtDialog::LoadOrteFromDatabase() {
    // Standard-Orte laden (1:1 Legacy)
    orte_database_.clear();
    
    // Einige Standard-Orte hinzufügen
    astro::GeoLocation berlin;
    berlin.latitude = 52.5200;
    berlin.longitude = 13.4050;
    berlin.altitude = 34;
    berlin.country_code = "Deutschland";
    berlin.timezone = "Europe/Berlin";
    orte_database_.push_back(berlin);
    
    astro::GeoLocation muenchen;
    muenchen.latitude = 48.1351;
    muenchen.longitude = 11.5820;
    muenchen.altitude = 519;
    muenchen.country_code = "Deutschland";
    muenchen.timezone = "Europe/Berlin";
    orte_database_.push_back(muenchen);
    
    astro::GeoLocation wien;
    wien.latitude = 48.2082;
    wien.longitude = 16.3738;
    wien.altitude = 171;
    wien.country_code = "Österreich";
    wien.timezone = "Europe/Vienna";
    orte_database_.push_back(wien);
    
    astro::GeoLocation zuerich;
    zuerich.latitude = 47.3769;
    zuerich.longitude = 8.5417;
    zuerich.altitude = 408;
    zuerich.country_code = "Schweiz";
    zuerich.timezone = "Europe/Zurich";
    orte_database_.push_back(zuerich);
    
    UpdateOrteList();
}

void OrtDialog::UpdateOrteList() {
    orte_liste_->DeleteAllItems();
    
    for (size_t i = 0; i < orte_database_.size(); ++i) {
        const auto& ort = orte_database_[i];
        
        long index = orte_liste_->InsertItem(i, wxString::FromUTF8("Ort")); // TODO: Ortsnamen aus DB
        orte_liste_->SetItem(index, 1, wxString::FromUTF8(ort.country_code.c_str()));
        orte_liste_->SetItem(index, 2, wxString::Format(wxT("%.4f"), ort.latitude));
        orte_liste_->SetItem(index, 3, wxString::Format(wxT("%.4f"), ort.longitude));
        orte_liste_->SetItem(index, 4, wxString::Format(wxT("%.0f"), ort.altitude));
        
        orte_liste_->SetItemData(index, i);
    }
}

// Event Handlers (1:1 Legacy)
void OrtDialog::OnOK(wxCommandEvent& event) {
    // Validierung
    if (ort_ctrl_->GetValue().IsEmpty()) {
        wxMessageBox(wxString::FromUTF8("Bitte Ort eingeben!"), wxString::FromUTF8("Fehler"), wxOK | wxICON_ERROR);
        ort_ctrl_->SetFocus();
        return;
    }
    
    if (land_ctrl_->GetValue().IsEmpty()) {
        wxMessageBox(wxString::FromUTF8("Bitte Land eingeben!"), wxString::FromUTF8("Fehler"), wxOK | wxICON_ERROR);
        land_ctrl_->SetFocus();
        return;
    }
    
    EndModal(wxID_OK);
}

void OrtDialog::OnCancel(wxCommandEvent& event) {
    EndModal(wxID_CANCEL);
}

void OrtDialog::OnNeu(wxCommandEvent& event) {
    // Neuen Ort zur Datenbank hinzufügen
    astro::GeoLocation neuer_ort = GetGeoLocation();
    
    if (!ort_ctrl_->GetValue().IsEmpty() && !land_ctrl_->GetValue().IsEmpty()) {
        orte_database_.push_back(neuer_ort);
        UpdateOrteList();
        
        wxMessageBox(wxString::FromUTF8("Ort zur Datenbank hinzugefügt!"), 
                    wxString::FromUTF8("Info"), wxOK | wxICON_INFORMATION);
    } else {
        wxMessageBox(wxString::FromUTF8("Bitte Ort und Land eingeben!"), 
                    wxString::FromUTF8("Fehler"), wxOK | wxICON_ERROR);
    }
}

void OrtDialog::OnLoeschen(wxCommandEvent& event) {
    long selected = orte_liste_->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (selected == -1) {
        wxMessageBox(wxString::FromUTF8("Bitte Ort aus Liste auswählen!"), 
                    wxString::FromUTF8("Fehler"), wxOK | wxICON_ERROR);
        return;
    }
    
    int result = wxMessageBox(wxString::FromUTF8("Ort wirklich löschen?"), 
                             wxString::FromUTF8("Bestätigung"), 
                             wxYES_NO | wxICON_QUESTION);
    
    if (result == wxYES) {
        size_t index = orte_liste_->GetItemData(selected);
        if (index < orte_database_.size()) {
            orte_database_.erase(orte_database_.begin() + index);
            UpdateOrteList();
        }
    }
}

void OrtDialog::OnSuchen(wxCommandEvent& event) {
    wxString suchtext = ort_ctrl_->GetValue();
    if (suchtext.IsEmpty()) {
        wxMessageBox(wxString::FromUTF8("Bitte Suchbegriff eingeben!"), 
                    wxString::FromUTF8("Info"), wxOK | wxICON_INFORMATION);
        return;
    }
    
    // Einfache Suche in der Liste
    for (long i = 0; i < orte_liste_->GetItemCount(); ++i) {
        wxString ort_name = orte_liste_->GetItemText(i, 0);
        wxString land_name = orte_liste_->GetItemText(i, 1);
        
        if (ort_name.Lower().Contains(suchtext.Lower()) || 
            land_name.Lower().Contains(suchtext.Lower())) {
            orte_liste_->SetItemState(i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            orte_liste_->EnsureVisible(i);
            break;
        }
    }
}

void OrtDialog::OnOrtAuswahl(wxListEvent& event) {
    long selected = event.GetIndex();
    if (selected == -1) return;
    
    size_t index = orte_liste_->GetItemData(selected);
    if (index >= orte_database_.size()) return;
    
    // Ausgewählten Ort in die Eingabefelder laden
    SetGeoLocation(orte_database_[index]);
}

astro::GeoLocation OrtDialog::GetGeoLocation() const {
    astro::GeoLocation location;
    
    location.latitude = breite_ctrl_->GetValue();
    location.longitude = laenge_ctrl_->GetValue();
    location.altitude = hoehe_ctrl_->GetValue();
    location.country_code = land_ctrl_->GetValue().ToStdString();
    location.timezone = zeitzone_ctrl_->GetValue().ToStdString();
    
    return location;
}

void OrtDialog::SetGeoLocation(const astro::GeoLocation& location) {
    breite_ctrl_->SetValue(location.latitude);
    laenge_ctrl_->SetValue(location.longitude);
    hoehe_ctrl_->SetValue(static_cast<int>(location.altitude));
    land_ctrl_->SetValue(wxString::FromUTF8(location.country_code.c_str()));
    zeitzone_ctrl_->SetValue(wxString::FromUTF8(location.timezone.c_str()));
}
