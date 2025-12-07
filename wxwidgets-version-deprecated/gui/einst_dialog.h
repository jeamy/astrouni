#pragma once

#include <wx/wx.h>

class EinstDialog : public wxDialog {
public:
    EinstDialog(wxWindow* parent);

private:
    void OnOk(wxCommandEvent& event);
};
