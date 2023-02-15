cmake_minimum_required(VERSION 3.0.0)
project(Client VERSION 0.1)

set(CMAKE_CXX_STANDARD_REQUIRED 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(lib/Socket)
include_directories(lib/Socket/include)

add_subdirectory(Client)
add_subdirectory(Server)