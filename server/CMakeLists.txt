project(server)
add_executable(server server.cpp session.cpp)
target_link_libraries(server Boost::log Boost::boost)