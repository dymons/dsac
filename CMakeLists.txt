cmake_minimum_required(VERSION 3.16.3)
project(interviews VERSION 1.0 LANGUAGES CXX)

include(cmake/CheckCompiler.cmake)
include(cmake/CompileOptions.cmake)

add_subdirectory(third_party)
add_subdirectory(algorithms)
add_subdirectory(structures)
add_subdirectory(cpp)