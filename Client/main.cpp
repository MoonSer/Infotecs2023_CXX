#include <iostream>

#include <thread>
#include <chrono>
#include "include/UserInputController.hpp"
#include "include/ServerController.hpp"
int main() {
    #ifdef WIN32
        Socket::WSAInitialize();
    #endif
    Buffer sharedBuffer;
    
    ServerController serverController(std::cout, std::ref(sharedBuffer));
    serverController.start();

    UserInputController userController(std::ref(sharedBuffer), std::cin, std::cout);
    userController.addValidator([] (const std::string &str) -> bool { return str.size() < 65; });
    userController.start();
}