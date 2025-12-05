#include <wx/wx.h>
#include <wx/sysopt.h>
#include "main_frame.h"

// 1:1 Port von WinMain aus legacy/astrouni.c
class AstroApp : public wxApp {
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(AstroApp);

bool AstroApp::OnInit() {
    // Setze helles Theme für die gesamte Anwendung (Legacy-Look)
    wxSystemOptions::SetOption("gtk.theme.name", "Adwaita");
    
    // 1:1 Port von InitApplication & InitInstance aus legacy/astrouni.c
    MainFrame* frame = new MainFrame("AstroUniverse 1:1 Port", wxPoint(50, 50), wxSize(800, 600));
    frame->SetBackgroundColour(*wxWHITE);
    frame->Show(true);
    SetTopWindow(frame);

    return true;
}
