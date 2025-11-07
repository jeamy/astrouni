// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgTransEin from astrofil.c

#include "transit_einstellungen_dialog.h"
#include "astro/i18n.h"
#include <wx/statbox.h>
#include <wx/sizer.h>

using namespace astro::i18n;

// Event Table (1:1 Legacy)
wxBEGIN_EVENT_TABLE(TransitEinstellungenDialog, wxDialog)
    EVT_BUTTON(wxID_OK, TransitEinstellungenDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, TransitEinstellungenDialog::OnCancel)
    EVT_BUTTON(ID_RESET_TRANSIT, TransitEinstellungenDialog::OnReset)
    EVT_BUTTON(ID_ALLE_PLANETEN, TransitEinstellungenDialog::OnAlleAuswahl)
    EVT_BUTTON(ID_KEINE_PLANETEN, TransitEinstellungenDialog::OnKeineAuswahl)
    EVT_BUTTON(ID_ALLE_ASPEKTE, TransitEinstellungenDialog::OnAlleAuswahl)
    EVT_BUTTON(ID_KEINE_ASPEKTE, TransitEinstellungenDialog::OnKeineAuswahl)
    // Planeten-Events
    EVT_CHECKBOX(ID_SUN, TransitEinstellungenDialog::OnPlanetChange)
    EVT_CHECKBOX(ID_MOON, TransitEinstellungenDialog::OnPlanetChange)
    EVT_CHECKBOX(ID_MERCURY, TransitEinstellungenDialog::OnPlanetChange)
    EVT_CHECKBOX(ID_VENUS, TransitEinstellungenDialog::OnPlanetChange)
    EVT_CHECKBOX(ID_MARS, TransitEinstellungenDialog::OnPlanetChange)
    EVT_CHECKBOX(ID_JUPITER, TransitEinstellungenDialog::OnPlanetChange)
    EVT_CHECKBOX(ID_SATURN, TransitEinstellungenDialog::OnPlanetChange)
    EVT_CHECKBOX(ID_URANUS, TransitEinstellungenDialog::OnPlanetChange)
    EVT_CHECKBOX(ID_NEPTUNE, TransitEinstellungenDialog::OnPlanetChange)
    EVT_CHECKBOX(ID_PLUTO, TransitEinstellungenDialog::OnPlanetChange)
    EVT_CHECKBOX(ID_NODES, TransitEinstellungenDialog::OnPlanetChange)
    EVT_CHECKBOX(ID_CHIRON, TransitEinstellungenDialog::OnPlanetChange)
    // Aspekt-Events
    EVT_CHECKBOX(ID_CONJUNCTION, TransitEinstellungenDialog::OnAspektChange)
    EVT_CHECKBOX(ID_SEXTILE, TransitEinstellungenDialog::OnAspektChange)
    EVT_CHECKBOX(ID_SQUARE, TransitEinstellungenDialog::OnAspektChange)
    EVT_CHECKBOX(ID_TRINE, TransitEinstellungenDialog::OnAspektChange)
    EVT_CHECKBOX(ID_OPPOSITION, TransitEinstellungenDialog::OnAspektChange)
    EVT_CHECKBOX(ID_SEMISEXTILE, TransitEinstellungenDialog::OnAspektChange)
    EVT_CHECKBOX(ID_SEMISQUARE, TransitEinstellungenDialog::OnAspektChange)
    EVT_CHECKBOX(ID_SESQUIQUADRATE, TransitEinstellungenDialog::OnAspektChange)
    EVT_CHECKBOX(ID_QUINCUNX, TransitEinstellungenDialog::OnAspektChange)
wxEND_EVENT_TABLE()

