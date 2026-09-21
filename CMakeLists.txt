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
)

# Unit tests for Node (is_terminal / terminal_utility)
add_executable(test_node
        kuhn/test_node.cpp
        kuhn/node.cpp
        kuhn/node.h
)
