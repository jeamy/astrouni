#include "person_dialog.h"

// 1:1 Port von DlgPErfassen aus astrofil.c
PersonDialog::PersonDialog(wxWindow* parent, astro::LegacyRadix& radix_data)
    : wxDialog(parent, wxID_ANY, "Person Erfassen/Bearbeiten"), radix_data_(radix_data) {

    wxPanel* panel = new wxPanel(this, wxID_ANY);

    // UI-Layout erstellen (vereinfacht)
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(panel, wxID_ANY, "Name:"), 0, wxALL, 5);
    name_ctrl_ = new wxTextCtrl(panel, wxID_ANY, radix_data_.szName);
    sizer->Add(name_ctrl_, 0, wxEXPAND | wxALL, 5);

    sizer->Add(new wxStaticText(panel, wxID_ANY, "Vorname:"), 0, wxALL, 5);
    vorname_ctrl_ = new wxTextCtrl(panel, wxID_ANY, radix_data_.szVorName);
    sizer->Add(vorname_ctrl_, 0, wxEXPAND | wxALL, 5);

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
    Bind(wxEVT_BUTTON, &PersonDialog::OnOk, this, wxID_OK);
}

void PersonDialog::OnOk(wxCommandEvent& event) {
    // Daten aus den UI-Elementen in radix_data_ speichern
    strncpy(radix_data_.szName, name_ctrl_->GetValue().ToStdString().c_str(), 30);
    strncpy(radix_data_.szVorName, vorname_ctrl_->GetValue().ToStdString().c_str(), 30);

    EndModal(wxID_OK);
}
