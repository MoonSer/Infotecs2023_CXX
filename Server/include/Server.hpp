#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"

#ifdef WIN32
    #define poll(x, y, z) (::WSAPoll(x, y, z))
#else
    #include <poll.h>
    #define poll(x, y, z) (::poll(x, y, z))
#endif
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <ostream>
#include <functional>

class Server {
    using ValidatorType = std::function<bool(const std::string&)>;
    public:
        Server(std::ostream &writeStream, unsigned short port = 3000);
        ~Server();
        
        void loop();

        void addValidator(ValidatorType validator) noexcept;
        
        void signalHandled();

    private:
        void _initializeSocket(unsigned short port);
        void _acceptConnection();
        bool _readAndProcessFrom(Socket &&clientSock);
        void _cleanup();
        
        void processData(const std::string &data) const noexcept;
        bool validateMessage(const std::string &message) const;
    
    private:
        std::vector<pollfd> m_sockets; // m_sockets[0] - это серверный сокет!
        std::vector<ValidatorType> m_validators;
        std::ostream &m_writeStream;
        bool m_inWork;


    private:
        Server() = delete;
};

#endif //SERVER_HPP