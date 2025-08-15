# Core library
add_library(astrocore
  src/api.cpp
  src/planets.cpp
  src/ephemeris.cpp
)

target_include_directories(astrocore
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Allow C++ consumers
set_target_properties(astrocore PROPERTIES
  OUTPUT_NAME astrocore
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED YES
)

# Basic compile definitions for portability
if (WIN32)
  target_compile_definitions(astrocore PUBLIC ASTROCORE_PLATFORM_WINDOWS)
elseif(UNIX)
  target_compile_definitions(astrocore PUBLIC ASTROCORE_PLATFORM_UNIX)
endif()
