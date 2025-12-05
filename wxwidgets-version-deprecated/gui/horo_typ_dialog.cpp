#include "horo_typ_dialog.h"

// 1:1 Port von DlgHoroAuswahl aus astrofil.c
HoroTypDialog::HoroTypDialog(wxWindow* parent, int& selected_typ)
    : wxDialog(parent, wxID_ANY, "Horoskop-Typ Wählen", wxDefaultPosition, wxDefaultSize), 
      selected_typ_(selected_typ) {
    SetBackgroundColour(*wxWHITE);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    wxString choices[] = {
        "Radix", "Synastrie", "Kombin", "Komposit", "Transit"
    };
    radio_box_ = new wxRadioBox(this, wxID_ANY, "Horoskop-Typ", wxDefaultPosition, wxDefaultSize, WXSIZEOF(choices), choices, 1, wxRA_SPECIFY_COLS);
    
    if (selected_typ_ >= 0 && selected_typ_ < 5) {
        radio_box_->SetSelection(selected_typ_);
    } else {
        radio_box_->SetSelection(0); // Default: Radix
    }
    
    sizer->Add(radio_box_, 1, wxEXPAND | wxALL, 10);

    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    button_sizer->Add(new wxButton(this, wxID_OK, "OK"), 0, wxALL, 5);
    button_sizer->Add(new wxButton(this, wxID_CANCEL, "Abbrechen"), 0, wxALL, 5);
    sizer->Add(button_sizer, 0, wxALIGN_CENTER | wxALL, 5);

    SetSizerAndFit(sizer);
    Centre();

    Bind(wxEVT_BUTTON, &HoroTypDialog::OnOk, this, wxID_OK);
}

void HoroTypDialog::OnOk(wxCommandEvent& event) {
    selected_typ_ = radio_box_->GetSelection();
    EndModal(wxID_OK);
}
