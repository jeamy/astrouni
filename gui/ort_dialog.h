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
    wxTextCtrl* lange_g_ctrl_;
    wxTextCtrl* lange_m_ctrl_;
    wxTextCtrl* lange_s_ctrl_;
    wxTextCtrl* breite_g_ctrl_;
    wxTextCtrl* breite_m_ctrl_;
    wxTextCtrl* breite_s_ctrl_;
    wxTextCtrl* zeitzone_ctrl_;
    wxRadioBox* lange_dir_ctrl_;
    wxRadioBox* breite_dir_ctrl_;
};
