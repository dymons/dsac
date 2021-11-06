project(cpp VERSION 1.0 LANGUAGES CXX)

get_filename_component(PACKAGE_INCLUDE_PATH ".." ABSOLUTE)
get_filename_component(PACKAGE_PATH "." ABSOLUTE)

file(GLOB LIB_HEADERS ${PROJECT_SOURCE_DIR}/pointers)
include_directories(${LIB_HEADERS})

file(GLOB CPP_TEST_SRC ${PROJECT_SOURCE_DIR}/tests/*.cpp)
add_executable(cpp_tests ${CPP_TEST_SRC})
target_link_libraries(cpp_tests PUBLIC Catch2::Catch2)