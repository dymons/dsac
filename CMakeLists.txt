cmake_minimum_required(VERSION 3.3.2 FATAL_ERROR)

set(PACKAGE_NAME "dsac")
if (NOT DEFINED PACKAGE_VERSION)
    set(PACKAGE_VERSION "0.0.0-dev")
endif ()
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_TARNAME "${PACKAGE_NAME}-${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "https://github.com/dymons/dsac/issues")

project(${PACKAGE_NAME} CXX)

option(BUILD_TESTS
        "Whether tests should be built" ON)
option(BUILD_BENCHMARKS
        "Whether benchmarks should be built" ON)
option(BUILD_DOCS
        "Whether doxygen documentation should be installed" OFF)
option(ENABLE_SANITIZE_ADDRESS
        "Enable AddressSanitizer for sanitized targets." OFF)
option(ENABLE_THREAD_ADDRESS
        "Enable ThreadSanitizer for sanitized targets." OFF)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/sanitizers" ${CMAKE_MODULE_PATH})

include(cmake/CompileOptions.cmake)
include(cmake/Macro.cmake)
include(cmake/Sanitizers.cmake)

add_subdirectory(dsac)

if (BUILD_DOCS)
    add_subdirectory(docs)
endif (BUILD_DOCS)
