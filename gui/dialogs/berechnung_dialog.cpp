// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgCalc from astrofil.c

#include "berechnung_dialog.h"
#include "astro/i18n.h"
#include <wx/sizer.h>

using namespace astro::i18n;

// Event Table (1:1 Legacy)
wxBEGIN_EVENT_TABLE(BerechnungDialog, wxDialog)
    EVT_BUTTON(ID_ABBRECHEN, BerechnungDialog::OnAbbrechen)
    EVT_CLOSE(BerechnungDialog::OnClose)
wxEND_EVENT_TABLE()

BerechnungDialog::BerechnungDialog(wxWindow* parent, const wxString& title)
    : wxDialog(parent, wxID_ANY, title, 
               wxDefaultPosition, wxSize(400, 200),
               wxDEFAULT_DIALOG_STYLE & ~wxCLOSE_BOX),
      berechnung_laeuft_(false),
      abgebrochen_(false),
      max_schritte_(100),
      aktueller_schritt_(0) {
    
    CreateLayout();
    Centre();
}

void BerechnungDialog::CreateLayout() {
    // Haupt-Sizer
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Beschreibung (1:1 Legacy)
    beschreibung_text_ = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Berechnung wird vorbereitet..."));
    main_sizer->Add(beschreibung_text_, 0, wxEXPAND | wxALL, 15);
    
    // Fortschritts-Balken (1:1 Legacy)
    fortschritt_gauge_ = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(-1, 25));
    main_sizer->Add(fortschritt_gauge_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 15);
    
    // Status-Text (1:1 Legacy)
    status_text_ = new wxStaticText(this, wxID_ANY, wxString::FromUTF8(""));
    main_sizer->Add(status_text_, 0, wxEXPAND | wxLEFT | wxRIGHT, 15);
    
    // Prozent-Anzeige (1:1 Legacy)
    prozent_text_ = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("0%"), 
                                    wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    main_sizer->Add(prozent_text_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 15);
    
    // Abbrechen-Button (1:1 Legacy)
    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    abbrechen_btn_ = new wxButton(this, ID_ABBRECHEN, wxString::FromUTF8("Abbrechen"));
    button_sizer->AddStretchSpacer();
    button_sizer->Add(abbrechen_btn_, 0);
    button_sizer->AddStretchSpacer();
    
    main_sizer->Add(button_sizer, 0, wxEXPAND | wxALL, 15);
    
    SetSizer(main_sizer);
}

void BerechnungDialog::StartBerechnung(const wxString& beschreibung, int max_schritte) {
    beschreibung_text_->SetLabel(beschreibung);
    max_schritte_ = max_schritte;
    aktueller_schritt_ = 0;
    berechnung_laeuft_ = true;
    abgebrochen_ = false;
    
    fortschritt_gauge_->SetRange(max_schritte);
    fortschritt_gauge_->SetValue(0);
    
    status_text_->SetLabel(wxString::FromUTF8("Wird gestartet..."));
    prozent_text_->SetLabel(wxString::FromUTF8("0%"));
    
    abbrechen_btn_->Enable(true);
    
    // Dialog anzeigen falls noch nicht sichtbar
    if (!IsShown()) {
        Show();
    }
    
    // GUI aktualisieren
    wxYield();
}

void BerechnungDialog::UpdateFortschritt(int schritt, const wxString& status) {
    if (abgebrochen_ || !berechnung_laeuft_) {
        return;
    }
    
    aktueller_schritt_ = schritt;
    
    // Fortschritts-Balken aktualisieren
    if (schritt <= max_schritte_) {
        fortschritt_gauge_->SetValue(schritt);
        
        // Prozent berechnen
        int prozent = (schritt * 100) / max_schritte_;
        prozent_text_->SetLabel(wxString::Format(wxT("%d%%"), prozent));
    }
    
    // Status-Text aktualisieren
    if (!status.IsEmpty()) {
        status_text_->SetLabel(status);
    }
    
    // GUI aktualisieren (wichtig für Responsivität)
    wxYield();
}

void BerechnungDialog::BeendeBerechnung(bool erfolgreich) {
    berechnung_laeuft_ = false;
    
    if (erfolgreich && !abgebrochen_) {
        // Erfolgreiche Beendigung
        fortschritt_gauge_->SetValue(max_schritte_);
        prozent_text_->SetLabel(wxString::FromUTF8("100%"));
        status_text_->SetLabel(wxString::FromUTF8("Berechnung abgeschlossen"));
        abbrechen_btn_->SetLabel(wxString::FromUTF8("Schließen"));
        
        // Kurz anzeigen, dann automatisch schließen
        wxYield();
        wxMilliSleep(500);
        EndModal(wxID_OK);
    } else if (abgebrochen_) {
        // Abgebrochen
        status_text_->SetLabel(wxString::FromUTF8("Berechnung abgebrochen"));
        abbrechen_btn_->SetLabel(wxString::FromUTF8("Schließen"));
        EndModal(wxID_CANCEL);
    } else {
        // Fehler
        status_text_->SetLabel(wxString::FromUTF8("Fehler bei der Berechnung"));
        abbrechen_btn_->SetLabel(wxString::FromUTF8("Schließen"));
        EndModal(wxID_CANCEL);
    }
}

void BerechnungDialog::UpdateAnzeige() {
    Layout();
    Refresh();
    Update();
}

// Event Handlers (1:1 Legacy)
void BerechnungDialog::OnAbbrechen(wxCommandEvent& event) {
    if (berechnung_laeuft_) {
        // Berechnung abbrechen
        abgebrochen_ = true;
        berechnung_laeuft_ = false;
        
        status_text_->SetLabel(wxString::FromUTF8("Wird abgebrochen..."));
        abbrechen_btn_->Enable(false);
        
        wxYield();
        
        // Dialog schließen
        EndModal(wxID_CANCEL);
    } else {
        // Dialog schließen
        EndModal(wxID_CANCEL);
    }
}

void BerechnungDialog::OnClose(wxCloseEvent& event) {
    if (berechnung_laeuft_) {
        // Berechnung abbrechen
        abgebrochen_ = true;
        berechnung_laeuft_ = false;
    }
    
    event.Skip();
}
