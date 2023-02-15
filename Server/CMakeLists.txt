cmake_minimum_required(VERSION 3.0.0)
project(Server VERSION 0.1)

set(CMAKE_CXX_STANDARD_REQUIRED 11)

set(SOURCES main.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} Socket)