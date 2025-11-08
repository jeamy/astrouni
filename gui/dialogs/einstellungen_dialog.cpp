// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgAspekte from astrofil.c

#include "einstellungen_dialog.h"
#include "astro/i18n.h"
#include <wx/statbox.h>
#include <wx/sizer.h>

using namespace astro::i18n;

// Event Table (1:1 Legacy)
wxBEGIN_EVENT_TABLE(EinstellungenDialog, wxDialog)
    EVT_BUTTON(wxID_OK, EinstellungenDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, EinstellungenDialog::OnCancel)
    EVT_BUTTON(ID_RESET_SETTINGS, EinstellungenDialog::OnReset)
    EVT_CHECKBOX(ID_SHOW_ASPECTS, EinstellungenDialog::OnSettingChange)
    EVT_CHECKBOX(ID_SHOW_HOUSES, EinstellungenDialog::OnSettingChange)
    EVT_CHECKBOX(ID_SHOW_DEGREES, EinstellungenDialog::OnSettingChange)
    EVT_CHECKBOX(ID_SHOW_RETROGRADE, EinstellungenDialog::OnSettingChange)
    EVT_CHECKBOX(ID_SHOW_MAJOR_ASPECTS, EinstellungenDialog::OnSettingChange)
    EVT_CHECKBOX(ID_SHOW_MINOR_ASPECTS, EinstellungenDialog::OnSettingChange)
    EVT_CHECKBOX(ID_SHOW_ASPECT_LINES, EinstellungenDialog::OnSettingChange)
    EVT_CHECKBOX(ID_SHOW_ASPECT_SYMBOLS, EinstellungenDialog::OnSettingChange)
    EVT_CHECKBOX(ID_SHOW_ALL_PLANETS, EinstellungenDialog::OnSettingChange)
    EVT_CHECKBOX(ID_SHOW_ASTEROIDS, EinstellungenDialog::OnSettingChange)
    EVT_CHECKBOX(ID_SHOW_FIXED_STARS, EinstellungenDialog::OnSettingChange)
    EVT_CHECKBOX(ID_SHOW_HOUSE_NUMBERS, EinstellungenDialog::OnSettingChange)
    EVT_CHECKBOX(ID_SHOW_HOUSE_CUSPS, EinstellungenDialog::OnSettingChange)
    EVT_CHECKBOX(ID_SHOW_HOUSE_RULERS, EinstellungenDialog::OnSettingChange)
    EVT_CHECKBOX(ID_SHOW_PLANET_SYMBOLS, EinstellungenDialog::OnSettingChange)
    EVT_CHECKBOX(ID_SHOW_ZODIAC_SYMBOLS, EinstellungenDialog::OnSettingChange)
    EVT_CHECKBOX(ID_SHOW_ELEMENT_COLORS, EinstellungenDialog::OnSettingChange)
wxEND_EVENT_TABLE()

