cmake_minimum_required(VERSION 3.29)
project(SolvingPoker)

set(CMAKE_CXX_STANDARD 20)

# Collect every executable in <build>/bin (web/scripts/generate-kuhn-cfr.sh relies on it)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

enable_testing()

# Root demo executable
add_executable(SolvingPoker main.cpp)

add_subdirectory(kuhn)
add_subdirectory(leduc)