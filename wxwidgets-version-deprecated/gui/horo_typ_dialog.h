#pragma once

#include <wx/wx.h>

class HoroTypDialog : public wxDialog {
public:
    HoroTypDialog(wxWindow* parent, int& selected_typ);

private:
    void OnOk(wxCommandEvent& event);

    int& selected_typ_;
    wxRadioBox* radio_box_;
};
