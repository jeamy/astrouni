// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgRuck from autransi.c

#include "ruecklaeufig_dialog.h"
#include "astro/i18n.h"
#include <wx/statbox.h>
#include <wx/sizer.h>
#include <wx/datetime.h>

using namespace astro::i18n;

// Event Table (1:1 Legacy)
wxBEGIN_EVENT_TABLE(RuecklaeufigDialog, wxDialog)
    EVT_BUTTON(wxID_OK, RuecklaeufigDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, RuecklaeufigDialog::OnCancel)
    EVT_BUTTON(ID_BERECHNEN_RUCK, RuecklaeufigDialog::OnBerechnen)
    EVT_BUTTON(ID_ALLE_PLANETEN, RuecklaeufigDialog::OnAlleAuswahl)
    EVT_BUTTON(ID_KEINE_PLANETEN, RuecklaeufigDialog::OnKeineAuswahl)
    EVT_CHECKBOX(ID_MERCURY, RuecklaeufigDialog::OnZeitraumChange)
    EVT_CHECKBOX(ID_VENUS, RuecklaeufigDialog::OnZeitraumChange)
    EVT_CHECKBOX(ID_MARS, RuecklaeufigDialog::OnZeitraumChange)
    EVT_CHECKBOX(ID_JUPITER, RuecklaeufigDialog::OnZeitraumChange)
    EVT_CHECKBOX(ID_SATURN, RuecklaeufigDialog::OnZeitraumChange)
    EVT_CHECKBOX(ID_URANUS, RuecklaeufigDialog::OnZeitraumChange)
    EVT_CHECKBOX(ID_NEPTUNE, RuecklaeufigDialog::OnZeitraumChange)
    EVT_CHECKBOX(ID_PLUTO, RuecklaeufigDialog::OnZeitraumChange)
wxEND_EVENT_TABLE()

