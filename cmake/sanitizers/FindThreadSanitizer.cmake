if (ENABLE_THREAD_ADDRESS)
    message(STATUS "ThreadSanitizer is enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -g -fno-omit-frame-pointer")
endif (ENABLE_THREAD_ADDRESS)
