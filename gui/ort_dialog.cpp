#include "ort_dialog.h"
#include <wx/gbsizer.h>

// 1:1 Port von DlgOErfassen aus astrofil.c
OrtDialog::OrtDialog(wxWindow* parent, astro::LegacyOrte& ort_data)
    : wxDialog(parent, wxID_ANY, "Ort Erfassen/Bearbeiten"), ort_data_(ort_data) {

    wxGridBagSizer* sizer = new wxGridBagSizer(5, 5);

    // Zeile 0: Ort, Staat
    sizer->Add(new wxStaticText(this, wxID_ANY, "Ort:"), wxGBPosition(0, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL, 5);
    ort_ctrl_ = new wxTextCtrl(this, wxID_ANY, ort_data_.szOrt);
    sizer->Add(ort_ctrl_, wxGBPosition(0, 1), wxGBSpan(1, 2), wxEXPAND);
    sizer->Add(new wxStaticText(this, wxID_ANY, "Staat:"), wxGBPosition(0, 3), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL, 5);
    staat_ctrl_ = new wxTextCtrl(this, wxID_ANY, ort_data_.szStaat);
    sizer->Add(staat_ctrl_, wxGBPosition(0, 4), wxGBSpan(1, 2), wxEXPAND);

    // Zeile 1: Längengrad
    sizer->Add(new wxStaticText(this, wxID_ANY, "Länge:"), wxGBPosition(1, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL, 5);
    lange_g_ctrl_ = new wxTextCtrl(this, wxID_ANY, "");
    sizer->Add(lange_g_ctrl_, wxGBPosition(1, 1), wxGBSpan(1, 1), wxEXPAND);
    lange_m_ctrl_ = new wxTextCtrl(this, wxID_ANY, "");
    sizer->Add(lange_m_ctrl_, wxGBPosition(1, 2), wxGBSpan(1, 1), wxEXPAND);
    lange_s_ctrl_ = new wxTextCtrl(this, wxID_ANY, "");
    sizer->Add(lange_s_ctrl_, wxGBPosition(1, 3), wxGBSpan(1, 1), wxEXPAND);
    wxString lange_dirs[] = {"O", "W"};
    lange_dir_ctrl_ = new wxRadioBox(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 2, lange_dirs, 1, wxRA_SPECIFY_COLS);
    sizer->Add(lange_dir_ctrl_, wxGBPosition(1, 4), wxGBSpan(1, 1), wxEXPAND);

    // Zeile 2: Breitengrad
    sizer->Add(new wxStaticText(this, wxID_ANY, "Breite:"), wxGBPosition(2, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL, 5);
    breite_g_ctrl_ = new wxTextCtrl(this, wxID_ANY, "");
    sizer->Add(breite_g_ctrl_, wxGBPosition(2, 1), wxGBSpan(1, 1), wxEXPAND);
    breite_m_ctrl_ = new wxTextCtrl(this, wxID_ANY, "");
    sizer->Add(breite_m_ctrl_, wxGBPosition(2, 2), wxGBSpan(1, 1), wxEXPAND);
    breite_s_ctrl_ = new wxTextCtrl(this, wxID_ANY, "");
    sizer->Add(breite_s_ctrl_, wxGBPosition(2, 3), wxGBSpan(1, 1), wxEXPAND);
    wxString breite_dirs[] = {"N", "S"};
    breite_dir_ctrl_ = new wxRadioBox(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 2, breite_dirs, 1, wxRA_SPECIFY_COLS);
    sizer->Add(breite_dir_ctrl_, wxGBPosition(2, 4), wxGBSpan(1, 1), wxEXPAND);

    // Zeile 3: Zeitzone
    sizer->Add(new wxStaticText(this, wxID_ANY, "Zeitzone:"), wxGBPosition(3, 0), wxGBSpan(1, 1), wxALIGN_CENTER_VERTICAL | wxALL, 5);
    zeitzone_ctrl_ = new wxTextCtrl(this, wxID_ANY, "");
    sizer->Add(zeitzone_ctrl_, wxGBPosition(3, 1), wxGBSpan(1, 1), wxEXPAND);

    // Zeile 4: Buttons
    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    button_sizer->Add(new wxButton(this, wxID_OK, "OK"), 0, wxALL, 5);
    button_sizer->Add(new wxButton(this, wxID_CANCEL, "Abbrechen"), 0, wxALL, 5);
    sizer->Add(button_sizer, wxGBPosition(5, 0), wxGBSpan(1, 5), wxALIGN_CENTER);

    this->SetSizerAndFit(sizer);
    Centre();

    // Event-Handler
    Bind(wxEVT_BUTTON, &OrtDialog::OnOk, this, wxID_OK);
}

void OrtDialog::OnOk(wxCommandEvent& event) {
    // Daten aus den UI-Elementen in ort_data_ speichern
    strncpy(ort_data_.szOrt, ort_ctrl_->GetValue().ToStdString().c_str(), 30);
    strncpy(ort_data_.szStaat, staat_ctrl_->GetValue().ToStdString().c_str(), 30);
    // ... weitere Daten speichern

    EndModal(wxID_OK);
}
