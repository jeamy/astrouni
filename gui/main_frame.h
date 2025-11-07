// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project

#pragma once

#include <wx/wx.h>
#include "chart_panel.h"
#include "dialogs/person_dialog.h"
#include "dialogs/ort_dialog.h"
#include "dialogs/haus_dialog.h"
#include "dialogs/orben_dialog.h"
#include "dialogs/farben_dialog.h"
#include "database/legacy_data_manager.h"
#include "astro/swiss_ephemeris.h"
#include "astro/calculations.h"
#include <memory>

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title);
    
private:
    // Event Handlers - Datei-Menü (1:1 Legacy CM_U_*)
    void OnNew(wxCommandEvent& event);           // CM_U_NEW
    void OnOpen(wxCommandEvent& event);          // CM_U_FILEOPEN
    void OnSave(wxCommandEvent& event);          // CM_U_SAVE
    void OnSaveAs(wxCommandEvent& event);        // CM_U_SAVEU
    void OnPrint(wxCommandEvent& event);         // CM_U_PRINT
    void OnPrinterSetup(wxCommandEvent& event);  // CM_U_PINST
    void OnExit(wxCommandEvent& event);          // CM_U_EXIT
    
    // Event Handlers - Daten-Menü
    void OnPersonDialog(wxCommandEvent& event);  // CM_U_PERSON
    void OnOrtDialog(wxCommandEvent& event);     // CM_U_ORT
    
    // Event Handlers - Horoskop-Menü
    void OnHoroTyp(wxCommandEvent& event);       // CM_U_HOROTYP
    void OnHausSystem(wxCommandEvent& event);    // CM_U_HAUSER
    void OnOrben(wxCommandEvent& event);         // CM_U_ORBEN
    void OnFarben(wxCommandEvent& event);        // CM_U_FARBEN
    void OnEinstellungen(wxCommandEvent& event); // CM_U_EINST
    
    // Event Handlers - Hilfe-Menü
    void OnAbout(wxCommandEvent& event);         // CM_U_HELPABOUT
    
    // GUI Components
    ChartPanel* chart_panel_;
    wxMenuBar* menu_bar_;
    wxStatusBar* status_bar_;
    
    // Legacy Data Manager
    std::unique_ptr<LegacyDataManager> data_manager_;
    
    // Event IDs (1:1 Legacy CM_U_* Mapping)
    enum {
        // Datei
        CM_U_NEW = wxID_HIGHEST + 1,
        CM_U_FILEOPEN,
        CM_U_SAVE,
        CM_U_SAVEU,
        CM_U_PRINT,
        CM_U_PINST,
        CM_U_EXIT,
        
        // Daten
        CM_U_PERSON,
        CM_U_ORT,
        
        // Horoskop
        CM_U_HOROTYP,
        CM_U_HAUSER,
        CM_U_ORBEN,
        CM_U_FARBEN,
        CM_U_EINST,
        
        // Hilfe
        CM_U_HELPABOUT
    };
    
    wxDECLARE_EVENT_TABLE();
};
