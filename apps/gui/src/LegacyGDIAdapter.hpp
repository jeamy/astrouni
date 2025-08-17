#pragma once

#include <wx/wx.h>
#include <wx/graphics.h>

// Minimal adapter to map a subset of legacy GDI-style API to wxGraphicsContext
class LegacyGDIAdapter {
public:
    explicit LegacyGDIAdapter(wxGraphicsContext* gc)
        : m_gc(gc), m_hasCurrentPoint(false), m_cx(0.0), m_cy(0.0) {}

    wxGraphicsContext* Get() const { return m_gc; }

    // Pen controls
    void SetPen(const wxColour& color, double width = 1.0, wxPenStyle style = wxPENSTYLE_SOLID) {
        wxPen pen(color, std::max(1, static_cast<int>(std::round(width))));
        pen.SetStyle(style);
        m_pen = pen;
        if (m_gc) m_gc->SetPen(m_pen);
    }

    // MoveTo/LineTo like GDI
    void MoveTo(double x, double y) {
        m_cx = x; m_cy = y; m_hasCurrentPoint = true;
    }

    void LineTo(double x, double y) {
        if (!m_gc) return;
        if (!m_hasCurrentPoint) { m_cx = x; m_cy = y; m_hasCurrentPoint = true; return; }
        m_gc->SetPen(m_pen);
        m_gc->StrokeLine(m_cx, m_cy, x, y);
        m_cx = x; m_cy = y;
    }

    // Circles (stroke only)
    void StrokeCircle(double cx, double cy, double r) {
        if (!m_gc) return;
        double d = r * 2.0;
        m_gc->SetPen(m_pen);
        wxGraphicsPath path = m_gc->CreatePath();
        path.AddEllipse(cx - r, cy - r, d, d);
        m_gc->StrokePath(path);
    }

    // Simple text output (no rotation)
    void TextOut(double x, double y, const wxString& text, const wxColour& color, const wxFont& font) {
        if (!m_gc) return;
        m_gc->SetFont(font, color);
        m_gc->DrawText(text, x, y);
    }

private:
    wxGraphicsContext* m_gc{nullptr};
    wxPen m_pen{*wxBLACK_PEN};
    bool m_hasCurrentPoint{false};
    double m_cx{0.0};
    double m_cy{0.0};
};
