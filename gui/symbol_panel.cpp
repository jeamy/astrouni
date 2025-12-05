#include "symbol_panel.h"
#include "astro/colors.h"

PlanetOrAspectSymbolPanel::PlanetOrAspectSymbolPanel(wxWindow* parent, wxWindowID id, const wxString& symbol, const wxFont& font)
    : wxPanel(parent, id), symbol_(symbol), font_(font) {
    Bind(wxEVT_PAINT, &PlanetOrAspectSymbolPanel::OnPaint, this);
    
    // Lade Farben beim ersten Symbol-Panel
    static bool colors_loaded = false;
    if (!colors_loaded) {
        AstroColors::instance().LoadFromFile();
        colors_loaded = true;
    }
}

void PlanetOrAspectSymbolPanel::SetSymbol(const wxString& symbol) {
    symbol_ = symbol;
    Refresh();
}

void PlanetOrAspectSymbolPanel::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);
    dc.SetFont(font_);
    
    // Setze Farbe basierend auf Symbol (Planeten = Blau, Aspekte = Grau)
    dc.SetTextForeground(AstroColors::instance().GetPlanetColor());
    
    wxCoord width, height;
    dc.GetTextExtent(symbol_, &width, &height);
    wxSize size = GetSize();
    dc.DrawText(symbol_, (size.GetWidth() - width) / 2, (size.GetHeight() - height) / 2);
}
