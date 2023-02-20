#include "Socket.hpp"
#include <sstream>

void SocketError::throwError() 
    { throw std::runtime_error("Error: " + SocketError::getStrError()); }

void SocketError::throwError(const std::string &helpStr) 
    { throw std::runtime_error(helpStr + ". Error: " + SocketError::getStrError()); }

const std::string SocketError::getStrError() noexcept {
#ifdef WIN32
    static char message[256] = {0};
    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
    0, SocketError::getLastErrno(), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), message, 256, 0);
    char *nl = strrchr(message, '\n');
    if (nl) *nl = 0;
    return message;
#else

    return strerror(SocketError::getLastErrno());
#endif
}

int SocketError::getLastErrno() noexcept {
#ifdef WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

bool SocketError::isSuccess() noexcept 
    { return SocketError::getLastErrno() == 0; }

bool SocketError::isAlreadyConnected() noexcept { 
#ifdef WIN32
    return SocketError::getLastErrno() == WSAEISCONN;
#else
    return SocketError::getLastErrno() == EISCONN;
#endif
}

bool SocketError::isEAgain() noexcept {
    int error = SocketError::getLastErrno();
#ifdef WIN32
    return error == WSAEWOULDBLOCK;
#else
    return error == EWOULDBLOCK || error == EAGAIN;
#endif
}

bool SocketError::isConnectionDroped() noexcept {
int errnoCode = SocketError::getLastErrno();
#ifdef WIN32
    return errnoCode == WSAENETRESET || errnoCode == WSAECONNRESET;
#else
    return errnoCode == ECONNABORTED || errnoCode == ENOTCONN || errnoCode == ECONNREFUSED;
#endif
}



Socket::Socket() : m_sock(-1) {
#ifdef WIN32
    if (!Socket::m_WSAInitialized)
        throw std::runtime_error("Socket::Socket() - Error: WSAData not initialized. Please, use Socket::WSAInitialize()");
#endif
    this->_initialize();
}

Socket::Socket(Socket &&sock) {
    std::swap(this->m_sock, sock.m_sock);
}

Socket &Socket::operator=(Socket &&sock) {
    std::swap(this->m_sock, sock.m_sock);
    return *this;
}

#ifdef WIN32
bool Socket::m_WSAInitialized = false;

void Socket::WSAStartup() {
    WSADATA wsData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsData) != 0)
        SocketError::throwError("Socket::WSAStartup()");
    Socket::m_WSAInitialized = true;
}

void Socket::WSACleanup() {
    ::WSACleanup();
}

Socket::Socket(SOCKET socket) 
    : m_sock(socket) {}

#else
Socket::Socket(int socket) 
    : m_sock(socket) {}
#endif

bool Socket::isValid() const noexcept 
    { return this->m_sock > 0; }

#ifdef WIN32
SOCKET Socket::getRaw() const noexcept
#else
int Socket::getRaw() const noexcept
#endif
    { return this->m_sock; }


void Socket::_initialize() {
    if (this->isValid())
        this->cleanup();
    this->m_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (this->m_sock < 0)
        SocketError::throwError("Socket::_initialize()");
}

void Socket::cleanup() noexcept {
#ifdef WIN32
    closesocket(this->m_sock);
#else 
    close(this->m_sock);
#endif
    this->m_sock = -1;
}


bool Socket::setKeepAlive(bool status) noexcept {
    return setsockopt(this->m_sock, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char *>(&status), sizeof(status)) != 0;
}

bool Socket::setNonBlocking(bool status) noexcept {
#ifdef WIN32
    return ioctlsocket(this->m_sock, FIONBIO, reinterpret_cast<unsigned long *>(&status));
#else
    int flags = fcntl(this->m_sock, F_GETFL, 0);
    if (flags == -1) return false;
    flags = status ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    return (fcntl(this->m_sock, F_SETFL, flags) == 0)
#endif
}

bool Socket::connect(const std::string &ip, unsigned short port) noexcept{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

    try{
        if (::connect(this->m_sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) != 0) {
            if (SocketError::isAlreadyConnected()) {
                this->_initialize();
                return this->connect(ip, port);
            }
            return false;
        }
    }catch (...) {
        return false;
    }
    
    return true;
}

void Socket::bindOrThrow(const std::string &ip, unsigned short port) {
    if (!this->bind(ip, port))
        SocketError::throwError("Socket::bindOrThrow()");
}

bool Socket::bind(const std::string &ip, unsigned short port) noexcept {
    sockaddr_in server;
    
    server.sin_family = AF_INET;
	inet_pton(AF_INET, ip.c_str(), &server.sin_addr);
	server.sin_port = htons(port);
    
    try{
        return (::bind(this->m_sock, reinterpret_cast<sockaddr *>(&server) , sizeof(server)) == 0);
    }catch(...) {
        return false;
    }
}

void Socket::bindOrThrow(unsigned short port) {
    if (!this->bind(port))
        SocketError::throwError("Socket::bindOrThrow()");
}

bool Socket::bind(unsigned short port) noexcept {
    sockaddr_in server;
    
    server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
    
    try{
        return (::bind(this->m_sock, reinterpret_cast<sockaddr *>(&server) , sizeof(server)) == 0);
    }catch (...) {
        return false;
    }
}

bool Socket::listen(int count) noexcept {
    return (::listen(this->m_sock, count) == 0);
}

void Socket::listenOrThrow(int count) {
    if (!this->listen(count))
        SocketError::throwError("Socket::listenOrThrow()");
}

std::optional<Socket> Socket::accept() noexcept {
    try{
        auto clientSock = ::accept(this->m_sock, nullptr, nullptr);
        if (clientSock == -1) 
            return std::nullopt;
        return Socket(clientSock);
    }catch(...) {
        return std::nullopt;
    }
}

Socket Socket::acceptOrThrow() {
    auto clientSock = this->accept();
    if (!clientSock.has_value())
        SocketError::throwError("Socket::acceptOrThrow()");
    return std::move(clientSock.value());
}

bool Socket::send(const std::string &message) noexcept {
    int bytesTotalSend = 0;
    try{
        while(bytesTotalSend < message.size()) {
        
            int currentSended = ::send(this->m_sock, message.c_str()+bytesTotalSend, message.size()-bytesTotalSend, 0);
    
            if (currentSended == -1)
                return false;
           
            bytesTotalSend += currentSended;
        }
    }catch(...) {
        return false;
    }
    return true; 
}

std::optional<std::string> Socket::recv() noexcept {
    std::stringstream readedBuf;
    int bytesReaded;
    do {
        char buf[1024];
        try{
            bytesReaded = ::recv(this->m_sock, buf, 1024, 0);
            if (bytesReaded == -1)
                if (readedBuf.str().empty())
                    return std::nullopt;
                else
                    break;
        }catch(...) {
            return std::nullopt;
        }
        
        buf[bytesReaded] = '\0';
        readedBuf << buf;
    }while(bytesReaded > 0);
    
    return readedBuf.str();
}

bool operator==(const Socket &l, const Socket &r) {
    return l.m_sock == r.m_sock;
}
