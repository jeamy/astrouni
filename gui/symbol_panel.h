#pragma once

#include <wx/wx.h>

class PlanetOrAspectSymbolPanel : public wxPanel {
public:
    PlanetOrAspectSymbolPanel(wxWindow* parent, wxWindowID id, const wxString& symbol, const wxFont& font);
    void SetSymbol(const wxString& symbol);

private:
    void OnPaint(wxPaintEvent& event);

    wxString symbol_;
    wxFont font_;
};