TransitEinstellungenDialog::TransitEinstellungenDialog(wxWindow* parent, LegacyDataManager* data_manager)
    : wxDialog(parent, wxID_ANY, wxString::FromUTF8("Transit Einstellungen"), 
               wxDefaultPosition, wxSize(650, 700),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
      data_manager_(data_manager) {
    
    CreateLayout();
    InitializeControls();
    Centre();
}

void TransitEinstellungenDialog::CreateLayout() {
    // Haupt-Sizer
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Planeten-Auswahl (1:1 Legacy)
    wxStaticBoxSizer* planeten_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Transit-Planeten"));
    
    // Planeten-Buttons
    wxBoxSizer* planeten_btn_sizer = new wxBoxSizer(wxHORIZONTAL);
    alle_planeten_btn_ = new wxButton(this, ID_ALLE_PLANETEN, wxString::FromUTF8("Alle"));
    keine_planeten_btn_ = new wxButton(this, ID_KEINE_PLANETEN, wxString::FromUTF8("Keine"));
    planeten_btn_sizer->Add(alle_planeten_btn_, 0, wxRIGHT, 5);
    planeten_btn_sizer->Add(keine_planeten_btn_, 0);
    planeten_box->Add(planeten_btn_sizer, 0, wxALL, 5);
    
    // Planeten-Grid
    wxFlexGridSizer* planeten_grid = new wxFlexGridSizer(4, 3, 5, 10);
    
    sun_cb_ = new wxCheckBox(this, ID_SUN, wxString::FromUTF8("☉ Sonne"));
    moon_cb_ = new wxCheckBox(this, ID_MOON, wxString::FromUTF8("☽ Mond"));
    mercury_cb_ = new wxCheckBox(this, ID_MERCURY, wxString::FromUTF8("☿ Merkur"));
    venus_cb_ = new wxCheckBox(this, ID_VENUS, wxString::FromUTF8("♀ Venus"));
    mars_cb_ = new wxCheckBox(this, ID_MARS, wxString::FromUTF8("♂ Mars"));
    jupiter_cb_ = new wxCheckBox(this, ID_JUPITER, wxString::FromUTF8("♃ Jupiter"));
    saturn_cb_ = new wxCheckBox(this, ID_SATURN, wxString::FromUTF8("♄ Saturn"));
    uranus_cb_ = new wxCheckBox(this, ID_URANUS, wxString::FromUTF8("♅ Uranus"));
    neptune_cb_ = new wxCheckBox(this, ID_NEPTUNE, wxString::FromUTF8("♆ Neptun"));
    pluto_cb_ = new wxCheckBox(this, ID_PLUTO, wxString::FromUTF8("♇ Pluto"));
    nodes_cb_ = new wxCheckBox(this, ID_NODES, wxString::FromUTF8("☊ Mondknoten"));
    chiron_cb_ = new wxCheckBox(this, ID_CHIRON, wxString::FromUTF8("⚷ Chiron"));
    
    planeten_grid->Add(sun_cb_, 0, wxEXPAND);
    planeten_grid->Add(moon_cb_, 0, wxEXPAND);
    planeten_grid->Add(mercury_cb_, 0, wxEXPAND);
    planeten_grid->Add(venus_cb_, 0, wxEXPAND);
    planeten_grid->Add(mars_cb_, 0, wxEXPAND);
    planeten_grid->Add(jupiter_cb_, 0, wxEXPAND);
    planeten_grid->Add(saturn_cb_, 0, wxEXPAND);
    planeten_grid->Add(uranus_cb_, 0, wxEXPAND);
    planeten_grid->Add(neptune_cb_, 0, wxEXPAND);
    planeten_grid->Add(pluto_cb_, 0, wxEXPAND);
    planeten_grid->Add(nodes_cb_, 0, wxEXPAND);
    planeten_grid->Add(chiron_cb_, 0, wxEXPAND);
    
    planeten_box->Add(planeten_grid, 0, wxEXPAND | wxALL, 5);
    main_sizer->Add(planeten_box, 0, wxEXPAND | wxALL, 10);
    
    // Aspekt-Auswahl (1:1 Legacy)
    wxStaticBoxSizer* aspekte_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Transit-Aspekte"));
    
    // Aspekt-Buttons
    wxBoxSizer* aspekte_btn_sizer = new wxBoxSizer(wxHORIZONTAL);
    alle_aspekte_btn_ = new wxButton(this, ID_ALLE_ASPEKTE, wxString::FromUTF8("Alle"));
    keine_aspekte_btn_ = new wxButton(this, ID_KEINE_ASPEKTE, wxString::FromUTF8("Keine"));
    aspekte_btn_sizer->Add(alle_aspekte_btn_, 0, wxRIGHT, 5);
    aspekte_btn_sizer->Add(keine_aspekte_btn_, 0);
    aspekte_box->Add(aspekte_btn_sizer, 0, wxALL, 5);
    
    // Aspekt-Grid
    wxFlexGridSizer* aspekte_grid = new wxFlexGridSizer(3, 3, 5, 10);
    
    conjunction_cb_ = new wxCheckBox(this, ID_CONJUNCTION, wxString::FromUTF8("☌ Konjunktion (0°)"));
    sextile_cb_ = new wxCheckBox(this, ID_SEXTILE, wxString::FromUTF8("⚹ Sextil (60°)"));
    square_cb_ = new wxCheckBox(this, ID_SQUARE, wxString::FromUTF8("□ Quadrat (90°)"));
    trine_cb_ = new wxCheckBox(this, ID_TRINE, wxString::FromUTF8("△ Trigon (120°)"));
    opposition_cb_ = new wxCheckBox(this, ID_OPPOSITION, wxString::FromUTF8("☍ Opposition (180°)"));
    semisextile_cb_ = new wxCheckBox(this, ID_SEMISEXTILE, wxString::FromUTF8("⚺ Halbsextil (30°)"));
    semisquare_cb_ = new wxCheckBox(this, ID_SEMISQUARE, wxString::FromUTF8("∠ Halbquadrat (45°)"));
    sesquiquadrate_cb_ = new wxCheckBox(this, ID_SESQUIQUADRATE, wxString::FromUTF8("⚼ Sesquiquadrat (135°)"));
    quincunx_cb_ = new wxCheckBox(this, ID_QUINCUNX, wxString::FromUTF8("⚻ Quincunx (150°)"));
    
    aspekte_grid->Add(conjunction_cb_, 0, wxEXPAND);
    aspekte_grid->Add(sextile_cb_, 0, wxEXPAND);
    aspekte_grid->Add(square_cb_, 0, wxEXPAND);
    aspekte_grid->Add(trine_cb_, 0, wxEXPAND);
    aspekte_grid->Add(opposition_cb_, 0, wxEXPAND);
    aspekte_grid->Add(semisextile_cb_, 0, wxEXPAND);
    aspekte_grid->Add(semisquare_cb_, 0, wxEXPAND);
    aspekte_grid->Add(sesquiquadrate_cb_, 0, wxEXPAND);
    aspekte_grid->Add(quincunx_cb_, 0, wxEXPAND);
    
    aspekte_box->Add(aspekte_grid, 0, wxEXPAND | wxALL, 5);
    main_sizer->Add(aspekte_box, 0, wxEXPAND | wxALL, 10);
    
    // Orben-Einstellungen (1:1 Legacy)
    wxStaticBoxSizer* orben_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Transit-Orben"));
    
    wxFlexGridSizer* orben_grid = new wxFlexGridSizer(2, 3, 5, 10);
    
    orben_grid->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Hauptaspekte:")), 0, wxALIGN_CENTER_VERTICAL);
    major_orb_ctrl_ = new wxSpinCtrlDouble(this, ID_MAJOR_ORB, "", wxDefaultPosition, wxSize(80, -1));
    major_orb_ctrl_->SetRange(0.0, 15.0);
    major_orb_ctrl_->SetDigits(1);
    major_orb_ctrl_->SetIncrement(0.5);
    orben_grid->Add(major_orb_ctrl_, 0);
    orben_grid->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("°")), 0, wxALIGN_CENTER_VERTICAL);
    
    orben_grid->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Nebenaspekte:")), 0, wxALIGN_CENTER_VERTICAL);
    minor_orb_ctrl_ = new wxSpinCtrlDouble(this, ID_MINOR_ORB, "", wxDefaultPosition, wxSize(80, -1));
    minor_orb_ctrl_->SetRange(0.0, 10.0);
    minor_orb_ctrl_->SetDigits(1);
    minor_orb_ctrl_->SetIncrement(0.5);
    orben_grid->Add(minor_orb_ctrl_, 0);
    orben_grid->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("°")), 0, wxALIGN_CENTER_VERTICAL);
    
    orben_box->Add(orben_grid, 0, wxEXPAND | wxALL, 5);
    main_sizer->Add(orben_box, 0, wxEXPAND | wxALL, 10);
    
    // Anzeige-Optionen (1:1 Legacy)
    wxStaticBoxSizer* anzeige_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Anzeige-Optionen"));
    
    exact_times_cb_ = new wxCheckBox(this, ID_EXACT_TIMES, wxString::FromUTF8("Exakte Zeiten berechnen"));
    applying_separating_cb_ = new wxCheckBox(this, ID_APPLYING_SEPARATING, wxString::FromUTF8("Laufend/Abnehmend anzeigen"));
    sort_by_date_cb_ = new wxCheckBox(this, ID_SORT_BY_DATE, wxString::FromUTF8("Nach Datum sortieren"));
    group_by_planet_cb_ = new wxCheckBox(this, ID_GROUP_BY_PLANET, wxString::FromUTF8("Nach Planeten gruppieren"));
    
    anzeige_box->Add(exact_times_cb_, 0, wxEXPAND | wxALL, 3);
    anzeige_box->Add(applying_separating_cb_, 0, wxEXPAND | wxALL, 3);
    anzeige_box->Add(sort_by_date_cb_, 0, wxEXPAND | wxALL, 3);
    anzeige_box->Add(group_by_planet_cb_, 0, wxEXPAND | wxALL, 3);
    
    main_sizer->Add(anzeige_box, 0, wxEXPAND | wxALL, 10);
    
    // Dialog-Buttons (1:1 Legacy)
    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    
    reset_btn_ = new wxButton(this, ID_RESET_TRANSIT, wxString::FromUTF8("Standard"));
    button_sizer->Add(reset_btn_, 0, wxRIGHT, 10);
    
    ok_btn_ = new wxButton(this, wxID_OK, wxString::FromUTF8("OK"));
    cancel_btn_ = new wxButton(this, wxID_CANCEL, wxString::FromUTF8("Abbrechen"));
    
    button_sizer->AddStretchSpacer();
    button_sizer->Add(ok_btn_, 0, wxRIGHT, 5);
    button_sizer->Add(cancel_btn_, 0);
    
    main_sizer->Add(button_sizer, 0, wxEXPAND | wxALL, 10);
    
    SetSizer(main_sizer);
}

