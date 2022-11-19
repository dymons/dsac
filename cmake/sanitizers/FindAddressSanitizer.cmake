if (ENABLE_SANITIZE_ADDRESS)
    message(STATUS "AddressSanitizer is enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -static-libasan -g -fno-omit-frame-pointer")
endif (ENABLE_SANITIZE_ADDRESS)
