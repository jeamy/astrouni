// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgODProc from astrofil.c

#include "ort_duplikat_dialog.h"
#include "astro/i18n.h"
#include <wx/statbox.h>
#include <wx/sizer.h>
#include <cmath>

using namespace astro::i18n;

// Event Table (1:1 Legacy)
wxBEGIN_EVENT_TABLE(OrtDuplikatDialog, wxDialog)
    EVT_BUTTON(ID_ERSETZEN, OrtDuplikatDialog::OnErsetzen)
    EVT_BUTTON(ID_HINZUFUEGEN, OrtDuplikatDialog::OnHinzufuegen)
    EVT_BUTTON(ID_IGNORIEREN, OrtDuplikatDialog::OnIgnorieren)
    EVT_BUTTON(wxID_CANCEL, OrtDuplikatDialog::OnAbbrechen)
    EVT_LIST_ITEM_SELECTED(ID_DUPLIKATE_LISTE, OrtDuplikatDialog::OnOrtAuswahl)
wxEND_EVENT_TABLE()

OrtDuplikatDialog::OrtDuplikatDialog(wxWindow* parent, LegacyDataManager* data_manager, 
                                   const LegacyPlace& neuer_ort)
    : wxDialog(parent, wxID_ANY, wxString::FromUTF8("Ort-Duplikat gefunden"), 
               wxDefaultPosition, wxSize(600, 450),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
      data_manager_(data_manager),
      neuer_ort_(neuer_ort),
      aktion_(AKTION_ABBRECHEN),
      ausgewaehlter_index_(-1) {
    
    CreateLayout();
    InitializeControls();
    Centre();
}

void OrtDuplikatDialog::CreateLayout() {
    // Haupt-Sizer
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Info-Text (1:1 Legacy)
    info_text_ = new wxStaticText(this, wxID_ANY, 
        wxString::FromUTF8("Es wurden ähnliche Orte in der Datenbank gefunden.\n"
                          "Bitte wählen Sie eine Aktion:"));
    main_sizer->Add(info_text_, 0, wxEXPAND | wxALL, 10);
    
    // Neuer Ort-Bereich (1:1 Legacy)
    wxStaticBoxSizer* neuer_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Neuer Ort"));
    
    neuer_ort_text_ = new wxStaticText(this, wxID_ANY, wxString::FromUTF8(""));
    neuer_ort_text_->Wrap(550);
    neuer_box->Add(neuer_ort_text_, 0, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(neuer_box, 0, wxEXPAND | wxALL, 10);
    
    // Duplikate-Liste (1:1 Legacy)
    wxStaticBoxSizer* duplikate_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Ähnliche Orte"));
    
    duplikate_liste_ = new wxListCtrl(this, ID_DUPLIKATE_LISTE, wxDefaultPosition, wxSize(-1, 200), 
                                     wxLC_REPORT | wxLC_SINGLE_SEL);
    
    // Spalten definieren
    duplikate_liste_->AppendColumn(wxString::FromUTF8("Ort"), wxLIST_FORMAT_LEFT, 150);
    duplikate_liste_->AppendColumn(wxString::FromUTF8("Land"), wxLIST_FORMAT_LEFT, 100);
    duplikate_liste_->AppendColumn(wxString::FromUTF8("Breite"), wxLIST_FORMAT_LEFT, 80);
    duplikate_liste_->AppendColumn(wxString::FromUTF8("Länge"), wxLIST_FORMAT_LEFT, 80);
    duplikate_liste_->AppendColumn(wxString::FromUTF8("Höhe"), wxLIST_FORMAT_LEFT, 60);
    duplikate_liste_->AppendColumn(wxString::FromUTF8("Entfernung"), wxLIST_FORMAT_LEFT, 80);
    
    duplikate_box->Add(duplikate_liste_, 1, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(duplikate_box, 1, wxEXPAND | wxALL, 10);
    
    // Dialog-Buttons (1:1 Legacy)
    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    
    ersetzen_btn_ = new wxButton(this, ID_ERSETZEN, wxString::FromUTF8("Ersetzen"));
    hinzufuegen_btn_ = new wxButton(this, ID_HINZUFUEGEN, wxString::FromUTF8("Hinzufügen"));
    ignorieren_btn_ = new wxButton(this, ID_IGNORIEREN, wxString::FromUTF8("Ignorieren"));
    abbrechen_btn_ = new wxButton(this, wxID_CANCEL, wxString::FromUTF8("Abbrechen"));
    
    button_sizer->Add(ersetzen_btn_, 0, wxRIGHT, 5);
    button_sizer->Add(hinzufuegen_btn_, 0, wxRIGHT, 5);
    button_sizer->Add(ignorieren_btn_, 0, wxRIGHT, 10);
    button_sizer->AddStretchSpacer();
    button_sizer->Add(abbrechen_btn_, 0);
    
    main_sizer->Add(button_sizer, 0, wxEXPAND | wxALL, 10);
    
    SetSizer(main_sizer);
}

void OrtDuplikatDialog::InitializeControls() {
    // Neuen Ort anzeigen
    wxString neuer_ort_info = wxString::Format(
        wxString::FromUTF8("%s, %s\nBreite: %.4f°, Länge: %.4f°, Höhe: %dm"),
        wxString::FromUTF8(neuer_ort_.name.c_str()),
        wxString::FromUTF8(neuer_ort_.country.c_str()),
        neuer_ort_.latitude,
        neuer_ort_.longitude,
        static_cast<int>(neuer_ort_.altitude)
    );
    neuer_ort_text_->SetLabel(neuer_ort_info);
    
    FindeDuplikate();
    UpdateButtons();
}

void OrtDuplikatDialog::FindeDuplikate() {
    duplikate_.clear();
    duplikate_liste_->DeleteAllItems();
    
    if (!data_manager_) return;
    
    const auto& orte = data_manager_->GetPlaces();
    
    // Duplikate finden (ähnlicher Name oder nahe Koordinaten)
    for (const auto& ort : orte) {
        bool ist_duplikat = false;
        double entfernung = 0.0;
        
        // Name-Ähnlichkeit prüfen
        wxString neuer_name = wxString::FromUTF8(neuer_ort_.name.c_str()).Lower();
        wxString ort_name = wxString::FromUTF8(ort.name.c_str()).Lower();
        
        if (neuer_name == ort_name || 
            neuer_name.Contains(ort_name) || 
            ort_name.Contains(neuer_name)) {
            ist_duplikat = true;
        }
        
        // Koordinaten-Nähe prüfen (< 10km)
        double lat_diff = neuer_ort_.latitude - ort.latitude;
        double lon_diff = neuer_ort_.longitude - ort.longitude;
        entfernung = std::sqrt(lat_diff * lat_diff + lon_diff * lon_diff) * 111.0; // Approximation in km
        
        if (entfernung < 10.0) {
            ist_duplikat = true;
        }
        
        if (ist_duplikat) {
            duplikate_.push_back(ort);
            
            long index = duplikate_liste_->InsertItem(duplikate_.size() - 1, 
                                                     wxString::FromUTF8(ort.name.c_str()));
            duplikate_liste_->SetItem(index, 1, wxString::FromUTF8(ort.country.c_str()));
            duplikate_liste_->SetItem(index, 2, wxString::Format(wxT("%.4f°"), ort.latitude));
            duplikate_liste_->SetItem(index, 3, wxString::Format(wxT("%.4f°"), ort.longitude));
            duplikate_liste_->SetItem(index, 4, wxString::Format(wxT("%dm"), static_cast<int>(ort.altitude)));
            duplikate_liste_->SetItem(index, 5, wxString::Format(wxT("%.1f km"), entfernung));
        }
    }
}

void OrtDuplikatDialog::UpdateButtons() {
    bool has_selection = ausgewaehlter_index_ >= 0;
    ersetzen_btn_->Enable(has_selection);
    
    // Hinzufügen und Ignorieren sind immer möglich
    hinzufuegen_btn_->Enable(true);
    ignorieren_btn_->Enable(true);
}

// Event Handlers (1:1 Legacy)
void OrtDuplikatDialog::OnErsetzen(wxCommandEvent& event) {
    if (ausgewaehlter_index_ < 0) {
        wxMessageBox(wxString::FromUTF8("Bitte wählen Sie einen Ort zum Ersetzen aus!"), 
                    wxString::FromUTF8("Keine Auswahl"), wxOK | wxICON_WARNING);
        return;
    }
    
    aktion_ = AKTION_ERSETZEN;
    EndModal(wxID_OK);
}

void OrtDuplikatDialog::OnHinzufuegen(wxCommandEvent& event) {
    aktion_ = AKTION_HINZUFUEGEN;
    EndModal(wxID_OK);
}

void OrtDuplikatDialog::OnIgnorieren(wxCommandEvent& event) {
    aktion_ = AKTION_IGNORIEREN;
    EndModal(wxID_OK);
}

void OrtDuplikatDialog::OnAbbrechen(wxCommandEvent& event) {
    aktion_ = AKTION_ABBRECHEN;
    EndModal(wxID_CANCEL);
}

void OrtDuplikatDialog::OnOrtAuswahl(wxListEvent& event) {
    ausgewaehlter_index_ = event.GetIndex();
    UpdateButtons();
}

LegacyPlace OrtDuplikatDialog::GetAusgewaehlterOrt() const {
    if (ausgewaehlter_index_ >= 0 && ausgewaehlter_index_ < static_cast<int>(duplikate_.size())) {
        return duplikate_[ausgewaehlter_index_];
    }
    return LegacyPlace();
}
