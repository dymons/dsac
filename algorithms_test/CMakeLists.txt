cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(algorithms_test VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(google-test)

add_executable(algorithm_sort_unit_tests algorithm_sort_unit_tests.cpp)

target_link_libraries(algorithm_sort_unit_tests
    algorithms::algorithms_sort
    GTest::GTest
    GTest::Main
)

add_test(NAME algorithm_sort_unit_tests COMMAND algorithm_sort_unit_tests)