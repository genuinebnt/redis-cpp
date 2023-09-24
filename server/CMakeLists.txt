project(server)
add_executable(server server.cpp)
target_include_directories(server PRIVATE ${PROJECT_SOUCE_DIR})