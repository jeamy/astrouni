// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgHoroAuswahl from astrofil.c

#include "horotyp_dialog.h"
#include "astro/i18n.h"
#include <wx/statbox.h>
#include <wx/sizer.h>

using namespace astro::i18n;

// Event Table (1:1 Legacy)
wxBEGIN_EVENT_TABLE(HoroTypDialog, wxDialog)
    EVT_BUTTON(wxID_OK, HoroTypDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, HoroTypDialog::OnCancel)
    EVT_RADIOBUTTON(ID_RADIX, HoroTypDialog::OnTypeChange)
    EVT_RADIOBUTTON(ID_TRANSIT, HoroTypDialog::OnTypeChange)
    EVT_RADIOBUTTON(ID_SYNASTRIE, HoroTypDialog::OnTypeChange)
    EVT_RADIOBUTTON(ID_COMPOSIT, HoroTypDialog::OnTypeChange)
    EVT_RADIOBUTTON(ID_SOLAR, HoroTypDialog::OnTypeChange)
    EVT_RADIOBUTTON(ID_LUNAR, HoroTypDialog::OnTypeChange)
    EVT_RADIOBUTTON(ID_PROGRESSION, HoroTypDialog::OnTypeChange)
wxEND_EVENT_TABLE()

HoroTypDialog::HoroTypDialog(wxWindow* parent, LegacyDataManager* data_manager)
    : wxDialog(parent, wxID_ANY, wxString::FromUTF8("Horoskop-Typ Auswahl"), 
               wxDefaultPosition, wxSize(450, 500),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
      data_manager_(data_manager),
      current_type_(HoroType::RADIX) {
    
    CreateLayout();
    InitializeControls();
    Centre();
}

void HoroTypDialog::CreateLayout() {
    // Haupt-Sizer
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Horoskop-Typ-Gruppe (1:1 Legacy)
    wxStaticBoxSizer* type_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Horoskop-Typ"));
    
    // Radio Buttons für alle Horoskop-Typen (1:1 Legacy)
    radix_rb_ = new wxRadioButton(this, ID_RADIX, wxString::FromUTF8("Radix (Geburtshoroskop)"), 
                                 wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    transit_rb_ = new wxRadioButton(this, ID_TRANSIT, wxString::FromUTF8("Transit (Planetenlauf)"));
    synastrie_rb_ = new wxRadioButton(this, ID_SYNASTRIE, wxString::FromUTF8("Synastrie (Partnervergleich)"));
    composit_rb_ = new wxRadioButton(this, ID_COMPOSIT, wxString::FromUTF8("Composit (Beziehungshoroskop)"));
    solar_rb_ = new wxRadioButton(this, ID_SOLAR, wxString::FromUTF8("Solar Return (Jahreshoroskop)"));
    lunar_rb_ = new wxRadioButton(this, ID_LUNAR, wxString::FromUTF8("Lunar Return (Mondwiederkehr)"));
    progression_rb_ = new wxRadioButton(this, ID_PROGRESSION, wxString::FromUTF8("Progression (Direktionen)"));
    
    // Radio Buttons hinzufügen
    type_box->Add(radix_rb_, 0, wxEXPAND | wxALL, 5);
    type_box->Add(transit_rb_, 0, wxEXPAND | wxALL, 5);
    type_box->Add(synastrie_rb_, 0, wxEXPAND | wxALL, 5);
    type_box->Add(composit_rb_, 0, wxEXPAND | wxALL, 5);
    type_box->Add(solar_rb_, 0, wxEXPAND | wxALL, 5);
    type_box->Add(lunar_rb_, 0, wxEXPAND | wxALL, 5);
    type_box->Add(progression_rb_, 0, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(type_box, 0, wxEXPAND | wxALL, 10);
    
    // Beschreibung (1:1 Legacy)
    wxStaticBoxSizer* desc_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Beschreibung"));
    
    beschreibung_text_ = new wxStaticText(this, wxID_ANY, wxString::FromUTF8(""), 
                                         wxDefaultPosition, wxSize(-1, 150), 
                                         wxST_NO_AUTORESIZE);
    beschreibung_text_->Wrap(400);
    
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

void HoroTypDialog::InitializeControls() {
    // Aus Einstellungen laden, sonst Radix als Standard (1:1 Legacy)
    if (data_manager_) {
        uint8_t saved = data_manager_->GetSettings().horo_type;
        if (saved <= static_cast<uint8_t>(HoroType::PROGRESSION)) {
            current_type_ = static_cast<HoroType>(saved);
        } else {
            current_type_ = HoroType::RADIX;
        }
    } else {
        current_type_ = HoroType::RADIX;
    }
    
    // Auswahl im UI setzen
    SetHoroType(current_type_);
    UpdateBeschreibung();
}

void HoroTypDialog::UpdateBeschreibung() {
    wxString beschreibung;
    
    switch (current_type_) {
        case HoroType::RADIX:
            beschreibung = wxString::FromUTF8(
                "Radix (Geburtshoroskop):\n\n"
                "Das Grundhoroskop einer Person, berechnet für den exakten Zeitpunkt "
                "und Ort der Geburt. Zeigt die Persönlichkeitsstruktur, Anlagen, "
                "Talente und Lebensmuster.\n\n"
                "Verwendet werden:\n"
                "• Geburtsdatum und -zeit\n"
                "• Geburtsort (Koordinaten)\n"
                "• Alle Planeten und Häuser\n"
                "• Aspekte zwischen den Planeten"
            );
            break;
            
        case HoroType::TRANSIT:
            beschreibung = wxString::FromUTF8(
                "Transit (Planetenlauf):\n\n"
                "Vergleich der aktuellen Planetenpositionen mit dem Geburtshoroskop. "
                "Zeigt aktuelle Einflüsse, Trends und Entwicklungen.\n\n"
                "Berechnung:\n"
                "• Radix-Planeten (Geburt)\n"
                "• Transit-Planeten (aktuell)\n"
                "• Aspekte zwischen Radix und Transit\n"
                "• Zeitraum wählbar"
            );
            break;
            
        case HoroType::SYNASTRIE:
            beschreibung = wxString::FromUTF8(
                "Synastrie (Partnervergleich):\n\n"
                "Vergleich zweier Geburtshoroskope zur Analyse der Beziehungsdynamik. "
                "Zeigt Harmonien, Spannungen und Ergänzungen zwischen Partnern.\n\n"
                "Benötigt:\n"
                "• Geburtsdaten beider Partner\n"
                "• Aspekte zwischen den Horoskopen\n"
                "• Häuser-Überlagerungen\n"
                "• Planeten-Verbindungen"
            );
            break;
            
        case HoroType::COMPOSIT:
            beschreibung = wxString::FromUTF8(
                "Composit (Beziehungshoroskop):\n\n"
                "Mittelpunkt-Horoskop aus zwei Geburtshoroskopen. Zeigt die "
                "Qualität und den Charakter der Beziehung selbst.\n\n"
                "Berechnung:\n"
                "• Mittelpunkte aller Planeten\n"
                "• Mittelpunkt von Ort und Zeit\n"
                "• Eigenständiges Horoskop\n"
                "• Beziehungs-Themen"
            );
            break;
            
        case HoroType::SOLAR:
            beschreibung = wxString::FromUTF8(
                "Solar Return (Jahreshoroskop):\n\n"
                "Horoskop für den Moment, wenn die Sonne wieder die exakte "
                "Geburtsposition erreicht (Geburtstag). Prognose für das kommende Jahr.\n\n"
                "Eigenschaften:\n"
                "• Jährliche Wiederkehr\n"
                "• Aktueller Wohnort\n"
                "• Jahres-Themen und -Trends\n"
                "• Kombination mit Radix"
            );
            break;
            
        case HoroType::LUNAR:
            beschreibung = wxString::FromUTF8(
                "Lunar Return (Mondwiederkehr):\n\n"
                "Horoskop für den Moment, wenn der Mond wieder die exakte "
                "Geburtsposition erreicht (ca. alle 28 Tage). Monatliche Prognose.\n\n"
                "Eigenschaften:\n"
                "• Monatliche Wiederkehr\n"
                "• Emotionale Zyklen\n"
                "• Kurzfristige Trends\n"
                "• Gefühlsbetontes Timing"
            );
            break;
            
        case HoroType::PROGRESSION:
            beschreibung = wxString::FromUTF8(
                "Progression (Direktionen):\n\n"
                "Symbolische Weiterentwicklung des Geburtshoroskops. "
                "Ein Tag nach der Geburt entspricht einem Lebensjahr.\n\n"
                "Methoden:\n"
                "• Sekundäre Progression (1 Tag = 1 Jahr)\n"
                "• Primäre Direktion\n"
                "• Langfristige Entwicklung\n"
                "• Innere Reifung"
            );
            break;
    }
    
    beschreibung_text_->SetLabel(beschreibung);
    beschreibung_text_->Wrap(400);
}

// Event Handlers (1:1 Legacy)
void HoroTypDialog::OnOK(wxCommandEvent& event) {
    // Horoskop-Typ in Einstellungen speichern
    if (data_manager_) {
        LegacySettings s = data_manager_->GetSettings();
        s.horo_type = static_cast<uint8_t>(current_type_);
        data_manager_->SetSettings(s);
        data_manager_->SaveUserSettings();
    }
    EndModal(wxID_OK);
}

void HoroTypDialog::OnCancel(wxCommandEvent& event) {
    EndModal(wxID_CANCEL);
}

void HoroTypDialog::OnTypeChange(wxCommandEvent& event) {
    // Aktuellen Typ basierend auf Event ID bestimmen
    switch (event.GetId()) {
        case ID_RADIX:
            current_type_ = HoroType::RADIX;
            break;
        case ID_TRANSIT:
            current_type_ = HoroType::TRANSIT;
            break;
        case ID_SYNASTRIE:
            current_type_ = HoroType::SYNASTRIE;
            break;
        case ID_COMPOSIT:
            current_type_ = HoroType::COMPOSIT;
            break;
        case ID_SOLAR:
            current_type_ = HoroType::SOLAR;
            break;
        case ID_LUNAR:
            current_type_ = HoroType::LUNAR;
            break;
        case ID_PROGRESSION:
            current_type_ = HoroType::PROGRESSION;
            break;
    }
    
    UpdateBeschreibung();
}

HoroType HoroTypDialog::GetHoroType() const {
    return current_type_;
}

void HoroTypDialog::SetHoroType(HoroType type) {
    current_type_ = type;
    
    // Entsprechenden Radio Button auswählen
    switch (type) {
        case HoroType::RADIX:
            radix_rb_->SetValue(true);
            break;
        case HoroType::TRANSIT:
            transit_rb_->SetValue(true);
            break;
        case HoroType::SYNASTRIE:
            synastrie_rb_->SetValue(true);
            break;
        case HoroType::COMPOSIT:
            composit_rb_->SetValue(true);
            break;
        case HoroType::SOLAR:
            solar_rb_->SetValue(true);
            break;
        case HoroType::LUNAR:
            lunar_rb_->SetValue(true);
            break;
        case HoroType::PROGRESSION:
            progression_rb_->SetValue(true);
            break;
    }
    
    UpdateBeschreibung();
}
