# Leduc unit tests
add_executable(leduc_tests
        test_chance_tables.cpp
)
target_link_libraries(leduc_tests PRIVATE leduc_core)

add_test(NAME leduc_tests COMMAND leduc_tests)
