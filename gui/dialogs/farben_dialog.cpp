// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgFarben from astrofil.c

#include "farben_dialog.h"
#include "astro/i18n.h"
#include <wx/statbox.h>
#include <wx/sizer.h>
#include <wx/dcbuffer.h>

using namespace astro::i18n;

// Event Table (1:1 Legacy)
wxBEGIN_EVENT_TABLE(FarbenDialog, wxDialog)
    EVT_BUTTON(wxID_OK, FarbenDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, FarbenDialog::OnCancel)
    EVT_BUTTON(ID_RESET_COLORS, FarbenDialog::OnReset)
    EVT_RADIOBUTTON(ID_SCREEN_MODE, FarbenDialog::OnModeChange)
    EVT_RADIOBUTTON(ID_PRINT_MODE, FarbenDialog::OnModeChange)
    EVT_RADIOBUTTON(ID_MONO_MODE, FarbenDialog::OnModeChange)
    EVT_COLOURPICKER_CHANGED(ID_BACKGROUND, FarbenDialog::OnColorChange)
    EVT_COLOURPICKER_CHANGED(ID_CIRCLES, FarbenDialog::OnColorChange)
    EVT_COLOURPICKER_CHANGED(ID_HOUSES, FarbenDialog::OnColorChange)
    EVT_COLOURPICKER_CHANGED(ID_PLANETS, FarbenDialog::OnColorChange)
    EVT_COLOURPICKER_CHANGED(ID_ASPECTS_HARMONIOUS, FarbenDialog::OnColorChange)
    EVT_COLOURPICKER_CHANGED(ID_ASPECTS_DISHARMONIOUS, FarbenDialog::OnColorChange)
    EVT_COLOURPICKER_CHANGED(ID_TEXT, FarbenDialog::OnColorChange)
    EVT_COLOURPICKER_CHANGED(ID_ZODIAC, FarbenDialog::OnColorChange)
wxEND_EVENT_TABLE()

