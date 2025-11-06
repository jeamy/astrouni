# Google Test
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    message(STATUS "Google Test not found, downloading...")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)
endif()

# Test executable
add_executable(astro_tests
    test_math_utils.cpp
    test_swiss_ephemeris.cpp
    test_calculations.cpp
)

target_link_libraries(astro_tests
    PRIVATE
        astro_core
        GTest::gtest_main
)

# Enable testing
include(GoogleTest)
gtest_discover_tests(astro_tests)
