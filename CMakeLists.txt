cmake_minimum_required(VERSION 3.16)
project(AstroPort LANGUAGES C CXX)

# Default to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(ASTRO_BUILD_CLI "Build CLI demo app" ON)
option(ASTRO_ENABLE_WARNINGS "Enable strict warnings" ON)
option(ASTRO_BUILD_GUI "Build wxWidgets GUI app" ON)
option(ASTRO_BUILD_TESTS "Build unit tests" ON)

if(ASTRO_ENABLE_WARNINGS)
  if (MSVC)
    add_compile_options(/W4 /permissive-)
  else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow)
  endif()
endif()

add_subdirectory(core)
if(ASTRO_BUILD_CLI)
  add_subdirectory(apps/cli)
endif()
if(ASTRO_BUILD_GUI)
  add_subdirectory(apps/gui)
endif()

if(ASTRO_BUILD_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()