void TransitEinstellungenDialog::InitializeControls() {
    // Standard-Einstellungen (1:1 Legacy)
    // Hauptplaneten aktiviert
    sun_cb_->SetValue(true);
    moon_cb_->SetValue(true);
    mercury_cb_->SetValue(true);
    venus_cb_->SetValue(true);
    mars_cb_->SetValue(true);
    jupiter_cb_->SetValue(true);
    saturn_cb_->SetValue(true);
    
    // Äußere Planeten optional
    uranus_cb_->SetValue(true);
    neptune_cb_->SetValue(true);
    pluto_cb_->SetValue(false);
    nodes_cb_->SetValue(true);
    chiron_cb_->SetValue(false);
    
    // Hauptaspekte aktiviert
    conjunction_cb_->SetValue(true);
    sextile_cb_->SetValue(true);
    square_cb_->SetValue(true);
    trine_cb_->SetValue(true);
    opposition_cb_->SetValue(true);
    
    // Nebenaspekte deaktiviert
    semisextile_cb_->SetValue(false);
    semisquare_cb_->SetValue(false);
    sesquiquadrate_cb_->SetValue(false);
    quincunx_cb_->SetValue(false);
    
    // Standard-Orben
    major_orb_ctrl_->SetValue(2.0);
    minor_orb_ctrl_->SetValue(1.0);
    
    // Standard-Anzeige
    exact_times_cb_->SetValue(true);
    applying_separating_cb_->SetValue(true);
    sort_by_date_cb_->SetValue(true);
    group_by_planet_cb_->SetValue(false);
    
    UpdateAuswahl();
}

