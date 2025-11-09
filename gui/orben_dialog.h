#pragma once

#include <wx/wx.h>

class PlanetOrAspectSymbolPanel;

class OrbenDialog : public wxDialog {
public:
    OrbenDialog(wxWindow* parent);

private:
    void OnOk(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
};
