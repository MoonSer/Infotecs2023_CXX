#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <cstring>
#ifdef WIN32
    #include <Winsock2.h>
    #include <WS2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    // #include <unistd.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    // #include <netdb.h>
#endif

#include <string>
#include <stdexcept>

class Socket {
    public:
        Socket();
        ~Socket();

        void connect(const std::string &ipPort);
        void connect(const std::string &ip, unsigned int port);
    
    private:
        void initialize();

        void throwSocketError();

    private:
#ifndef WIN32
        int m_sock;
#else 
        SOCKET m_sock;
#endif
};

#endif //SOCKET_HPP