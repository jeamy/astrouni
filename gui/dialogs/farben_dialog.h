// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgFarben from astrofil.c

#pragma once

#include <wx/wx.h>
#include <wx/clrpicker.h>
#include <wx/radiobut.h>
#include "../database/legacy_data_manager.h"

// 1:1 Port von DlgFarben (FARBEN Dialog)
class FarbenDialog : public wxDialog {
public:
    FarbenDialog(wxWindow* parent, LegacyDataManager* data_manager);
    
    // Farben abrufen/setzen
    LegacySettings::Colors GetColors() const;
    void SetColors(const LegacySettings::Colors& colors);
    
private:
    // Event Handlers (1:1 Legacy)
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnReset(wxCommandEvent& event);
    void OnModeChange(wxCommandEvent& event);
    void OnColorChange(wxColourPickerEvent& event);
    
    // GUI Controls (1:1 Legacy Layout)
    // Modus-Auswahl
    wxRadioButton* screen_rb_;
    wxRadioButton* print_rb_;
    wxRadioButton* mono_rb_;
    
    // Farb-Picker
    wxColourPickerCtrl* background_picker_;
    wxColourPickerCtrl* circles_picker_;
    wxColourPickerCtrl* houses_picker_;
    wxColourPickerCtrl* planets_picker_;
    wxColourPickerCtrl* aspects_harmonious_picker_;
    wxColourPickerCtrl* aspects_disharmonious_picker_;
    wxColourPickerCtrl* text_picker_;
    wxColourPickerCtrl* zodiac_picker_;
    
    // Vorschau-Panel
    wxPanel* preview_panel_;
    
    // Buttons
    wxButton* ok_btn_;
    wxButton* cancel_btn_;
    wxButton* reset_btn_;
    
    // Daten-Manager
    LegacyDataManager* data_manager_;
    
    // Aktueller Modus
    enum ColorMode {
        SCREEN_MODE,
        PRINT_MODE,
        MONO_MODE
    } current_mode_;
    
    // Farb-Sets
    LegacySettings::Colors screen_colors_;
    LegacySettings::Colors print_colors_;
    LegacySettings::Colors mono_colors_;
    
    // Layout erstellen
    void CreateLayout();
    void InitializeControls();
    void UpdateColorPickers();
    void UpdatePreview();
    void LoadColorsFromMode();
    void SaveColorsToMode();
    
    // Hilfsfunktionen
    wxColour UInt32ToWxColour(uint32_t color);
    uint32_t WxColourToUInt32(const wxColour& color);
    
    // Event IDs
    enum {
        ID_SCREEN_MODE = wxID_HIGHEST + 500,
        ID_PRINT_MODE,
        ID_MONO_MODE,
        ID_BACKGROUND,
        ID_CIRCLES,
        ID_HOUSES,
        ID_PLANETS,
        ID_ASPECTS_HARMONIOUS,
        ID_ASPECTS_DISHARMONIOUS,
        ID_TEXT,
        ID_ZODIAC,
        ID_RESET_COLORS
    };
    
    wxDECLARE_EVENT_TABLE();
};
