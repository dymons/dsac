set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

get_filename_component(PACKAGE_INCLUDE_PATH ".." ABSOLUTE)
get_filename_component(PACKAGE_PATH "." ABSOLUTE)

file(GLOB_RECURSE PACKAGE_HEADERS
     ${PACKAGE_PATH}/executors/*.hpp
     ${PACKAGE_PATH}/futures/*.hpp
     ${PACKAGE_PATH}/support/*.hpp
     ${PACKAGE_PATH}/syncing/*.hpp
     ${PACKAGE_PATH}/utility/*.hpp)

file(GLOB_RECURSE PACKAGE_CXX_SOURCES
     ${PACKAGE_PATH}/executors/*.cpp
     ${PACKAGE_PATH}/futures/*.cpp
     ${PACKAGE_PATH}/support/*.cpp
     ${PACKAGE_PATH}/syncing/*.cpp
     ${PACKAGE_PATH}/utility/*.cpp)

add_library(concurrency STATIC ${PACKAGE_CXX_SOURCES} ${PACKAGE_HEADERS})
target_include_directories(concurrency PUBLIC ${PACKAGE_INCLUDE_PATH})

file(GLOB CONCURRENCY_TEST_SRC ${PACKAGE_PATH}/tests/*.cpp)
add_executable(concurrency_tests ${CONCURRENCY_TEST_SRC})
target_link_libraries(concurrency_tests PUBLIC Catch2::Catch2)
target_link_libraries(concurrency_tests PUBLIC concurrency)