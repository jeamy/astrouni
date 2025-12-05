# Core Library CMakeLists.txt

find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

add_library(astro_core STATIC
    src/colors.cpp
)

target_include_directories(astro_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(astro_core PUBLIC
    ${wxWidgets_LIBRARIES}
)
