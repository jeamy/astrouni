// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project

#pragma once

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "astro/calculations.h"
#include "legacy_renderer.h"
#include <memory>

class ChartPanel : public wxPanel {
public:
    ChartPanel(wxWindow* parent);
    
    // Chart setzen
    void SetChart(const astro::AstroCalculator::ChartData& chart);
    void ClearChart();
    
private:
    // Event Handlers
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    
    // Daten
    legacy::LegacyRenderer renderer_;
    std::unique_ptr<astro::AstroCalculator::ChartData> chart_data_;
    bool has_chart_;
    
    wxDECLARE_EVENT_TABLE();
};
