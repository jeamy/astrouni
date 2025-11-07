// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgPErfassen from astrofil.c

#include "person_dialog.h"
#include "astro/i18n.h"
#include <wx/statbox.h>
#include <wx/sizer.h>

using namespace astro::i18n;

// Event Table (1:1 Legacy)
wxBEGIN_EVENT_TABLE(PersonDialog, wxDialog)
    EVT_BUTTON(wxID_OK, PersonDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, PersonDialog::OnCancel)
    EVT_BUTTON(ID_PERSON_AUSWAHL, PersonDialog::OnPersonAuswahl)
    EVT_BUTTON(ID_ORT_AUSWAHL, PersonDialog::OnOrtAuswahl)
    EVT_BUTTON(ID_BERECHNEN, PersonDialog::OnBerechnen)
wxEND_EVENT_TABLE()

PersonDialog::PersonDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, wxString::FromUTF8("Personen Erfassen"), 
               wxDefaultPosition, wxSize(400, 500),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) {
    
    CreateLayout();
    InitializeControls();
    Centre();
}

void PersonDialog::CreateLayout() {
    // Haupt-Sizer
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Person-Gruppe (1:1 Legacy)
    wxStaticBoxSizer* person_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Person"));
    
    // Name
    wxBoxSizer* name_sizer = new wxBoxSizer(wxHORIZONTAL);
    name_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Name:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    name_ctrl_ = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(120, -1));
    name_sizer->Add(name_ctrl_, 1, wxEXPAND);
    person_box->Add(name_sizer, 0, wxEXPAND | wxALL, 5);
    
    // Vorname
    wxBoxSizer* vorname_sizer = new wxBoxSizer(wxHORIZONTAL);
    vorname_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Vorname:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    vorname_ctrl_ = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(120, -1));
    vorname_sizer->Add(vorname_ctrl_, 1, wxEXPAND);
    person_box->Add(vorname_sizer, 0, wxEXPAND | wxALL, 5);
    
    // Geschlecht
    wxBoxSizer* geschlecht_sizer = new wxBoxSizer(wxHORIZONTAL);
    geschlecht_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Geschlecht:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    wxArrayString geschlechter;
    geschlechter.Add(wxString::FromUTF8("Männlich"));
    geschlechter.Add(wxString::FromUTF8("Weiblich"));
    geschlechter.Add(wxString::FromUTF8("Unbekannt"));
    geschlecht_ctrl_ = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, geschlechter);
    geschlecht_ctrl_->SetSelection(2); // Unbekannt als Default
    geschlecht_sizer->Add(geschlecht_ctrl_, 0, wxEXPAND);
    person_box->Add(geschlecht_sizer, 0, wxEXPAND | wxALL, 5);
    
    // Person auswählen Button
    person_btn_ = new wxButton(this, ID_PERSON_AUSWAHL, wxString::FromUTF8("Person auswählen..."));
    person_box->Add(person_btn_, 0, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(person_box, 0, wxEXPAND | wxALL, 10);
    
    // Geburtsdatum & Zeit Gruppe (1:1 Legacy)
    wxStaticBoxSizer* geburt_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Geburtsdatum & -zeit"));
    
    // Datum
    wxBoxSizer* datum_sizer = new wxBoxSizer(wxHORIZONTAL);
    datum_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Datum:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    datum_ctrl_ = new wxDatePickerCtrl(this, wxID_ANY);
    datum_sizer->Add(datum_ctrl_, 1, wxEXPAND);
    geburt_box->Add(datum_sizer, 0, wxEXPAND | wxALL, 5);
    
    // Zeit
    wxBoxSizer* zeit_sizer = new wxBoxSizer(wxHORIZONTAL);
    zeit_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Zeit:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    zeit_ctrl_ = new wxTimePickerCtrl(this, wxID_ANY);
    zeit_sizer->Add(zeit_ctrl_, 0, wxEXPAND);
    
    // Sommerzeit Checkbox
    sommerzeit_ctrl_ = new wxCheckBox(this, wxID_ANY, wxString::FromUTF8("Sommerzeit"));
    zeit_sizer->Add(sommerzeit_ctrl_, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 10);
    geburt_box->Add(zeit_sizer, 0, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(geburt_box, 0, wxEXPAND | wxALL, 10);
    
    // Geburtsort Gruppe (1:1 Legacy)
    wxStaticBoxSizer* ort_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Geburtsort"));
    
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
    
    // Koordinaten
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
    
    // Ort auswählen Button
    ort_btn_ = new wxButton(this, ID_ORT_AUSWAHL, wxString::FromUTF8("Ort auswählen..."));
    ort_box->Add(ort_btn_, 0, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(ort_box, 1, wxEXPAND | wxALL, 10);
    
    // Buttons (1:1 Legacy)
    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    
    berechnen_btn_ = new wxButton(this, ID_BERECHNEN, wxString::FromUTF8("Berechnen"));
    button_sizer->Add(berechnen_btn_, 0, wxRIGHT, 10);
    
    ok_btn_ = new wxButton(this, wxID_OK, wxString::FromUTF8("OK"));
    cancel_btn_ = new wxButton(this, wxID_CANCEL, wxString::FromUTF8("Abbrechen"));
    
    button_sizer->AddStretchSpacer();
    button_sizer->Add(ok_btn_, 0, wxRIGHT, 5);
    button_sizer->Add(cancel_btn_, 0);
    
    main_sizer->Add(button_sizer, 0, wxEXPAND | wxALL, 10);
    
    SetSizer(main_sizer);
}

void PersonDialog::InitializeControls() {
    // Default-Werte setzen (1:1 Legacy)
    name_ctrl_->SetValue(wxString::FromUTF8("Mustermann"));
    vorname_ctrl_->SetValue(wxString::FromUTF8("Max"));
    ort_ctrl_->SetValue(wxString::FromUTF8("Berlin"));
    land_ctrl_->SetValue(wxString::FromUTF8("Deutschland"));
    breite_ctrl_->SetValue(52.5200);  // Berlin
    laenge_ctrl_->SetValue(13.4050);  // Berlin
    hoehe_ctrl_->SetValue(34);        // Berlin Höhe
    
    // Fokus auf Name setzen
    name_ctrl_->SetFocus();
}

// Event Handlers (1:1 Legacy)
void PersonDialog::OnOK(wxCommandEvent& event) {
    // Validierung
    if (name_ctrl_->GetValue().IsEmpty()) {
        wxMessageBox(wxString::FromUTF8("Bitte Namen eingeben!"), wxString::FromUTF8("Fehler"), wxOK | wxICON_ERROR);
        name_ctrl_->SetFocus();
        return;
    }
    
    if (ort_ctrl_->GetValue().IsEmpty()) {
        wxMessageBox(wxString::FromUTF8("Bitte Ort eingeben!"), wxString::FromUTF8("Fehler"), wxOK | wxICON_ERROR);
        ort_ctrl_->SetFocus();
        return;
    }
    
    EndModal(wxID_OK);
}

void PersonDialog::OnCancel(wxCommandEvent& event) {
    EndModal(wxID_CANCEL);
}

void PersonDialog::OnPersonAuswahl(wxCommandEvent& event) {
    wxMessageBox(wxString::FromUTF8("Person-Datenbank wird implementiert..."), wxString::FromUTF8("Info"), wxOK | wxICON_INFORMATION);
    // TODO: PersonAuswahl Dialog implementieren
}

void PersonDialog::OnOrtAuswahl(wxCommandEvent& event) {
    wxMessageBox(wxString::FromUTF8("Orte-Datenbank wird implementiert..."), wxString::FromUTF8("Info"), wxOK | wxICON_INFORMATION);
    // TODO: OrtAuswahl Dialog implementieren
}

void PersonDialog::OnBerechnen(wxCommandEvent& event) {
    // Daten validieren und Chart berechnen
    astro::RadixData radix = GetRadixData();
    
    wxString info = wxString::Format(
        wxString::FromUTF8("Chart berechnen für:\n%s\n%s\nBreite: %.4f°, Länge: %.4f°"),
        wxString::FromUTF8(radix.name.c_str()),
        wxString::FromUTF8(radix.birth_location.country_code.c_str()),
        radix.birth_location.latitude,
        radix.birth_location.longitude
    );
    
    wxMessageBox(info, wxString::FromUTF8("Chart-Berechnung"), wxOK | wxICON_INFORMATION);
    
    // TODO: Tatsächliche Berechnung implementieren
    EndModal(wxID_OK);
}

astro::RadixData PersonDialog::GetRadixData() const {
    astro::RadixData data;
    
    // Name (kombiniert)
    wxString full_name = vorname_ctrl_->GetValue() + " " + name_ctrl_->GetValue();
    data.name = full_name.ToStdString();
    
    // Datum & Zeit
    wxDateTime datum = datum_ctrl_->GetValue();
    wxDateTime zeit = zeit_ctrl_->GetValue();
    
    data.birth_datetime.year = datum.GetYear();
    data.birth_datetime.month = datum.GetMonth() + 1;  // wxDateTime: 0-11, astro: 1-12
    data.birth_datetime.day = datum.GetDay();
    data.birth_datetime.hour = zeit.GetHour();
    data.birth_datetime.minute = zeit.GetMinute();
    data.birth_datetime.second = zeit.GetSecond();
    
    // Ort
    data.birth_location.latitude = breite_ctrl_->GetValue();
    data.birth_location.longitude = laenge_ctrl_->GetValue();
    data.birth_location.altitude = hoehe_ctrl_->GetValue();
    data.birth_location.country_code = land_ctrl_->GetValue().ToStdString();
    
    // Julianisches Datum berechnen
    data.julian_date = astro::JulianDate(data.birth_datetime);
    
    return data;
}

void PersonDialog::SetRadixData(const astro::RadixData& data) {
    // Name aufteilen (vereinfacht)
    wxString full_name = wxString::FromUTF8(data.name.c_str());
    wxString vorname = full_name.BeforeFirst(' ');
    wxString nachname = full_name.AfterFirst(' ');
    
    vorname_ctrl_->SetValue(vorname);
    name_ctrl_->SetValue(nachname);
    
    // Datum & Zeit
    wxDateTime datum;
    datum.Set(data.birth_datetime.day, static_cast<wxDateTime::Month>(data.birth_datetime.month - 1), data.birth_datetime.year);
    datum_ctrl_->SetValue(datum);
    
    wxDateTime zeit;
    zeit.Set(data.birth_datetime.hour, data.birth_datetime.minute, static_cast<int>(data.birth_datetime.second));
    zeit_ctrl_->SetValue(zeit);
    
    // Ort
    land_ctrl_->SetValue(wxString::FromUTF8(data.birth_location.country_code.c_str()));
    breite_ctrl_->SetValue(data.birth_location.latitude);
    laenge_ctrl_->SetValue(data.birth_location.longitude);
    hoehe_ctrl_->SetValue(static_cast<int>(data.birth_location.altitude));
}
