#pragma once

#include <wx/wx.h>

class OrbenDialog : public wxDialog {
public:
    OrbenDialog(wxWindow* parent);

private:
    void OnOk(wxCommandEvent& event);
};
