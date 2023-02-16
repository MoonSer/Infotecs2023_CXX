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
    
    // Socket::WSAInitialize();
    // Socket sock;
    // sock.connectTo("127.0.0.1:3000");
    // if (!sock.sendAll("320"))
    //     std::cout << WSAGetLastError() << "\n";
    // else   
    //     std::cout << "Sended!\n";
    // std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    //  if (!sock.sendAll("4"))
    //     std::cout << WSAGetLastError() << "\n";
    // else   
    //     std::cout << "Sended!2\n";
    // std::this_thread::sleep_for(std::chrono::seconds(10));
}