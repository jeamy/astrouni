add_executable(astrotext_export
    astrotext_export.cpp
)

target_link_libraries(astrotext_export PRIVATE
    astrouni_core
    Qt6::Core
)
