cmake_minimum_required(VERSION 3.10)

project("client")
project("server")


include_directories("./include")
link_directories("./lib")


add_executable(client src/client.cpp)
target_link_libraries(client enet ws2_32 winmm)


add_executable(server src/server.cpp)
target_link_libraries(server enet ws2_32 winmm)