FarbenDialog::FarbenDialog(wxWindow* parent, LegacyDataManager* data_manager)
    : wxDialog(parent, wxID_ANY, wxString::FromUTF8("Farben Einstellungen"), 
               wxDefaultPosition, wxSize(600, 500),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
      data_manager_(data_manager),
      current_mode_(SCREEN_MODE) {
    
    CreateLayout();
    InitializeControls();
    Centre();
}

void FarbenDialog::CreateLayout() {
    // Haupt-Sizer
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Modus-Auswahl (1:1 Legacy)
    wxStaticBoxSizer* mode_box = new wxStaticBoxSizer(wxHORIZONTAL, this, wxString::FromUTF8("Farb-Modus"));
    
    screen_rb_ = new wxRadioButton(this, ID_SCREEN_MODE, wxString::FromUTF8("Bildschirm"), 
                                  wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    print_rb_ = new wxRadioButton(this, ID_PRINT_MODE, wxString::FromUTF8("Druck"));
    mono_rb_ = new wxRadioButton(this, ID_MONO_MODE, wxString::FromUTF8("Monochrom"));
    
    mode_box->Add(screen_rb_, 0, wxALL, 5);
    mode_box->Add(print_rb_, 0, wxALL, 5);
    mode_box->Add(mono_rb_, 0, wxALL, 5);
    
    main_sizer->Add(mode_box, 0, wxEXPAND | wxALL, 10);
    
    // Farben-Gruppe (1:1 Legacy)
    wxStaticBoxSizer* colors_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Farben"));
    
    wxFlexGridSizer* grid_sizer = new wxFlexGridSizer(8, 2, 5, 10);
    grid_sizer->AddGrowableCol(1);
    
    // Hintergrund
    grid_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Hintergrund:")), 0, wxALIGN_CENTER_VERTICAL);
    background_picker_ = new wxColourPickerCtrl(this, ID_BACKGROUND);
    grid_sizer->Add(background_picker_, 0, wxEXPAND);
    
    // Kreise
    grid_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Kreise:")), 0, wxALIGN_CENTER_VERTICAL);
    circles_picker_ = new wxColourPickerCtrl(this, ID_CIRCLES);
    grid_sizer->Add(circles_picker_, 0, wxEXPAND);
    
    // Häuser
    grid_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Häuser:")), 0, wxALIGN_CENTER_VERTICAL);
    houses_picker_ = new wxColourPickerCtrl(this, ID_HOUSES);
    grid_sizer->Add(houses_picker_, 0, wxEXPAND);
    
    // Planeten
    grid_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Planeten:")), 0, wxALIGN_CENTER_VERTICAL);
    planets_picker_ = new wxColourPickerCtrl(this, ID_PLANETS);
    grid_sizer->Add(planets_picker_, 0, wxEXPAND);
    
    // Harmonische Aspekte
    grid_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Harmonische Aspekte:")), 0, wxALIGN_CENTER_VERTICAL);
    aspects_harmonious_picker_ = new wxColourPickerCtrl(this, ID_ASPECTS_HARMONIOUS);
    grid_sizer->Add(aspects_harmonious_picker_, 0, wxEXPAND);
    
    // Disharmonische Aspekte
    grid_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Disharmonische Aspekte:")), 0, wxALIGN_CENTER_VERTICAL);
    aspects_disharmonious_picker_ = new wxColourPickerCtrl(this, ID_ASPECTS_DISHARMONIOUS);
    grid_sizer->Add(aspects_disharmonious_picker_, 0, wxEXPAND);
    
    // Text
    grid_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Text:")), 0, wxALIGN_CENTER_VERTICAL);
    text_picker_ = new wxColourPickerCtrl(this, ID_TEXT);
    grid_sizer->Add(text_picker_, 0, wxEXPAND);
    
    // Tierkreis
    grid_sizer->Add(new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Tierkreis:")), 0, wxALIGN_CENTER_VERTICAL);
    zodiac_picker_ = new wxColourPickerCtrl(this, ID_ZODIAC);
    grid_sizer->Add(zodiac_picker_, 0, wxEXPAND);
    
    colors_box->Add(grid_sizer, 0, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(colors_box, 0, wxEXPAND | wxALL, 10);
    
    // Vorschau-Panel (1:1 Legacy)
    wxStaticBoxSizer* preview_box = new wxStaticBoxSizer(wxVERTICAL, this, wxString::FromUTF8("Vorschau"));
    
    preview_panel_ = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(-1, 150));
    preview_panel_->SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    preview_panel_->Bind(wxEVT_PAINT, [this](wxPaintEvent& event) {
        wxAutoBufferedPaintDC dc(preview_panel_);
        
        // Einfache Vorschau zeichnen
        wxSize size = preview_panel_->GetSize();
        
        // Hintergrund
        dc.SetBrush(wxBrush(background_picker_->GetColour()));
        dc.DrawRectangle(0, 0, size.x, size.y);
        
        // Kreis
        dc.SetPen(wxPen(circles_picker_->GetColour(), 2));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawCircle(size.x/2, size.y/2, std::min(size.x, size.y)/3);
        
        // Häuser-Linien
        dc.SetPen(wxPen(houses_picker_->GetColour(), 1));
        dc.DrawLine(size.x/4, size.y/2, 3*size.x/4, size.y/2);
        dc.DrawLine(size.x/2, size.y/4, size.x/2, 3*size.y/4);
        
        // Planeten-Symbol
        dc.SetTextForeground(planets_picker_->GetColour());
        dc.SetFont(wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
        dc.DrawText(wxString::FromUTF8("☉"), size.x/2 + 20, size.y/2 - 20);
        
        // Aspekt-Linie
        dc.SetPen(wxPen(aspects_harmonious_picker_->GetColour(), 2));
        dc.DrawLine(size.x/3, size.y/3, 2*size.x/3, 2*size.y/3);
        
        // Text
        dc.SetTextForeground(text_picker_->GetColour());
        dc.SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        dc.DrawText(wxString::FromUTF8("Vorschau"), 10, 10);
    });
    
    preview_box->Add(preview_panel_, 1, wxEXPAND | wxALL, 5);
    
    main_sizer->Add(preview_box, 1, wxEXPAND | wxALL, 10);
    
    // Dialog-Buttons (1:1 Legacy)
    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    
    reset_btn_ = new wxButton(this, ID_RESET_COLORS, wxString::FromUTF8("Standard"));
    button_sizer->Add(reset_btn_, 0, wxRIGHT, 10);
    
    ok_btn_ = new wxButton(this, wxID_OK, wxString::FromUTF8("OK"));
    cancel_btn_ = new wxButton(this, wxID_CANCEL, wxString::FromUTF8("Abbrechen"));
    
    button_sizer->AddStretchSpacer();
    button_sizer->Add(ok_btn_, 0, wxRIGHT, 5);
    button_sizer->Add(cancel_btn_, 0);
    
    main_sizer->Add(button_sizer, 0, wxEXPAND | wxALL, 10);
    
    SetSizer(main_sizer);
}

void FarbenDialog::InitializeControls() {
    if (data_manager_) {
        const auto& settings = data_manager_->GetSettings();
        screen_colors_ = settings.screen_colors;
        print_colors_ = settings.print_colors;
        mono_colors_ = settings.mono_colors;
    } else {
        // Standard-Farben initialisieren
        wxCommandEvent dummy_event;
        OnReset(dummy_event);
    }
    
    // Bildschirm-Modus als Standard
    screen_rb_->SetValue(true);
    current_mode_ = SCREEN_MODE;
    UpdateColorPickers();
}

void FarbenDialog::UpdateColorPickers() {
    LegacySettings::Colors* colors = nullptr;
    
    switch (current_mode_) {
        case SCREEN_MODE:
            colors = &screen_colors_;
            break;
        case PRINT_MODE:
            colors = &print_colors_;
            break;
        case MONO_MODE:
            colors = &mono_colors_;
            break;
    }
    
    if (colors) {
        background_picker_->SetColour(UInt32ToWxColour(colors->background));
        circles_picker_->SetColour(UInt32ToWxColour(colors->circles));
        houses_picker_->SetColour(UInt32ToWxColour(colors->houses));
        planets_picker_->SetColour(UInt32ToWxColour(colors->planets));
        aspects_harmonious_picker_->SetColour(UInt32ToWxColour(colors->aspects_harmonious));
        aspects_disharmonious_picker_->SetColour(UInt32ToWxColour(colors->aspects_disharmonious));
        text_picker_->SetColour(UInt32ToWxColour(colors->text));
        zodiac_picker_->SetColour(UInt32ToWxColour(colors->zodiac));
    }
    
    UpdatePreview();
}

void FarbenDialog::UpdatePreview() {
    if (preview_panel_) {
        preview_panel_->Refresh();
    }
}

void FarbenDialog::LoadColorsFromMode() {
    // Aktuelle Farben aus den Pickern in das entsprechende Set laden
    LegacySettings::Colors* colors = nullptr;
    
    switch (current_mode_) {
        case SCREEN_MODE:
            colors = &screen_colors_;
            break;
        case PRINT_MODE:
            colors = &print_colors_;
            break;
        case MONO_MODE:
            colors = &mono_colors_;
            break;
    }
    
    if (colors) {
        colors->background = WxColourToUInt32(background_picker_->GetColour());
        colors->circles = WxColourToUInt32(circles_picker_->GetColour());
        colors->houses = WxColourToUInt32(houses_picker_->GetColour());
        colors->planets = WxColourToUInt32(planets_picker_->GetColour());
        colors->aspects_harmonious = WxColourToUInt32(aspects_harmonious_picker_->GetColour());
        colors->aspects_disharmonious = WxColourToUInt32(aspects_disharmonious_picker_->GetColour());
        colors->text = WxColourToUInt32(text_picker_->GetColour());
        colors->zodiac = WxColourToUInt32(zodiac_picker_->GetColour());
    }
}

// Event Handlers (1:1 Legacy)
void FarbenDialog::OnOK(wxCommandEvent& event) {
    // Aktuelle Farben speichern
    LoadColorsFromMode();
    
    if (data_manager_) {
        LegacySettings settings = data_manager_->GetSettings();
        settings.screen_colors = screen_colors_;
        settings.print_colors = print_colors_;
        settings.mono_colors = mono_colors_;
        data_manager_->SetSettings(settings);
        data_manager_->SaveDefaultSettings();
    }
    
    EndModal(wxID_OK);
}

void FarbenDialog::OnCancel(wxCommandEvent& event) {
    EndModal(wxID_CANCEL);
}

void FarbenDialog::OnReset(wxCommandEvent& event) {
    // Standard-Farben setzen (1:1 Legacy)
    screen_colors_.background = 0xC0C0C0;      // Hellgrau
    screen_colors_.circles = 0x000000;         // Schwarz
    screen_colors_.houses = 0x000000;          // Schwarz
    screen_colors_.planets = 0x0000FF;         // Blau
    screen_colors_.aspects_harmonious = 0x008000; // Grün
    screen_colors_.aspects_disharmonious = 0xFF0000; // Rot
    screen_colors_.text = 0x000000;            // Schwarz
    screen_colors_.zodiac = 0x000000;          // Schwarz
    
    // Print-Farben (alle schwarz auf weiß)
    print_colors_.background = 0xFFFFFF;       // Weiß
    print_colors_.circles = 0x000000;          // Schwarz
    print_colors_.houses = 0x000000;           // Schwarz
    print_colors_.planets = 0x000000;          // Schwarz
    print_colors_.aspects_harmonious = 0x000000; // Schwarz
    print_colors_.aspects_disharmonious = 0x000000; // Schwarz
    print_colors_.text = 0x000000;             // Schwarz
    print_colors_.zodiac = 0x000000;           // Schwarz
    
    // Mono-Farben
    mono_colors_ = print_colors_;
    
    UpdateColorPickers();
}

void FarbenDialog::OnModeChange(wxCommandEvent& event) {
    // Aktuelle Farben speichern
    LoadColorsFromMode();
    
    // Neuen Modus setzen
    switch (event.GetId()) {
        case ID_SCREEN_MODE:
            current_mode_ = SCREEN_MODE;
            break;
        case ID_PRINT_MODE:
            current_mode_ = PRINT_MODE;
            break;
        case ID_MONO_MODE:
            current_mode_ = MONO_MODE;
            break;
    }
    
    UpdateColorPickers();
}

void FarbenDialog::OnColorChange(wxColourPickerEvent& event) {
    UpdatePreview();
}

// Hilfsfunktionen
wxColour FarbenDialog::UInt32ToWxColour(uint32_t color) {
    return wxColour((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
}

uint32_t FarbenDialog::WxColourToUInt32(const wxColour& color) {
    return (color.Red() << 16) | (color.Green() << 8) | color.Blue();
}

LegacySettings::Colors FarbenDialog::GetColors() const {
    switch (current_mode_) {
        case SCREEN_MODE:
            return screen_colors_;
        case PRINT_MODE:
            return print_colors_;
        case MONO_MODE:
            return mono_colors_;
        default:
            return screen_colors_;
    }
}

void FarbenDialog::SetColors(const LegacySettings::Colors& colors) {
    switch (current_mode_) {
        case SCREEN_MODE:
            screen_colors_ = colors;
            break;
        case PRINT_MODE:
            print_colors_ = colors;
            break;
        case MONO_MODE:
            mono_colors_ = colors;
            break;
    }
    
    UpdateColorPickers();
}
