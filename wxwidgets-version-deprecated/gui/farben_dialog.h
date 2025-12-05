#pragma once

#include <wx/wx.h>

class FarbenDialog : public wxDialog {
public:
    FarbenDialog(wxWindow* parent);

private:
    void OnOk(wxCommandEvent& event);
};
