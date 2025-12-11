# GUI Library - Qt6 Benutzeroberfläche
# 1:1 Port von: astrouni.c (GUI), auwurzel.c (Zeichnung), astrofil.c (Dialoge)

add_library(astrouni_gui STATIC
    main_window.h
    main_window.cpp
    radix_window.h
    radix_window.cpp
    chart_widget.h
    chart_widget.cpp
    print_manager.h
    print_manager.cpp
    pdf_exporter.h
    pdf_exporter.cpp
    
    # Dialoge
    dialogs/person_dialog.h
    dialogs/person_dialog.cpp
    dialogs/ort_dialog.h
    dialogs/ort_dialog.cpp
    dialogs/ort_search_dialog.h
    dialogs/ort_search_dialog.cpp
    dialogs/person_search_dialog.h
    dialogs/person_search_dialog.cpp
    dialogs/haus_dialog.h
    dialogs/haus_dialog.cpp
    dialogs/horo_typ_dialog.h
    dialogs/horo_typ_dialog.cpp
    dialogs/orben_dialog.h
    dialogs/orben_dialog.cpp
    dialogs/farben_dialog.h
    dialogs/farben_dialog.cpp
    dialogs/settings_dialog.h
    dialogs/settings_dialog.cpp
    dialogs/transit_abort_dialog.h
    dialogs/transit_abort_dialog.cpp
    dialogs/transit_dialog.h
    dialogs/transit_dialog.cpp
    dialogs/trans_sel_dialog.h
    dialogs/trans_sel_dialog.cpp
    dialogs/ort_duplikat_dialog.h
    dialogs/ort_duplikat_dialog.cpp
    dialogs/retrograde_dialog.h
    dialogs/retrograde_dialog.cpp
    transit_result_window.h
    transit_result_window.cpp
)

target_include_directories(astrouni_gui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
)

target_link_libraries(astrouni_gui PUBLIC
    astrouni_core
    astrouni_data
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PrintSupport
)
