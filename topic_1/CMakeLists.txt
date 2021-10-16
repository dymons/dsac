project(topic_1 VERSION 1.0 LANGUAGES CXX)

file(GLOB LIB_HEADERS ${PROJECT_SOURCE_DIR}/sorts)
include_directories(${LIB_HEADERS})

file(GLOB TOPIC_1_TEST_SRC ${PROJECT_SOURCE_DIR}/tests/*.cpp)
add_executable(topic_1_tests ${TOPIC_1_TEST_SRC})
target_link_libraries(topic_1_tests PUBLIC Catch2::Catch2)