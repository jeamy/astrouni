#pragma once

#include <wx/wx.h>
#include "astro/data_types.h"

// 1:1 Port von DlgPErfassen aus astrofil.c
class PersonDialog : public wxDialog {
public:
    PersonDialog(wxWindow* parent, astro::LegacyRadix& radix_data);

private:
    void OnOk(wxCommandEvent& event);

    astro::LegacyRadix& radix_data_;

    // UI-Elemente
    wxTextCtrl* name_ctrl_;
    wxTextCtrl* vorname_ctrl_;
    // ... weitere UI-Elemente für Datum, Ort, etc.
};
