# Tools CMakeLists.txt

add_executable(show_dat show_dat.cpp)

target_include_directories(show_dat PRIVATE
    ${CMAKE_SOURCE_DIR}/core/include
)

add_executable(test_orben_load test_orben_load.cpp)

target_include_directories(test_orben_load PRIVATE
    ${CMAKE_SOURCE_DIR}/core/include
)

target_link_libraries(test_orben_load PRIVATE
    astro_database
)

add_executable(test_orben_save test_orben_save.cpp)

target_include_directories(test_orben_save PRIVATE
    ${CMAKE_SOURCE_DIR}/core/include
)

target_link_libraries(test_orben_save PRIVATE
    astro_database
)
