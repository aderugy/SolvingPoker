add_library(leduc_core STATIC
        node.h
        consts.h
        export.cpp
        chance.cpp
        chance.h
        chance.h
)
target_include_directories(leduc_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(leduc_export export.cpp)
target_link_libraries(leduc_export PRIVATE leduc_core)

add_subdirectory(tests)