cmake_minimum_required(VERSION 3.16)
project(MOMON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_library(poly_lib INTERFACE)
target_include_directories(poly_lib INTERFACE
        ${CMAKE_SOURCE_DIR}/src
)

add_executable(tests
        test/tests.cpp
)

target_link_libraries(tests
        poly_lib
        GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(tests)