cmake_minimum_required(VERSION 3.20)
project(AstroUniverse VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler Flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Subdirectories
add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(database)
add_subdirectory(tests)
