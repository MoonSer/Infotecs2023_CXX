#include <iostream>

#include "include/UserInputController.hpp"
#include "include/ServerController.hpp"

int main() {
    Buffer sharedBuffer;
    
    ServerController serverController(std::ref(sharedBuffer));
    serverController.start();

    UserInputController userController(std::ref(sharedBuffer), std::cin, std::cout);
    userController.addValidator([] (const std::string &str) -> bool { return str.size() < 65; });
    userController.start();
}
