// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgDDProc from astrofil.c

#pragma once

#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/timectrl.h>
#include <wx/spinctrl.h>
#include "astro/data_types.h"

// 1:1 Port von DlgDDProc (DD Dialog)
class DatumDialog : public wxDialog {
public:
    DatumDialog(wxWindow* parent, const wxString& title = wxString::FromUTF8("Datum eingeben"));
    
    // Datum/Zeit abrufen/setzen
    astro::DateTime GetDateTime() const;
    void SetDateTime(const astro::DateTime& datetime);
    
    // Julianisches Datum
    double GetJulianDate() const;
    void SetJulianDate(double jd);
    
private:
    // Event Handlers (1:1 Legacy)
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnHeute(wxCommandEvent& event);
    void OnJulianChange(wxSpinDoubleEvent& event);
    void OnDatumChange(wxCommandEvent& event);
    
    // GUI Controls (1:1 Legacy Layout)
    // Gregorianisches Datum
    wxDatePickerCtrl* datum_ctrl_;
    wxTimePickerCtrl* zeit_ctrl_;
    
    // Julianisches Datum
    wxSpinCtrlDouble* julian_ctrl_;
    
    // Zusatz-Informationen
    wxStaticText* wochentag_text_;
    wxStaticText* kalenderwoche_text_;
    wxStaticText* tag_im_jahr_text_;
    
    // Buttons
    wxButton* heute_btn_;
    wxButton* ok_btn_;
    wxButton* cancel_btn_;
    
    // Layout erstellen
    void CreateLayout();
    void InitializeControls();
    void UpdateAnzeige();
    void UpdateJulianFromDatum();
    void UpdateDatumFromJulian();
    
    // Event IDs
    enum {
        ID_DATUM_PICKER = wxID_HIGHEST + 1400,
        ID_ZEIT_PICKER,
        ID_JULIAN_CTRL,
        ID_HEUTE_BTN
    };
    
    wxDECLARE_EVENT_TABLE();
};
