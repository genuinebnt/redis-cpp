cmake_minimum_required(VERSION 3.5)
project(redis_clone)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(client)
add_subdirectory(server)