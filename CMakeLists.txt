cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(network VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_subdirectory(algorithms_sort)
add_subdirectory(algorithms_test)