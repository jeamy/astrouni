#pragma once

#include <wx/wx.h>
#include "astro/data_types.h"

// 1:1 Port von DlgOErfassen aus astrofil.c
class OrtDialog : public wxDialog {
public:
    OrtDialog(wxWindow* parent, astro::LegacyOrte& ort_data);

private:
    void OnOk(wxCommandEvent& event);

    astro::LegacyOrte& ort_data_;

    // UI-Elemente
    wxTextCtrl* ort_ctrl_;
    wxTextCtrl* staat_ctrl_;
    // ... weitere UI-Elemente für Koordinaten, etc.
};
