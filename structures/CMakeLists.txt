get_filename_component(PACKAGE_INCLUDE_PATH ".." ABSOLUTE)
get_filename_component(PACKAGE_PATH "." ABSOLUTE)

file(GLOB_RECURSE PACKAGE_CXX_SOURCES ${PACKAGE_PATH}/graph/*.cpp ${PACKAGE_PATH}/tree/*.cpp)
file(GLOB_RECURSE PACKAGE_HEADERS ${PACKAGE_PATH}/graph/*.hpp ${PACKAGE_PATH}/tree/*.hpp ${PACKAGE_PATH}/tree/binary_search_tree/*.hpp)

add_library(structures STATIC ${PACKAGE_CXX_SOURCES} ${PACKAGE_HEADERS})
target_include_directories(structures PUBLIC ${PACKAGE_INCLUDE_PATH})

file(GLOB STRUCTURES_TEST_SRC ${PACKAGE_PATH}/tests/*.cpp)
add_executable(structures_tests ${STRUCTURES_TEST_SRC})
target_link_libraries(structures_tests PUBLIC Catch2::Catch2)
target_link_libraries(structures_tests PUBLIC structures)