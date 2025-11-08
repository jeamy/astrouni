#include <wx/wx.h>
#include "main_frame.h"
#include "person_dialog.h"
#include "legacy_data_manager.h"

// 1:1 Port von WinMain aus legacy/astrouni.c
class AstroApp : public wxApp {
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(AstroApp);

bool AstroApp::OnInit() {
    // 1:1 Port von InitApplication & InitInstance aus legacy/astrouni.c
    MainFrame* frame = new MainFrame("AstroUniverse 1:1 Port", wxPoint(50, 50), wxSize(800, 600));
    frame->Show(true);

    // --- Temporärer Test-Code für Dialog-Entwicklung ---
    astro::LegacyDataManager dm;
    astro::LegacyRadix p1, p2;
    strcpy(p1.szName, "Einstein");
    strcpy(p1.szVorName, "Albert");
    p1.o.dt.iTag = 14; p1.o.dt.iMon = 3; p1.o.dt.iJah = 1879;
    p1.o.dt.iStu = 11; p1.o.dt.iMin = 30;
    p1.o.dt.iZone = 1; p1.o.dt.iSom = 0;
    strcpy(p1.o.szOrt, "Ulm");

    strcpy(p2.szName, "Curie");
    strcpy(p2.szVorName, "Marie");
    p2.o.dt.iTag = 7; p2.o.dt.iMon = 11; p2.o.dt.iJah = 1867;
    p2.o.dt.iStu = 12; p2.o.dt.iMin = 0;
    p2.o.dt.iZone = 1; p2.o.dt.iSom = 0;
    strcpy(p2.o.szOrt, "Warschau");

    dm.WritePerson("astronam.dat", p1, 0);
    dm.WritePerson("astronam.dat", p2, 1);

    // Öffne den Personen-Dialog zum Testen der Suche
    astro::LegacyRadix dummy_radix;
    PersonDialog dlg(frame, dummy_radix);
    dlg.ShowModal();
    // --- Ende Test-Code ---

    return true;
}
