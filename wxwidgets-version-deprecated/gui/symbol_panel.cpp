#include "symbol_panel.h"

PlanetOrAspectSymbolPanel::PlanetOrAspectSymbolPanel(wxWindow* parent, wxWindowID id, const wxString& symbol, const wxFont& font)
    : wxPanel(parent, id), symbol_(symbol), font_(font) {
    Bind(wxEVT_PAINT, &PlanetOrAspectSymbolPanel::OnPaint, this);
}

void PlanetOrAspectSymbolPanel::SetSymbol(const wxString& symbol) {
    symbol_ = symbol;
    Refresh();
}

void PlanetOrAspectSymbolPanel::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);
    dc.SetFont(font_);
    wxCoord width, height;
    dc.GetTextExtent(symbol_, &width, &height);
    wxSize size = GetSize();
    dc.DrawText(symbol_, (size.GetWidth() - width) / 2, (size.GetHeight() - height) / 2);
}
