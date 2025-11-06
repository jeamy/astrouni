// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project

#include "new_chart_dialog.h"
#include "astro/i18n.h"
#include <wx/statline.h>

using namespace astro::i18n;

wxBEGIN_EVENT_TABLE(NewChartDialog, wxDialog)
    EVT_BUTTON(wxID_OK, NewChartDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, NewChartDialog::OnCancel)
wxEND_EVENT_TABLE()

NewChartDialog::NewChartDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, tr("dialog.new_chart"), wxDefaultPosition, wxSize(550, 550)) {
    
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Scrolled Window für den Inhalt
    wxScrolledWindow* scroll = new wxScrolledWindow(this, wxID_ANY);
    scroll->SetScrollRate(5, 5);
    wxBoxSizer* scroll_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Person
    wxStaticBoxSizer* person_box = new wxStaticBoxSizer(wxVERTICAL, scroll, tr("dialog.person"));
    wxFlexGridSizer* person_grid = new wxFlexGridSizer(2, 10, 10);
    person_grid->AddGrowableCol(1);
    
    person_grid->Add(new wxStaticText(scroll, wxID_ANY, tr("dialog.name")), 0, wxALIGN_CENTER_VERTICAL);
    name_ctrl_ = new wxTextCtrl(scroll, wxID_ANY, "Beispiel Person");
    person_grid->Add(name_ctrl_, 1, wxEXPAND);
    
    person_box->Add(person_grid, 0, wxEXPAND | wxALL, 10);
    scroll_sizer->Add(person_box, 0, wxEXPAND | wxALL, 10);
    
    // Datum & Zeit
    wxStaticBoxSizer* datetime_box = new wxStaticBoxSizer(wxVERTICAL, scroll, tr("dialog.birth_date"));
    wxFlexGridSizer* datetime_grid = new wxFlexGridSizer(2, 10, 10);
    datetime_grid->AddGrowableCol(1);
    
    datetime_grid->Add(new wxStaticText(scroll, wxID_ANY, tr("dialog.date")), 0, wxALIGN_CENTER_VERTICAL);
    date_ctrl_ = new wxDatePickerCtrl(scroll, wxID_ANY);
    datetime_grid->Add(date_ctrl_, 1, wxEXPAND);
    
    datetime_grid->Add(new wxStaticText(scroll, wxID_ANY, tr("dialog.time_ut")), 0, wxALIGN_CENTER_VERTICAL);
    time_ctrl_ = new wxTimePickerCtrl(scroll, wxID_ANY);
    datetime_grid->Add(time_ctrl_, 1, wxEXPAND);
    
    datetime_box->Add(datetime_grid, 0, wxEXPAND | wxALL, 10);
    scroll_sizer->Add(datetime_box, 0, wxEXPAND | wxALL, 10);
    
    // Ort
    wxStaticBoxSizer* location_box = new wxStaticBoxSizer(wxVERTICAL, scroll, tr("dialog.birth_place"));
    wxFlexGridSizer* location_grid = new wxFlexGridSizer(2, 10, 10);
    location_grid->AddGrowableCol(1);
    
    location_grid->Add(new wxStaticText(scroll, wxID_ANY, tr("dialog.city")), 0, wxALIGN_CENTER_VERTICAL);
    city_ctrl_ = new wxTextCtrl(scroll, wxID_ANY, "Berlin");
    location_grid->Add(city_ctrl_, 1, wxEXPAND);
    
    location_grid->Add(new wxStaticText(scroll, wxID_ANY, tr("dialog.latitude")), 0, wxALIGN_CENTER_VERTICAL);
    latitude_ctrl_ = new wxTextCtrl(scroll, wxID_ANY, "52.52");
    location_grid->Add(latitude_ctrl_, 1, wxEXPAND);
    
    location_grid->Add(new wxStaticText(scroll, wxID_ANY, tr("dialog.longitude")), 0, wxALIGN_CENTER_VERTICAL);
    longitude_ctrl_ = new wxTextCtrl(scroll, wxID_ANY, "13.40");
    location_grid->Add(longitude_ctrl_, 1, wxEXPAND);
    
    location_grid->Add(new wxStaticText(scroll, wxID_ANY, tr("dialog.altitude")), 0, wxALIGN_CENTER_VERTICAL);
    altitude_ctrl_ = new wxTextCtrl(scroll, wxID_ANY, "34");
    location_grid->Add(altitude_ctrl_, 1, wxEXPAND);
    
    location_box->Add(location_grid, 0, wxEXPAND | wxALL, 10);
    scroll_sizer->Add(location_box, 0, wxEXPAND | wxALL, 10);
    
    scroll->SetSizer(scroll_sizer);
    scroll->FitInside();
    
    main_sizer->Add(scroll, 1, wxEXPAND);
    
    // Buttons (außerhalb des Scroll-Bereichs)
    main_sizer->AddSpacer(10);
    
    wxStdDialogButtonSizer* button_sizer = new wxStdDialogButtonSizer();
    
    wxButton* ok_btn = new wxButton(this, wxID_OK);
    ok_btn->SetLabel(tr("dialog.calculate"));
    ok_btn->SetDefault();
    button_sizer->AddButton(ok_btn);
    
    wxButton* cancel_btn = new wxButton(this, wxID_CANCEL);
    cancel_btn->SetLabel(tr("dialog.cancel"));
    button_sizer->AddButton(cancel_btn);
    
    button_sizer->Realize();
    main_sizer->Add(button_sizer, 0, wxALIGN_RIGHT | wxALL, 15);
    
    SetSizer(main_sizer);
    Centre();
}

void NewChartDialog::OnOK(wxCommandEvent& event) {
    // Validierung
    if (name_ctrl_->GetValue().IsEmpty()) {
        wxString msg = I18n::instance().get_language() == Language::German ?
            "Bitte geben Sie einen Namen ein." : "Please enter a name.";
        wxString title = I18n::instance().get_language() == Language::German ?
            "Fehler" : "Error";
        wxMessageBox(msg, title, wxOK | wxICON_ERROR);
        return;
    }
    
    EndModal(wxID_OK);
}

void NewChartDialog::OnCancel(wxCommandEvent& event) {
    EndModal(wxID_CANCEL);
}

astro::RadixData NewChartDialog::GetRadixData() const {
    astro::RadixData radix;
    
    // Name
    radix.name = name_ctrl_->GetValue().ToStdString();
    
    // Datum
    wxDateTime date = date_ctrl_->GetValue();
    wxDateTime time = time_ctrl_->GetValue();
    
    radix.birth_datetime.year = date.GetYear();
    radix.birth_datetime.month = date.GetMonth() + 1;  // wxDateTime: 0-11, wir: 1-12
    radix.birth_datetime.day = date.GetDay();
    radix.birth_datetime.hour = time.GetHour();
    radix.birth_datetime.minute = time.GetMinute();
    radix.birth_datetime.second = time.GetSecond();
    
    // Ort
    double lat, lon, alt;
    latitude_ctrl_->GetValue().ToDouble(&lat);
    longitude_ctrl_->GetValue().ToDouble(&lon);
    altitude_ctrl_->GetValue().ToDouble(&alt);
    
    radix.birth_location.latitude = lat;
    radix.birth_location.longitude = lon;
    radix.birth_location.altitude = alt;
    
    // Julianisches Datum berechnen
    radix.julian_date = astro::JulianDate(radix.birth_datetime);
    
    return radix;
}
