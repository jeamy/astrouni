// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgDDProc from astrofil.c

#include "datum_dialog.h"
#include "astro/i18n.h"
#include <wx/statbox.h>
#include <wx/sizer.h>
#include <wx/datetime.h>

using namespace astro::i18n;

// Event Table (1:1 Legacy)
wxBEGIN_EVENT_TABLE(DatumDialog, wxDialog)
    EVT_BUTTON(wxID_OK, DatumDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, DatumDialog::OnCancel)
    EVT_BUTTON(ID_HEUTE_BTN, DatumDialog::OnHeute)
    EVT_SPINCTRLDOUBLE(ID_JULIAN_CTRL, DatumDialog::OnJulianChange)
wxEND_EVENT_TABLE()

DatumDialog::DatumDialog(wxWindow* parent, const wxString& title)
    : wxDialog(parent, wxID_ANY, title, 
               wxDefaultPosition, wxSize(400, 350),
               wxDEFAULT_DIALOG_STYLE) {
    
    CreateLayout();
    InitializeControls();
    Centre();
}

void DatumDialog::CreateLayout() {
    // Haupt-Sizer
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Gregorianisches Datum (1:1 Legacy)
    wxStaticBoxSizer* greg_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Gregorianisches Datum"));
    
    // Datum
    wxBoxSizer* datum_sizer = new wxBoxSizer(wxHORIZONTAL);
    datum_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Datum:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    datum_ctrl_ = new wxDatePickerCtrl(this, ID_DATUM_PICKER);
    datum_sizer->Add(datum_ctrl_, 1, wxEXPAND);
    greg_box->Add(datum_sizer, 0, wxEXPAND | wxALL, 5);
    
    // Zeit
    wxBoxSizer* zeit_sizer = new wxBoxSizer(wxHORIZONTAL);
    zeit_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Zeit:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    zeit_ctrl_ = new wxTimePickerCtrl(this, ID_ZEIT_PICKER);
    zeit_sizer->Add(zeit_ctrl_, 0);
    
    // Heute-Button
    heute_btn_ = new wxButton(this, ID_HEUTE_BTN, wxString::FromUTF8("Heute"));
    zeit_sizer->Add(heute_btn_, 0, wxLEFT, 10);
    
    greg_box->Add(zeit_sizer, 0, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(greg_box, 0, wxEXPAND | wxALL, 10);
    
    // Julianisches Datum (1:1 Legacy)
    wxStaticBoxSizer* julian_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Julianisches Datum"));
    
    wxBoxSizer* julian_sizer = new wxBoxSizer(wxHORIZONTAL);
    julian_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("JD:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    julian_ctrl_ = new wxSpinCtrlDouble(this, ID_JULIAN_CTRL, "", wxDefaultPosition, wxSize(150, -1));
    julian_ctrl_->SetRange(0.0, 5000000.0);
    julian_ctrl_->SetDigits(6);
    julian_ctrl_->SetIncrement(1.0);
    julian_sizer->Add(julian_ctrl_, 1, wxEXPAND);
    julian_box->Add(julian_sizer, 0, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(julian_box, 0, wxEXPAND | wxALL, 10);
    
    // Zusatz-Informationen (1:1 Legacy)
    wxStaticBoxSizer* info_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Informationen"));
    
    wochentag_text_ = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Wochentag: -"));
    kalenderwoche_text_ = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Kalenderwoche: -"));
    tag_im_jahr_text_ = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Tag im Jahr: -"));
    
    info_box->Add(wochentag_text_, 0, wxEXPAND | wxALL, 3);
    info_box->Add(kalenderwoche_text_, 0, wxEXPAND | wxALL, 3);
    info_box->Add(tag_im_jahr_text_, 0, wxEXPAND | wxALL, 3);
    
    main_sizer->Add(info_box, 0, wxEXPAND | wxALL, 10);
    
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

void DatumDialog::InitializeControls() {
    // Aktuelles Datum/Zeit setzen (1:1 Legacy)
    wxDateTime jetzt = wxDateTime::Now();
    datum_ctrl_->SetValue(jetzt);
    zeit_ctrl_->SetValue(jetzt);
    
    UpdateAnzeige();
}

void DatumDialog::UpdateAnzeige() {
    wxDateTime datum = datum_ctrl_->GetValue();
    wxDateTime zeit = zeit_ctrl_->GetValue();
    
    // Kombiniertes Datum/Zeit
    wxDateTime kombiniert = datum;
    kombiniert.SetHour(zeit.GetHour());
    kombiniert.SetMinute(zeit.GetMinute());
    kombiniert.SetSecond(zeit.GetSecond());
    
    // Wochentag
    wxString wochentage[] = {
        wxString::FromUTF8("Sonntag"), wxString::FromUTF8("Montag"), wxString::FromUTF8("Dienstag"),
        wxString::FromUTF8("Mittwoch"), wxString::FromUTF8("Donnerstag"), wxString::FromUTF8("Freitag"),
        wxString::FromUTF8("Samstag")
    };
    wochentag_text_->SetLabel(wxString::Format(wxString::FromUTF8("Wochentag: %s"), 
                                              wochentage[kombiniert.GetWeekDay()]));
    
    // Kalenderwoche
    kalenderwoche_text_->SetLabel(wxString::Format(wxString::FromUTF8("Kalenderwoche: %d"), 
                                                   kombiniert.GetWeekOfYear()));
    
    // Tag im Jahr
    tag_im_jahr_text_->SetLabel(wxString::Format(wxString::FromUTF8("Tag im Jahr: %d"), 
                                                 kombiniert.GetDayOfYear()));
    
    // Julianisches Datum aktualisieren
    UpdateJulianFromDatum();
}

void DatumDialog::UpdateJulianFromDatum() {
    astro::DateTime dt = GetDateTime();
    astro::JulianDate jd(dt);
    julian_ctrl_->SetValue(jd.to_julian());
}

void DatumDialog::UpdateDatumFromJulian() {
    // TODO: JulianDate zu DateTime Konvertierung implementieren
    // Für jetzt nur Info-Text aktualisieren
    UpdateAnzeige();
}

// Event Handlers (1:1 Legacy)
void DatumDialog::OnOK(wxCommandEvent& event) {
    EndModal(wxID_OK);
}

void DatumDialog::OnCancel(wxCommandEvent& event) {
    EndModal(wxID_CANCEL);
}

void DatumDialog::OnHeute(wxCommandEvent& event) {
    wxDateTime jetzt = wxDateTime::Now();
    datum_ctrl_->SetValue(jetzt);
    zeit_ctrl_->SetValue(jetzt);
    UpdateAnzeige();
}

void DatumDialog::OnJulianChange(wxSpinDoubleEvent& event) {
    UpdateDatumFromJulian();
}

void DatumDialog::OnDatumChange(wxCommandEvent& event) {
    UpdateAnzeige();
}

astro::DateTime DatumDialog::GetDateTime() const {
    astro::DateTime dt;
    
    wxDateTime datum = datum_ctrl_->GetValue();
    wxDateTime zeit = zeit_ctrl_->GetValue();
    
    dt.year = datum.GetYear();
    dt.month = datum.GetMonth() + 1;  // wxDateTime: 0-11, astro: 1-12
    dt.day = datum.GetDay();
    dt.hour = zeit.GetHour();
    dt.minute = zeit.GetMinute();
    dt.second = zeit.GetSecond();
    
    return dt;
}

void DatumDialog::SetDateTime(const astro::DateTime& datetime) {
    // Datum setzen
    wxDateTime datum;
    datum.Set(datetime.day, static_cast<wxDateTime::Month>(datetime.month - 1), datetime.year);
    datum_ctrl_->SetValue(datum);
    
    // Zeit setzen
    wxDateTime zeit;
    zeit.Set(datetime.hour, datetime.minute, static_cast<int>(datetime.second));
    zeit_ctrl_->SetValue(zeit);
    
    UpdateAnzeige();
}

double DatumDialog::GetJulianDate() const {
    return julian_ctrl_->GetValue();
}

void DatumDialog::SetJulianDate(double jd) {
    julian_ctrl_->SetValue(jd);
    UpdateDatumFromJulian();
}
