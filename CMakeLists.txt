project(foo)
cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

include_directories(include)

file(GLOB SERVER_SOURCES "src/server.c")
file(GLOB CLIENT_SOURCES "src/client.c")
file(GLOB MULTI_SERVER_SOURCES "src/multi_server.c")

add_executable(server ${SERVER_SOURCES})
add_executable(multiserver ${MULTI_SERVER_SOURCES})
add_executable(client ${CLIENT_SOURCES})

target_link_libraries(multiserver pthread)