EinstellungenDialog::EinstellungenDialog(wxWindow* parent, LegacyDataManager* data_manager)
    : wxDialog(parent, wxID_ANY, wxString::FromUTF8("Einstellungen"), 
               wxDefaultPosition, wxSize(500, 600),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
      data_manager_(data_manager) {
    
    CreateLayout();
    InitializeControls();
    Centre();
}

void EinstellungenDialog::CreateLayout() {
    // Haupt-Sizer
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Anzeige-Optionen (1:1 Legacy)
    wxStaticBoxSizer* display_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Anzeige-Optionen"));
    
    show_aspects_ctrl_ = new wxCheckBox(this, ID_SHOW_ASPECTS, wxString::FromUTF8("Aspekte anzeigen"));
    show_houses_ctrl_ = new wxCheckBox(this, ID_SHOW_HOUSES, wxString::FromUTF8("Häuser anzeigen"));
    show_degrees_ctrl_ = new wxCheckBox(this, ID_SHOW_DEGREES, wxString::FromUTF8("Grad/Minuten anzeigen"));
    show_retrograde_ctrl_ = new wxCheckBox(this, ID_SHOW_RETROGRADE, wxString::FromUTF8("Retrograde 'r' anzeigen"));
    
    display_box->Add(show_aspects_ctrl_, 0, wxEXPAND | wxALL, 3);
    display_box->Add(show_houses_ctrl_, 0, wxEXPAND | wxALL, 3);
    display_box->Add(show_degrees_ctrl_, 0, wxEXPAND | wxALL, 3);
    display_box->Add(show_retrograde_ctrl_, 0, wxEXPAND | wxALL, 3);
    
    main_sizer->Add(display_box, 0, wxEXPAND | wxALL, 10);
    
    // Aspekt-Einstellungen (1:1 Legacy)
    wxStaticBoxSizer* aspect_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Aspekt-Einstellungen"));
    
    show_major_aspects_ctrl_ = new wxCheckBox(this, ID_SHOW_MAJOR_ASPECTS, wxString::FromUTF8("Hauptaspekte (0°, 60°, 90°, 120°, 180°)"));
    show_minor_aspects_ctrl_ = new wxCheckBox(this, ID_SHOW_MINOR_ASPECTS, wxString::FromUTF8("Nebenaspekte (30°, 45°, 135°, 150°)"));
    show_aspect_lines_ctrl_ = new wxCheckBox(this, ID_SHOW_ASPECT_LINES, wxString::FromUTF8("Aspekt-Linien zeichnen"));
    show_aspect_symbols_ctrl_ = new wxCheckBox(this, ID_SHOW_ASPECT_SYMBOLS, wxString::FromUTF8("Aspekt-Symbole anzeigen"));
    
    aspect_box->Add(show_major_aspects_ctrl_, 0, wxEXPAND | wxALL, 3);
    aspect_box->Add(show_minor_aspects_ctrl_, 0, wxEXPAND | wxALL, 3);
    aspect_box->Add(show_aspect_lines_ctrl_, 0, wxEXPAND | wxALL, 3);
    aspect_box->Add(show_aspect_symbols_ctrl_, 0, wxEXPAND | wxALL, 3);
    
    main_sizer->Add(aspect_box, 0, wxEXPAND | wxALL, 10);
    
    // Planeten-Einstellungen (1:1 Legacy)
    wxStaticBoxSizer* planet_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Planeten-Einstellungen"));
    
    show_all_planets_ctrl_ = new wxCheckBox(this, ID_SHOW_ALL_PLANETS, wxString::FromUTF8("Alle Planeten (inkl. Uranus, Neptun, Pluto)"));
    show_asteroids_ctrl_ = new wxCheckBox(this, ID_SHOW_ASTEROIDS, wxString::FromUTF8("Asteroiden (Chiron, Mondknoten)"));
    show_fixed_stars_ctrl_ = new wxCheckBox(this, ID_SHOW_FIXED_STARS, wxString::FromUTF8("Fixsterne"));
    
    planet_box->Add(show_all_planets_ctrl_, 0, wxEXPAND | wxALL, 3);
    planet_box->Add(show_asteroids_ctrl_, 0, wxEXPAND | wxALL, 3);
    planet_box->Add(show_fixed_stars_ctrl_, 0, wxEXPAND | wxALL, 3);
    
    main_sizer->Add(planet_box, 0, wxEXPAND | wxALL, 10);
    
    // Häuser-Einstellungen (1:1 Legacy)
    wxStaticBoxSizer* house_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Häuser-Einstellungen"));
    
    show_house_numbers_ctrl_ = new wxCheckBox(this, ID_SHOW_HOUSE_NUMBERS, wxString::FromUTF8("Hausnummern anzeigen"));
    show_house_cusps_ctrl_ = new wxCheckBox(this, ID_SHOW_HOUSE_CUSPS, wxString::FromUTF8("Häuserspitzen anzeigen"));
    show_house_rulers_ctrl_ = new wxCheckBox(this, ID_SHOW_HOUSE_RULERS, wxString::FromUTF8("Häuserherrscher anzeigen"));
    
    house_box->Add(show_house_numbers_ctrl_, 0, wxEXPAND | wxALL, 3);
    house_box->Add(show_house_cusps_ctrl_, 0, wxEXPAND | wxALL, 3);
    house_box->Add(show_house_rulers_ctrl_, 0, wxEXPAND | wxALL, 3);
    
    main_sizer->Add(house_box, 0, wxEXPAND | wxALL, 10);
    
    // Darstellungs-Optionen (1:1 Legacy)
    wxStaticBoxSizer* render_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Darstellungs-Optionen"));
    
    show_planet_symbols_ctrl_ = new wxCheckBox(this, ID_SHOW_PLANET_SYMBOLS, wxString::FromUTF8("Planeten-Symbole (☉☽☿♀♂♃♄)"));
    show_zodiac_symbols_ctrl_ = new wxCheckBox(this, ID_SHOW_ZODIAC_SYMBOLS, wxString::FromUTF8("Tierkreis-Symbole (♈♉♊♋♌♍♎♏♐♑♒♓)"));
    show_element_colors_ctrl_ = new wxCheckBox(this, ID_SHOW_ELEMENT_COLORS, wxString::FromUTF8("Element-Farben (Feuer/Erde/Luft/Wasser)"));
    
    render_box->Add(show_planet_symbols_ctrl_, 0, wxEXPAND | wxALL, 3);
    render_box->Add(show_zodiac_symbols_ctrl_, 0, wxEXPAND | wxALL, 3);
    render_box->Add(show_element_colors_ctrl_, 0, wxEXPAND | wxALL, 3);
    
    main_sizer->Add(render_box, 0, wxEXPAND | wxALL, 10);
    
    // Dialog-Buttons (1:1 Legacy)
    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    
    reset_btn_ = new wxButton(this, ID_RESET_SETTINGS, wxString::FromUTF8("Standard"));
    button_sizer->Add(reset_btn_, 0, wxRIGHT, 10);
    
    ok_btn_ = new wxButton(this, wxID_OK, wxString::FromUTF8("OK"));
    cancel_btn_ = new wxButton(this, wxID_CANCEL, wxString::FromUTF8("Abbrechen"));
    
    button_sizer->AddStretchSpacer();
    button_sizer->Add(ok_btn_, 0, wxRIGHT, 5);
    button_sizer->Add(cancel_btn_, 0);
    
    main_sizer->Add(button_sizer, 0, wxEXPAND | wxALL, 10);
    
    SetSizer(main_sizer);
}

void EinstellungenDialog::InitializeControls() {
    if (data_manager_) {
        const auto& settings = data_manager_->GetSettings();
        SetSettings(settings);
    } else {
        // Standard-Werte (1:1 Legacy)
        show_aspects_ctrl_->SetValue(true);
        show_houses_ctrl_->SetValue(true);
        show_degrees_ctrl_->SetValue(true);
        show_retrograde_ctrl_->SetValue(true);
        show_major_aspects_ctrl_->SetValue(true);
        show_minor_aspects_ctrl_->SetValue(false);
        show_aspect_lines_ctrl_->SetValue(true);
        show_aspect_symbols_ctrl_->SetValue(false);
        show_all_planets_ctrl_->SetValue(true);
        show_asteroids_ctrl_->SetValue(true);
        show_fixed_stars_ctrl_->SetValue(false);
        show_house_numbers_ctrl_->SetValue(true);
        show_house_cusps_ctrl_->SetValue(true);
        show_house_rulers_ctrl_->SetValue(false);
        show_planet_symbols_ctrl_->SetValue(true);
        show_zodiac_symbols_ctrl_->SetValue(true);
        show_element_colors_ctrl_->SetValue(false);
    }
}

void EinstellungenDialog::UpdateControls() {
    // Abhängigkeiten prüfen (1:1 Legacy)
    if (!show_aspects_ctrl_->GetValue()) {
        show_major_aspects_ctrl_->Enable(false);
        show_minor_aspects_ctrl_->Enable(false);
        show_aspect_lines_ctrl_->Enable(false);
        show_aspect_symbols_ctrl_->Enable(false);
    } else {
        show_major_aspects_ctrl_->Enable(true);
        show_minor_aspects_ctrl_->Enable(true);
        show_aspect_lines_ctrl_->Enable(true);
        show_aspect_symbols_ctrl_->Enable(true);
    }
    
    if (!show_houses_ctrl_->GetValue()) {
        show_house_numbers_ctrl_->Enable(false);
        show_house_cusps_ctrl_->Enable(false);
        show_house_rulers_ctrl_->Enable(false);
    } else {
        show_house_numbers_ctrl_->Enable(true);
        show_house_cusps_ctrl_->Enable(true);
        show_house_rulers_ctrl_->Enable(true);
    }
}

// Event Handlers (1:1 Legacy)
void EinstellungenDialog::OnOK(wxCommandEvent& event) {
    if (data_manager_) {
        // Einstellungen in LegacyDataManager speichern
        LegacySettings settings = data_manager_->GetSettings();
        
        // Basis-Einstellungen
        settings.show_aspects = show_aspects_ctrl_->GetValue();
        settings.show_houses = show_houses_ctrl_->GetValue();
        settings.show_degrees = show_degrees_ctrl_->GetValue();
        settings.show_retrograde = show_retrograde_ctrl_->GetValue();
        
        // TODO: Erweiterte Einstellungen in LegacySettings-Struktur hinzufügen
        
        data_manager_->SetSettings(settings);
        data_manager_->SaveUserSettings();
    }
    
    EndModal(wxID_OK);
}

void EinstellungenDialog::OnCancel(wxCommandEvent& event) {
    EndModal(wxID_CANCEL);
}

void EinstellungenDialog::OnReset(wxCommandEvent& event) {
    // Standard-Werte setzen (1:1 Legacy)
    show_aspects_ctrl_->SetValue(true);
    show_houses_ctrl_->SetValue(true);
    show_degrees_ctrl_->SetValue(true);
    show_retrograde_ctrl_->SetValue(true);
    show_major_aspects_ctrl_->SetValue(true);
    show_minor_aspects_ctrl_->SetValue(false);
    show_aspect_lines_ctrl_->SetValue(true);
    show_aspect_symbols_ctrl_->SetValue(false);
    show_all_planets_ctrl_->SetValue(true);
    show_asteroids_ctrl_->SetValue(true);
    show_fixed_stars_ctrl_->SetValue(false);
    show_house_numbers_ctrl_->SetValue(true);
    show_house_cusps_ctrl_->SetValue(true);
    show_house_rulers_ctrl_->SetValue(false);
    show_planet_symbols_ctrl_->SetValue(true);
    show_zodiac_symbols_ctrl_->SetValue(true);
    show_element_colors_ctrl_->SetValue(false);
    
    UpdateControls();
}

void EinstellungenDialog::OnSettingChange(wxCommandEvent& event) {
    UpdateControls();
}

LegacySettings EinstellungenDialog::GetSettings() const {
    LegacySettings settings;
    if (data_manager_) {
        settings = data_manager_->GetSettings();
    }
    
    // Basis-Einstellungen übernehmen
    settings.show_aspects = show_aspects_ctrl_->GetValue();
    settings.show_houses = show_houses_ctrl_->GetValue();
    settings.show_degrees = show_degrees_ctrl_->GetValue();
    settings.show_retrograde = show_retrograde_ctrl_->GetValue();
    
    return settings;
}

void EinstellungenDialog::SetSettings(const LegacySettings& settings) {
    show_aspects_ctrl_->SetValue(settings.show_aspects);
    show_houses_ctrl_->SetValue(settings.show_houses);
    show_degrees_ctrl_->SetValue(settings.show_degrees);
    show_retrograde_ctrl_->SetValue(settings.show_retrograde);
    
    UpdateControls();
}
