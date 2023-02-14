#include "Socket.hpp"

#include <iostream>
Socket::Socket() {
    this->initialize();
}

Socket::~Socket() {
#ifdef WIN32
    WSACleanup();
#endif
}

void Socket::connect(const std::string &ipPort) {
    auto doubleDotPos = ipPort.find(':');
    std::string ip = ipPort.substr(0, doubleDotPos);
    std::string port = ipPort.substr(doubleDotPos+1);
    
    std::cout << ip << " " << port << "\n";
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_aton(ip.c_str(), &addr.sin_addr);
    try{
        addr.sin_port = htons(std::stoi(port));
    }catch(std::exception &e) {
        throw std::runtime_error(e.what() + std::string("Ip:port - " + ip + ":" + port));
    }

    if (connect(this->m_sock, (struct sockaddr *)&addr, sizeof(addr)) != 0) 
        this->throwSocketError();
}

void Socket::initialize() {
    #ifdef WIN32
        WSADATA wsData;

	    if ( WSAStartup(MAKEWORD(2,2), &wsData) != 0 )
	    	this->throwSocketError();
    #endif

    this->m_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (this->m_sock < 0)
        this->throwSocketError();
    
    int flag = 1;
    if (setsockopt(this->m_sock, SOL_SOCKET, SO_KEEPALIVE, (void *)&flag, sizeof(flag))) {
        // TODO
    }
}

void Socket::throwSocketError() {
#ifdef WIN32
    throw std::runtime_error("Socket error! Code is: " + std::string{WSAGetLastError()});
#else
    throw std::runtime_error("Socket error! Code is: " + std::string{std::strerror(errno)});
#endif
}
