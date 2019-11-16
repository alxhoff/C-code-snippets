project(foo)
cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

include_directories(include)

file(GLOB SERVER_SOURCES "src/main_server.c")
file(GLOB CLIENT_SOURCES "src/main_client.c")

add_executable(server ${SERVER_SOURCES})
add_executable(client ${CLIENT_SOURCES})

target_link_libraries(server rt)
target_link_libraries(client rt)

