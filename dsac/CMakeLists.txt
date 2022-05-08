file(GLOB_RECURSE DSAC_LIBRARY_SOURCES "src/*.hpp" "src/*.cpp")
file(GLOB_RECURSE DSAC_LIBRARY_HEADERS "include/*.hpp")

set(DSAC_LIBRARY_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${DSAC_LIBRARY_INCLUDE_DIRS})

add_library(${PROJECT_NAME} ${DSAC_LIBRARY_HEADERS} ${DSAC_LIBRARY_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC .)

if (BUILD_TESTS)
    extract_subdirs(test_subdirs ${CMAKE_CURRENT_SOURCE_DIR}/test)
    foreach (subdir ${test_subdirs})
        file(GLOB DSAC_ALGORITHM_TEST_SRC "test/${subdir}/*.cpp")
        add_executable(${PROJECT_NAME}-${subdir}-test ${DSAC_ALGORITHM_TEST_SRC})
        target_link_libraries(${PROJECT_NAME}-${subdir}-test PUBLIC Catch2 ${PROJECT_NAME} pthread)
    endforeach ()
endif (BUILD_TESTS)
