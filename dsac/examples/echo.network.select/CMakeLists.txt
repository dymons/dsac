find_package(fmt REQUIRED)

add_executable(echo.network.select main.cpp)
target_link_libraries(echo.network.select fmt::fmt)
