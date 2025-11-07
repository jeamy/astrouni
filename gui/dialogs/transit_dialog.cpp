// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgTransit from astrofil.c

#include "transit_dialog.h"
#include "astro/i18n.h"
#include <wx/statbox.h>
#include <wx/sizer.h>
#include <wx/datetime.h>

using namespace astro::i18n;

// Event Table (1:1 Legacy)
wxBEGIN_EVENT_TABLE(TransitDialog, wxDialog)
    EVT_BUTTON(wxID_OK, TransitDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, TransitDialog::OnCancel)
    EVT_BUTTON(ID_BERECHNEN, TransitDialog::OnBerechnen)
    EVT_BUTTON(ID_PERSON_AUSWAHL, TransitDialog::OnPersonAuswahl)
    EVT_SPINCTRL(ID_INTERVALL, TransitDialog::OnSpinChange)
    EVT_CHECKBOX(ID_ALLE_PLANETEN, TransitDialog::OnZeitraumChange)
    EVT_CHECKBOX(ID_ASPEKTE, TransitDialog::OnZeitraumChange)
    EVT_CHECKBOX(ID_INGRESS, TransitDialog::OnZeitraumChange)
    EVT_CHECKBOX(ID_EXAKTE_ZEITEN, TransitDialog::OnZeitraumChange)
wxEND_EVENT_TABLE()

