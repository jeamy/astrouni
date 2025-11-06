// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project

#include "main_frame.h"
#include "dialogs/new_chart_dialog.h"
#include "astro/i18n.h"
#include <wx/filedlg.h>
#include <wx/msgdlg.h>

using namespace astro::i18n;

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(ID_New, MainFrame::OnNew)
    EVT_MENU(ID_Open, MainFrame::OnOpen)
    EVT_MENU(ID_Save, MainFrame::OnSave)
    EVT_MENU(ID_ExportPNG, MainFrame::OnExportPNG)
    EVT_MENU(ID_ExportSVG, MainFrame::OnExportSVG)
    EVT_MENU(wxID_EXIT, MainFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(1024, 768)) {
    
    // Menü erstellen (mit i18n)
    wxMenu* file_menu = new wxMenu;
    wxString new_label = tr("menu.new");
    if (new_label.IsEmpty()) new_label = "Neues Chart...";
    file_menu->Append(ID_New, new_label + "\tCtrl-N");
    
    wxString open_label = tr("menu.open");
    if (open_label.IsEmpty()) open_label = "Öffnen...";
    file_menu->Append(ID_Open, open_label + "\tCtrl-O");
    
    wxString save_label = tr("menu.save");
    if (save_label.IsEmpty()) save_label = "Speichern...";
    file_menu->Append(ID_Save, save_label + "\tCtrl-S");
    
    file_menu->AppendSeparator();
    
    wxString export_png = tr("menu.export_png");
    if (export_png.IsEmpty()) export_png = "Export als PNG...";
    file_menu->Append(ID_ExportPNG, export_png);
    
    wxString export_svg = tr("menu.export_svg");
    if (export_svg.IsEmpty()) export_svg = "Export als SVG...";
    file_menu->Append(ID_ExportSVG, export_svg);
    
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, "Beenden");
    
    wxMenu* help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT, "Über...");
    
    menu_bar_ = new wxMenuBar;
    wxString file_menu_label = tr("menu.file");
    if (file_menu_label.IsEmpty()) file_menu_label = "Datei";
    menu_bar_->Append(file_menu, file_menu_label);
    
    wxString help_menu_label = tr("menu.help");
    if (help_menu_label.IsEmpty()) help_menu_label = "Hilfe";
    menu_bar_->Append(help_menu, help_menu_label);
    
    SetMenuBar(menu_bar_);
    
    // Statusleiste
    status_bar_ = CreateStatusBar(2);
    SetStatusText(tr("status.ready"), 0);
    
    // Chart Panel erstellen
    chart_panel_ = new ChartPanel(this);
    // Layout: Panel füllt das Fenster
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(chart_panel_, 1, wxEXPAND);
    SetSizer(sizer);

    // Zentrieren
    Centre();
}

void MainFrame::OnNew(wxCommandEvent& event) {
    NewChartDialog dialog(this);
    if (dialog.ShowModal() != wxID_OK) return;

    SetStatusText(tr("status.ready"), 0);

    try {
        // Daten aus Dialog
        astro::RadixData radix = dialog.GetRadixData();

        // Ephemeris
        auto ephemeris = std::make_shared<astro::SwissEphemeris>();
        ephemeris->initialize("../swisseph/ephe");

        // Berechnen
        auto calculator = std::make_unique<astro::AstroCalculator>(ephemeris);
        auto chart = calculator->calculate_chart(
            radix,
            astro::HouseSystem::Placidus,
            astro::AstroCalculator::DEFAULT_PLANETS,
            astro::AstroCalculator::DEFAULT_ASPECTS,
            8.0
        );

        // Anzeigen
        chart_panel_->SetChart(chart);
        SetStatusText(tr("status.chart_created"), 0);

    } catch (const std::exception& e) {
        wxMessageBox(wxString::FromUTF8(e.what()), tr("menu.error"), wxOK | wxICON_ERROR);
    }
}

void MainFrame::OnOpen(wxCommandEvent& event) {
    wxFileDialog dialog(this, tr("dialog.open_chart"), "", "",
                       "AstroUniverse Charts (*.auc)|*.auc",
                       wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    
    if (dialog.ShowModal() == wxID_OK) {
        wxString path = dialog.GetPath();
        SetStatusText(tr("status.file_opened") + " " + path, 0);
        // TODO: Chart laden
    }
}

void MainFrame::OnSave(wxCommandEvent& event) {
    wxFileDialog dialog(this, tr("dialog.save_chart"), "", "",
                       "AstroUniverse Charts (*.auc)|*.auc",
                       wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    
    if (dialog.ShowModal() == wxID_OK) {
        wxString path = dialog.GetPath();
        SetStatusText(tr("status.file_saved") + " " + path, 0);
        // TODO: Chart speichern
    }
}

void MainFrame::OnExportPNG(wxCommandEvent& event) {
    wxFileDialog dialog(this, tr("dialog.export_png"), "", "",
                       "PNG Bilder (*.png)|*.png",
                       wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    
    if (dialog.ShowModal() == wxID_OK) {
        wxString path = dialog.GetPath();
        SetStatusText(tr("status.exported") + " " + path, 0);
        // TODO: PNG Export
    }
}

void MainFrame::OnExportSVG(wxCommandEvent& event) {
    wxFileDialog dialog(this, tr("dialog.export_svg"), "", "",
                       "SVG Vektorgrafiken (*.svg)|*.svg",
                       wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    
    if (dialog.ShowModal() == wxID_OK) {
        wxString path = dialog.GetPath();
        SetStatusText(tr("status.exported") + " " + path, 0);
        // TODO: SVG Export
    }
}

void MainFrame::OnExit(wxCommandEvent& event) {
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& event) {
    wxString about_text = 
        "AstroUniverse - Astrologie-Software\n\n"
        "Version 0.1.0 (Phase 3 - GUI)\n"
        "Copyright (C) 2025 AstroUniverse Project\n\n"
        "Lizenz: GPL v2\n"
        "Swiss Ephemeris: GPL v2 / LGPL\n\n"
        "Language: " + wxString(I18n::instance().get_language() == Language::German ? "Deutsch" : "English");
    
    wxMessageBox(about_text, tr("menu.about"), wxOK | wxICON_INFORMATION);
}
