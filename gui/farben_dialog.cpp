#include "farben_dialog.h"

// 1:1 Port von DlgFarben aus astrofil.c
FarbenDialog::FarbenDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Farb-Einstellungen") {

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Hier wird der Inhalt des Dialogs eingefügt
    sizer->Add(new wxStaticText(this, wxID_ANY, "Farb-Einstellungen (noch nicht implementiert)"), 1, wxALL | wxEXPAND, 10);

    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    button_sizer->Add(new wxButton(this, wxID_OK, "OK"), 0, wxALL, 5);
    button_sizer->Add(new wxButton(this, wxID_CANCEL, "Abbrechen"), 0, wxALL, 5);
    sizer->Add(button_sizer, 0, wxALIGN_CENTER | wxALL, 5);

    SetSizerAndFit(sizer);
    Centre();

    Bind(wxEVT_BUTTON, &FarbenDialog::OnOk, this, wxID_OK);
}

void FarbenDialog::OnOk(wxCommandEvent& event) {
    EndModal(wxID_OK);
}
