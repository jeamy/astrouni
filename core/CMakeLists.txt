# Core Library
add_library(astro_core STATIC
    src/swiss_ephemeris.cpp
    src/data_types.cpp
    src/math_utils.cpp
    src/calculations.cpp
)

target_include_directories(astro_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${SWISSEPH_INCLUDE}
)

target_link_libraries(astro_core
    PUBLIC
        ${SWISSEPH_LIB}
)

# Compiler features
target_compile_features(astro_core PUBLIC cxx_std_20)
