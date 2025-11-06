// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project

#pragma once

#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/timectrl.h>
#include "astro/data_types.h"

class NewChartDialog : public wxDialog {
public:
    NewChartDialog(wxWindow* parent);
    
    // Daten abrufen
    astro::RadixData GetRadixData() const;
    
private:
    // Event Handlers
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    
    // Input Fields
    wxTextCtrl* name_ctrl_;
    wxDatePickerCtrl* date_ctrl_;
    wxTimePickerCtrl* time_ctrl_;
    wxTextCtrl* latitude_ctrl_;
    wxTextCtrl* longitude_ctrl_;
    wxTextCtrl* altitude_ctrl_;
    wxTextCtrl* city_ctrl_;
    
    wxDECLARE_EVENT_TABLE();
};
