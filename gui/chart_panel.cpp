// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project

#include "chart_panel.h"
#include "astro/i18n.h"
#include <cmath>

using namespace astro::i18n;

wxBEGIN_EVENT_TABLE(ChartPanel, wxPanel)
    EVT_PAINT(ChartPanel::OnPaint)
    EVT_SIZE(ChartPanel::OnSize)
wxEND_EVENT_TABLE()

ChartPanel::ChartPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY), has_chart_(false) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetBackgroundColour(*wxWHITE);
}

void ChartPanel::SetChart(const astro::AstroCalculator::ChartData& chart) {
    chart_data_ = std::make_unique<astro::AstroCalculator::ChartData>(chart);
    has_chart_ = true;
    
    // Sichere Log-Ausgabe: ohne wx-Format, mit std::to_string
    const std::string s =
        std::string("Chart gesetzt: ") + std::to_string(chart_data_->planets.size()) +
        " Planeten, " + std::to_string(chart_data_->houses.size()) +
        " Häuser, " + std::to_string(chart_data_->aspects.size()) + " Aspekte";
    wxLogMessage("%s", wxString::FromUTF8(s.c_str()));
    Refresh();
    Update();
}

void ChartPanel::ClearChart() {
    chart_data_.reset();
    has_chart_ = false;
    Refresh();
}

void ChartPanel::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();
    
    if (has_chart_ && chart_data_) {
        wxSize size = GetClientSize();
        renderer_.DrawRadix(dc, size.x, size.y, *chart_data_);
    } else {
        // Platzhalter-Text
        wxSize size = GetClientSize();
        dc.SetTextForeground(*wxLIGHT_GREY);
        wxFont font = dc.GetFont();
        font.SetPointSize(16);
        dc.SetFont(font);
        
        wxString text = tr("chart.no_chart");
        wxSize text_size = dc.GetTextExtent(text);
        dc.DrawText(text, (size.x - text_size.x) / 2, (size.y - text_size.y) / 2);
    }
}

void ChartPanel::OnSize(wxSizeEvent& event) {
    Refresh();
    event.Skip();
}
