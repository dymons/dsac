find_package(fmt REQUIRED)

add_executable(echo.network.select main.cpp)
target_link_libraries(echo.network.select fmt::fmt)

FILE(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/static/config.json
        "{\n"
        "  \"echo_server_binary\" : \"${CMAKE_CURRENT_BINARY_DIR}/echo.network.select\"\n"
        "}\n"
        )
