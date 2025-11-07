// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgOrbenEin from astrofil.c

#include "orben_dialog.h"
#include "astro/i18n.h"
#include <wx/statbox.h>
#include <wx/sizer.h>

using namespace astro::i18n;

// Event Table (1:1 Legacy)
wxBEGIN_EVENT_TABLE(OrbenDialog, wxDialog)
    EVT_BUTTON(wxID_OK, OrbenDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, OrbenDialog::OnCancel)
    EVT_BUTTON(ID_RESET, OrbenDialog::OnReset)
    EVT_SPINCTRLDOUBLE(ID_SUN_MOON, OrbenDialog::OnOrbChange)
    EVT_SPINCTRLDOUBLE(ID_SUN_PLANETS, OrbenDialog::OnOrbChange)
    EVT_SPINCTRLDOUBLE(ID_MOON_PLANETS, OrbenDialog::OnOrbChange)
    EVT_SPINCTRLDOUBLE(ID_PLANETS_PLANETS, OrbenDialog::OnOrbChange)
    EVT_SPINCTRLDOUBLE(ID_MINOR_ASPECTS, OrbenDialog::OnOrbChange)
wxEND_EVENT_TABLE()

OrbenDialog::OrbenDialog(wxWindow* parent, LegacyDataManager* data_manager)
    : wxDialog(parent, wxID_ANY, wxString::FromUTF8("Orben Einstellungen"), 
               wxDefaultPosition, wxSize(450, 400),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
      data_manager_(data_manager) {
    
    CreateLayout();
    InitializeControls();
    Centre();
}

void OrbenDialog::CreateLayout() {
    // Haupt-Sizer
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Orben-Gruppe (1:1 Legacy)
    wxStaticBoxSizer* orben_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Aspekt-Orben (in Grad)"));
    
    // Sonne-Mond
    wxBoxSizer* sun_moon_sizer = new wxBoxSizer(wxHORIZONTAL);
    sun_moon_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Sonne - Mond:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    sun_moon_ctrl_ = new wxSpinCtrlDouble(this, ID_SUN_MOON, "", wxDefaultPosition, wxSize(80, -1));
    sun_moon_ctrl_->SetRange(0.0, 20.0);
    sun_moon_ctrl_->SetDigits(1);
    sun_moon_ctrl_->SetIncrement(0.5);
    sun_moon_sizer->Add(sun_moon_ctrl_, 0);
    sun_moon_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("°")), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
    orben_box->Add(sun_moon_sizer, 0, wxEXPAND | wxALL, 5);
    
    // Sonne-Planeten
    wxBoxSizer* sun_planets_sizer = new wxBoxSizer(wxHORIZONTAL);
    sun_planets_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Sonne - Planeten:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    sun_planets_ctrl_ = new wxSpinCtrlDouble(this, ID_SUN_PLANETS, "", wxDefaultPosition, wxSize(80, -1));
    sun_planets_ctrl_->SetRange(0.0, 20.0);
    sun_planets_ctrl_->SetDigits(1);
    sun_planets_ctrl_->SetIncrement(0.5);
    sun_planets_sizer->Add(sun_planets_ctrl_, 0);
    sun_planets_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("°")), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
    orben_box->Add(sun_planets_sizer, 0, wxEXPAND | wxALL, 5);
    
    // Mond-Planeten
    wxBoxSizer* moon_planets_sizer = new wxBoxSizer(wxHORIZONTAL);
    moon_planets_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Mond - Planeten:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    moon_planets_ctrl_ = new wxSpinCtrlDouble(this, ID_MOON_PLANETS, "", wxDefaultPosition, wxSize(80, -1));
    moon_planets_ctrl_->SetRange(0.0, 20.0);
    moon_planets_ctrl_->SetDigits(1);
    moon_planets_ctrl_->SetIncrement(0.5);
    moon_planets_sizer->Add(moon_planets_ctrl_, 0);
    moon_planets_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("°")), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
    orben_box->Add(moon_planets_sizer, 0, wxEXPAND | wxALL, 5);
    
    // Planeten-Planeten
    wxBoxSizer* planets_planets_sizer = new wxBoxSizer(wxHORIZONTAL);
    planets_planets_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Planeten - Planeten:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    planets_planets_ctrl_ = new wxSpinCtrlDouble(this, ID_PLANETS_PLANETS, "", wxDefaultPosition, wxSize(80, -1));
    planets_planets_ctrl_->SetRange(0.0, 20.0);
    planets_planets_ctrl_->SetDigits(1);
    planets_planets_ctrl_->SetIncrement(0.5);
    planets_planets_sizer->Add(planets_planets_ctrl_, 0);
    planets_planets_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("°")), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
    orben_box->Add(planets_planets_sizer, 0, wxEXPAND | wxALL, 5);
    
    // Nebenaspekte
    wxBoxSizer* minor_aspects_sizer = new wxBoxSizer(wxHORIZONTAL);
    minor_aspects_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Nebenaspekte:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    minor_aspects_ctrl_ = new wxSpinCtrlDouble(this, ID_MINOR_ASPECTS, "", wxDefaultPosition, wxSize(80, -1));
    minor_aspects_ctrl_->SetRange(0.0, 10.0);
    minor_aspects_ctrl_->SetDigits(1);
    minor_aspects_ctrl_->SetIncrement(0.5);
    minor_aspects_sizer->Add(minor_aspects_ctrl_, 0);
    minor_aspects_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("°")), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
    orben_box->Add(minor_aspects_sizer, 0, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(orben_box, 0, wxEXPAND | wxALL, 10);
    
    // Beschreibung (1:1 Legacy)
    wxStaticBoxSizer* desc_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Erklärung"));
    
    beschreibung_text_ = new wxStaticText(this, wxID_ANY, 
        wxString::FromUTF8("Orben bestimmen die Toleranz für Aspekte.\n"
                          "Größere Orben = mehr Aspekte werden erkannt.\n"
                          "Kleinere Orben = nur exakte Aspekte.\n\n"
                          "Standard-Werte:\n"
                          "• Sonne-Mond: 10°\n"
                          "• Sonne-Planeten: 8°\n"
                          "• Mond-Planeten: 6°\n"
                          "• Planeten-Planeten: 6°\n"
                          "• Nebenaspekte: 3°"), 
        wxDefaultPosition, wxSize(-1, 120));
    
    desc_box->Add(beschreibung_text_, 1, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(desc_box, 1, wxEXPAND | wxALL, 10);
    
    // Dialog-Buttons (1:1 Legacy)
    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    
    reset_btn_ = new wxButton(this, ID_RESET, wxString::FromUTF8("Standard"));
    button_sizer->Add(reset_btn_, 0, wxRIGHT, 10);
    
    ok_btn_ = new wxButton(this, wxID_OK, wxString::FromUTF8("OK"));
    cancel_btn_ = new wxButton(this, wxID_CANCEL, wxString::FromUTF8("Abbrechen"));
    
    button_sizer->AddStretchSpacer();
    button_sizer->Add(ok_btn_, 0, wxRIGHT, 5);
    button_sizer->Add(cancel_btn_, 0);
    
    main_sizer->Add(button_sizer, 0, wxEXPAND | wxALL, 10);
    
    SetSizer(main_sizer);
}

void OrbenDialog::InitializeControls() {
    if (data_manager_) {
        const auto& orbs = data_manager_->GetSettings().orbs;
        SetOrbs(orbs);
    } else {
        // Standard-Werte (1:1 Legacy)
        sun_moon_ctrl_->SetValue(10.0);
        sun_planets_ctrl_->SetValue(8.0);
        moon_planets_ctrl_->SetValue(6.0);
        planets_planets_ctrl_->SetValue(6.0);
        minor_aspects_ctrl_->SetValue(3.0);
    }
}

void OrbenDialog::UpdateBeschreibung() {
    // Beschreibung basierend auf aktuellen Werten aktualisieren
    wxString desc = wxString::Format(
        wxString::FromUTF8("Aktuelle Orben-Einstellungen:\n"
                          "• Sonne-Mond: %.1f°\n"
                          "• Sonne-Planeten: %.1f°\n"
                          "• Mond-Planeten: %.1f°\n"
                          "• Planeten-Planeten: %.1f°\n"
                          "• Nebenaspekte: %.1f°\n\n"
                          "Diese Werte bestimmen, wie weit ein Aspekt\n"
                          "vom exakten Winkel abweichen darf."),
        sun_moon_ctrl_->GetValue(),
        sun_planets_ctrl_->GetValue(),
        moon_planets_ctrl_->GetValue(),
        planets_planets_ctrl_->GetValue(),
        minor_aspects_ctrl_->GetValue()
    );
    
    beschreibung_text_->SetLabel(desc);
}

// Event Handlers (1:1 Legacy)
void OrbenDialog::OnOK(wxCommandEvent& event) {
    if (data_manager_) {
        // Orben in Einstellungen speichern
        LegacySettings settings = data_manager_->GetSettings();
        settings.orbs = GetOrbs();
        data_manager_->SetSettings(settings);
        data_manager_->SaveDefaultSettings();
    }
    
    EndModal(wxID_OK);
}

void OrbenDialog::OnCancel(wxCommandEvent& event) {
    EndModal(wxID_CANCEL);
}

void OrbenDialog::OnReset(wxCommandEvent& event) {
    // Standard-Werte setzen (1:1 Legacy)
    sun_moon_ctrl_->SetValue(10.0);
    sun_planets_ctrl_->SetValue(8.0);
    moon_planets_ctrl_->SetValue(6.0);
    planets_planets_ctrl_->SetValue(6.0);
    minor_aspects_ctrl_->SetValue(3.0);
    
    UpdateBeschreibung();
}

void OrbenDialog::OnOrbChange(wxSpinDoubleEvent& event) {
    UpdateBeschreibung();
}

LegacySettings::Orbs OrbenDialog::GetOrbs() const {
    LegacySettings::Orbs orbs;
    orbs.sun_moon = sun_moon_ctrl_->GetValue();
    orbs.sun_planets = sun_planets_ctrl_->GetValue();
    orbs.moon_planets = moon_planets_ctrl_->GetValue();
    orbs.planets_planets = planets_planets_ctrl_->GetValue();
    orbs.minor_aspects = minor_aspects_ctrl_->GetValue();
    return orbs;
}

void OrbenDialog::SetOrbs(const LegacySettings::Orbs& orbs) {
    sun_moon_ctrl_->SetValue(orbs.sun_moon);
    sun_planets_ctrl_->SetValue(orbs.sun_planets);
    moon_planets_ctrl_->SetValue(orbs.moon_planets);
    planets_planets_ctrl_->SetValue(orbs.planets_planets);
    minor_aspects_ctrl_->SetValue(orbs.minor_aspects);
    
    UpdateBeschreibung();
}
