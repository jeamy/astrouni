#include <gtest/gtest.h>
#include <wx/wx.h>
#include <wx/app.h>
#include <wx/init.h>
#include "ChartPanel.hpp"

TEST(GuiAspectOrbTest, SetAspectOrbDoesNotCrash) {
  // Opt-in gate: only run when explicitly requested
  const char* run_gui = std::getenv("ASTRO_RUN_GUI_TESTS");
  if (!run_gui || run_gui[0] == '\0' || run_gui[0] == '0') {
    GTEST_SKIP() << "ASTRO_RUN_GUI_TESTS not set; skipping GUI test";
  }
  // Headless detection for GTK (common in CI)
#if defined(__WXGTK__)
  if (!std::getenv("DISPLAY") && !std::getenv("WAYLAND_DISPLAY")) {
    GTEST_SKIP() << "No DISPLAY/WAYLAND_DISPLAY; skipping GUI test";
  }
#endif

  // Initialize wxWidgets without a custom wxApp
  wxInitializer initializer;
  if (!initializer.IsOk()) {
    GTEST_SKIP() << "wxInitializer failed; skipping GUI test";
  }
  wxInitAllImageHandlers();

  // Create a top-level frame and the ChartPanel under test
  wxFrame* frame = new wxFrame(nullptr, wxID_ANY, "TestFrame", wxDefaultPosition, wxSize(600, 400));
  ASSERT_NE(frame, nullptr);

  ChartPanel* panel = new ChartPanel(frame);
  ASSERT_NE(panel, nullptr);

  // Show aspect grid window to exercise related code paths
  panel->SetShowAspectGrid(true);

  // Set reasonable defaults then adjust the orb
  astrocore::Date d{2024, 3, 21};
  astrocore::Time t{12, 0, 0.0};
  astrocore::TimeZone tz{0.0, 0.0};
  panel->SetDateTime(d, t, tz);
  panel->SetLocation(13.405, 52.52);

  // This mimics the "Set Aspect Orb" action
  panel->SetAspectOrbDeg(2.5);
  EXPECT_DOUBLE_EQ(panel->GetAspectOrbDeg(), 2.5);

  // Toggle to exercise refresh and aspect drawing paths
  panel->SetShowAspectLines(true);
  panel->SetShowPlanets(true);

  // Force a paint to go through drawing code paths in a safe offscreen bitmap
  // Use ExportPNG to render without requiring the actual screen GC
  EXPECT_TRUE(panel->ExportPNG("/tmp/astro_gui_test.png", 400, 300));

  // Cleanup
  frame->Destroy();
}
