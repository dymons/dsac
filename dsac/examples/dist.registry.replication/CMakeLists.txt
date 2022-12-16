file(GLOB_RECURSE DSAC_DIST_CONSENSUS "src/*.hpp" "src/*.cpp")

add_executable(dist.registry.replication main.cpp ${DSAC_DIST_CONSENSUS})

include(FetchContent)
FetchContent_Declare(json
        URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
        )
FetchContent_MakeAvailable(json)
target_link_libraries(dist.registry.replication PRIVATE fmt::fmt dsac nlohmann_json::nlohmann_json)
FILE(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/static/config.json
        "{\n"
        "  \"executable\" : \"${CMAKE_CURRENT_BINARY_DIR}/dist.registry.replication\"\n"
        "}\n"
        )

find_package(Threads)
extract_subdirs(test_subdirs ${CMAKE_CURRENT_SOURCE_DIR}/unittests)
foreach (subdir ${test_subdirs})
    file(GLOB DSAC_UNITTESTS_SRC "unittests/${subdir}/*.cpp")
    add_executable(dist.registry.replication-${subdir}-test ${DSAC_UNITTESTS_SRC} ${DSAC_DIST_CONSENSUS})
    target_link_libraries(dist.registry.replication-${subdir}-test ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} fmt::fmt dsac nlohmann_json::nlohmann_json)
endforeach ()