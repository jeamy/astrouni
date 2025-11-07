# GUI CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# wxWidgets finden
find_package(wxWidgets REQUIRED COMPONENTS core base gl)
include(${wxWidgets_USE_FILE})

# GUI Sources (1:1 Legacy Port)
set(GUI_SOURCES
    main.cpp
    main_frame.cpp
    chart_panel.cpp
    legacy_renderer.cpp
    dialogs/person_dialog.cpp
    dialogs/ort_dialog.cpp
    dialogs/haus_dialog.cpp
    dialogs/orben_dialog.cpp
    dialogs/farben_dialog.cpp
    dialogs/einstellungen_dialog.cpp
    dialogs/horotyp_dialog.cpp
    dialogs/ort_auswahl_dialog.cpp
    dialogs/transit_dialog.cpp
    dialogs/transit_einstellungen_dialog.cpp
    dialogs/berechnung_dialog.cpp
    database/radix_database.cpp
    database/legacy_data_manager.cpp
)

set(GUI_HEADERS
    main_frame.h
    chart_panel.h
    legacy_renderer.h
    dialogs/person_dialog.h
    dialogs/ort_dialog.h
    dialogs/haus_dialog.h
    dialogs/orben_dialog.h
    dialogs/farben_dialog.h
    dialogs/einstellungen_dialog.h
    dialogs/horotyp_dialog.h
    dialogs/ort_auswahl_dialog.h
    dialogs/transit_dialog.h
    dialogs/transit_einstellungen_dialog.h
    dialogs/berechnung_dialog.h
    database/radix_database.h
    database/legacy_data_manager.h
)

# GUI Executable
add_executable(astro_gui ${GUI_SOURCES} ${GUI_HEADERS})

target_include_directories(astro_gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/core/include
)

target_link_libraries(astro_gui PRIVATE
    astro_core
    ${wxWidgets_LIBRARIES}
)

# Windows: GUI-Subsystem (keine Konsole)
if(WIN32)
    set_target_properties(astro_gui PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Installation
install(TARGETS astro_gui DESTINATION bin)
