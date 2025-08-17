#include "LegacyPortRenderer.hpp"
#include "LegacyGDIAdapter.hpp"

#include <cmath>
#include <numbers>

namespace LegacyPort {

void DrawSignRing(wxGraphicsContext* gc, const wxPoint& center, double radius) {
    if (!gc) return;

    LegacyGDIAdapter g(gc);

    // Geometry for the sign ring band
    const double rInner = radius + 12.0;
    const double rOuter = radius + 52.0;

    // Pens
    const wxColour ringColor(176, 176, 176);
    const wxColour minorColor(196, 196, 196);
    const wxColour microColor(210, 210, 210);

    // Inner and outer circles defining the band
    g.SetPen(ringColor, 1.0, wxPENSTYLE_SOLID);
    g.StrokeCircle(center.x, center.y, rInner);
    g.StrokeCircle(center.x, center.y, rOuter);

    // Ticks: draw within the band
    // - Major (30°): span whole band
    // - Medium (10°, not 30°): span middle 60% of the band
    // - Small (5°, not 10°): span inner 35% of the band
    for (int deg = 0; deg < 360; deg += 5) {
        bool isMajor = (deg % 30) == 0;
        bool isTen   = (!isMajor && (deg % 10) == 0);
        bool isFive  = (!isMajor && !isTen);

        double a = (90.0 - deg) * std::numbers::pi / 180.0;

        double startR, endR; wxColour col; double w = 1.0;
        if (isMajor) {
            startR = rInner;
            endR   = rOuter;
            col    = ringColor;
            w      = 1.0;
        } else if (isTen) {
            startR = rInner + (rOuter - rInner) * 0.2;
            endR   = rInner + (rOuter - rInner) * 0.8;
            col    = minorColor;
            w      = 1.0;
        } else { // 5°
            startR = rInner + (rOuter - rInner) * 0.35;
            endR   = rInner + (rOuter - rInner) * 0.65;
            col    = microColor;
            w      = 1.0;
        }

        g.SetPen(col, w, wxPENSTYLE_SOLID);
        double x1 = center.x + std::cos(a) * startR;
        double y1 = center.y - std::sin(a) * startR;
        double x2 = center.x + std::cos(a) * endR;
        double y2 = center.y - std::sin(a) * endR;
        g.MoveTo(x1, y1);
        g.LineTo(x2, y2);
    }
}

} // namespace LegacyPort
