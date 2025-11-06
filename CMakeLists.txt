cmake_minimum_required(VERSION 3.20)
project(AstroUniverse VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler Flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Swiss Ephemeris
find_library(SWISSEPH_LIB swe PATHS ${CMAKE_SOURCE_DIR}/swisseph/lib)
find_path(SWISSEPH_INCLUDE swephexp.h PATHS ${CMAKE_SOURCE_DIR}/swisseph/include)

if(NOT SWISSEPH_LIB OR NOT SWISSEPH_INCLUDE)
    message(STATUS "Swiss Ephemeris not found in swisseph/, trying system paths...")
    find_library(SWISSEPH_LIB swe REQUIRED)
    find_path(SWISSEPH_INCLUDE swephexp.h REQUIRED)
endif()

message(STATUS "Swiss Ephemeris Library: ${SWISSEPH_LIB}")
message(STATUS "Swiss Ephemeris Include: ${SWISSEPH_INCLUDE}")

# Core Library
add_subdirectory(core)

# Example program
add_executable(example_chart example_chart.cpp)
target_link_libraries(example_chart PRIVATE astro_core)

# Tests (optional)
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# GUI (optional)
option(BUILD_GUI "Build wxWidgets GUI" ON)
if(BUILD_GUI)
    add_subdirectory(gui)
endif()

# Installation
install(TARGETS example_chart DESTINATION bin)
install(DIRECTORY swisseph/ephe/ DESTINATION share/astro_universe/ephe)
