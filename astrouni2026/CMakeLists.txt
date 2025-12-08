cmake_minimum_required(VERSION 3.16)

project(AstroUniverse2026 
    VERSION 1.0.0 
    LANGUAGES C CXX
    DESCRIPTION "AstroUniverse - Astrologie Software"
)

# C++20 Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# MSVC: __cplusplus korrekt setzen (Qt6 Anforderung)
if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

# Qt6 Auto-Tools
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Build-Typ
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Qt6 finden
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Gui 
    Widgets 
    PrintSupport
)

# Swiss Ephemeris
add_subdirectory(swisseph)

# Core Library (GUI-unabhängig)
add_subdirectory(src/core)

# Data Library
add_subdirectory(src/data)

# GUI Library
add_subdirectory(src/gui)

# Hauptprogramm (WIN32 = kein Konsolenfenster unter Windows)
add_executable(astrouni2026 WIN32
    src/main.cpp
)

target_link_libraries(astrouni2026 PRIVATE
    astrouni_gui
    astrouni_core
    astrouni_data
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Daten-Dateien kopieren
file(COPY ${CMAKE_SOURCE_DIR}/data/ DESTINATION ${CMAKE_BINARY_DIR}/data/)

# Resources kopieren
file(COPY ${CMAKE_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_BINARY_DIR}/resources/)

# Tests (optional)
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
install(TARGETS astrouni2026
    RUNTIME DESTINATION bin
)

install(DIRECTORY data/ DESTINATION share/astrouni2026/data)
install(DIRECTORY resources/ DESTINATION share/astrouni2026/resources)
