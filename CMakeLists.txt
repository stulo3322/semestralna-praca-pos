# cmake_minimum_required(VERSION <specify CMake version here>)
project(piskvorky C)

set(CMAKE_C_STANDARD 99)

add_executable(piskvorky main.c client.c server.c)

target_link_libraries(piskvorky pthread)