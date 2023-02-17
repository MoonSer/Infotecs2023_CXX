#include <iostream>

#include "Server.hpp"


int main() {
#ifdef WIN32
    Socket::WSAInitialize();
#endif
    
    Server serv(std::cout);
    serv.loop();
    
#ifdef WIN32
    Socket::WSAClean();
#endif
}
