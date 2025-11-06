// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project

#pragma once

#include <wx/wx.h>
#include "chart_panel.h"
#include "astro/swiss_ephemeris.h"
#include "astro/calculations.h"
#include <memory>

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title);
    
private:
    // Event Handlers
    void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnExportPNG(wxCommandEvent& event);
    void OnExportSVG(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    
    // GUI Components
    ChartPanel* chart_panel_;
    wxMenuBar* menu_bar_;
    wxStatusBar* status_bar_;
    
    // Event IDs
    enum {
        ID_New = wxID_HIGHEST + 1,
        ID_Open,
        ID_Save,
        ID_ExportPNG,
        ID_ExportSVG
    };
    
    wxDECLARE_EVENT_TABLE();
};
