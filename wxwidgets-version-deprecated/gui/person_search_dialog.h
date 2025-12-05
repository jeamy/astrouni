#pragma once

#include <wx/wx.h>
#include <vector>
#include "astro/data_types.h"

// 1:1 Port von DlgPHProc aus astrofil.c
class PersonSearchDialog : public wxDialog {
public:
    PersonSearchDialog(wxWindow* parent, const std::vector<astro::LegacyRadix>& all_persons);

    int GetSelectedIndex() const { return selected_index_; }

private:
    void OnSearch(wxCommandEvent& event);
    void OnSelect(wxCommandEvent& event);

    int selected_index_;
    std::vector<astro::LegacyRadix> all_persons_;

    // UI-Elemente
    wxTextCtrl* search_ctrl_;
    wxListBox* list_box_;
};
