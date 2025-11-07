// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgHausAuswahl from astrofil.c

#include "haus_dialog.h"
#include "astro/i18n.h"
#include <wx/statbox.h>
#include <wx/sizer.h>

using namespace astro::i18n;

// Event Table (1:1 Legacy)
wxBEGIN_EVENT_TABLE(HausDialog, wxDialog)
    EVT_BUTTON(wxID_OK, HausDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, HausDialog::OnCancel)
    EVT_RADIOBUTTON(ID_PLACIDUS, HausDialog::OnHausSystemChange)
    EVT_RADIOBUTTON(ID_KOCH, HausDialog::OnHausSystemChange)
    EVT_RADIOBUTTON(ID_PORPHYRY, HausDialog::OnHausSystemChange)
    EVT_RADIOBUTTON(ID_REGIOMONTANUS, HausDialog::OnHausSystemChange)
    EVT_RADIOBUTTON(ID_CAMPANUS, HausDialog::OnHausSystemChange)
    EVT_RADIOBUTTON(ID_EQUAL, HausDialog::OnHausSystemChange)
    EVT_RADIOBUTTON(ID_WHOLE_SIGN, HausDialog::OnHausSystemChange)
    EVT_RADIOBUTTON(ID_ALCABITIUS, HausDialog::OnHausSystemChange)
    EVT_RADIOBUTTON(ID_MORINUS, HausDialog::OnHausSystemChange)
    EVT_RADIOBUTTON(ID_TOPOCENTRIC, HausDialog::OnHausSystemChange)
wxEND_EVENT_TABLE()

HausDialog::HausDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, wxString::FromUTF8("Häusersystem Auswahl"), 
               wxDefaultPosition, wxSize(400, 500),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
      current_system_(astro::HouseSystem::Placidus) {
    
    CreateLayout();
    InitializeControls();
    Centre();
}

