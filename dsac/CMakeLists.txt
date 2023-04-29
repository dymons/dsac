file(GLOB_RECURSE DSAC_LIBRARY_SOURCES "src/*.hpp" "src/*.cpp" "src/*.S")
file(GLOB_RECURSE DSAC_LIBRARY_HEADERS "include/*.hpp")

enable_language(ASM)

set(DSAC_LIBRARY_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${DSAC_LIBRARY_INCLUDE_DIRS})

add_library(${PROJECT_NAME} ${DSAC_LIBRARY_HEADERS} ${DSAC_LIBRARY_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC .)
target_link_libraries(${PROJECT_NAME} fmt::fmt)

find_package(benchmark REQUIRED)
find_package(fmt REQUIRED)

if (BUILD_TESTS)
    find_package(Threads)
    extract_subdirs(test_subdirs ${CMAKE_CURRENT_SOURCE_DIR}/test)
    foreach (subdir ${test_subdirs})
        file(GLOB DSAC_TEST_SRC "test/${subdir}/*.cpp")
        add_executable(${PROJECT_NAME}-${subdir}-test ${DSAC_TEST_SRC})
        target_link_libraries(${PROJECT_NAME}-${subdir}-test ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} fmt::fmt)
    endforeach ()
endif (BUILD_TESTS)

if (BUILD_BENCHMARKS)
    find_package(Threads)
    extract_subdirs(benchmark_subdirs ${CMAKE_CURRENT_SOURCE_DIR}/benchmark)
    foreach (subdir ${benchmark_subdirs})
        file(GLOB DSAC_TEST_SRC "benchmark/${subdir}/*.cpp")
        add_executable(${PROJECT_NAME}-${subdir}-benchmark ${DSAC_TEST_SRC})
        target_link_libraries(${PROJECT_NAME}-${subdir}-benchmark ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} fmt::fmt benchmark::benchmark)
    endforeach ()
endif (BUILD_BENCHMARKS)

add_subdirectory(examples)
