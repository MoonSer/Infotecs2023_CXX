cmake_minimum_required(VERSION 3.0.0)
project(Server VERSION 0.1)

set(CMAKE_CXX_STANDARD_REQUIRED 17)

set(SOURCES main.cpp 
            include/Server.hpp 
            src/Server.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} Socket)