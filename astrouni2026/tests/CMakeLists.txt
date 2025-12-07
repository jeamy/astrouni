# Unit Tests

find_package(Qt6 REQUIRED COMPONENTS Test)

# Test für Berechnungen
add_executable(test_calculations
    test_calculations.cpp
)

target_link_libraries(test_calculations PRIVATE
    astrouni_core
    Qt6::Core
    Qt6::Test
)

add_test(NAME test_calculations COMMAND test_calculations)

# Test für Chart-Berechnung
add_executable(test_chart_calc
    test_chart_calc.cpp
)

target_link_libraries(test_chart_calc PRIVATE
    astrouni_core
    Qt6::Core
    Qt6::Test
)

add_test(NAME test_chart_calc COMMAND test_chart_calc)