void TransitEinstellungenDialog::UpdateAuswahl() {
    // Anzahl ausgewählter Planeten/Aspekte anzeigen
    int planeten_count = 0;
    if (sun_cb_->GetValue()) planeten_count++;
    if (moon_cb_->GetValue()) planeten_count++;
    if (mercury_cb_->GetValue()) planeten_count++;
    if (venus_cb_->GetValue()) planeten_count++;
    if (mars_cb_->GetValue()) planeten_count++;
    if (jupiter_cb_->GetValue()) planeten_count++;
    if (saturn_cb_->GetValue()) planeten_count++;
    if (uranus_cb_->GetValue()) planeten_count++;
    if (neptune_cb_->GetValue()) planeten_count++;
    if (pluto_cb_->GetValue()) planeten_count++;
    if (nodes_cb_->GetValue()) planeten_count++;
    if (chiron_cb_->GetValue()) planeten_count++;
    
    int aspekte_count = 0;
    if (conjunction_cb_->GetValue()) aspekte_count++;
    if (sextile_cb_->GetValue()) aspekte_count++;
    if (square_cb_->GetValue()) aspekte_count++;
    if (trine_cb_->GetValue()) aspekte_count++;
    if (opposition_cb_->GetValue()) aspekte_count++;
    if (semisextile_cb_->GetValue()) aspekte_count++;
    if (semisquare_cb_->GetValue()) aspekte_count++;
    if (sesquiquadrate_cb_->GetValue()) aspekte_count++;
    if (quincunx_cb_->GetValue()) aspekte_count++;
    
    // OK-Button nur aktivieren wenn mindestens 1 Planet und 1 Aspekt ausgewählt
    ok_btn_->Enable(planeten_count > 0 && aspekte_count > 0);
}

