//
// Created by aderugy on 09/10/2026.
//

// test_main.cpp -- runs every Kuhn test suite.
//
// build:  cmake --build cmake-build-debug --target kuhn_tests
// run:    ./cmake-build-debug/bin/kuhn_tests     (or ctest --test-dir cmake-build-debug)

#include <iostream>

#include "test.h"

int main() {
    for (const auto &[name, run] : {std::pair{"node", &run_node_tests},
                                    {"exploitability", &run_exploitability_tests},
                                    {"best_response", &run_best_response_tests},
                                    {"cfr_plus", &run_cfr_plus_tests}}) {
        std::cout << "\n##### " << name << " #####\n";
        run();
    }

    std::cout << "\n" << test::g_pass << " passed, " << test::g_fail << " failed";
    if (test::g_skip) std::cout << ", " << test::g_skip << " unfilled";
    std::cout << "\n";
    return test::g_fail == 0 ? 0 : 1;
}
