cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

set(PACKAGE_NAME "dsac")
if (NOT DEFINED PACKAGE_VERSION)
    set(PACKAGE_VERSION "0.0.0-dev")
endif ()
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_TARNAME "${PACKAGE_NAME}-${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "https://github.com/dymons/dsac/issues")

project(${PACKAGE_NAME} CXX C ASM)

option(BUILD_TESTS
        "Whether tests should be built" ON)
option(BUILD_DOCS
        "Whether doxygen documentation should be installed" OFF)
option(ENABLE_SANITIZE_ADDRESS
        "Enable AddressSanitizer for sanitized targets." OFF)
option(ENABLE_THREAD_ADDRESS
        "Enable ThreadSanitizer for sanitized targets." OFF)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/sanitizers" ${CMAKE_MODULE_PATH})

include(cmake/CheckCompiler.cmake)
include(cmake/CompileOptions.cmake)
include(cmake/Macro.cmake)
include(cmake/Sanitizers.cmake)
include(cmake/Conan.cmake)

add_subdirectory(dsac)
add_subdirectory(dsac/examples)

if (BUILD_DOCS)
    add_subdirectory(docs)
endif (BUILD_DOCS)
