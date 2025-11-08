# Core Library CMakeLists.txt

add_library(astro_core INTERFACE)

target_include_directories(astro_core INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
