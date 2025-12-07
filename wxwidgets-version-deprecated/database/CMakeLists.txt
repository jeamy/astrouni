# Database Library CMakeLists.txt

add_library(astro_database STATIC)

target_sources(astro_database PRIVATE
    legacy_data_manager.cpp
)

target_include_directories(astro_database PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(astro_database PUBLIC astro_core)
