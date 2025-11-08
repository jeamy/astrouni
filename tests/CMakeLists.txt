# Tests CMakeLists.txt

# Tests CMakeLists.txt

enable_testing()

# Google Test via FetchContent herunterladen und einbinden
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(astro_tests
    test_data_manager.cpp
)

# Den Test-Executable gegen GTest und die Datenbank-Bibliothek linken
target_include_directories(astro_tests PRIVATE ${CMAKE_SOURCE_DIR}/core/include)

target_link_libraries(astro_tests PRIVATE
    GTest::gtest_main
    astro_database
)

# Tests automatisch finden lassen
include(GoogleTest)
gtest_discover_tests(astro_tests)

