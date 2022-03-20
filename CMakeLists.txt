cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

set(PACKAGE_NAME "dsac")
if (NOT DEFINED PACKAGE_VERSION)
    set(PACKAGE_VERSION "1.0.0-dev")
endif ()
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_TARNAME "${PACKAGE_NAME}-${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "https://github.com/dymons/dsac/issues")

project(${PACKAGE_NAME} CXX C ASM)

include(cmake/CheckCompiler.cmake)
include(cmake/CompileOptions.cmake)
include(cmake/Modules.cmake)

add_subdirectory(dsac)
add_subdirectory(docs)
