#include <iostream>
#include "Server.hpp"

int main() {
    Server serv(std::cout);
    serv.addValidator([] (const std::string &message) { return message.size() > 1;});
    serv.addValidator([] (const std::string &message) { 
        try{
            return std::stoi(message) % 32 == 0;
        }catch (...) {
            return false;
        }
    });
    serv.loop();
}
