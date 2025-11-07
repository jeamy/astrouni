// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgOrtAuswahl from astrofil.c

#include "ort_auswahl_dialog.h"
#include "astro/i18n.h"
#include <wx/statbox.h>
#include <wx/sizer.h>
#include <algorithm>

using namespace astro::i18n;

// Event Table (1:1 Legacy)
wxBEGIN_EVENT_TABLE(OrtAuswahlDialog, wxDialog)
    EVT_BUTTON(wxID_OK, OrtAuswahlDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, OrtAuswahlDialog::OnCancel)
    EVT_BUTTON(ID_SUCHEN, OrtAuswahlDialog::OnSuchen)
    EVT_TEXT(ID_SUCH_TEXT, OrtAuswahlDialog::OnSuchTextChange)
    EVT_LIST_ITEM_SELECTED(ID_ORTE_LISTE, OrtAuswahlDialog::OnOrtAuswahl)
    EVT_LIST_ITEM_ACTIVATED(ID_ORTE_LISTE, OrtAuswahlDialog::OnOrtDoppelklick)
wxEND_EVENT_TABLE()

OrtAuswahlDialog::OrtAuswahlDialog(wxWindow* parent, LegacyDataManager* data_manager)
    : wxDialog(parent, wxID_ANY, wxString::FromUTF8("Ort Auswahl"), 
               wxDefaultPosition, wxSize(600, 500),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
      data_manager_(data_manager),
      selected_index_(-1) {
    
    CreateLayout();
    InitializeControls();
    LoadAlleOrte();
    Centre();
}

void OrtAuswahlDialog::CreateLayout() {
    // Haupt-Sizer
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Such-Bereich (1:1 Legacy)
    wxStaticBoxSizer* such_box = new wxStaticBoxSizer(wxHORIZONTAL, this, wxString::FromUTF8("Ort suchen"));
    
    such_ctrl_ = new wxTextCtrl(this, ID_SUCH_TEXT, "", wxDefaultPosition, wxSize(200, -1));
    suchen_btn_ = new wxButton(this, ID_SUCHEN, wxString::FromUTF8("Suchen"));
    
    such_box->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Suchbegriff:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    such_box->Add(such_ctrl_, 1, wxEXPAND | wxRIGHT, 5);
    such_box->Add(suchen_btn_, 0);
    
    main_sizer->Add(such_box, 0, wxEXPAND | wxALL, 10);
    
    // Orte-Liste (1:1 Legacy)
    wxStaticBoxSizer* liste_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Verfügbare Orte"));
    
    orte_liste_ = new wxListCtrl(this, ID_ORTE_LISTE, wxDefaultPosition, wxSize(-1, 300), 
                                wxLC_REPORT | wxLC_SINGLE_SEL);
    orte_liste_->AppendColumn(wxString::FromUTF8("Ort"), wxLIST_FORMAT_LEFT, 200);
    orte_liste_->AppendColumn(wxString::FromUTF8("Land"), wxLIST_FORMAT_LEFT, 150);
    orte_liste_->AppendColumn(wxString::FromUTF8("Breite"), wxLIST_FORMAT_RIGHT, 80);
    orte_liste_->AppendColumn(wxString::FromUTF8("Länge"), wxLIST_FORMAT_RIGHT, 80);
    orte_liste_->AppendColumn(wxString::FromUTF8("Höhe"), wxLIST_FORMAT_RIGHT, 60);
    
    liste_box->Add(orte_liste_, 1, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(liste_box, 1, wxEXPAND | wxALL, 10);
    
    // Info-Text (1:1 Legacy)
    info_text_ = new wxStaticText(this, wxID_ANY, 
        wxString::FromUTF8("Doppelklick auf einen Ort oder Auswahl + OK"));
    main_sizer->Add(info_text_, 0, wxEXPAND | wxALL, 10);
    
    // Dialog-Buttons (1:1 Legacy)
    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    
    ok_btn_ = new wxButton(this, wxID_OK, wxString::FromUTF8("OK"));
    cancel_btn_ = new wxButton(this, wxID_CANCEL, wxString::FromUTF8("Abbrechen"));
    
    ok_btn_->Enable(false); // Erst nach Auswahl aktivieren
    
    button_sizer->AddStretchSpacer();
    button_sizer->Add(ok_btn_, 0, wxRIGHT, 5);
    button_sizer->Add(cancel_btn_, 0);
    
    main_sizer->Add(button_sizer, 0, wxEXPAND | wxALL, 10);
    
    SetSizer(main_sizer);
}

void OrtAuswahlDialog::InitializeControls() {
    such_ctrl_->SetFocus();
}

void OrtAuswahlDialog::LoadAlleOrte() {
    alle_orte_.clear();
    ort_namen_.clear();
    
    if (!data_manager_) return;
    
    // Standard-Orte laden
    const auto& places = data_manager_->GetPlaces();
    for (const auto& place : places) {
        alle_orte_.push_back(place);
        ort_namen_.push_back(place.name);
    }
    
    // Europa-Orte laden
    const auto& europa_places = data_manager_->GetEuropaPlaces();
    for (const auto& place : europa_places) {
        alle_orte_.push_back(place);
        ort_namen_.push_back(place.name);
    }
    
    // Alle Orte anzeigen
    UpdateOrteList();
    
    // Info aktualisieren
    wxString info = wxString::Format(
        wxString::FromUTF8("%zu Orte verfügbar. Doppelklick zum Auswählen."),
        alle_orte_.size()
    );
    info_text_->SetLabel(info);
}

void OrtAuswahlDialog::UpdateOrteList(const std::vector<size_t>& indices) {
    orte_liste_->DeleteAllItems();
    
    if (indices.empty()) {
        // Alle Orte anzeigen
        for (size_t i = 0; i < alle_orte_.size(); ++i) {
            const auto& ort = alle_orte_[i];
            
            long index = orte_liste_->InsertItem(i, wxString::FromUTF8(ort.name.c_str()));
            orte_liste_->SetItem(index, 1, wxString::FromUTF8(ort.country.c_str()));
            orte_liste_->SetItem(index, 2, wxString::Format(wxT("%.4f"), ort.latitude));
            orte_liste_->SetItem(index, 3, wxString::Format(wxT("%.4f"), ort.longitude));
            orte_liste_->SetItem(index, 4, wxString::Format(wxT("%d"), ort.altitude));
            
            orte_liste_->SetItemData(index, i);
        }
    } else {
        // Nur gefilterte Orte anzeigen
        for (size_t i = 0; i < indices.size(); ++i) {
            size_t ort_index = indices[i];
            if (ort_index >= alle_orte_.size()) continue;
            
            const auto& ort = alle_orte_[ort_index];
            
            long index = orte_liste_->InsertItem(i, wxString::FromUTF8(ort.name.c_str()));
            orte_liste_->SetItem(index, 1, wxString::FromUTF8(ort.country.c_str()));
            orte_liste_->SetItem(index, 2, wxString::Format(wxT("%.4f"), ort.latitude));
            orte_liste_->SetItem(index, 3, wxString::Format(wxT("%.4f"), ort.longitude));
            orte_liste_->SetItem(index, 4, wxString::Format(wxT("%d"), ort.altitude));
            
            orte_liste_->SetItemData(index, ort_index);
        }
    }
}

void OrtAuswahlDialog::FilterOrte(const wxString& suchtext) {
    if (suchtext.IsEmpty()) {
        UpdateOrteList();
        return;
    }
    
    std::vector<size_t> gefundene_indices;
    wxString such_lower = suchtext.Lower();
    
    for (size_t i = 0; i < alle_orte_.size(); ++i) {
        const auto& ort = alle_orte_[i];
        
        wxString ort_name = wxString::FromUTF8(ort.name.c_str()).Lower();
        wxString ort_land = wxString::FromUTF8(ort.country.c_str()).Lower();
        
        if (ort_name.Contains(such_lower) || ort_land.Contains(such_lower)) {
            gefundene_indices.push_back(i);
        }
    }
    
    UpdateOrteList(gefundene_indices);
    
    // Info aktualisieren
    wxString info = wxString::Format(
        wxString::FromUTF8("%zu von %zu Orten gefunden für '%s'"),
        gefundene_indices.size(),
        alle_orte_.size(),
        suchtext
    );
    info_text_->SetLabel(info);
}

// Event Handlers (1:1 Legacy)
void OrtAuswahlDialog::OnOK(wxCommandEvent& event) {
    if (selected_index_ < 0) {
        wxMessageBox(wxString::FromUTF8("Bitte wählen Sie einen Ort aus der Liste!"), 
                    wxString::FromUTF8("Kein Ort ausgewählt"), wxOK | wxICON_WARNING);
        return;
    }
    
    EndModal(wxID_OK);
}

void OrtAuswahlDialog::OnCancel(wxCommandEvent& event) {
    EndModal(wxID_CANCEL);
}

void OrtAuswahlDialog::OnSuchen(wxCommandEvent& event) {
    wxString suchtext = such_ctrl_->GetValue();
    FilterOrte(suchtext);
}

void OrtAuswahlDialog::OnSuchTextChange(wxCommandEvent& event) {
    // Live-Suche bei Texteingabe
    wxString suchtext = such_ctrl_->GetValue();
    FilterOrte(suchtext);
}

void OrtAuswahlDialog::OnOrtAuswahl(wxListEvent& event) {
    long selected = event.GetIndex();
    if (selected >= 0) {
        selected_index_ = static_cast<int>(orte_liste_->GetItemData(selected));
        ok_btn_->Enable(true);
        
        // Ausgewählten Ort in Info anzeigen
        if (selected_index_ < static_cast<int>(alle_orte_.size())) {
            const auto& ort = alle_orte_[selected_index_];
            wxString info = wxString::Format(
                wxString::FromUTF8("Ausgewählt: %s, %s (%.4f°, %.4f°, %dm)"),
                wxString::FromUTF8(ort.name.c_str()),
                wxString::FromUTF8(ort.country.c_str()),
                ort.latitude,
                ort.longitude,
                ort.altitude
            );
            info_text_->SetLabel(info);
        }
    }
}

void OrtAuswahlDialog::OnOrtDoppelklick(wxListEvent& event) {
    // Doppelklick = Auswahl und OK
    OnOrtAuswahl(event);
    if (selected_index_ >= 0) {
        EndModal(wxID_OK);
    }
}

LegacyPlace OrtAuswahlDialog::GetSelectedPlace() const {
    if (selected_index_ >= 0 && selected_index_ < static_cast<int>(alle_orte_.size())) {
        return alle_orte_[selected_index_];
    }
    return LegacyPlace();
}
