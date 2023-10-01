cmake_minimum_required(VERSION 3.5)
project(redis_clone)

if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conaninfo.txt)
    list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}")
endif()

find_package(Boost REQUIRED COMPONENTS log)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS} log)
else()
  message(FATAL_ERROR "Boost library and/or Boost.Log component not found. Please make sure Boost is installed.")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(client)
add_subdirectory(server)