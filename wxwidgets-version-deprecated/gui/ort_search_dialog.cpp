#include "ort_search_dialog.h"

// 1:1 Port von DlgOrtAuswahl aus astrofil.c
OrtSearchDialog::OrtSearchDialog(wxWindow* parent, const std::vector<astro::LegacyOrte>& all_orte)
    : wxDialog(parent, wxID_ANY, "Ort Suchen"), all_orte_(all_orte), selected_index_(-1) {

    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    search_ctrl_ = new wxTextCtrl(panel, wxID_ANY, "");
    sizer->Add(search_ctrl_, 0, wxEXPAND | wxALL, 5);

    list_box_ = new wxListBox(panel, wxID_ANY);
    sizer->Add(list_box_, 1, wxEXPAND | wxALL, 5);

    // Fülle die ListBox mit allen Orten
    for (const auto& ort : all_orte_) {
        list_box_->Append(wxString::Format("%s, %s", ort.szOrt, ort.szStaat));
    }

    wxBoxSizer* button_sizer = new wxBoxSizer(wxHORIZONTAL);
    button_sizer->Add(new wxButton(panel, wxID_OK, "OK"), 0, wxALL, 5);
    button_sizer->Add(new wxButton(panel, wxID_CANCEL, "Abbrechen"), 0, wxALL, 5);
    sizer->Add(button_sizer, 0, wxALIGN_CENTER | wxALL, 5);

    panel->SetSizerAndFit(sizer);
    Centre();

    // Event-Handler
    Bind(wxEVT_TEXT, &OrtSearchDialog::OnSearch, this, search_ctrl_->GetId());
    Bind(wxEVT_LISTBOX_DCLICK, &OrtSearchDialog::OnSelect, this, list_box_->GetId());
    Bind(wxEVT_BUTTON, &OrtSearchDialog::OnSelect, this, wxID_OK);
}

void OrtSearchDialog::OnSearch(wxCommandEvent& event) {
    // Filter-Logik (vereinfacht)
    wxString search_term = search_ctrl_->GetValue().Lower();
    list_box_->Clear();
    for (const auto& ort : all_orte_) {
        wxString full_name = wxString::Format("%s, %s", ort.szOrt, ort.szStaat).Lower();
        if (full_name.Contains(search_term)) {
            list_box_->Append(wxString::Format("%s, %s", ort.szOrt, ort.szStaat));
        }
    }
}

void OrtSearchDialog::OnSelect(wxCommandEvent& event) {
    int selection = list_box_->GetSelection();
    if (selection != wxNOT_FOUND) {
        // Finde den originalen Index
        wxString selected_string = list_box_->GetString(selection);
        for (size_t i = 0; i < all_orte_.size(); ++i) {
            wxString full_name = wxString::Format("%s, %s", all_orte_[i].szOrt, all_orte_[i].szStaat);
            if (full_name == selected_string) {
                selected_index_ = i;
                break;
            }
        }
    }
    EndModal(wxID_OK);
}
