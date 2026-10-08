cmake_minimum_required(VERSION 3.29)
project(SolvingPoker)

set(CMAKE_CXX_STANDARD 20)

# Root demo executable
add_executable(SolvingPoker main.cpp)

# Kuhn poker solver — built separately
add_executable(kuhn
        kuhn/main.cpp
        kuhn/player.cpp
        kuhn/player.h
        kuhn/node.cpp
        kuhn/node.h
        kuhn/cfr.cpp
        kuhn/cfr.h
        kuhn/tree.cpp
        kuhn/tree.h
        kuhn/consts.h
        kuhn/exploitability.cpp
        kuhn/exploitability.h
)

# Unit tests for Node (is_terminal / terminal_utility)
add_executable(test_node
        kuhn/test_node.cpp
        kuhn/node.cpp
        kuhn/node.h
)

# Unit tests for best_response_reach (exploitability pass 1)
add_executable(test_exploitability
        kuhn/test_exploitability.cpp
        kuhn/exploitability.cpp
        kuhn/exploitability.h
        kuhn/cfr.cpp
        kuhn/node.cpp
        kuhn/player.cpp
        kuhn/tree.cpp
)

# Unit tests for best_response_value / exploitability (exploitability pass 2)
add_executable(test_best_response
        kuhn/test_best_response.cpp
        kuhn/exploitability.cpp
        kuhn/exploitability.h
        kuhn/cfr.cpp
        kuhn/node.cpp
        kuhn/player.cpp
        kuhn/tree.cpp
)

# JSON exporter that generates the web demo data (web/scripts/generate-kuhn-cfr.sh)
add_executable(kuhn_export
        kuhn/export.cpp
        kuhn/exploitability.cpp
        kuhn/exploitability.h
        kuhn/cfr.cpp
        kuhn/node.cpp
        kuhn/player.cpp
        kuhn/tree.cpp
)

# Black-box tests for cfr_plus (regret clamp, convergence, equilibrium)
add_executable(test_kuhn_cfr_plus
        kuhn/test_cfr_plus.cpp
        kuhn/cfr_plus.cpp
        kuhn/cfr_plus.h
        kuhn/cfr.cpp
        kuhn/exploitability.cpp
        kuhn/node.cpp
        kuhn/player.cpp
        kuhn/tree.cpp
)
