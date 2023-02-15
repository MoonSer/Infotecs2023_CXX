#include "Socket.hpp"

inline void NetworkHelper::throwError(const std::string &str) {
    if (str.empty())
        throw std::runtime_error("Error: " + NetworkHelper::getStrError());
    throw std::runtime_error("Error: " + str);
}

inline std::string NetworkHelper::getStrError() {
#ifdef WIN32
        static char message[256] = {0};
        FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
        0, WSAGetLastError(), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), message, 256, 0);
        char *nl = strrchr(message, '\n');
        if (nl) *nl = 0;
        return message;
#else
        return strerror(errno);
#endif
}

inline std::pair<std::string, std::string> NetworkHelper::splitIpPort(const std::string &ipPort) {
    auto doubleDotPos = ipPort.find(':');
    return {ipPort.substr(0, doubleDotPos), ipPort.substr(doubleDotPos+1)};
}

inline int NetworkHelper::portToInt(const std::string &port) {
    try{
        return std::stoi(port);
    }catch(std::exception &e) {
        throw std::invalid_argument("Error: can't convert to int - \"" + port + "\"");
    }
}




#ifdef WIN32
bool Socket::m_WSAInitialized = false;

void Socket::WSAInitialize() {
    WSADATA wsData;

    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0)
        NetworkHelper::throwError();
    Socket::m_WSAInitialized = true;
}

void Socket::WSAClean() {
    WSACleanup();
}
#endif

Socket::Socket() : m_connected(true) {
#ifdef WIN32
    if (!Socket::m_WSAInitialized)
        throw std::runtime_error("WSAData not initialized. Please, use Socket::WSAInitialize()");
#endif
    this->initialize();
}

Socket::Socket(Socket &&sock) {
    std::swap(this->m_sock, sock.m_sock);
}

Socket &Socket::operator=(Socket &&sock) {
    std::swap(this->m_sock, sock.m_sock);
    return *this;
}

#ifdef WIN32
Socket::Socket(SOCKET socket, bool isConnected) : m_sock(socket), m_connected(isConnected) {
    this->enableKeepAlive();
}
#else
Socket::Socket(int socket, bool isConnected) : m_sock(socket), m_connected(isConnected) {
    this->enableKeepAlive();
}
#endif

Socket::~Socket() {
    this->cleanup();
}

bool Socket::isConnected() const {
    return this->m_connected;
}

void Socket::initialize() {
    this->m_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (this->m_sock < 0)
        NetworkHelper::throwError();
    
    this->enableKeepAlive();
}

void Socket::cleanup() {
#ifdef WIN32
    closesocket(this->m_sock);
#else 
    close(this->m_sock);
#endif
}

bool Socket::setNonBlocking(bool yes) {
    unsigned long enabled = yes;
#ifdef WIN32
   return ioctlsocket(this->m_sock, FIONBIO, &enabled) == 0;
#else
    return fcntl(this->m_sock, F_SETFL, O_NONBLOCK, enabled) != -1;
#endif
}

void Socket::enableKeepAlive() {
    int flag = 1;
    if (setsockopt(this->m_sock, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char *>(&flag), sizeof(flag)) != 0)
        NetworkHelper::throwError("Can't enable keep-alive");
}



bool Socket::connectTo(const std::string &ipPort) {
    auto [ip, portStr] = NetworkHelper::splitIpPort(ipPort);    
    return this->connectTo(ip, NetworkHelper::portToInt(portStr));
}

bool Socket::connectTo(const std::string &ip, unsigned int port) {   
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

    
    if (connect(this->m_sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) != 0) 
        return false;
    
    this->m_connected = true;
    
    return true;
}



bool Socket::bindTo(const std::string &ipPort) {
    auto [ip, portStr] = NetworkHelper::splitIpPort(ipPort);
    return this->bindTo(ip, NetworkHelper::portToInt(portStr));
}

bool Socket::bindTo(const std::string &ip, unsigned int port) {
    sockaddr_in server;
    
    server.sin_family = AF_INET;
	inet_pton(AF_INET, ip.c_str(), &server.sin_addr);
	server.sin_port = htons(port);
    
    return (bind(this->m_sock, reinterpret_cast<sockaddr *>(&server) , sizeof(server)) == 0);
}

bool Socket::bindTo(unsigned int port) {
    sockaddr_in server;
    
    server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
    
    return (bind(this->m_sock, reinterpret_cast<sockaddr *>(&server) , sizeof(server)) == 0);
}



void Socket::startListen(int count) {
    listen(this->m_sock, count);
}



std::optional<Socket> Socket::tryAccept() {    
    auto ret = accept(this->m_sock, nullptr, nullptr);
    
    if (WSAGetLastError() == WSAEWOULDBLOCK)
        return std::nullopt;
    
    if (WSAGetLastError() != 0)
        throw std::runtime_error(NetworkHelper::getStrError());
    
    return Socket(ret, true);
}

bool Socket::sendAll(const std::string &message) {
    int bytesTotalSend = 0;
    while(bytesTotalSend < message.size()) {
        int currentSended = send(this->m_sock, message.c_str()+bytesTotalSend, message.size()-bytesTotalSend, 0);

        int error = WSAGetLastError();
        if (currentSended == -1 || error == WSAENETRESET || error == WSAECONNRESET) {
            this->m_connected = false;
            return false;
        }
        
        bytesTotalSend += currentSended;
    }

    return true; 
}

std::optional<std::string> Socket::readAll() {
    std::string readedString;
    int bytesReaded;
    do {
        char buf[1024];
        bytesReaded = recv(this->m_sock, buf, 1024, 0);

        int error = WSAGetLastError();
        if (bytesReaded == -1) {
            if  (error == WSAENETRESET || error == WSAECONNRESET) {
                this->m_connected = false;
                return std::nullopt;
            }else if (error == WSAEWOULDBLOCK || error == WSAENOTSOCK) {
                return readedString.empty() ? std::nullopt : std::optional(readedString);
            }
        }
        
        readedString.append(buf, bytesReaded);
    }while(bytesReaded > 0);

    return readedString;
}

bool operator==(const Socket &l, const Socket &r) {
    return l.m_sock == r.m_sock;
}
