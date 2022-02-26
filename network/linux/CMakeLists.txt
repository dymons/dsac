add_executable(client client.cpp)
target_link_libraries(client fmt::fmt)

add_executable(server_select server_select.cpp)
add_executable(server_poll server_poll.cpp)
add_executable(server_epoll server_epoll.cpp)