# GUI CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# wxWidgets finden
find_package(wxWidgets REQUIRED COMPONENTS core base gl)
include(${wxWidgets_USE_FILE})

# GUI Sources
set(GUI_SOURCES
    main.cpp
    main_frame.cpp
    chart_panel.cpp
    chart_renderer.cpp
    dialogs/new_chart_dialog.cpp
)

set(GUI_HEADERS
    main_frame.h
    chart_panel.h
    chart_renderer.h
    dialogs/new_chart_dialog.h
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