void HausDialog::CreateLayout() {
    // Haupt-Sizer
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Häusersystem-Gruppe (1:1 Legacy)
    wxStaticBoxSizer* haus_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Häusersystem"));
    
    // Radio Buttons für alle Häusersysteme (1:1 Legacy)
    placidus_rb_ = new wxRadioButton(this, ID_PLACIDUS, wxString::FromUTF8("Placidus"), 
                                    wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    koch_rb_ = new wxRadioButton(this, ID_KOCH, wxString::FromUTF8("Koch"));
    porphyry_rb_ = new wxRadioButton(this, ID_PORPHYRY, wxString::FromUTF8("Porphyry"));
    regiomontanus_rb_ = new wxRadioButton(this, ID_REGIOMONTANUS, wxString::FromUTF8("Regiomontanus"));
    campanus_rb_ = new wxRadioButton(this, ID_CAMPANUS, wxString::FromUTF8("Campanus"));
    equal_rb_ = new wxRadioButton(this, ID_EQUAL, wxString::FromUTF8("Equal House"));
    whole_sign_rb_ = new wxRadioButton(this, ID_WHOLE_SIGN, wxString::FromUTF8("Whole Sign"));
    alcabitius_rb_ = new wxRadioButton(this, ID_ALCABITIUS, wxString::FromUTF8("Alcabitius"));
    morinus_rb_ = new wxRadioButton(this, ID_MORINUS, wxString::FromUTF8("Morinus"));
    topocentric_rb_ = new wxRadioButton(this, ID_TOPOCENTRIC, wxString::FromUTF8("Topocentric"));
    
    // Radio Buttons hinzufügen
    haus_box->Add(placidus_rb_, 0, wxEXPAND | wxALL, 3);
    haus_box->Add(koch_rb_, 0, wxEXPAND | wxALL, 3);
    haus_box->Add(porphyry_rb_, 0, wxEXPAND | wxALL, 3);
    haus_box->Add(regiomontanus_rb_, 0, wxEXPAND | wxALL, 3);
    haus_box->Add(campanus_rb_, 0, wxEXPAND | wxALL, 3);
    haus_box->Add(equal_rb_, 0, wxEXPAND | wxALL, 3);
    haus_box->Add(whole_sign_rb_, 0, wxEXPAND | wxALL, 3);
    haus_box->Add(alcabitius_rb_, 0, wxEXPAND | wxALL, 3);
    haus_box->Add(morinus_rb_, 0, wxEXPAND | wxALL, 3);
    haus_box->Add(topocentric_rb_, 0, wxEXPAND | wxALL, 3);
    
    main_sizer->Add(haus_box, 0, wxEXPAND | wxALL, 10);
    
    // Beschreibung (1:1 Legacy)
    wxStaticBoxSizer* desc_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Beschreibung"));
    
    beschreibung_text_ = new wxStaticText(this, wxID_ANY, wxString::FromUTF8(""), 
                                         wxDefaultPosition, wxSize(-1, 100), 
                                         wxST_NO_AUTORESIZE);
    beschreibung_text_->Wrap(350);
    
    desc_box->Add(beschreibung_text_, 1, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(desc_box, 1, wxEXPAND | wxALL, 10);
    
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

void HausDialog::InitializeControls() {
    // Placidus als Standard auswählen (1:1 Legacy)
    placidus_rb_->SetValue(true);
    current_system_ = astro::HouseSystem::Placidus;
    UpdateBeschreibung();
}

void HausDialog::UpdateBeschreibung() {
    wxString beschreibung;
    
    switch (current_system_) {
        case astro::HouseSystem::Placidus:
            beschreibung = wxString::FromUTF8(
                "Placidus-System:\n"
                "Das am häufigsten verwendete Häusersystem in der westlichen Astrologie. "
                "Basiert auf der Zeiteinteilung und berücksichtigt die geografische Breite. "
                "Entwickelt von Placidus de Titis im 17. Jahrhundert."
            );
            break;
            
        case astro::HouseSystem::Koch:
            beschreibung = wxString::FromUTF8(
                "Koch-System:\n"
                "Auch als 'Geburtsort-System' bekannt. Teilt den Raum zwischen Aszendent "
                "und MC in drei gleiche Zeitabschnitte. Entwickelt von Walter Koch."
            );
            break;
            
        case astro::HouseSystem::Porphyry:
            beschreibung = wxString::FromUTF8(
                "Porphyry-System:\n"
                "Einfaches System, das die Quadranten zwischen ASC-MC, MC-DSC, DSC-IC "
                "und IC-ASC jeweils in drei gleiche Teile unterteilt."
            );
            break;
            
        case astro::HouseSystem::Regiomontanus:
            beschreibung = wxString::FromUTF8(
                "Regiomontanus-System:\n"
                "Eines der ältesten Häusersysteme, entwickelt von Johannes Müller "
                "(Regiomontanus) im 15. Jahrhundert. Basiert auf der Zeiteinteilung."
            );
            break;
            
        case astro::HouseSystem::Campanus:
            beschreibung = wxString::FromUTF8(
                "Campanus-System:\n"
                "Entwickelt von Giovanni Campano im 13. Jahrhundert. Teilt den "
                "Primvertikalkreis in 12 gleiche Teile von 30° auf."
            );
            break;
            
        case astro::HouseSystem::Equal:
            beschreibung = wxString::FromUTF8(
                "Equal House System:\n"
                "Jedes Haus hat exakt 30°. Das erste Haus beginnt am Aszendenten, "
                "alle anderen folgen in 30°-Abständen."
            );
            break;
            
        case astro::HouseSystem::WholeSign:
            beschreibung = wxString::FromUTF8(
                "Whole Sign System:\n"
                "Das älteste Häusersystem. Jedes Haus entspricht einem kompletten "
                "Tierkreiszeichen (30°). Das erste Haus ist das Zeichen des Aszendenten."
            );
            break;
            
        case astro::HouseSystem::Alcabitius:
            beschreibung = wxString::FromUTF8(
                "Alcabitius-System:\n"
                "Entwickelt vom arabischen Astrologen Alcabitius im 10. Jahrhundert. "
                "Ähnlich dem Placidus-System, aber mit anderer Berechnungsmethode."
            );
            break;
            
        case astro::HouseSystem::Morinus:
            beschreibung = wxString::FromUTF8(
                "Morinus-System:\n"
                "Entwickelt von Jean-Baptiste Morin im 17. Jahrhundert. "
                "Teilt den Äquator in 12 gleiche Teile von 30° auf."
            );
            break;
            
        case astro::HouseSystem::Topocentric:
            beschreibung = wxString::FromUTF8(
                "Topocentric-System:\n"
                "Modernes System, entwickelt von Wendel Polich und A.P. Nelson Page. "
                "Berücksichtigt die Erdkrümmung und ist sehr präzise."
            );
            break;
    }
    
    beschreibung_text_->SetLabel(beschreibung);
    beschreibung_text_->Wrap(350);
}

// Event Handlers (1:1 Legacy)
void HausDialog::OnOK(wxCommandEvent& event) {
    EndModal(wxID_OK);
}

void HausDialog::OnCancel(wxCommandEvent& event) {
    EndModal(wxID_CANCEL);
}

void HausDialog::OnHausSystemChange(wxCommandEvent& event) {
    // Aktuelles System basierend auf Event ID bestimmen
    switch (event.GetId()) {
        case ID_PLACIDUS:
            current_system_ = astro::HouseSystem::Placidus;
            break;
        case ID_KOCH:
            current_system_ = astro::HouseSystem::Koch;
            break;
        case ID_PORPHYRY:
            current_system_ = astro::HouseSystem::Porphyry;
            break;
        case ID_REGIOMONTANUS:
            current_system_ = astro::HouseSystem::Regiomontanus;
            break;
        case ID_CAMPANUS:
            current_system_ = astro::HouseSystem::Campanus;
            break;
        case ID_EQUAL:
            current_system_ = astro::HouseSystem::Equal;
            break;
        case ID_WHOLE_SIGN:
            current_system_ = astro::HouseSystem::WholeSign;
            break;
        case ID_ALCABITIUS:
            current_system_ = astro::HouseSystem::Alcabitius;
            break;
        case ID_MORINUS:
            current_system_ = astro::HouseSystem::Morinus;
            break;
        case ID_TOPOCENTRIC:
            current_system_ = astro::HouseSystem::Topocentric;
            break;
    }
    
    UpdateBeschreibung();
}

astro::HouseSystem HausDialog::GetHouseSystem() const {
    return current_system_;
}

void HausDialog::SetHouseSystem(astro::HouseSystem system) {
    current_system_ = system;
    
    // Entsprechenden Radio Button auswählen
    switch (system) {
        case astro::HouseSystem::Placidus:
            placidus_rb_->SetValue(true);
            break;
        case astro::HouseSystem::Koch:
            koch_rb_->SetValue(true);
            break;
        case astro::HouseSystem::Porphyry:
            porphyry_rb_->SetValue(true);
            break;
        case astro::HouseSystem::Regiomontanus:
            regiomontanus_rb_->SetValue(true);
            break;
        case astro::HouseSystem::Campanus:
            campanus_rb_->SetValue(true);
            break;
        case astro::HouseSystem::Equal:
            equal_rb_->SetValue(true);
            break;
        case astro::HouseSystem::WholeSign:
            whole_sign_rb_->SetValue(true);
            break;
        case astro::HouseSystem::Alcabitius:
            alcabitius_rb_->SetValue(true);
            break;
        case astro::HouseSystem::Morinus:
            morinus_rb_->SetValue(true);
            break;
        case astro::HouseSystem::Topocentric:
            topocentric_rb_->SetValue(true);
            break;
    }
    
    UpdateBeschreibung();
}
