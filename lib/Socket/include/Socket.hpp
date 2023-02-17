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
    #include <unistd.h>
    #include <fcntl.h>
#endif

#include <string>
#include <cstring>
#include <stdexcept>
#include <optional>

namespace NetworkHelper {
    const std::string getStrError() noexcept;
    void throwError(const std::string &str = "");
    
    std::pair<std::string, std::string> splitIpPort(const std::string &ipPort) noexcept;
    
    int portToInt(const std::string &port);
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

        bool isConnected() const noexcept;
        bool isValid() const noexcept;

#ifdef WIN32
        SOCKET get();
#else
        int get() const noexcept;
#endif

        bool setNonBlocking(bool yes = true) noexcept;
        bool enableKeepAlive() noexcept;

        bool connectTo(const std::string &ipPort);
        bool connectTo(const std::string &ip, unsigned int port) noexcept;

        bool bindTo(const std::string &ipPort);
        bool bindTo(const std::string &ip, unsigned int port) noexcept;
        bool bindTo(unsigned int port) noexcept;

        bool startListen(int count = 5) noexcept;
        
        std::optional<Socket> tryAccept();

        bool sendAll(const std::string &message);
        
        std::optional<std::string> readAll();

        void cleanup() noexcept;

        friend bool operator==(const Socket &l, const Socket &r);

#ifdef WIN32
        static void WSAInitialize();
        static void WSAClean();
#endif

    private:
        Socket(const Socket &) = delete;
        Socket &operator=(const Socket &) = delete;

        void initialize();

        bool isHasErrors() const noexcept;
        bool isEAgain() const noexcept;
        bool isEConnectionDrop() const noexcept;


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