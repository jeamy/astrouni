#include "orben_dialog.h"
#include "symbol_panel.h"
#include "../database/orben_file_manager.h"
#include <wx/gbsizer.h>
#include <wx/statline.h>

wxBEGIN_EVENT_TABLE(OrbenDialog, wxDialog)
    EVT_BUTTON(wxID_OK, OrbenDialog::OnOk)
    EVT_BUTTON(wxID_CANCEL, OrbenDialog::OnCancel)
wxEND_EVENT_TABLE()

// 1:1 Port von DlgOrbenEin aus astrofil.c
OrbenDialog::OrbenDialog(wxWindow* parent, OrbenData& orben_data)
    : wxDialog(parent, wxID_ANY, "Orben", wxDefaultPosition, wxSize(650, 350)), 
      orben_data_(orben_data), current_typ_(0), current_planet_(0) {

    SetBackgroundColour(*wxWHITE);

    // Lade Astro-Schriftart
    if (!wxFont::AddPrivateFont("gui/asu_____.ttf")) {
        wxLogError("Konnte Schriftart nicht laden.");
    }
    wxFont astro_font(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("AstroUniverse"));

    // Planeten-Symbole (17 Objekte)
    const wchar_t* planet_symbols[] = { 
        L"\u00AA", L"\u00AB", L"\u00AC", L"\u00AD", L"\u00AE", L"\u00AF", L"\u00B0", L"\u00B1", 
        L"\u00B2", L"\u00B3", L"\u00B4", L"\u00B5", L"\u00B6", L"\u00B7", L"\u00B8", L"\u00B9", L"\u00BA" 
    };
    
    // Aspekt-Symbole (7 Aspekte)
    const wchar_t* aspect_symbols[] = { 
        L"\u2018", L"\u2014", L"\u201C", L"\u201D", L"\u2019", L"\u2022", L"\u2013" 
    };

    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* top_sizer = new wxBoxSizer(wxHORIZONTAL);

    // === LINKE SEITE: Grid mit Orben-Eingaben ===
    wxBoxSizer* left_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Obere Zeile: Aktueller Planet (links oben) + Aspekt-Symbole (horizontal)
    wxBoxSizer* header_sizer = new wxBoxSizer(wxHORIZONTAL);
    
    // Links oben: Aktuelles Planeten-Symbol (scrollbar)
    planet_symbol_panel_ = new PlanetOrAspectSymbolPanel(this, wxID_ANY, planet_symbols[0], astro_font);
    planet_symbol_panel_->SetMinSize(wxSize(35, 30));
    planet_symbol_panel_->SetBackgroundColour(*wxWHITE);
    header_sizer->Add(planet_symbol_panel_, 0, wxALL | wxALIGN_CENTER, 2);
    
    // Aspekt-Symbole horizontal (7 Spalten)
    for (int i = 0; i < 7; ++i) {
        PlanetOrAspectSymbolPanel* panel = new PlanetOrAspectSymbolPanel(this, wxID_ANY, aspect_symbols[i], astro_font);
        panel->SetMinSize(wxSize(45, 30));
        panel->SetBackgroundColour(*wxWHITE);
        header_sizer->Add(panel, 0, wxALL | wxALIGN_CENTER, 2);
    }
    left_sizer->Add(header_sizer, 0, wxEXPAND | wxLEFT, 5);
    
    // Grid mit Planeten-Symbolen links + Eingabefelder
    wxFlexGridSizer* grid = new wxFlexGridSizer(17, 8, 1, 1);  // 17 Zeilen, 8 Spalten (1 Symbol + 7 Werte)
    text_ctrls_.reserve(17 * 7);
    
    for (int row = 0; row < 17; ++row) {
        // Planeten-Symbol links
        PlanetOrAspectSymbolPanel* planet_panel = new PlanetOrAspectSymbolPanel(this, wxID_ANY, planet_symbols[row], astro_font);
        planet_panel->SetMinSize(wxSize(35, 22));
        planet_panel->SetBackgroundColour(*wxWHITE);
        grid->Add(planet_panel, 0, wxALIGN_CENTER | wxALL, 1);
        
        // 7 Eingabefelder für Aspekte
        for (int col = 0; col < 7; ++col) {
            wxTextCtrl* textCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(45, 22), wxTE_CENTER);
            grid->Add(textCtrl, 0, wxEXPAND | wxALL, 1);
            text_ctrls_.push_back(textCtrl);
        }
    }
    left_sizer->Add(grid, 1, wxEXPAND | wxALL, 5);
    
    top_sizer->Add(left_sizer, 1, wxEXPAND);
    
    // === RECHTE SEITE: Einstellungen ===
    wxBoxSizer* right_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Planetenname zwischen Linien
    right_sizer->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxLEFT | wxRIGHT, 5);
    planet_name_text_ = new wxStaticText(this, wxID_ANY, "Sonne", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
    right_sizer->Add(planet_name_text_, 0, wxEXPAND | wxALL, 5);
    right_sizer->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxLEFT | wxRIGHT, 5);
    
    right_sizer->AddSpacer(10);
    
    // "Orben für:" Label
    right_sizer->Add(new wxStaticText(this, wxID_ANY, "Orben für:"), 0, wxALL, 5);
    
    // RadioButtons: Radix, Transit, Synastrie
    wxRadioButton* radix_rb = new wxRadioButton(this, wxID_ANY, "Radix", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    wxRadioButton* transit_rb = new wxRadioButton(this, wxID_ANY, "Transit");
    wxRadioButton* synastrie_rb = new wxRadioButton(this, wxID_ANY, "Synastrie");
    
    radix_rb->SetValue(true);
    radix_rb->Bind(wxEVT_RADIOBUTTON, [this](wxCommandEvent&) { 
        SaveOrbenValues();
        current_typ_ = 0; 
        LoadOrbenValues(); 
    });
    transit_rb->Bind(wxEVT_RADIOBUTTON, [this](wxCommandEvent&) { 
        SaveOrbenValues();
        current_typ_ = 1; 
        LoadOrbenValues(); 
    });
    synastrie_rb->Bind(wxEVT_RADIOBUTTON, [this](wxCommandEvent&) { 
        SaveOrbenValues();
        current_typ_ = 2; 
        LoadOrbenValues(); 
    });
    
    right_sizer->Add(radix_rb, 0, wxALL, 5);
    right_sizer->Add(transit_rb, 0, wxALL, 5);
    right_sizer->Add(synastrie_rb, 0, wxALL, 5);
    
    right_sizer->AddSpacer(10);
    
    // Zombie Checkbox
    wxCheckBox* zombie_check = new wxCheckBox(this, wxID_ANY, "Zombie");
    right_sizer->Add(zombie_check, 0, wxALL, 5);
    
    top_sizer->Add(right_sizer, 0, wxEXPAND | wxALL, 10);
    
    main_sizer->Add(top_sizer, 1, wxEXPAND);
    
    // === BUTTONS ===
    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    button_sizer->Add(new wxButton(this, wxID_OK, "Ok"), 0, wxALL, 5);
    
    wxButton* default_btn = new wxButton(this, wxID_ANY, "Default");
    default_btn->Bind(wxEVT_BUTTON, &OrbenDialog::OnDefault, this);
    button_sizer->Add(default_btn, 0, wxALL, 5);
    
    button_sizer->Add(new wxButton(this, wxID_CANCEL, "Abbruch"), 0, wxALL, 5);
    button_sizer->AddStretchSpacer();
    button_sizer->Add(new wxButton(this, wxID_HELP, "Hilfe"), 0, wxALL, 5);
    
    wxButton* zap_btn = new wxButton(this, wxID_ANY, "ZAP!");
    zap_btn->Bind(wxEVT_BUTTON, &OrbenDialog::OnZap, this);
    button_sizer->Add(zap_btn, 0, wxALL, 5);
    
    main_sizer->Add(button_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    
    SetSizer(main_sizer);
    Centre();
    
    // Lade initiale Werte
    LoadOrbenValues();
}

void OrbenDialog::LoadOrbenValues() {
    // Lade Orben für aktuellen Ursprungs-Planeten (current_planet_)
    // zu allen 17 Planeten für alle 7 Aspekte
    
    float (*data)[7] = nullptr;
    switch (current_typ_) {
        case 0: data = orben_data_.radix_planet; break;
        case 1: data = orben_data_.transit_planet; break;
        case 2: data = orben_data_.synastrie_planet; break;
    }
    
    if (!data) return;
    
    // Grid: 17 Zeilen (Ziel-Planeten) × 7 Spalten (Aspekte)
    // data[planet][aspekt] = Orb-Wert
    for (int row = 0; row < 17; ++row) {  // Ziel-Planet
        for (int col = 0; col < 7; ++col) {  // Aspekt
            int index = row * 7 + col;
            if (index < (int)text_ctrls_.size()) {
                // Im Legacy: Orben sind symmetrisch, aber wir zeigen nur eine Richtung
                // Verwende den Ziel-Planeten als Index
                text_ctrls_[index]->SetValue(wxString::Format("%.1f", data[row][col]));
            }
        }
    }
}

void OrbenDialog::SaveOrbenValues() {
    float (*data)[7] = nullptr;
    switch (current_typ_) {
        case 0: data = orben_data_.radix_planet; break;
        case 1: data = orben_data_.transit_planet; break;
        case 2: data = orben_data_.synastrie_planet; break;
    }
    
    if (!data) return;
    
    for (int row = 0; row < 17; ++row) {
        for (int col = 0; col < 7; ++col) {
            int index = row * 7 + col;
            if (index < (int)text_ctrls_.size()) {
                double value;
                if (text_ctrls_[index]->GetValue().ToDouble(&value)) {
                    data[row][col] = static_cast<float>(value);
                }
            }
        }
    }
}

void OrbenDialog::LoadDefaultValues() {
    OrbenFileManager file_mgr;
    file_mgr.LoadFromDefaultFile(orben_data_);
    LoadOrbenValues();
}

void OrbenDialog::OnOk(wxCommandEvent&) {
    SaveOrbenValues();
    
    OrbenFileManager file_mgr;
    file_mgr.SaveToIniFile(orben_data_);
    
    EndModal(wxID_OK);
}

void OrbenDialog::OnDefault(wxCommandEvent&) {
    LoadDefaultValues();
}

void OrbenDialog::OnZap(wxCommandEvent&) {
    OrbenFileManager file_mgr;
    file_mgr.LoadFromDefaultFile(orben_data_);
    LoadOrbenValues();
}

void OrbenDialog::OnCancel(wxCommandEvent&) {
    LoadOrbenValues();
    EndModal(wxID_CANCEL);
}
