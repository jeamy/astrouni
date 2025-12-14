# Core Library - GUI-unabhängige Logik
# 1:1 Port von: aucalc.c, auwurzel.c (Berechnungen), autransi.c

add_library(astrouni_core STATIC
    constants.h
    data_types.h
    data_types.cpp
    calculations.h
    calculations.cpp
    chart_calc.h
    chart_calc.cpp
    transit_calc.h
    transit_calc.cpp
    swiss_eph.h
    swiss_eph.cpp
    astro_font_provider.h
    astro_font_provider.cpp
    astro_text_analyzer.h
    astro_text_analyzer.cpp
)

target_include_directories(astrouni_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(astrouni_core PUBLIC
    swisseph
    Qt6::Core
    Qt6::Gui
)
