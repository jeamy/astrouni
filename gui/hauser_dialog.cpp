#include "hauser_dialog.h"

// 1:1 Port von DlgHausAuswahl aus astrofil.c
HauserDialog::HauserDialog(wxWindow* parent, int& selected_system)
    : wxDialog(parent, wxID_ANY, "Häusersystem Wählen"), 
      selected_system_(selected_system) {

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    wxString choices[] = {
        "Placidus", "Koch", "Equal", "Campanus", "Meridian", "Regiomontanus", "Topocentric", "Morinus"
    };
    radio_box_ = new wxRadioBox(this, wxID_ANY, "Häusersystem", wxDefaultPosition, wxDefaultSize, WXSIZEOF(choices), choices, 1, wxRA_SPECIFY_COLS);
    
    // Validiere selected_system_ vor SetSelection
    if (selected_system_ >= 0 && selected_system_ < 8) {
        radio_box_->SetSelection(selected_system_);
    } else {
        radio_box_->SetSelection(0); // Default: Placidus
    }
    
    sizer->Add(radio_box_, 1, wxEXPAND | wxALL, 10);

    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    button_sizer->Add(new wxButton(this, wxID_OK, "OK"), 0, wxALL, 5);
    button_sizer->Add(new wxButton(this, wxID_CANCEL, "Abbrechen"), 0, wxALL, 5);
    sizer->Add(button_sizer, 0, wxALIGN_CENTER | wxALL, 5);

    SetSizerAndFit(sizer);
    Centre();

    // Event-Handler
    Bind(wxEVT_BUTTON, &HauserDialog::OnOk, this, wxID_OK);
}

void HauserDialog::OnOk(wxCommandEvent& event) {
    selected_system_ = radio_box_->GetSelection();
    EndModal(wxID_OK);
}