TransitDialog::TransitDialog(wxWindow* parent, LegacyDataManager* data_manager)
    : wxDialog(parent, wxID_ANY, wxString::FromUTF8("Transit Berechnung"), 
               wxDefaultPosition, wxSize(500, 600),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
      data_manager_(data_manager),
      person_selected_(false) {
    
    CreateLayout();
    InitializeControls();
    Centre();
}

void TransitDialog::CreateLayout() {
    // Haupt-Sizer
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Person-Bereich (1:1 Legacy)
    wxStaticBoxSizer* person_box = new wxStaticBoxSizer(wxHORIZONTAL, this, wxString::FromUTF8("Person"));
    
    person_text_ = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Keine Person ausgewählt"));
    person_btn_ = new wxButton(this, ID_PERSON_AUSWAHL, wxString::FromUTF8("Person auswählen..."));
    
    person_box->Add(person_text_, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    person_box->Add(person_btn_, 0);
    
    main_sizer->Add(person_box, 0, wxEXPAND | wxALL, 10);
    
    // Zeitraum-Bereich (1:1 Legacy)
    wxStaticBoxSizer* zeit_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Transit-Zeitraum"));
    
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
    
    // Intervall
    wxBoxSizer* intervall_sizer = new wxBoxSizer(wxHORIZONTAL);
    intervall_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Intervall:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    intervall_ctrl_ = new wxSpinCtrl(this, ID_INTERVALL, "", wxDefaultPosition, wxSize(80, -1));
    intervall_ctrl_->SetRange(1, 365);
    intervall_ctrl_->SetValue(1);
    intervall_text_ = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Tag(e)"));
    intervall_sizer->Add(intervall_ctrl_, 0, wxRIGHT, 5);
    intervall_sizer->Add(intervall_text_, 0, wxALIGN_CENTER_VERTICAL);
    zeit_box->Add(intervall_sizer, 0, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(zeit_box, 0, wxEXPAND | wxALL, 10);
    
    // Optionen-Bereich (1:1 Legacy)
    wxStaticBoxSizer* optionen_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Transit-Optionen"));
    
    alle_planeten_ctrl_ = new wxCheckBox(this, ID_ALLE_PLANETEN, wxString::FromUTF8("Alle Planeten (inkl. äußere Planeten)"));
    aspekte_ctrl_ = new wxCheckBox(this, ID_ASPEKTE, wxString::FromUTF8("Aspekte zu Radix-Planeten"));
    ingress_ctrl_ = new wxCheckBox(this, ID_INGRESS, wxString::FromUTF8("Zeichen-Eintritte (Ingress)"));
    exakte_zeiten_ctrl_ = new wxCheckBox(this, ID_EXAKTE_ZEITEN, wxString::FromUTF8("Exakte Zeiten berechnen"));
    
    optionen_box->Add(alle_planeten_ctrl_, 0, wxEXPAND | wxALL, 3);
    optionen_box->Add(aspekte_ctrl_, 0, wxEXPAND | wxALL, 3);
    optionen_box->Add(ingress_ctrl_, 0, wxEXPAND | wxALL, 3);
    optionen_box->Add(exakte_zeiten_ctrl_, 0, wxEXPAND | wxALL, 3);
    
    main_sizer->Add(optionen_box, 0, wxEXPAND | wxALL, 10);
    
    // Vorschau-Bereich (1:1 Legacy)
    wxStaticBoxSizer* vorschau_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Vorschau"));
    
    vorschau_text_ = new wxStaticText(this, wxID_ANY, wxString::FromUTF8(""), 
                                     wxDefaultPosition, wxSize(-1, 100));
    vorschau_text_->Wrap(450);
    
    vorschau_box->Add(vorschau_text_, 1, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(vorschau_box, 1, wxEXPAND | wxALL, 10);
    
    // Dialog-Buttons (1:1 Legacy)
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

void TransitDialog::InitializeControls() {
    // Standard-Zeitraum: Heute bis in 30 Tagen (1:1 Legacy)
    wxDateTime heute = wxDateTime::Now();
    wxDateTime in_30_tagen = heute;
    in_30_tagen.Add(wxDateSpan::Days(30));
    
    start_datum_ctrl_->SetValue(heute);
    start_zeit_ctrl_->SetValue(heute);
    end_datum_ctrl_->SetValue(in_30_tagen);
    end_zeit_ctrl_->SetValue(heute);
    
    // Standard-Optionen (1:1 Legacy)
    alle_planeten_ctrl_->SetValue(true);
    aspekte_ctrl_->SetValue(true);
    ingress_ctrl_->SetValue(false);
    exakte_zeiten_ctrl_->SetValue(true);
    
    // Berechnen-Button erst nach Person-Auswahl aktivieren
    berechnen_btn_->Enable(false);
    ok_btn_->Enable(false);
    
    UpdateVorschau();
}

void TransitDialog::UpdateVorschau() {
    wxString vorschau;
    
    if (!person_selected_) {
        vorschau = wxString::FromUTF8("Bitte wählen Sie zuerst eine Person aus.");
    } else {
        // Zeitraum berechnen
        wxDateTime start = start_datum_ctrl_->GetValue();
        wxDateTime end = end_datum_ctrl_->GetValue();
        wxTimeSpan dauer = end.Subtract(start);
        int tage = dauer.GetDays();
        
        vorschau = wxString::Format(
            wxString::FromUTF8("Transit-Berechnung für: %s %s\n\n"
                              "Zeitraum: %s bis %s\n"
                              "Dauer: %d Tage\n"
                              "Intervall: %d Tag(e)\n"
                              "Berechnungen: ca. %d\n\n"
                              "Optionen:\n"
                              "• Alle Planeten: %s\n"
                              "• Aspekte: %s\n"
                              "• Zeichen-Eintritte: %s\n"
                              "• Exakte Zeiten: %s"),
            wxString::FromUTF8(current_person_.first_name.c_str()),
            wxString::FromUTF8(current_person_.name.c_str()),
            start.Format("%d.%m.%Y %H:%M"),
            end.Format("%d.%m.%Y %H:%M"),
            tage,
            intervall_ctrl_->GetValue(),
            tage / intervall_ctrl_->GetValue(),
            alle_planeten_ctrl_->GetValue() ? "Ja" : "Nein",
            aspekte_ctrl_->GetValue() ? "Ja" : "Nein",
            ingress_ctrl_->GetValue() ? "Ja" : "Nein",
            exakte_zeiten_ctrl_->GetValue() ? "Ja" : "Nein"
        );
    }
    
    vorschau_text_->SetLabel(vorschau);
    vorschau_text_->Wrap(450);
}

void TransitDialog::UpdateIntervallText() {
    int intervall = intervall_ctrl_->GetValue();
    if (intervall == 1) {
        intervall_text_->SetLabel(wxString::FromUTF8("Tag"));
    } else {
        intervall_text_->SetLabel(wxString::FromUTF8("Tage"));
    }
}

// Event Handlers (1:1 Legacy)
void TransitDialog::OnOK(wxCommandEvent& event) {
    if (!person_selected_) {
        wxMessageBox(wxString::FromUTF8("Bitte wählen Sie eine Person aus!"), 
                    wxString::FromUTF8("Keine Person"), wxOK | wxICON_WARNING);
        return;
    }
    
    EndModal(wxID_OK);
}

void TransitDialog::OnCancel(wxCommandEvent& event) {
    EndModal(wxID_CANCEL);
}

void TransitDialog::OnBerechnen(wxCommandEvent& event) {
    if (!person_selected_) {
        wxMessageBox(wxString::FromUTF8("Bitte wählen Sie zuerst eine Person aus!"), 
                    wxString::FromUTF8("Keine Person"), wxOK | wxICON_WARNING);
        return;
    }
    
    // TODO: Transit-Berechnung implementieren
    wxMessageBox(wxString::FromUTF8("Transit-Berechnung wird implementiert..."), 
                wxString::FromUTF8("Info"), wxOK | wxICON_INFORMATION);
}

void TransitDialog::OnPersonAuswahl(wxCommandEvent& event) {
    // TODO: PersonAuswahlDialog implementieren
    wxMessageBox(wxString::FromUTF8("Person-Auswahl wird implementiert..."), 
                wxString::FromUTF8("Info"), wxOK | wxICON_INFORMATION);
    
    // Dummy-Person für Test
    current_person_.name = "Mustermann";
    current_person_.first_name = "Max";
    person_selected_ = true;
    
    person_text_->SetLabel(wxString::Format(
        wxString::FromUTF8("%s %s"),
        wxString::FromUTF8(current_person_.first_name.c_str()),
        wxString::FromUTF8(current_person_.name.c_str())
    ));
    
    berechnen_btn_->Enable(true);
    ok_btn_->Enable(true);
    
    UpdateVorschau();
}

void TransitDialog::OnZeitraumChange(wxCommandEvent& event) {
    UpdateVorschau();
}

void TransitDialog::OnSpinChange(wxSpinEvent& event) {
    UpdateIntervallText();
    UpdateVorschau();
}

TransitDialog::TransitData TransitDialog::GetTransitData() const {
    TransitData data;
    
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
    
    // Optionen
    data.interval_days = intervall_ctrl_->GetValue();
    data.show_all_planets = alle_planeten_ctrl_->GetValue();
    data.show_aspects = aspekte_ctrl_->GetValue();
    data.show_ingress = ingress_ctrl_->GetValue();
    data.show_exact_times = exakte_zeiten_ctrl_->GetValue();
    
    return data;
}

void TransitDialog::SetTransitData(const TransitData& data) {
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
    
    // Optionen setzen
    intervall_ctrl_->SetValue(data.interval_days);
    alle_planeten_ctrl_->SetValue(data.show_all_planets);
    aspekte_ctrl_->SetValue(data.show_aspects);
    ingress_ctrl_->SetValue(data.show_ingress);
    exakte_zeiten_ctrl_->SetValue(data.show_exact_times);
    
    UpdateVorschau();
}
