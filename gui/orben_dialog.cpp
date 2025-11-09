#include "orben_dialog.h"
#include "symbol_panel.h"
#include <wx/gbsizer.h>
#include <wx/statline.h>

// 1:1 Port von DlgOrbenEin aus astrouni.rc
OrbenDialog::OrbenDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Orben", wxDefaultPosition, wxSize(500, 400), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) {

    // Lade die Astro-Schriftart
    wxFont::AddPrivateFont("asu_____.ttf");
    wxFont astro_font(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("asu_____"));

    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    wxGridBagSizer* gbs = new wxGridBagSizer(0, 0);

    // Symbole und Edit-Felder
    const char* planet_symbols[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K" };
    const char* aspect_symbols[] = { "a", "b", "c", "d", "e", "f", "g" };

    for (int i = 0; i < 11; ++i) {
        gbs->Add(new PlanetOrAspectSymbolPanel(this, wxID_ANY, planet_symbols[i], astro_font), wxGBPosition(0, i + 1), wxGBSpan(1, 1), wxALIGN_CENTER);
    }
    for (int i = 0; i < 7; ++i) {
        gbs->Add(new PlanetOrAspectSymbolPanel(this, wxID_ANY, aspect_symbols[i], astro_font), wxGBPosition(i + 1, 0), wxGBSpan(1, 1), wxALIGN_CENTER);
    }

    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 11; ++j) {
            gbs->Add(new wxTextCtrl(this, wxID_ANY, "10.00", wxDefaultPosition, wxSize(40, -1)), wxGBPosition(i + 1, j + 1));
        }
    }

    // Rechte Spalte
    wxBoxSizer* right_column = new wxBoxSizer(wxVERTICAL);
    right_column->Add(new wxStaticText(this, wxID_ANY, "Radix"), 0, wxALL, 5);
    right_column->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxALL, 5);
    wxString typ_choices[] = { "Radix", "Transit", "Synastrie" };
    wxRadioBox* typ_radio = new wxRadioBox(this, wxID_ANY, "Orben für:", wxDefaultPosition, wxDefaultSize, 3, typ_choices, 1, wxRA_SPECIFY_COLS);
    right_column->Add(typ_radio, 0, wxEXPAND | wxALL, 5);
    right_column->Add(new wxCheckBox(this, wxID_ANY, "Zombie"), 0, wxALL, 5);
    gbs->Add(right_column, wxGBPosition(0, 12), wxGBSpan(8, 1), wxEXPAND | wxALL, 10);

    main_sizer->Add(gbs, 1, wxEXPAND | wxALL, 10);

    // Buttons
    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    button_sizer->Add(new wxButton(this, wxID_OK, "Ok"), 0, wxALL, 5);
    button_sizer->Add(new wxButton(this, wxID_ANY, "Default"), 0, wxALL, 5);
    button_sizer->Add(new wxButton(this, wxID_CANCEL, "Abbruch"), 0, wxALL, 5);
    button_sizer->AddStretchSpacer();
    button_sizer->Add(new wxButton(this, wxID_HELP, "Hilfe"), 0, wxALL, 5);
    button_sizer->Add(new wxButton(this, wxID_ANY, "ZAP!"), 0, wxALL, 5);
    main_sizer->Add(button_sizer, 0, wxEXPAND | wxALL, 10);

    SetSizerAndFit(main_sizer);
    Centre();
}

void OrbenDialog::OnOk(wxCommandEvent& event) {
    EndModal(wxID_OK);
}
