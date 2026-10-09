# Kuhn poker solver: everything but the entry points, shared by the executables and the tests
add_library(kuhn_core STATIC
        player.cpp
        player.h
        node.cpp
        node.h
        cfr.cpp
        cfr.h
        cfr_plus.cpp
        cfr_plus.h
        tree.cpp
        tree.h
        consts.h
        exploitability.cpp
        exploitability.h
)
target_include_directories(kuhn_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# JSON exporter that generates the web demo data (web/scripts/generate-kuhn-cfr.sh)
add_executable(kuhn_export export.cpp)
target_link_libraries(kuhn_export PRIVATE kuhn_core)

add_subdirectory(tests)
