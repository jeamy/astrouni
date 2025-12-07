#pragma once

#include <wx/wx.h>

// 1:1 Port von DlgHausAuswahl aus astrofil.c
class HauserDialog : public wxDialog {
public:
    HauserDialog(wxWindow* parent, int& selected_system);

private:
    void OnOk(wxCommandEvent& event);

    int& selected_system_;

    // UI-Elemente
    wxRadioBox* radio_box_;
};
