// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgDGProc from astrofil.c

#pragma once

#include <wx/wx.h>
#include <wx/spinctrl.h>

// 1:1 Port von DlgDGProc (DG Dialog)
class GradDialog : public wxDialog {
public:
    GradDialog(wxWindow* parent, const wxString& title = wxString::FromUTF8("Grad eingeben"));
    
    // Grad-Werte abrufen/setzen
    double GetGradDecimal() const;
    void SetGradDecimal(double grad);
    
    // Grad/Minuten/Sekunden
    struct GradMinutenSekunden {
        int grad;
        int minuten;
        double sekunden;
        bool negativ;
    };
    
    GradMinutenSekunden GetGradMinutenSekunden() const;
    void SetGradMinutenSekunden(const GradMinutenSekunden& gms);
    
private:
    // Event Handlers (1:1 Legacy)
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnGradChange(wxCommandEvent& event);
    void OnDecimalChange(wxCommandEvent& event);
    void OnVorzeichenChange(wxCommandEvent& event);
    
    // GUI Controls (1:1 Legacy Layout)
    // Grad/Minuten/Sekunden
    wxCheckBox* negativ_cb_;
    wxSpinCtrl* grad_ctrl_;
    wxSpinCtrl* minuten_ctrl_;
    wxSpinCtrlDouble* sekunden_ctrl_;
    
    // Dezimal-Darstellung
    wxSpinCtrlDouble* decimal_ctrl_;
    
    // Informations-Text
    wxStaticText* info_text_;
    
    // Buttons
    wxButton* ok_btn_;
    wxButton* cancel_btn_;
    
    // Layout erstellen
    void CreateLayout();
    void InitializeControls();
    void UpdateDecimalFromGMS();
    void UpdateGMSFromDecimal();
    void UpdateInfoText();
    
    // Event IDs
    enum {
        ID_NEGATIV_CB = wxID_HIGHEST + 1500,
        ID_GRAD_CTRL,
        ID_MINUTEN_CTRL,
        ID_SEKUNDEN_CTRL,
        ID_DECIMAL_CTRL
    };
    
    wxDECLARE_EVENT_TABLE();
};
