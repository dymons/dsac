project(structures VERSION 1.0 LANGUAGES CXX)

file(GLOB LIB_HEADERS ${PROJECT_SOURCE_DIR}/graph)
include_directories(${LIB_HEADERS})

file(GLOB STRUCTURES_TEST_SRC ${PROJECT_SOURCE_DIR}/tests/*.cpp)
add_executable(structures_tests ${STRUCTURES_TEST_SRC})
target_link_libraries(structures_tests PUBLIC Catch2::Catch2)