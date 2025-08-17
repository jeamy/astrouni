cmake_minimum_required(VERSION 3.16)

include(FetchContent)

# Fetch GoogleTest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# -------------------- Core tests --------------------
add_executable(astro_tests_core
  test_core.cpp
  test_houses.cpp
  test_planets.cpp
)

target_link_libraries(astro_tests_core PRIVATE
  GTest::gtest
  GTest::gtest_main
  astrocore
)

add_test(NAME astro_tests_core COMMAND astro_tests_core)

# -------------------- GUI tests (optional, opt-in) --------------------
option(ASTRO_ENABLE_GUI_TESTS "Build and run GUI tests" OFF)
find_package(wxWidgets QUIET COMPONENTS core base)

if (wxWidgets_FOUND AND ASTRO_BUILD_GUI AND ASTRO_ENABLE_GUI_TESTS)
  include(${wxWidgets_USE_FILE})

  add_executable(astro_tests_gui
    test_gui.cpp
    ${CMAKE_SOURCE_DIR}/apps/gui/src/ChartPanel.cpp
    ${CMAKE_SOURCE_DIR}/apps/gui/src/GlyphRenderer.cpp
    ${CMAKE_SOURCE_DIR}/apps/gui/src/AspectGrid.cpp
    ${CMAKE_SOURCE_DIR}/apps/gui/src/AspectGridWindow.cpp
  )

  target_include_directories(astro_tests_gui PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/gui/src
    ${wxWidgets_INCLUDE_DIRS}
  )

  target_link_libraries(astro_tests_gui PRIVATE
    GTest::gtest
    GTest::gtest_main
    astrocore
    ${wxWidgets_LIBRARIES}
  )

  add_test(NAME astro_tests_gui COMMAND astro_tests_gui)
else()
  message(STATUS "GUI tests disabled (ASTRO_ENABLE_GUI_TESTS=OFF) or wxWidgets/GUI not available; skipping GUI tests")
endif()
