file(GLOB_RECURSE DSAC_DIST_CONSENSUS_ABD_CONSENSUS_HEADERS "consensus/*.hpp" "consensus/*.cpp")
file(GLOB_RECURSE DSAC_DIST_CONSENSUS_ABD_TRANSPORT_HEADERS "transport/*.hpp" "transport/*.cpp")

add_executable(dist.consensus.abd
        main.cpp
        ${DSAC_DIST_CONSENSUS_ABD_CONSENSUS_HEADERS}
        ${DSAC_DIST_CONSENSUS_ABD_TRANSPORT_HEADERS}
        )

include(FetchContent)

FetchContent_Declare(json
        URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
        )
FetchContent_MakeAvailable(json)

target_link_libraries(dist.consensus.abd PRIVATE fmt::fmt dsac nlohmann_json::nlohmann_json)

FILE(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/static/config.json
        "{\n"
        "  \"executable\" : \"${CMAKE_CURRENT_BINARY_DIR}/dist.consensus.abd\"\n"
        "}\n"
        )