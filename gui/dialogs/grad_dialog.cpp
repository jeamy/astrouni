// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgDGProc from astrofil.c

#include "grad_dialog.h"
#include "astro/i18n.h"
#include <wx/statbox.h>
#include <wx/sizer.h>
#include <cmath>

using namespace astro::i18n;

// Event Table (1:1 Legacy)
wxBEGIN_EVENT_TABLE(GradDialog, wxDialog)
    EVT_BUTTON(wxID_OK, GradDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, GradDialog::OnCancel)
    EVT_CHECKBOX(ID_NEGATIV_CB, GradDialog::OnVorzeichenChange)
wxEND_EVENT_TABLE()

GradDialog::GradDialog(wxWindow* parent, const wxString& title)
    : wxDialog(parent, wxID_ANY, title, 
               wxDefaultPosition, wxSize(350, 300),
               wxDEFAULT_DIALOG_STYLE) {
    
    CreateLayout();
    InitializeControls();
    Centre();
}

void GradDialog::CreateLayout() {
    // Haupt-Sizer
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Grad/Minuten/Sekunden (1:1 Legacy)
    wxStaticBoxSizer* gms_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Grad/Minuten/Sekunden"));
    
    // Vorzeichen
    negativ_cb_ = new wxCheckBox(this, ID_NEGATIV_CB, wxString::FromUTF8("Negativ (-)"));
    gms_box->Add(negativ_cb_, 0, wxEXPAND | wxALL, 5);
    
    // Grad
    wxBoxSizer* grad_sizer = new wxBoxSizer(wxHORIZONTAL);
    grad_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Grad:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    grad_ctrl_ = new wxSpinCtrl(this, ID_GRAD_CTRL, "", wxDefaultPosition, wxSize(80, -1));
    grad_ctrl_->SetRange(0, 359);
    grad_sizer->Add(grad_ctrl_, 0);
    grad_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("°")), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);
    gms_box->Add(grad_sizer, 0, wxEXPAND | wxALL, 5);
    
    // Minuten
    wxBoxSizer* minuten_sizer = new wxBoxSizer(wxHORIZONTAL);
    minuten_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Minuten:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    minuten_ctrl_ = new wxSpinCtrl(this, ID_MINUTEN_CTRL, "", wxDefaultPosition, wxSize(80, -1));
    minuten_ctrl_->SetRange(0, 59);
    minuten_sizer->Add(minuten_ctrl_, 0);
    minuten_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("'")), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);
    gms_box->Add(minuten_sizer, 0, wxEXPAND | wxALL, 5);
    
    // Sekunden
    wxBoxSizer* sekunden_sizer = new wxBoxSizer(wxHORIZONTAL);
    sekunden_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Sekunden:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    sekunden_ctrl_ = new wxSpinCtrlDouble(this, ID_SEKUNDEN_CTRL, "", wxDefaultPosition, wxSize(80, -1));
    sekunden_ctrl_->SetRange(0.0, 59.999);
    sekunden_ctrl_->SetDigits(3);
    sekunden_ctrl_->SetIncrement(0.001);
    sekunden_sizer->Add(sekunden_ctrl_, 0);
    sekunden_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("\"")), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);
    gms_box->Add(sekunden_sizer, 0, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(gms_box, 0, wxEXPAND | wxALL, 10);
    
    // Dezimal-Darstellung (1:1 Legacy)
    wxStaticBoxSizer* decimal_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Dezimal-Darstellung"));
    
    wxBoxSizer* decimal_sizer = new wxBoxSizer(wxHORIZONTAL);
    decimal_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Grad:")), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    decimal_ctrl_ = new wxSpinCtrlDouble(this, ID_DECIMAL_CTRL, "", wxDefaultPosition, wxSize(120, -1));
    decimal_ctrl_->SetRange(-359.999999, 359.999999);
    decimal_ctrl_->SetDigits(6);
    decimal_ctrl_->SetIncrement(0.000001);
    decimal_sizer->Add(decimal_ctrl_, 1, wxEXPAND);
    decimal_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("°")), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);
    decimal_box->Add(decimal_sizer, 0, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(decimal_box, 0, wxEXPAND | wxALL, 10);
    
    // Informations-Text (1:1 Legacy)
    wxStaticBoxSizer* info_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Information"));
    
    info_text_ = new wxStaticText(this, wxID_ANY, wxString::FromUTF8(""));
    info_text_->Wrap(300);
    info_box->Add(info_text_, 1, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(info_box, 1, wxEXPAND | wxALL, 10);
    
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

void GradDialog::InitializeControls() {
    // Standard-Werte (1:1 Legacy)
    grad_ctrl_->SetValue(0);
    minuten_ctrl_->SetValue(0);
    sekunden_ctrl_->SetValue(0.0);
    negativ_cb_->SetValue(false);
    
    UpdateDecimalFromGMS();
    UpdateInfoText();
}

void GradDialog::UpdateDecimalFromGMS() {
    int grad = grad_ctrl_->GetValue();
    int minuten = minuten_ctrl_->GetValue();
    double sekunden = sekunden_ctrl_->GetValue();
    bool negativ = negativ_cb_->GetValue();
    
    // Dezimal-Grad berechnen
    double decimal = grad + (minuten / 60.0) + (sekunden / 3600.0);
    if (negativ) {
        decimal = -decimal;
    }
    
    decimal_ctrl_->SetValue(decimal);
    UpdateInfoText();
}

void GradDialog::UpdateGMSFromDecimal() {
    double decimal = decimal_ctrl_->GetValue();
    
    // Vorzeichen
    bool negativ = decimal < 0;
    negativ_cb_->SetValue(negativ);
    
    // Absoluter Wert
    decimal = std::abs(decimal);
    
    // Grad
    int grad = static_cast<int>(decimal);
    grad_ctrl_->SetValue(grad);
    
    // Minuten
    double rest = (decimal - grad) * 60.0;
    int minuten = static_cast<int>(rest);
    minuten_ctrl_->SetValue(minuten);
    
    // Sekunden
    double sekunden = (rest - minuten) * 60.0;
    sekunden_ctrl_->SetValue(sekunden);
    
    UpdateInfoText();
}

void GradDialog::UpdateInfoText() {
    double decimal = decimal_ctrl_->GetValue();
    
    wxString info = wxString::Format(
        wxString::FromUTF8("Dezimal: %.6f°\n\n"
                          "Entspricht:\n"
                          "• %s%d° %d' %.3f\"\n"
                          "• %.6f Bogenmaß\n"
                          "• %.2f Gon"),
        decimal,
        negativ_cb_->GetValue() ? "-" : "",
        grad_ctrl_->GetValue(),
        minuten_ctrl_->GetValue(),
        sekunden_ctrl_->GetValue(),
        decimal * M_PI / 180.0,  // Bogenmaß
        decimal * 10.0 / 9.0     // Gon
    );
    
    info_text_->SetLabel(info);
    info_text_->Wrap(300);
}

// Event Handlers (1:1 Legacy)
void GradDialog::OnOK(wxCommandEvent& event) {
    EndModal(wxID_OK);
}

void GradDialog::OnCancel(wxCommandEvent& event) {
    EndModal(wxID_CANCEL);
}

void GradDialog::OnGradChange(wxCommandEvent& event) {
    UpdateDecimalFromGMS();
}

void GradDialog::OnDecimalChange(wxCommandEvent& event) {
    UpdateGMSFromDecimal();
}

void GradDialog::OnVorzeichenChange(wxCommandEvent& event) {
    UpdateDecimalFromGMS();
}

double GradDialog::GetGradDecimal() const {
    return decimal_ctrl_->GetValue();
}

void GradDialog::SetGradDecimal(double grad) {
    decimal_ctrl_->SetValue(grad);
    UpdateGMSFromDecimal();
}

GradDialog::GradMinutenSekunden GradDialog::GetGradMinutenSekunden() const {
    GradMinutenSekunden gms;
    gms.grad = grad_ctrl_->GetValue();
    gms.minuten = minuten_ctrl_->GetValue();
    gms.sekunden = sekunden_ctrl_->GetValue();
    gms.negativ = negativ_cb_->GetValue();
    return gms;
}

void GradDialog::SetGradMinutenSekunden(const GradMinutenSekunden& gms) {
    grad_ctrl_->SetValue(gms.grad);
    minuten_ctrl_->SetValue(gms.minuten);
    sekunden_ctrl_->SetValue(gms.sekunden);
    negativ_cb_->SetValue(gms.negativ);
    UpdateDecimalFromGMS();
}
