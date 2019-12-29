cmake_minimum_required(VERSION 3.2)

project(Abort VERSION 2.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/googletest/googletest
                 EXCLUDE_FROM_ALL)

# -----------------------------------------------------------------------------
# Build abort library
# -----------------------------------------------------------------------------
add_library(abort STATIC
    src/abort.cc
)

target_include_directories(abort
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# -----------------------------------------------------------------------------
# Build unit tests
# -----------------------------------------------------------------------------
add_executable(abort-test
    test/main.cc
    test/abort_ut.cc
)

target_include_directories(abort-test
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/googletest/googletest/include
)

target_link_libraries(abort-test
    PRIVATE
    abort
    gtest
)
