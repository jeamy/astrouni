#include "main_frame.h"
#include "person_dialog.h"
#include "ort_dialog.h"

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_BUTTON(static_cast<int>(astro::MenuID::PERSON), MainFrame::OnPerson)
    EVT_MENU(static_cast<int>(astro::MenuID::PERSON), MainFrame::OnPerson)
    EVT_BUTTON(static_cast<int>(astro::MenuID::ORT), MainFrame::OnOrt)
    EVT_MENU(static_cast<int>(astro::MenuID::ORT), MainFrame::OnOrt)
    EVT_MENU(static_cast<int>(astro::MenuID::DRUCK), MainFrame::OnDruck)
    EVT_MENU(static_cast<int>(astro::MenuID::APP_EXIT), MainFrame::OnExit)
    EVT_MENU(static_cast<int>(astro::MenuID::HOROTYP), MainFrame::OnHoroTyp)
    EVT_MENU(static_cast<int>(astro::MenuID::HAUSER), MainFrame::OnHauser)
    EVT_MENU(static_cast<int>(astro::MenuID::ORBEN), MainFrame::OnOrben)
    EVT_MENU(static_cast<int>(astro::MenuID::FARBEN), MainFrame::OnFarben)
    EVT_MENU(static_cast<int>(astro::MenuID::EINST), MainFrame::OnEinst)
    EVT_MENU(static_cast<int>(astro::MenuID::APP_INFO), MainFrame::OnInfo)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size) {

    // --- MENÜLEISTE TEMPORÄR DEAKTIVIERT WEGEN wxWidgets ASSERT-FEHLER ---
    /*
    // 1:1 Port der Menü-Erstellung aus legacy/astrouni.rc
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(static_cast<int>(astro::MenuID::PERSON), "&Personen...\tCtrl-P");
    menuFile->Append(static_cast<int>(astro::MenuID::ORT), "&Orte...\tCtrl-O");
    menuFile->AppendSeparator();
    menuFile->Append(static_cast<int>(astro::MenuID::DRUCK), "&Drucken...\tCtrl-D");
    menuFile->AppendSeparator();
    menuFile->Append(static_cast<int>(astro::MenuID::APP_EXIT), "&Beenden");

    wxMenu *menuOptions = new wxMenu;
    menuOptions->Append(static_cast<int>(astro::MenuID::HOROTYP), "&Horoskop-Typ...");
    menuOptions->Append(static_cast<int>(astro::MenuID::HAUSER), "&Häusersystem...");
    menuOptions->Append(static_cast<int>(astro::MenuID::ORBEN), "&Orben...");
    menuOptions->Append(static_cast<int>(astro::MenuID::FARBEN), "&Farben...");
    menuOptions->Append(static_cast<int>(astro::MenuID::EINST), "&Einstellungen...");

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(static_cast<int>(astro::MenuID::APP_INFO), "&Info...");

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&Datei");
    menuBar->Append(menuOptions, "&Optionen");
    menuBar->Append(menuHelp, "&Hilfe");

    SetMenuBar(menuBar);
    */

    CreateStatusBar();
    SetStatusText("Willkommen bei AstroUniverse 1:1 Port!");

    // Temporäre Buttons als Ersatz für die defekte Menüleiste
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* row1 = new wxBoxSizer(wxHORIZONTAL);
    row1->Add(new wxButton(panel, static_cast<int>(astro::MenuID::PERSON), "Personen-Dialog"), 0, wxALL, 5);
    row1->Add(new wxButton(panel, static_cast<int>(astro::MenuID::ORT), "Orte-Dialog"), 0, wxALL, 5);
    sizer->Add(row1);

    wxBoxSizer* row2 = new wxBoxSizer(wxHORIZONTAL);
    row2->Add(new wxButton(panel, static_cast<int>(astro::MenuID::HOROTYP), "Horoskop-Typ"), 0, wxALL, 5);
    row2->Add(new wxButton(panel, static_cast<int>(astro::MenuID::HAUSER), "Häusersystem"), 0, wxALL, 5);
    row2->Add(new wxButton(panel, static_cast<int>(astro::MenuID::ORBEN), "Orben"), 0, wxALL, 5);
    row2->Add(new wxButton(panel, static_cast<int>(astro::MenuID::FARBEN), "Farben"), 0, wxALL, 5);
    row2->Add(new wxButton(panel, static_cast<int>(astro::MenuID::EINST), "Einstellungen"), 0, wxALL, 5);
    sizer->Add(row2);

    panel->SetSizer(sizer);
}

void MainFrame::OnExit(wxCommandEvent& event) {
    Close(true);
}

void MainFrame::OnInfo(wxCommandEvent& event) {
    wxMessageBox("AstroUniverse v0.04Beta\n\n1:1 Portierung nach C++ mit wxWidgets", "Info", wxOK | wxICON_INFORMATION);
}

// Placeholder für die restlichen Dialoge

void MainFrame::OnPerson(wxCommandEvent& event) {
    // Dummy-Daten für den Dialog
    astro::LegacyRadix dummy_radix;
    strcpy(dummy_radix.szName, "Mustermann");
    strcpy(dummy_radix.szVorName, "Max");

    PersonDialog dlg(this, dummy_radix);
    if (dlg.ShowModal() == wxID_OK) {
        // Daten wurden gespeichert, hier könnte man sie weiterverarbeiten
        wxLogMessage("Gespeicherter Name: %s, %s", dummy_radix.szName, dummy_radix.szVorName);
    }
}

void MainFrame::OnOrt(wxCommandEvent& event) {
    // Dummy-Daten für den Dialog
    astro::LegacyOrte dummy_ort;
    strcpy(dummy_ort.szOrt, "Berlin");
    strcpy(dummy_ort.szStaat, "Deutschland");

    OrtDialog dlg(this, dummy_ort);
    if (dlg.ShowModal() == wxID_OK) {
        wxLogMessage("Gespeicherter Ort: %s, %s", dummy_ort.szOrt, dummy_ort.szStaat);
    }
}
void MainFrame::OnDruck(wxCommandEvent& event) { wxMessageBox("Drucken-Dialog (noch nicht implementiert)", "Platzhalter"); }
void MainFrame::OnHoroTyp(wxCommandEvent& event) { wxMessageBox("Horoskop-Typ-Dialog (noch nicht implementiert)", "Platzhalter"); }
void MainFrame::OnHauser(wxCommandEvent& event) { wxMessageBox("Häusersystem-Dialog (noch nicht implementiert)", "Platzhalter"); }
void MainFrame::OnOrben(wxCommandEvent& event) { wxMessageBox("Orben-Dialog (noch nicht implementiert)", "Platzhalter"); }
void MainFrame::OnFarben(wxCommandEvent& event) { wxMessageBox("Farben-Dialog (noch nicht implementiert)", "Platzhalter"); }
void MainFrame::OnEinst(wxCommandEvent& event) { wxMessageBox("Einstellungen-Dialog (noch nicht implementiert)", "Platzhalter"); }