// Event Handlers (1:1 Legacy)
void TransitEinstellungenDialog::OnOK(wxCommandEvent& event) {
    // TODO: Einstellungen in LegacyDataManager speichern
    EndModal(wxID_OK);
}

void TransitEinstellungenDialog::OnCancel(wxCommandEvent& event) {
    EndModal(wxID_CANCEL);
}

void TransitEinstellungenDialog::OnReset(wxCommandEvent& event) {
    InitializeControls();
}

void TransitEinstellungenDialog::OnAlleAuswahl(wxCommandEvent& event) {
    if (event.GetId() == ID_ALLE_PLANETEN) {
        // Alle Planeten auswählen
        sun_cb_->SetValue(true);
        moon_cb_->SetValue(true);
        mercury_cb_->SetValue(true);
        venus_cb_->SetValue(true);
        mars_cb_->SetValue(true);
        jupiter_cb_->SetValue(true);
        saturn_cb_->SetValue(true);
        uranus_cb_->SetValue(true);
        neptune_cb_->SetValue(true);
        pluto_cb_->SetValue(true);
        nodes_cb_->SetValue(true);
        chiron_cb_->SetValue(true);
    } else if (event.GetId() == ID_ALLE_ASPEKTE) {
        // Alle Aspekte auswählen
        conjunction_cb_->SetValue(true);
        sextile_cb_->SetValue(true);
        square_cb_->SetValue(true);
        trine_cb_->SetValue(true);
        opposition_cb_->SetValue(true);
        semisextile_cb_->SetValue(true);
        semisquare_cb_->SetValue(true);
        sesquiquadrate_cb_->SetValue(true);
        quincunx_cb_->SetValue(true);
    }
    UpdateAuswahl();
}

void TransitEinstellungenDialog::OnKeineAuswahl(wxCommandEvent& event) {
    if (event.GetId() == ID_KEINE_PLANETEN) {
        // Alle Planeten abwählen
        sun_cb_->SetValue(false);
        moon_cb_->SetValue(false);
        mercury_cb_->SetValue(false);
        venus_cb_->SetValue(false);
        mars_cb_->SetValue(false);
        jupiter_cb_->SetValue(false);
        saturn_cb_->SetValue(false);
        uranus_cb_->SetValue(false);
        neptune_cb_->SetValue(false);
        pluto_cb_->SetValue(false);
        nodes_cb_->SetValue(false);
        chiron_cb_->SetValue(false);
    } else if (event.GetId() == ID_KEINE_ASPEKTE) {
        // Alle Aspekte abwählen
        conjunction_cb_->SetValue(false);
        sextile_cb_->SetValue(false);
        square_cb_->SetValue(false);
        trine_cb_->SetValue(false);
        opposition_cb_->SetValue(false);
        semisextile_cb_->SetValue(false);
        semisquare_cb_->SetValue(false);
        sesquiquadrate_cb_->SetValue(false);
        quincunx_cb_->SetValue(false);
    }
    UpdateAuswahl();
}

