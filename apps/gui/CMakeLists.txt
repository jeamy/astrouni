# wxWidgets GUI target (optional)

# Try to find wxWidgets quietly so the whole project can still configure
# even if wx is not installed yet.
find_package(wxWidgets QUIET COMPONENTS core base)

if (NOT wxWidgets_FOUND)
  message(STATUS "wxWidgets not found; skipping GUI target (set ASTRO_BUILD_GUI=OFF or install wxWidgets dev packages)")
  return()
endif()

# wxWidgets CMake helper
include(${wxWidgets_USE_FILE})

add_executable(astro_gui
  src/main.cpp
  src/ChartPanel.cpp
  src/Dialogs.cpp
  src/GlyphRenderer.cpp
  src/AspectGrid.cpp
  src/AspectGridWindow.cpp
)

# Include directories provided by wxWidgets
# (wxWidgets_INCLUDE_DIRS is populated by find_package)
target_include_directories(astro_gui PRIVATE ${wxWidgets_INCLUDE_DIRS})

# Link against astrocore and wxWidgets libraries
target_link_libraries(astro_gui PRIVATE astrocore ${wxWidgets_LIBRARIES})

set_target_properties(astro_gui PROPERTIES OUTPUT_NAME astro_gui)