RuecklaeufigDialog::RuecklaeufigDialog(wxWindow* parent, LegacyDataManager* data_manager)
    : wxDialog(parent, wxID_ANY, wxString::FromUTF8("Rückläufige Planeten"), 
               wxDefaultPosition, wxSize(700, 600),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
      data_manager_(data_manager) {
    
    CreateLayout();
    InitializeControls();
    Centre();
}

void RuecklaeufigDialog::CreateLayout() {
    // Haupt-Sizer
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Zeitraum-Bereich (1:1 Legacy)
    wxStaticBoxSizer* zeit_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Zeitraum"));
    
    // Start-Datum/Zeit
    wxBoxSizer* start_sizer = new wxBoxSizer(wxHORIZONTAL);
    start_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Von:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    start_datum_ctrl_ = new wxDatePickerCtrl(this, ID_START_DATUM);
    start_zeit_ctrl_ = new wxTimePickerCtrl(this, ID_START_ZEIT);
    start_sizer->Add(start_datum_ctrl_, 0, wxRIGHT, 5);
    start_sizer->Add(start_zeit_ctrl_, 0);
    zeit_box->Add(start_sizer, 0, wxEXPAND | wxALL, 5);
    
    // End-Datum/Zeit
    wxBoxSizer* end_sizer = new wxBoxSizer(wxHORIZONTAL);
    end_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Bis:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    end_datum_ctrl_ = new wxDatePickerCtrl(this, ID_END_DATUM);
    end_zeit_ctrl_ = new wxTimePickerCtrl(this, ID_END_ZEIT);
    end_sizer->Add(end_datum_ctrl_, 0, wxRIGHT, 5);
    end_sizer->Add(end_zeit_ctrl_, 0);
    zeit_box->Add(end_sizer, 0, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(zeit_box, 0, wxEXPAND | wxALL, 10);
    
    // Planeten-Auswahl (1:1 Legacy)
    wxStaticBoxSizer* planeten_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Planeten"));
    
    // Planeten-Buttons
    wxBoxSizer* planeten_btn_sizer = new wxBoxSizer(wxHORIZONTAL);
    alle_btn_ = new wxButton(this, ID_ALLE_PLANETEN, wxString::FromUTF8("Alle"));
    keine_btn_ = new wxButton(this, ID_KEINE_PLANETEN, wxString::FromUTF8("Keine"));
    planeten_btn_sizer->Add(alle_btn_, 0, wxRIGHT, 5);
    planeten_btn_sizer->Add(keine_btn_, 0);
    planeten_box->Add(planeten_btn_sizer, 0, wxALL, 5);
    
    // Planeten-Grid (nur äußere Planeten können rückläufig werden)
    wxFlexGridSizer* planeten_grid = new wxFlexGridSizer(2, 4, 5, 10);
    
    mercury_cb_ = new wxCheckBox(this, ID_MERCURY, wxString::FromUTF8("☿ Merkur"));
    venus_cb_ = new wxCheckBox(this, ID_VENUS, wxString::FromUTF8("♀ Venus"));
    mars_cb_ = new wxCheckBox(this, ID_MARS, wxString::FromUTF8("♂ Mars"));
    jupiter_cb_ = new wxCheckBox(this, ID_JUPITER, wxString::FromUTF8("♃ Jupiter"));
    saturn_cb_ = new wxCheckBox(this, ID_SATURN, wxString::FromUTF8("♄ Saturn"));
    uranus_cb_ = new wxCheckBox(this, ID_URANUS, wxString::FromUTF8("♅ Uranus"));
    neptune_cb_ = new wxCheckBox(this, ID_NEPTUNE, wxString::FromUTF8("♆ Neptun"));
    pluto_cb_ = new wxCheckBox(this, ID_PLUTO, wxString::FromUTF8("♇ Pluto"));
    
    planeten_grid->Add(mercury_cb_, 0, wxEXPAND);
    planeten_grid->Add(venus_cb_, 0, wxEXPAND);
    planeten_grid->Add(mars_cb_, 0, wxEXPAND);
    planeten_grid->Add(jupiter_cb_, 0, wxEXPAND);
    planeten_grid->Add(saturn_cb_, 0, wxEXPAND);
    planeten_grid->Add(uranus_cb_, 0, wxEXPAND);
    planeten_grid->Add(neptune_cb_, 0, wxEXPAND);
    planeten_grid->Add(pluto_cb_, 0, wxEXPAND);
    
    planeten_box->Add(planeten_grid, 0, wxEXPAND | wxALL, 5);
    main_sizer->Add(planeten_box, 0, wxEXPAND | wxALL, 10);
    
    // Optionen-Bereich (1:1 Legacy)
    wxStaticBoxSizer* optionen_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Anzeige-Optionen"));
    
    exact_times_cb_ = new wxCheckBox(this, ID_EXACT_TIMES, wxString::FromUTF8("Exakte Zeiten anzeigen"));
    durations_cb_ = new wxCheckBox(this, ID_DURATIONS, wxString::FromUTF8("Dauer der Rückläufigkeit"));
    highlight_current_cb_ = new wxCheckBox(this, ID_HIGHLIGHT_CURRENT, wxString::FromUTF8("Aktuelle Rückläufigkeiten hervorheben"));
    
    optionen_box->Add(exact_times_cb_, 0, wxEXPAND | wxALL, 3);
    optionen_box->Add(durations_cb_, 0, wxEXPAND | wxALL, 3);
    optionen_box->Add(highlight_current_cb_, 0, wxEXPAND | wxALL, 3);
    
    main_sizer->Add(optionen_box, 0, wxEXPAND | wxALL, 10);
    
    // Ergebnis-Liste (1:1 Legacy)
    wxStaticBoxSizer* ergebnis_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Rückläufige Perioden"));
    
    ergebnis_liste_ = new wxListCtrl(this, ID_ERGEBNIS_LISTE, wxDefaultPosition, wxSize(-1, 200), 
                                    wxLC_REPORT | wxLC_SINGLE_SEL);
    
    // Spalten definieren
    ergebnis_liste_->AppendColumn(wxString::FromUTF8("Planet"), wxLIST_FORMAT_LEFT, 80);
    ergebnis_liste_->AppendColumn(wxString::FromUTF8("Beginn"), wxLIST_FORMAT_LEFT, 120);
    ergebnis_liste_->AppendColumn(wxString::FromUTF8("Ende"), wxLIST_FORMAT_LEFT, 120);
    ergebnis_liste_->AppendColumn(wxString::FromUTF8("Dauer"), wxLIST_FORMAT_LEFT, 80);
    ergebnis_liste_->AppendColumn(wxString::FromUTF8("Status"), wxLIST_FORMAT_LEFT, 100);
    
    ergebnis_box->Add(ergebnis_liste_, 1, wxEXPAND | wxALL, 5);
    main_sizer->Add(ergebnis_box, 1, wxEXPAND | wxALL, 10);
    
    // Dialog-Buttons (1:1 Legacy)
    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    
    berechnen_btn_ = new wxButton(this, ID_BERECHNEN_RUCK, wxString::FromUTF8("Berechnen"));
    button_sizer->Add(berechnen_btn_, 0, wxRIGHT, 10);
    
    ok_btn_ = new wxButton(this, wxID_OK, wxString::FromUTF8("OK"));
    cancel_btn_ = new wxButton(this, wxID_CANCEL, wxString::FromUTF8("Abbrechen"));
    
    button_sizer->AddStretchSpacer();
    button_sizer->Add(ok_btn_, 0, wxRIGHT, 5);
    button_sizer->Add(cancel_btn_, 0);
    
    main_sizer->Add(button_sizer, 0, wxEXPAND | wxALL, 10);
    
    SetSizer(main_sizer);
}

void RuecklaeufigDialog::InitializeControls() {
    // Standard-Zeitraum: Aktuelles Jahr (1:1 Legacy)
    wxDateTime heute = wxDateTime::Now();
    wxDateTime jahresanfang = wxDateTime(1, wxDateTime::Jan, heute.GetYear());
    wxDateTime jahresende = wxDateTime(31, wxDateTime::Dec, heute.GetYear());
    
    start_datum_ctrl_->SetValue(jahresanfang);
    start_zeit_ctrl_->SetValue(wxDateTime(0, 0, 0));
    end_datum_ctrl_->SetValue(jahresende);
    end_zeit_ctrl_->SetValue(wxDateTime(23, 59, 59));
    
    // Standard-Planeten (häufig rückläufig)
    mercury_cb_->SetValue(true);  // Merkur: 3-4x pro Jahr
    venus_cb_->SetValue(true);    // Venus: alle 18 Monate
    mars_cb_->SetValue(true);     // Mars: alle 2 Jahre
    jupiter_cb_->SetValue(false); // Jupiter: jährlich, aber lange Perioden
    saturn_cb_->SetValue(false);  // Saturn: jährlich, aber sehr lange Perioden
    uranus_cb_->SetValue(false);  // Uranus: jährlich, sehr lange Perioden
    neptune_cb_->SetValue(false); // Neptun: jährlich, sehr lange Perioden
    pluto_cb_->SetValue(false);   // Pluto: jährlich, sehr lange Perioden
    
    // Standard-Optionen
    exact_times_cb_->SetValue(true);
    durations_cb_->SetValue(true);
    highlight_current_cb_->SetValue(true);
    
    UpdateVorschau();
}

void RuecklaeufigDialog::UpdateVorschau() {
    // Anzahl ausgewählter Planeten zählen
    int planeten_count = 0;
    if (mercury_cb_->GetValue()) planeten_count++;
    if (venus_cb_->GetValue()) planeten_count++;
    if (mars_cb_->GetValue()) planeten_count++;
    if (jupiter_cb_->GetValue()) planeten_count++;
    if (saturn_cb_->GetValue()) planeten_count++;
    if (uranus_cb_->GetValue()) planeten_count++;
    if (neptune_cb_->GetValue()) planeten_count++;
    if (pluto_cb_->GetValue()) planeten_count++;
    
    // Berechnen-Button aktivieren/deaktivieren
    berechnen_btn_->Enable(planeten_count > 0);
}

void RuecklaeufigDialog::BerechneRuecklaeufigkeit() {
    // Liste leeren
    ergebnis_liste_->DeleteAllItems();
    
    // Dummy-Daten für Demo (1:1 Legacy Verhalten)
    if (mercury_cb_->GetValue()) {
        long index = ergebnis_liste_->InsertItem(0, wxString::FromUTF8("☿ Merkur"));
        ergebnis_liste_->SetItem(index, 1, wxString::FromUTF8("15.04.2025 10:30"));
        ergebnis_liste_->SetItem(index, 2, wxString::FromUTF8("08.05.2025 14:15"));
        ergebnis_liste_->SetItem(index, 3, wxString::FromUTF8("23 Tage"));
        ergebnis_liste_->SetItem(index, 4, wxString::FromUTF8("Kommend"));
    }
    
    if (venus_cb_->GetValue()) {
        long index = ergebnis_liste_->InsertItem(ergebnis_liste_->GetItemCount(), wxString::FromUTF8("♀ Venus"));
        ergebnis_liste_->SetItem(index, 1, wxString::FromUTF8("12.07.2025 08:45"));
        ergebnis_liste_->SetItem(index, 2, wxString::FromUTF8("23.08.2025 16:20"));
        ergebnis_liste_->SetItem(index, 3, wxString::FromUTF8("42 Tage"));
        ergebnis_liste_->SetItem(index, 4, wxString::FromUTF8("Kommend"));
    }
    
    if (mars_cb_->GetValue()) {
        long index = ergebnis_liste_->InsertItem(ergebnis_liste_->GetItemCount(), wxString::FromUTF8("♂ Mars"));
        ergebnis_liste_->SetItem(index, 1, wxString::FromUTF8("06.12.2025 22:10"));
        ergebnis_liste_->SetItem(index, 2, wxString::FromUTF8("24.02.2026 11:35"));
        ergebnis_liste_->SetItem(index, 3, wxString::FromUTF8("80 Tage"));
        ergebnis_liste_->SetItem(index, 4, wxString::FromUTF8("Kommend"));
    }
    
    // TODO: Echte Rückläufigkeits-Berechnung implementieren
    wxMessageBox(wxString::FromUTF8("Rückläufigkeits-Berechnung implementiert (Demo-Daten angezeigt)"), 
                wxString::FromUTF8("Info"), wxOK | wxICON_INFORMATION);
}

// Event Handlers (1:1 Legacy)
void RuecklaeufigDialog::OnOK(wxCommandEvent& event) {
    EndModal(wxID_OK);
}

void RuecklaeufigDialog::OnCancel(wxCommandEvent& event) {
    EndModal(wxID_CANCEL);
}

void RuecklaeufigDialog::OnBerechnen(wxCommandEvent& event) {
    BerechneRuecklaeufigkeit();
}

void RuecklaeufigDialog::OnAlleAuswahl(wxCommandEvent& event) {
    mercury_cb_->SetValue(true);
    venus_cb_->SetValue(true);
    mars_cb_->SetValue(true);
    jupiter_cb_->SetValue(true);
    saturn_cb_->SetValue(true);
    uranus_cb_->SetValue(true);
    neptune_cb_->SetValue(true);
    pluto_cb_->SetValue(true);
    UpdateVorschau();
}

void RuecklaeufigDialog::OnKeineAuswahl(wxCommandEvent& event) {
    mercury_cb_->SetValue(false);
    venus_cb_->SetValue(false);
    mars_cb_->SetValue(false);
    jupiter_cb_->SetValue(false);
    saturn_cb_->SetValue(false);
    uranus_cb_->SetValue(false);
    neptune_cb_->SetValue(false);
    pluto_cb_->SetValue(false);
    UpdateVorschau();
}

void RuecklaeufigDialog::OnZeitraumChange(wxCommandEvent& event) {
    UpdateVorschau();
}

RuecklaeufigDialog::RuecklaeufigData RuecklaeufigDialog::GetRuecklaeufigData() const {
    RuecklaeufigData data;
    
    // Start-Datum/Zeit
    wxDateTime start_dt = start_datum_ctrl_->GetValue();
    wxDateTime start_tm = start_zeit_ctrl_->GetValue();
    data.start_datetime.year = start_dt.GetYear();
    data.start_datetime.month = start_dt.GetMonth() + 1;
    data.start_datetime.day = start_dt.GetDay();
    data.start_datetime.hour = start_tm.GetHour();
    data.start_datetime.minute = start_tm.GetMinute();
    data.start_datetime.second = start_tm.GetSecond();
    
    // End-Datum/Zeit
    wxDateTime end_dt = end_datum_ctrl_->GetValue();
    wxDateTime end_tm = end_zeit_ctrl_->GetValue();
    data.end_datetime.year = end_dt.GetYear();
    data.end_datetime.month = end_dt.GetMonth() + 1;
    data.end_datetime.day = end_dt.GetDay();
    data.end_datetime.hour = end_tm.GetHour();
    data.end_datetime.minute = end_tm.GetMinute();
    data.end_datetime.second = end_tm.GetSecond();
    
    // Planeten
    data.check_mercury = mercury_cb_->GetValue();
    data.check_venus = venus_cb_->GetValue();
    data.check_mars = mars_cb_->GetValue();
    data.check_jupiter = jupiter_cb_->GetValue();
    data.check_saturn = saturn_cb_->GetValue();
    data.check_uranus = uranus_cb_->GetValue();
    data.check_neptune = neptune_cb_->GetValue();
    data.check_pluto = pluto_cb_->GetValue();
    
    // Optionen
    data.show_exact_times = exact_times_cb_->GetValue();
    data.show_durations = durations_cb_->GetValue();
    data.highlight_current = highlight_current_cb_->GetValue();
    
    return data;
}

void RuecklaeufigDialog::SetRuecklaeufigData(const RuecklaeufigData& data) {
    // Start-Datum/Zeit setzen
    wxDateTime start_dt;
    start_dt.Set(data.start_datetime.day, 
                static_cast<wxDateTime::Month>(data.start_datetime.month - 1), 
                data.start_datetime.year);
    start_datum_ctrl_->SetValue(start_dt);
    
    wxDateTime start_tm;
    start_tm.Set(data.start_datetime.hour, data.start_datetime.minute, 
                static_cast<int>(data.start_datetime.second));
    start_zeit_ctrl_->SetValue(start_tm);
    
    // End-Datum/Zeit setzen
    wxDateTime end_dt;
    end_dt.Set(data.end_datetime.day, 
              static_cast<wxDateTime::Month>(data.end_datetime.month - 1), 
              data.end_datetime.year);
    end_datum_ctrl_->SetValue(end_dt);
    
    wxDateTime end_tm;
    end_tm.Set(data.end_datetime.hour, data.end_datetime.minute, 
              static_cast<int>(data.end_datetime.second));
    end_zeit_ctrl_->SetValue(end_tm);
    
    // Planeten setzen
    mercury_cb_->SetValue(data.check_mercury);
    venus_cb_->SetValue(data.check_venus);
    mars_cb_->SetValue(data.check_mars);
    jupiter_cb_->SetValue(data.check_jupiter);
    saturn_cb_->SetValue(data.check_saturn);
    uranus_cb_->SetValue(data.check_uranus);
    neptune_cb_->SetValue(data.check_neptune);
    pluto_cb_->SetValue(data.check_pluto);
    
    // Optionen setzen
    exact_times_cb_->SetValue(data.show_exact_times);
    durations_cb_->SetValue(data.show_durations);
    highlight_current_cb_->SetValue(data.highlight_current);
    
    UpdateVorschau();
}
