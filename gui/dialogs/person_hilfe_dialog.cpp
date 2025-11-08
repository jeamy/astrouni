// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgPHProc from astrofil.c

#include "person_hilfe_dialog.h"
#include "astro/i18n.h"
#include <wx/statbox.h>
#include <wx/sizer.h>

using namespace astro::i18n;

// Event Table (1:1 Legacy)
wxBEGIN_EVENT_TABLE(PersonHilfeDialog, wxDialog)
    EVT_BUTTON(wxID_OK, PersonHilfeDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, PersonHilfeDialog::OnCancel)
    EVT_BUTTON(ID_SUCHEN_BTN, PersonHilfeDialog::OnSuchen)
    EVT_TEXT(ID_SUCH_CTRL, PersonHilfeDialog::OnSuchTextChange)
    EVT_LIST_ITEM_SELECTED(ID_PERSONEN_LISTE, PersonHilfeDialog::OnPersonAuswahl)
    EVT_LIST_ITEM_ACTIVATED(ID_PERSONEN_LISTE, PersonHilfeDialog::OnPersonDoppelklick)
wxEND_EVENT_TABLE()

PersonHilfeDialog::PersonHilfeDialog(wxWindow* parent, LegacyDataManager* data_manager)
    : wxDialog(parent, wxID_ANY, wxString::FromUTF8("Person auswählen"), 
               wxDefaultPosition, wxSize(500, 400),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
      data_manager_(data_manager),
      has_selection_(false) {
    
    CreateLayout();
    InitializeControls();
    Centre();
}

void PersonHilfeDialog::CreateLayout() {
    // Haupt-Sizer
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Such-Bereich (1:1 Legacy)
    wxStaticBoxSizer* such_box = new wxStaticBoxSizer(wxHORIZONTAL, this, wxString::FromUTF8("Suchen"));
    
    such_ctrl_ = new wxTextCtrl(this, ID_SUCH_CTRL, "", wxDefaultPosition, wxSize(-1, -1));
    suchen_btn_ = new wxButton(this, ID_SUCHEN_BTN, wxString::FromUTF8("Suchen"));
    
    such_box->Add(such_ctrl_, 1, wxEXPAND | wxRIGHT, 5);
    such_box->Add(suchen_btn_, 0);
    
    main_sizer->Add(such_box, 0, wxEXPAND | wxALL, 10);
    
    // Personen-Liste (1:1 Legacy)
    wxStaticBoxSizer* liste_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Personen"));
    
    personen_liste_ = new wxListCtrl(this, ID_PERSONEN_LISTE, wxDefaultPosition, wxSize(-1, 250), 
                                    wxLC_REPORT | wxLC_SINGLE_SEL);
    
    // Spalten definieren
    personen_liste_->AppendColumn(wxString::FromUTF8("Name"), wxLIST_FORMAT_LEFT, 120);
    personen_liste_->AppendColumn(wxString::FromUTF8("Vorname"), wxLIST_FORMAT_LEFT, 120);
    personen_liste_->AppendColumn(wxString::FromUTF8("Geburtsdatum"), wxLIST_FORMAT_LEFT, 100);
    personen_liste_->AppendColumn(wxString::FromUTF8("Geburtsort"), wxLIST_FORMAT_LEFT, 120);
    
    liste_box->Add(personen_liste_, 1, wxEXPAND | wxALL, 5);
    
    // Anzahl-Text
    anzahl_text_ = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("0 Personen"));
    liste_box->Add(anzahl_text_, 0, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(liste_box, 1, wxEXPAND | wxALL, 10);
    
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

void PersonHilfeDialog::InitializeControls() {
    LoadPersonenListe();
    such_ctrl_->SetFocus();
}

void PersonHilfeDialog::LoadPersonenListe() {
    personen_liste_->DeleteAllItems();
    
    if (!data_manager_) {
        UpdateAnzahlText();
        return;
    }
    
    const auto& personen = data_manager_->GetPersons();
    
    for (size_t i = 0; i < personen.size(); ++i) {
        const auto& person = personen[i];
        
        long index = personen_liste_->InsertItem(i, wxString::FromUTF8(person.name.c_str()));
        personen_liste_->SetItem(index, 1, wxString::FromUTF8(person.first_name.c_str()));
        
        // Geburtsdatum formatieren
        wxString geburtsdatum = wxString::Format(wxT("%02d.%02d.%04d"), 
                                               person.day, person.month, person.year);
        personen_liste_->SetItem(index, 2, geburtsdatum);
        
        personen_liste_->SetItem(index, 3, wxString::FromUTF8(person.place.c_str()));
        
        // Person-Daten als ItemData speichern
        personen_liste_->SetItemData(index, i);
    }
    
    UpdateAnzahlText();
}

void PersonHilfeDialog::FilterPersonenListe(const wxString& filter) {
    personen_liste_->DeleteAllItems();
    
    if (!data_manager_) {
        UpdateAnzahlText();
        return;
    }
    
    const auto& personen = data_manager_->GetPersons();
    wxString filter_lower = filter.Lower();
    
    int filtered_count = 0;
    for (size_t i = 0; i < personen.size(); ++i) {
        const auto& person = personen[i];
        
        // Filter auf Name, Vorname und Geburtsort anwenden
        wxString name_lower = wxString::FromUTF8(person.name.c_str()).Lower();
        wxString vorname_lower = wxString::FromUTF8(person.first_name.c_str()).Lower();
        wxString ort_lower = wxString::FromUTF8(person.place.c_str()).Lower();
        
        if (filter_lower.IsEmpty() || 
            name_lower.Contains(filter_lower) || 
            vorname_lower.Contains(filter_lower) || 
            ort_lower.Contains(filter_lower)) {
            
            long index = personen_liste_->InsertItem(filtered_count, wxString::FromUTF8(person.name.c_str()));
            personen_liste_->SetItem(index, 1, wxString::FromUTF8(person.first_name.c_str()));
            
            wxString geburtsdatum = wxString::Format(wxT("%02d.%02d.%04d"), 
                                                   person.day, person.month, person.year);
            personen_liste_->SetItem(index, 2, geburtsdatum);
            
            personen_liste_->SetItem(index, 3, wxString::FromUTF8(person.place.c_str()));
            
            personen_liste_->SetItemData(index, i);
            filtered_count++;
        }
    }
    
    UpdateAnzahlText();
}

void PersonHilfeDialog::UpdateAnzahlText() {
    int anzahl = personen_liste_->GetItemCount();
    if (anzahl == 1) {
        anzahl_text_->SetLabel(wxString::FromUTF8("1 Person"));
    } else {
        anzahl_text_->SetLabel(wxString::Format(wxString::FromUTF8("%d Personen"), anzahl));
    }
}

// Event Handlers (1:1 Legacy)
void PersonHilfeDialog::OnOK(wxCommandEvent& event) {
    if (!has_selection_) {
        wxMessageBox(wxString::FromUTF8("Bitte wählen Sie eine Person aus!"), 
                    wxString::FromUTF8("Keine Auswahl"), wxOK | wxICON_WARNING);
        return;
    }
    
    EndModal(wxID_OK);
}

void PersonHilfeDialog::OnCancel(wxCommandEvent& event) {
    EndModal(wxID_CANCEL);
}

void PersonHilfeDialog::OnPersonDoppelklick(wxListEvent& event) {
    // Doppelklick = OK
    if (has_selection_) {
        EndModal(wxID_OK);
    }
}

void PersonHilfeDialog::OnPersonAuswahl(wxListEvent& event) {
    long selected_index = event.GetIndex();
    if (selected_index >= 0) {
        // Person-Index aus ItemData abrufen
        size_t person_index = personen_liste_->GetItemData(selected_index);
        
        if (data_manager_) {
            const auto& personen = data_manager_->GetPersons();
            if (person_index < personen.size()) {
                selected_person_ = personen[person_index];
                has_selection_ = true;
                ok_btn_->Enable(true);
            }
        }
    }
}

void PersonHilfeDialog::OnSuchen(wxCommandEvent& event) {
    wxString filter = such_ctrl_->GetValue();
    FilterPersonenListe(filter);
}

void PersonHilfeDialog::OnSuchTextChange(wxCommandEvent& event) {
    // Live-Suche bei Texteingabe
    wxString filter = such_ctrl_->GetValue();
    FilterPersonenListe(filter);
}

LegacyPerson PersonHilfeDialog::GetSelectedPerson() const {
    return selected_person_;
}

bool PersonHilfeDialog::HasSelection() const {
    return has_selection_;
}
