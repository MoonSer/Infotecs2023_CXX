cmake_minimum_required(VERSION 3.0.0)
project(Client VERSION 0.1)

set(CMAKE_CXX_STANDARD_REQUIRED 11)

set(SOURCES main.cpp
            include/Buffer.hpp
            include/UserInputController.hpp
            include/ServerController.hpp
            include/Converter.hpp

            src/Buffer.cpp
            src/UserInputController.cpp
            src/ServerController.cpp
            src/Converter.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include)