void TransitEinstellungenDialog::OnPlanetChange(wxCommandEvent& event) {
    UpdateAuswahl();
}

void TransitEinstellungenDialog::OnAspektChange(wxCommandEvent& event) {
    UpdateAuswahl();
}

TransitEinstellungenDialog::TransitSettings TransitEinstellungenDialog::GetTransitSettings() const {
    TransitSettings settings;
    
    // Planeten
    settings.show_sun = sun_cb_->GetValue();
    settings.show_moon = moon_cb_->GetValue();
    settings.show_mercury = mercury_cb_->GetValue();
    settings.show_venus = venus_cb_->GetValue();
    settings.show_mars = mars_cb_->GetValue();
    settings.show_jupiter = jupiter_cb_->GetValue();
    settings.show_saturn = saturn_cb_->GetValue();
    settings.show_uranus = uranus_cb_->GetValue();
    settings.show_neptune = neptune_cb_->GetValue();
    settings.show_pluto = pluto_cb_->GetValue();
    settings.show_nodes = nodes_cb_->GetValue();
    settings.show_chiron = chiron_cb_->GetValue();
    
    // Aspekte
    settings.show_conjunction = conjunction_cb_->GetValue();
    settings.show_sextile = sextile_cb_->GetValue();
    settings.show_square = square_cb_->GetValue();
    settings.show_trine = trine_cb_->GetValue();
    settings.show_opposition = opposition_cb_->GetValue();
    settings.show_semisextile = semisextile_cb_->GetValue();
    settings.show_semisquare = semisquare_cb_->GetValue();
    settings.show_sesquiquadrate = sesquiquadrate_cb_->GetValue();
    settings.show_quincunx = quincunx_cb_->GetValue();
    
    // Orben
    settings.transit_orb_major = major_orb_ctrl_->GetValue();
    settings.transit_orb_minor = minor_orb_ctrl_->GetValue();
    
    // Anzeige
    settings.show_exact_times = exact_times_cb_->GetValue();
    settings.show_applying_separating = applying_separating_cb_->GetValue();
    settings.sort_by_date = sort_by_date_cb_->GetValue();
    settings.group_by_planet = group_by_planet_cb_->GetValue();
    
    return settings;
}

void TransitEinstellungenDialog::SetTransitSettings(const TransitSettings& settings) {
    // Planeten setzen
    sun_cb_->SetValue(settings.show_sun);
    moon_cb_->SetValue(settings.show_moon);
    mercury_cb_->SetValue(settings.show_mercury);
    venus_cb_->SetValue(settings.show_venus);
    mars_cb_->SetValue(settings.show_mars);
    jupiter_cb_->SetValue(settings.show_jupiter);
    saturn_cb_->SetValue(settings.show_saturn);
    uranus_cb_->SetValue(settings.show_uranus);
    neptune_cb_->SetValue(settings.show_neptune);
    pluto_cb_->SetValue(settings.show_pluto);
    nodes_cb_->SetValue(settings.show_nodes);
    chiron_cb_->SetValue(settings.show_chiron);
    
    // Aspekte setzen
    conjunction_cb_->SetValue(settings.show_conjunction);
    sextile_cb_->SetValue(settings.show_sextile);
    square_cb_->SetValue(settings.show_square);
    trine_cb_->SetValue(settings.show_trine);
    opposition_cb_->SetValue(settings.show_opposition);
    semisextile_cb_->SetValue(settings.show_semisextile);
    semisquare_cb_->SetValue(settings.show_semisquare);
    sesquiquadrate_cb_->SetValue(settings.show_sesquiquadrate);
    quincunx_cb_->SetValue(settings.show_quincunx);
    
    // Orben setzen
    major_orb_ctrl_->SetValue(settings.transit_orb_major);
    minor_orb_ctrl_->SetValue(settings.transit_orb_minor);
    
    // Anzeige setzen
    exact_times_cb_->SetValue(settings.show_exact_times);
    applying_separating_cb_->SetValue(settings.show_applying_separating);
    sort_by_date_cb_->SetValue(settings.sort_by_date);
    group_by_planet_cb_->SetValue(settings.group_by_planet);
    
    UpdateAuswahl();
}
