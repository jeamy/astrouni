#include "orben_dialog.h"
#include "symbol_panel.h"
#include <wx/gbsizer.h>
#include <wx/statline.h>

wxBEGIN_EVENT_TABLE(OrbenDialog, wxDialog)
    EVT_BUTTON(wxID_OK, OrbenDialog::OnOk)
wxEND_EVENT_TABLE()

// 1:1 Port von DlgOrbenEin aus astrouni.rc
OrbenDialog::OrbenDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Orben", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) {

    // Setze hellen Hintergrund (Legacy-Look)
    SetBackgroundColour(*wxWHITE);

    // Lade die Astro-Schriftart
    if (!wxFont::AddPrivateFont("gui/asu_____.ttf")) {
        wxLogError("Konnte Schriftart 'gui/asu_____.ttf' nicht laden.");
    }
    wxFont astro_font(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("AstroUniverse"));
    if (!astro_font.IsOk()) {
        wxLogError("Konnte Schriftart 'AstroUniverse' nicht korrekt erstellen.");
    }

    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    wxGridBagSizer* gbs = new wxGridBagSizer(0, 0);

    // Symbol-Mapping: Korrekte Unicode-Codepunkte aus der AstroUniverse-Schriftart
    // Planeten: U+00AA bis U+00B4 (Sonne, Mond, Merkur, Venus, Mars, Jupiter, Saturn, Uranus, Neptun, Pluto, Mondknoten)
    const wchar_t* planet_symbols[] = { L"\u00AA", L"\u00AB", L"\u00AC", L"\u00AD", L"\u00AE", L"\u00AF", L"\u00B0", L"\u00B1", L"\u00B2", L"\u00B3", L"\u00B4" };
    // Aspekte: U+0041 bis U+0047 (Konjunktion, Sextil, Quadrat, Trigon, Opposition, Quincunx, Halbsextil)
    const wchar_t* aspect_symbols[] = { L"\u2018", L"\u2014", L"\u201C", L"\u201D", L"\u2019", L"\u2022", L"\u2013" };

    // Obere Leiste (Alle Planeten-Symbole) - jedes Symbol in einer eigenen Spalte
    for (int i = 0; i < 11; ++i) {
        PlanetOrAspectSymbolPanel* panel = new PlanetOrAspectSymbolPanel(this, wxID_ANY, planet_symbols[i], astro_font);
        panel->SetMinSize(wxSize(45, 30));
        panel->SetBackgroundColour(*wxWHITE);
        gbs->Add(panel, wxGBPosition(0, i + 1), wxGBSpan(1, 1), wxEXPAND | wxALIGN_CENTER);
    }

    // Linke Leiste (Alle Aspekt-Symbole) - jedes Symbol in einer eigenen Zeile
    for (int i = 0; i < 7; ++i) {
        PlanetOrAspectSymbolPanel* panel = new PlanetOrAspectSymbolPanel(this, wxID_ANY, aspect_symbols[i], astro_font);
        panel->SetMinSize(wxSize(30, 30));
        panel->SetBackgroundColour(*wxWHITE);
        gbs->Add(panel, wxGBPosition(i + 1, 0), wxGBSpan(1, 1), wxEXPAND | wxALIGN_CENTER);
    }

    // Gitter für die Orben-Werte - jede Zelle einzeln hinzufügen für präzise Kontrolle
    for (int row = 0; row < 7; ++row) {
        for (int col = 0; col < 11; ++col) {
            wxTextCtrl* textCtrl = new wxTextCtrl(this, wxID_ANY, "10.0", wxDefaultPosition, wxSize(45, -1), wxTE_CENTER);
            gbs->Add(textCtrl, wxGBPosition(row + 1, col + 1), wxGBSpan(1, 1), wxEXPAND);
        }
    }

    // Rechte Spalte
    wxBoxSizer* right_column = new wxBoxSizer(wxVERTICAL);
    right_column->Add(new wxStaticText(this, wxID_ANY, "Radix"), 0, wxALL, 5);
    right_column->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxLEFT | wxRIGHT, 5);
    wxString typ_choices[] = { "Radix", "Transit", "Synastrie" };
    wxRadioBox* typ_radio = new wxRadioBox(this, wxID_ANY, "Orben für:", wxDefaultPosition, wxDefaultSize, 3, typ_choices, 1, wxRA_SPECIFY_COLS);
    right_column->Add(typ_radio, 0, wxEXPAND | wxALL, 5);
    right_column->Add(new wxCheckBox(this, wxID_ANY, "Zombie"), 0, wxALIGN_LEFT | wxALL, 5);
    gbs->Add(right_column, wxGBPosition(0, 12), wxGBSpan(8, 1), wxEXPAND | wxLEFT | wxRIGHT, 10);

    main_sizer->Add(gbs, 1, wxEXPAND | wxALL, 10);

    // Buttons
    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    button_sizer->Add(new wxButton(this, wxID_OK, "Ok"), 0, wxALL, 5);
    button_sizer->Add(new wxButton(this, wxID_ANY, "Default"), 0, wxALL, 5);
    button_sizer->Add(new wxButton(this, wxID_CANCEL, "Abbruch"), 0, wxALL, 5);
    button_sizer->AddStretchSpacer();
    button_sizer->Add(new wxButton(this, wxID_HELP, "Hilfe"), 0, wxALL, 5);
    button_sizer->Add(new wxButton(this, wxID_ANY, "ZAP!"), 0, wxALL, 5);
    main_sizer->Add(button_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    SetSizerAndFit(main_sizer);
    Centre();
}

void OrbenDialog::OnOk(wxCommandEvent& event) {
    EndModal(wxID_OK);
}
