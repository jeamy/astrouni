#pragma once

#include <wx/wx.h>
#include <vector>
#include "astro/data_types.h"

// 1:1 Port von DlgOrtAuswahl aus astrofil.c
class OrtSearchDialog : public wxDialog {
public:
    OrtSearchDialog(wxWindow* parent, const std::vector<astro::LegacyOrte>& all_orte);

    int GetSelectedIndex() const { return selected_index_; }

private:
    void OnSearch(wxCommandEvent& event);
    void OnSelect(wxCommandEvent& event);

    int selected_index_;
    std::vector<astro::LegacyOrte> all_orte_;

    // UI-Elemente
    wxTextCtrl* search_ctrl_;
    wxListBox* list_box_;
};
