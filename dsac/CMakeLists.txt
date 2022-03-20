project(libdsac CXX)

file(GLOB_RECURSE DSAC_LIBRARY_SOURCES "src/*.hpp" "src/*.cpp")
file(GLOB_RECURSE DSAC_LIBRARY_HEADERS "include/*.hpp")

set(DSAC_LIBRARY_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${DSAC_LIBRARY_INCLUDE_DIRS})

add_library(${PROJECT_NAME} ${DSAC_LIBRARY_HEADERS} ${DSAC_LIBRARY_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC .)

file(GLOB DSAC_ALGORITHM_TEST_SRC "test/algorithm/*.cpp")
add_executable(${PROJECT_NAME}-algorithm ${DSAC_ALGORITHM_TEST_SRC})
target_link_libraries(${PROJECT_NAME}-algorithm PUBLIC Catch2 ${PROJECT_NAME})