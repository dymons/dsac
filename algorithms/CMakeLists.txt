project(algorithms VERSION 1.0 LANGUAGES CXX)

file(GLOB LIB_HEADERS ${PROJECT_SOURCE_DIR}/sorts)
include_directories(${LIB_HEADERS})

file(GLOB ALGORITHMS_TEST_SRC ${PROJECT_SOURCE_DIR}/tests/*.cpp)
add_executable(algorithms_tests ${ALGORITHMS_TEST_SRC})
target_link_libraries(algorithms_tests PUBLIC Catch2::Catch2)