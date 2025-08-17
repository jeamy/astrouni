#pragma once

#include <wx/wx.h>
#include <wx/graphics.h>

namespace LegacyPort {

// Draw the zodiac sign ring (band + ticks) in a legacy-inspired style
// center: chart center in device coords
// radius: base radius of the inner chart circle
void DrawSignRing(wxGraphicsContext* gc, const wxPoint& center, double radius);

}
