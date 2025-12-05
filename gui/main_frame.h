#pragma once

#include <wx/wx.h>
#include "astro/constants.h"
#include "astro/orben_data.h"

// 1:1 Port von MainWndProc aus legacy/astrouni.c
class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
    // Event-Handler für Menübefehle
    void OnExit(wxCommandEvent& event);
    void OnInfo(wxCommandEvent& event);
    void OnPerson(wxCommandEvent& event);
    void OnOrt(wxCommandEvent& event);
    void OnDruck(wxCommandEvent& event);
    void OnHoroTyp(wxCommandEvent& event);
    void OnHauser(wxCommandEvent& event);
    void OnOrben(wxCommandEvent& event);
    void OnFarben(wxCommandEvent& event);
    void OnEinst(wxCommandEvent& event);
    
    void InitializeDefaultOrbenData();  // Initialisiert Default-Orben-Werte
    
    OrbenData orben_data_;  // Orben-Daten für die Anwendung

    wxDECLARE_EVENT_TABLE();
};
