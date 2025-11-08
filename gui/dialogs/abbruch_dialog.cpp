// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project
// 1:1 Port of DlgAbort from astrofil.c

#include "abbruch_dialog.h"
#include "astro/i18n.h"
#include <wx/sizer.h>

using namespace astro::i18n;

// Event Table (1:1 Legacy)
wxBEGIN_EVENT_TABLE(AbbruchDialog, wxDialog)
    EVT_BUTTON(ID_ABBRECHEN_ABORT, AbbruchDialog::OnAbbrechen)
    EVT_CLOSE(AbbruchDialog::OnClose)
    EVT_TIMER(ID_AUTO_CLOSE_TIMER, AbbruchDialog::OnTimer)
wxEND_EVENT_TABLE()

AbbruchDialog::AbbruchDialog(wxWindow* parent, const wxString& title)
    : wxDialog(parent, wxID_ANY, title, 
               wxDefaultPosition, wxSize(350, 150),
               wxDEFAULT_DIALOG_STYLE & ~wxCLOSE_BOX),
      abgebrochen_(false),
      auto_close_(false),
      auto_close_delay_(1000) {
    
    auto_close_timer_ = new wxTimer(this, ID_AUTO_CLOSE_TIMER);
    
    CreateLayout();
    Centre();
}

void AbbruchDialog::CreateLayout() {
    // Haupt-Sizer
    wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Status-Text (1:1 Legacy)
    status_text_ = new wxStaticText(this, wxID_ANY, wxString::FromUTF8("Berechnung wird durchgeführt..."));
    main_sizer->Add(status_text_, 0, wxEXPAND | wxALL, 15);
    
    // Fortschritts-Balken (1:1 Legacy)
    fortschritt_gauge_ = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(-1, 20));
    fortschritt_gauge_->SetValue(0);
    main_sizer->Add(fortschritt_gauge_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 15);
    
    // Abbrechen-Button (1:1 Legacy)
    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    abbrechen_btn_ = new wxButton(this, ID_ABBRECHEN_ABORT, wxString::FromUTF8("Abbrechen"));
    button_sizer->AddStretchSpacer();
    button_sizer->Add(abbrechen_btn_, 0);
    button_sizer->AddStretchSpacer();
    
    main_sizer->Add(button_sizer, 0, wxEXPAND | wxALL, 15);
    
    SetSizer(main_sizer);
}

void AbbruchDialog::UpdateFortschritt(const wxString& status, int prozent) {
    // Status-Text aktualisieren
    if (!status.IsEmpty()) {
        status_text_->SetLabel(status);
    }
    
    // Fortschritt aktualisieren
    if (prozent >= 0 && prozent <= 100) {
        fortschritt_gauge_->SetValue(prozent);
        
        // Bei 100% Auto-Close starten falls aktiviert
        if (prozent >= 100 && auto_close_) {
            abbrechen_btn_->SetLabel(wxString::FromUTF8("Schließen"));
            auto_close_timer_->StartOnce(auto_close_delay_);
        }
    }
    
    // GUI aktualisieren
    Layout();
    Update();
    wxYield();
}

void AbbruchDialog::SetzeMaximum(int maximum) {
    fortschritt_gauge_->SetRange(maximum);
}

void AbbruchDialog::SetAutoClose(bool auto_close, int delay_ms) {
    auto_close_ = auto_close;
    auto_close_delay_ = delay_ms;
}

// Event Handlers (1:1 Legacy)
void AbbruchDialog::OnAbbrechen(wxCommandEvent& event) {
    if (fortschritt_gauge_->GetValue() >= 100) {
        // Berechnung fertig - Dialog schließen
        EndModal(wxID_OK);
    } else {
        // Berechnung abbrechen
        abgebrochen_ = true;
        status_text_->SetLabel(wxString::FromUTF8("Wird abgebrochen..."));
        abbrechen_btn_->Enable(false);
        
        // Kurz warten, dann schließen
        auto_close_timer_->StartOnce(500);
    }
}

void AbbruchDialog::OnClose(wxCloseEvent& event) {
    // Berechnung abbrechen
    abgebrochen_ = true;
    event.Skip();
}

void AbbruchDialog::OnTimer(wxTimerEvent& event) {
    // Auto-Close Timer
    if (abgebrochen_) {
        EndModal(wxID_CANCEL);
    } else {
        EndModal(wxID_OK);
    }
}
