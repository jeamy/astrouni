#include "farben_dialog.h"
#include <wx/gbsizer.h>
#include <wx/clrpicker.h>

// 1:1 Port von DlgFarben aus astrouni.rc
FarbenDialog::FarbenDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Farben", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) {
    SetBackgroundColour(*wxWHITE);

    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    wxGridBagSizer* gbs = new wxGridBagSizer(5, 5);

    const char* labels[] = {
        "Hintergrund", "Radix", "Asc", "Häuser", "Planeten", "T-Planeten",
        "Markierung", "T-Markierung", "Feuer", "Erde", "Luft", "Wasser"
    };

    for (int i = 0; i < 12; ++i) {
        gbs->Add(new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(14, 10)), wxGBPosition(i, 0));
        gbs->Add(new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(14, 10)), wxGBPosition(i, 1));
        gbs->Add(new wxStaticText(this, wxID_ANY, labels[i]), wxGBPosition(i, 2));
    }

    wxGridSizer* rgb_grid = new wxGridSizer(3, 2, 5, 5);
    rgb_grid->Add(new wxTextCtrl(this, wxID_ANY, "255", wxDefaultPosition, wxSize(35, -1)));
    rgb_grid->Add(new wxScrollBar(this, wxID_ANY, wxDefaultPosition, wxSize(100, -1), wxSB_HORIZONTAL));
    rgb_grid->Add(new wxTextCtrl(this, wxID_ANY, "255", wxDefaultPosition, wxSize(35, -1)));
    rgb_grid->Add(new wxScrollBar(this, wxID_ANY, wxDefaultPosition, wxSize(100, -1), wxSB_HORIZONTAL));
    rgb_grid->Add(new wxTextCtrl(this, wxID_ANY, "255", wxDefaultPosition, wxSize(35, -1)));
    rgb_grid->Add(new wxScrollBar(this, wxID_ANY, wxDefaultPosition, wxSize(100, -1), wxSB_HORIZONTAL));
    gbs->Add(rgb_grid, wxGBPosition(12, 0), wxGBSpan(1, 3), wxEXPAND | wxTOP, 10);

    wxBoxSizer* mono_sizer = new wxBoxSizer(wxHORIZONTAL);
    mono_sizer->Add(new wxCheckBox(this, wxID_ANY, "Mono"), 0, wxRIGHT, 5);
    mono_sizer->Add(new wxCheckBox(this, wxID_ANY, "Prn"));
    gbs->Add(mono_sizer, wxGBPosition(13, 2), wxGBSpan(1, 1), wxALIGN_RIGHT);

    main_sizer->Add(gbs, 1, wxEXPAND | wxALL, 10);

    wxStdDialogButtonSizer* button_sizer = new wxStdDialogButtonSizer();
    button_sizer->AddButton(new wxButton(this, wxID_OK, "OK"));
    button_sizer->AddButton(new wxButton(this, wxID_ANY, "Default"));
    button_sizer->AddButton(new wxButton(this, wxID_CANCEL, "Cancel"));
    button_sizer->AddButton(new wxButton(this, wxID_HELP, "Hilfe"));
    button_sizer->Realize();
    main_sizer->Add(button_sizer, 0, wxEXPAND | wxALL, 5);

    SetSizerAndFit(main_sizer);
    Centre();
}

void FarbenDialog::OnOk(wxCommandEvent& event) {
    EndModal(wxID_OK);
}
