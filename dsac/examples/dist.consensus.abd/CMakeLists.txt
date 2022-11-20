add_executable(dist.consensus.abd main.cpp)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(dist.consensus.abd PRIVATE fmt::fmt dsac nlohmann_json::nlohmann_json)