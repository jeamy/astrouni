#include "einst_dialog.h"
#include <wx/gbsizer.h>

// 1:1 Port von EINSTELL DIALOG aus astrouni.rc
EinstDialog::EinstDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Einstellungen", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE) {

    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    wxGridBagSizer* gbs = new wxGridBagSizer(5, 5);

    // Aspekte
    wxStaticBoxSizer* aspekte_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "Aspekte");
    aspekte_sizer->Add(new wxCheckBox(this, wxID_ANY, "Konjunktion"));
    aspekte_sizer->Add(new wxCheckBox(this, wxID_ANY, "Halbsextil"));
    aspekte_sizer->Add(new wxCheckBox(this, wxID_ANY, "Sextil"));
    aspekte_sizer->Add(new wxCheckBox(this, wxID_ANY, "Trigon"));
    aspekte_sizer->Add(new wxCheckBox(this, wxID_ANY, "Quadratur"));
    aspekte_sizer->Add(new wxCheckBox(this, wxID_ANY, "Quincunx"));
    aspekte_sizer->Add(new wxCheckBox(this, wxID_ANY, "Oposition"));
    gbs->Add(aspekte_sizer, wxGBPosition(0, 0), wxGBSpan(2, 1), wxEXPAND);

    // Aspekte für
    wxStaticBoxSizer* aspekte_fuer_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "Aspekte für:");
    aspekte_fuer_sizer->Add(new wxCheckBox(this, wxID_ANY, "Planeten"));
    aspekte_fuer_sizer->Add(new wxCheckBox(this, wxID_ANY, "Asteroiden"));
    aspekte_fuer_sizer->Add(new wxCheckBox(this, wxID_ANY, "Häuser"));
    gbs->Add(aspekte_fuer_sizer, wxGBPosition(0, 1));

    // Teilungen
    wxStaticBoxSizer* teilungen_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "Teilungen:");
    teilungen_sizer->Add(new wxCheckBox(this, wxID_ANY, "3er"));
    teilungen_sizer->Add(new wxCheckBox(this, wxID_ANY, "9er"));
    gbs->Add(teilungen_sizer, wxGBPosition(1, 1));

    main_sizer->Add(gbs, 1, wxEXPAND | wxALL, 10);

    wxStdDialogButtonSizer* button_sizer = new wxStdDialogButtonSizer();
    button_sizer->AddButton(new wxButton(this, wxID_OK, "OK"));
    button_sizer->AddButton(new wxButton(this, wxID_CANCEL, "Abbruch"));
    button_sizer->AddButton(new wxButton(this, wxID_HELP, "Hilfe"));
    button_sizer->Realize();
    main_sizer->Add(button_sizer, 0, wxEXPAND | wxALL, 5);

    SetSizerAndFit(main_sizer);
    Centre();
}

void EinstDialog::OnOk(wxCommandEvent& event) {
    EndModal(wxID_OK);
}
