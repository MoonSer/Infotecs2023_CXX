cmake_minimum_required(VERSION 3.0.0)
project(Socket VERSION 0.1)

set(CMAKE_CXX_STANDARD_REQUIRED 11)

set(SOURCES include/Socket.hpp
            src/Socket.cpp)

add_library(${PROJECT_NAME} ${SOURCES})

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC ws2_32)
endif(WIN32)
target_include_directories(${PROJECT_NAME} PUBLIC include)