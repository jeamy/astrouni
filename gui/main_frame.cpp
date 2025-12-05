#include "main_frame.h"
#include "person_dialog.h"
#include "ort_dialog.h"
#include "hauser_dialog.h"
#include "horo_typ_dialog.h"
#include "orben_dialog.h"
#include "farben_dialog.h"
#include "einst_dialog.h"
#include "symbol_panel.h"
#include "../database/orben_file_manager.h"

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size) {

    // Initialisiere Default-Orben-Werte
    InitializeDefaultOrbenData();

    // 1:1 Port der MAINMENU Menüstruktur aus astrouni.rc
    wxMenuBar* menuBar = new wxMenuBar();

    // &Datei
    wxMenu* menuDatei = new wxMenu();
    menuDatei->Append(wxID_EXIT, wxT("&Beenden"));
    menuBar->Append(menuDatei, wxT("&Datei"));
    Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);

    // &Erfassen
    wxMenu* menuErfassen = new wxMenu();
    int idPerson = wxWindow::NewControlId();
    int idOrt = wxWindow::NewControlId();
    menuErfassen->Append(idPerson, wxT("&Personen Erfassen"));
    menuErfassen->Append(idOrt, wxT("&Orte Erfassen"));
    menuBar->Append(menuErfassen, wxT("&Erfassen"));
    Bind(wxEVT_MENU, &MainFrame::OnPerson, this, idPerson);
    Bind(wxEVT_MENU, &MainFrame::OnOrt, this, idOrt);

    // &Horoskop
    wxMenu* menuHoroskop = new wxMenu();
    int idHorotyp = wxWindow::NewControlId();
    int idHauser = wxWindow::NewControlId();
    int idOrben = wxWindow::NewControlId();
    int idFarben = wxWindow::NewControlId();
    int idEinst = wxWindow::NewControlId();
    menuHoroskop->Append(idHorotyp, wxT("&Horoskoptyp"));
    menuHoroskop->Append(idHauser, wxT("&Häusersystem"));
    menuHoroskop->Append(idOrben, wxT("&Orben"));
    menuHoroskop->Append(idFarben, wxT("&Farben"));
    menuHoroskop->Append(idEinst, wxT("&Einstellungen"));
    menuBar->Append(menuHoroskop, wxT("&Horoskop"));
    Bind(wxEVT_MENU, &MainFrame::OnHoroTyp, this, idHorotyp);
    Bind(wxEVT_MENU, &MainFrame::OnHauser, this, idHauser);
    Bind(wxEVT_MENU, &MainFrame::OnOrben, this, idOrben);
    Bind(wxEVT_MENU, &MainFrame::OnFarben, this, idFarben);
    Bind(wxEVT_MENU, &MainFrame::OnEinst, this, idEinst);

    // &Hilfe
    wxMenu* menuHilfe = new wxMenu();
    int idInfo = wxWindow::NewControlId();
    menuHilfe->Append(idInfo, wxT("&Info über..."));
    menuBar->Append(menuHilfe, wxT("&Hilfe"));
    Bind(wxEVT_MENU, &MainFrame::OnInfo, this, idInfo);

    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText("Willkommen bei AstroUniverse 1:1 Port!");
}

void MainFrame::OnExit(wxCommandEvent&) { Close(true); }

void MainFrame::OnInfo(wxCommandEvent&) { 
    wxMessageBox("AstroUniverse v0.04Beta\n\n1:1 Portierung nach C++ mit wxWidgets", "Info", wxOK | wxICON_INFORMATION); 
}

void MainFrame::OnPerson(wxCommandEvent&) {
    astro::LegacyRadix dummy_radix;
    strcpy(dummy_radix.szName, "Mustermann");
    strcpy(dummy_radix.szVorName, "Max");
    
    PersonDialog dlg(this, dummy_radix);
    if (dlg.ShowModal() == wxID_OK) {
        wxLogMessage("Gespeicherter Name: %s, %s", dummy_radix.szName, dummy_radix.szVorName);
    }
}

void MainFrame::OnOrt(wxCommandEvent&) {
    astro::LegacyOrte dummy_ort;
    strcpy(dummy_ort.szOrt, "Berlin");
    strcpy(dummy_ort.szStaat, "Deutschland");

    OrtDialog dlg(this, dummy_ort);
    if (dlg.ShowModal() == wxID_OK) {
        wxLogMessage("Gespeicherter Ort: %s, %s", dummy_ort.szOrt, dummy_ort.szStaat);
    }
}

void MainFrame::OnHauser(wxCommandEvent&) {
    int selected_system = 0;
    HauserDialog dlg(this, selected_system);
    dlg.ShowModal();
}

void MainFrame::OnDruck(wxCommandEvent&) { wxMessageBox("Drucken-Dialog (noch nicht implementiert)", "Platzhalter"); }
void MainFrame::OnHoroTyp(wxCommandEvent&) {
    int selected_typ = 0; // Radix
    HoroTypDialog dlg(this, selected_typ);
    dlg.ShowModal();
}
void MainFrame::OnOrben(wxCommandEvent&) {
    OrbenDialog dlg(this, orben_data_);
    dlg.ShowModal();
}

void MainFrame::InitializeDefaultOrbenData() {
    // Versuche Orben-Daten aus astroini.dat zu laden
    OrbenFileManager file_mgr;
    if (!file_mgr.LoadFromIniFile(orben_data_)) {
        // Falls keine Datei vorhanden, verwende Hard-Coded Defaults
        file_mgr.InitializeHardcodedDefaults(orben_data_);
    }
}
void MainFrame::OnFarben(wxCommandEvent&) {
    FarbenDialog dlg(this);
    dlg.ShowModal();
}
void MainFrame::OnEinst(wxCommandEvent&) {
    EinstDialog dlg(this);
    dlg.ShowModal();
}
