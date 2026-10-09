# All Kuhn unit tests in one executable (node, exploitability, best response, cfr_plus)
add_executable(kuhn_tests
        test.h
        test_main.cpp
        test_node.cpp
        test_exploitability.cpp
        test_best_response.cpp
        test_cfr_plus.cpp
)
target_link_libraries(kuhn_tests PRIVATE kuhn_core)

add_test(NAME kuhn_tests COMMAND kuhn_tests)
