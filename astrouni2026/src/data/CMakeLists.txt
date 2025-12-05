# Data Library - Daten-Handling
# 1:1 Port von: io.c, astrofil.c, auhelper.c (Daten-Teil)

add_library(astrouni_data STATIC
    legacy_io.h
    legacy_io.cpp
    orte_db.h
    orte_db.cpp
    person_db.h
    person_db.cpp
)

target_include_directories(astrouni_data PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(astrouni_data PUBLIC
    astrouni_core
    Qt6::Core
)
