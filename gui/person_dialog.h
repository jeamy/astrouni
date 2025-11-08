#pragma once

#include <wx/wx.h>
#include "astro/data_types.h"

// 1:1 Port von DlgPErfassen aus astrofil.c
class PersonDialog : public wxDialog {
public:
    PersonDialog(wxWindow* parent, astro::LegacyRadix& radix_data);

private:
    void OnOk(wxCommandEvent& event);
    void OnSearchName(wxCommandEvent& event);
    void OnSearchOrt(wxCommandEvent& event);

    astro::LegacyRadix& radix_data_;

    // UI-Elemente
    wxTextCtrl* vorname_ctrl_;
    wxTextCtrl* name_ctrl_;
    wxTextCtrl* ort_ctrl_;
    wxTextCtrl* zeit_ctrl_;
    wxTextCtrl* datum_ctrl_;
    wxTextCtrl* beruf_ctrl_;
    wxTextCtrl* sommer_ctrl_;
    wxTextCtrl* zeitzone_ctrl_;
};
