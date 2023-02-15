#ifndef SOCKET_HPP
#define SOCKET_HPP


#ifdef WIN32
    #include <Winsock2.h>
    #include <WS2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif

#include <string>
#include <cstring>
#include <stdexcept>
#include <optional>

namespace NetworkHelper {
    inline std::string getStrError();
    inline void throwError(const std::string &str = "");
    inline std::pair<std::string, std::string> splitIpPort(const std::string &ipPort);
    inline int portToInt(const std::string &port);
};



class Socket {
    public:
        Socket();
        Socket(Socket &&sock);
        Socket &operator=(Socket &&);
#ifdef WIN32
        Socket(SOCKET socket, bool isConnected = false);
#else
        Socket(int socket, bool isConnected = false);
#endif
        ~Socket();

        bool isConnected() const;

        bool setNonBlocking(bool yes = true);
        void enableKeepAlive();

        bool connectTo(const std::string &ipPort);
        bool connectTo(const std::string &ip, unsigned int port);

        bool bindTo(const std::string &ipPort);
        bool bindTo(const std::string &ip, unsigned int port);
        bool bindTo(unsigned int port);

        void startListen(int count = 3);

        std::optional<Socket> tryAccept();

        bool sendAll(const std::string &message);

        std::optional<std::string> readAll();

        friend bool operator==(const Socket &l, const Socket &r);

#ifdef WIN32
        static void WSAInitialize();
        static void WSAClean();
#endif

    private:
        Socket(const Socket &) = delete;
        Socket &operator=(const Socket &) = delete;

        void initialize();
        void cleanup();


    private:
        bool m_connected;
#ifdef WIN32
        SOCKET m_sock;
        static bool m_WSAInitialized;
#else 
        int m_sock;
#endif
};

#endif //SOCKET_HPP