# Swiss Ephemeris Library
# Cross-Platform: Quellen werden direkt kompiliert (Mac, Windows, Linux)

set(SWISSEPH_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Prüfen ob Quellen vorhanden sind (bevorzugt für Cross-Platform)
if(EXISTS "${SWISSEPH_SRC_DIR}/swephexp.h")
    add_library(swisseph STATIC
        ${SWISSEPH_SRC_DIR}/swecl.c
        ${SWISSEPH_SRC_DIR}/swedate.c
        ${SWISSEPH_SRC_DIR}/swehel.c
        ${SWISSEPH_SRC_DIR}/swehouse.c
        ${SWISSEPH_SRC_DIR}/swejpl.c
        ${SWISSEPH_SRC_DIR}/swemmoon.c
        ${SWISSEPH_SRC_DIR}/swemplan.c
        ${SWISSEPH_SRC_DIR}/sweph.c
        ${SWISSEPH_SRC_DIR}/swephlib.c
    )
    
    target_include_directories(swisseph PUBLIC ${SWISSEPH_SRC_DIR})
    
    # Plattform-spezifische Definitionen
    if(UNIX)
        target_compile_definitions(swisseph PRIVATE _GNU_SOURCE)
        target_link_libraries(swisseph PUBLIC ${CMAKE_DL_LIBS})
    endif()
    
    if(WIN32)
        target_compile_definitions(swisseph PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()
    
    # Warnungen unterdrücken für externe Bibliothek
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(swisseph PRIVATE -w)
    elseif(MSVC)
        target_compile_options(swisseph PRIVATE /W0)
    endif()
    
    message(STATUS "Swiss Ephemeris: Building from sources (Cross-Platform)")
else()
    # Fallback für Entwicklung: System-Bibliothek (nur Linux)
    find_library(SWISSEPH_LIBRARY NAMES swe swisseph
        PATHS /usr/lib /usr/lib/x86_64-linux-gnu
    )
    find_path(SWISSEPH_INCLUDE_DIR swephexp.h
        PATHS /usr/include /usr/include/swisseph
    )
    
    if(SWISSEPH_LIBRARY AND SWISSEPH_INCLUDE_DIR)
        add_library(swisseph INTERFACE)
        target_link_libraries(swisseph INTERFACE ${SWISSEPH_LIBRARY})
        target_include_directories(swisseph INTERFACE ${SWISSEPH_INCLUDE_DIR})
        message(STATUS "Swiss Ephemeris: Using system library (dev only)")
    else()
        message(FATAL_ERROR 
            "Swiss Ephemeris sources not found in ${SWISSEPH_SRC_DIR}\n"
            "Download from: https://github.com/aloistr/swisseph")
    endif()
endif()
