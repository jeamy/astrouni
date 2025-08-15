add_executable(astrocli
  src/main.cpp
)

target_link_libraries(astrocli PRIVATE astrocore)

target_include_directories(astrocli PRIVATE ${CMAKE_SOURCE_DIR}/core/include)

set_target_properties(astrocli PROPERTIES OUTPUT_NAME astrocli)
