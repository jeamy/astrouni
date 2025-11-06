// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2025 AstroUniverse Project

#include "main_frame.h"
#include <wx/wx.h>

class AstroApp : public wxApp {
public:
    virtual bool OnInit() override {
        MainFrame* frame = new MainFrame("AstroUniverse");
        frame->Show(true);
        wxLog::SetActiveTarget(new wxLogStderr());
        return true;
    }
};

// wxWidgets Entry Point
wxIMPLEMENT_APP(AstroApp);
