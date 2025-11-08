#include "ort_dialog.h"

// 1:1 Port von DlgOErfassen aus astrofil.c
OrtDialog::OrtDialog(wxWindow* parent, astro::LegacyOrte& ort_data)
    : wxDialog(parent, wxID_ANY, "Ort Erfassen/Bearbeiten"), ort_data_(ort_data) {

    wxPanel* panel = new wxPanel(this, wxID_ANY);

    // UI-Layout erstellen (vereinfacht)
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(panel, wxID_ANY, "Ort:"), 0, wxALL, 5);
    ort_ctrl_ = new wxTextCtrl(panel, wxID_ANY, ort_data_.szOrt);
    sizer->Add(ort_ctrl_, 0, wxEXPAND | wxALL, 5);

    sizer->Add(new wxStaticText(panel, wxID_ANY, "Staat:"), 0, wxALL, 5);
    staat_ctrl_ = new wxTextCtrl(panel, wxID_ANY, ort_data_.szStaat);
    sizer->Add(staat_ctrl_, 0, wxEXPAND | wxALL, 5);

    // OK/Abbrechen Buttons
    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* ok_button = new wxButton(panel, wxID_OK, "OK");
    wxButton* cancel_button = new wxButton(panel, wxID_CANCEL, "Abbrechen");
    button_sizer->Add(ok_button, 0, wxALL, 5);
    button_sizer->Add(cancel_button, 0, wxALL, 5);
    sizer->Add(button_sizer, 0, wxALIGN_CENTER | wxALL, 5);

    panel->SetSizerAndFit(sizer);
    Centre();

    // Event-Handler
    Bind(wxEVT_BUTTON, &OrtDialog::OnOk, this, wxID_OK);
}

void OrtDialog::OnOk(wxCommandEvent& event) {
    // Daten aus den UI-Elementen in ort_data_ speichern
    strncpy(ort_data_.szOrt, ort_ctrl_->GetValue().ToStdString().c_str(), 30);
    strncpy(ort_data_.szStaat, staat_ctrl_->GetValue().ToStdString().c_str(), 30);

    EndModal(wxID_OK);
}
