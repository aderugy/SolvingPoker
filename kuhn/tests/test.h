//
// Created by aderugy on 09/10/2026.
//

// test.h -- minimal shared harness for the Kuhn test suites (see test_main.cpp).

#pragma once

#include <cmath>
#include <iostream>
#include <string>

namespace test {

inline int g_pass = 0, g_fail = 0, g_skip = 0;

inline void check(const bool ok, const std::string &what) {
    if (ok) { ++g_pass; return; }
    ++g_fail;
    std::cout << "  FAIL  " << what << "\n";
}

inline void check_near(const double got, const double want, const double eps, const std::string &what) {
    check(std::abs(got - want) < eps,
          what + "  (got " + std::to_string(got) + ", want " + std::to_string(want) + ")");
}

}  // namespace test

// One entry point per suite, each defined in its test_*.cpp.
void run_node_tests();
void run_exploitability_tests();
void run_best_response_tests();
void run_cfr_plus_tests();
