# GUI CMakeLists.txt

find_package(wxWidgets REQUIRED COMPONENTS core base adv aui)
include(${wxWidgets_USE_FILE})

add_executable(astro_gui main.cpp main_frame.cpp person_dialog.cpp ort_dialog.cpp person_search_dialog.cpp ort_search_dialog.cpp hauser_dialog.cpp horo_typ_dialog.cpp orben_dialog.cpp farben_dialog.cpp einst_dialog.cpp symbol_panel.cpp)

target_include_directories(astro_gui PRIVATE
    ${CMAKE_SOURCE_DIR}/core/include
)

target_compile_definitions(astro_gui PRIVATE wxNO_ASSERT_CHECK)

target_link_libraries(astro_gui PRIVATE
    astro_core
    astro_database
    ${wxWidgets_LIBRARIES}
)
