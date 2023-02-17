#ifndef SERVER_HPP
#define SERVER_HPP


#include "Socket.hpp"

#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <ostream>

class Server {
    
    public:
        Server(std::ostream &writeStream, int port = 3000);
        void loop();
    
    private:
        void tryAcceptConnection();
        
        bool tryReadFrom(Socket &socket);

        void processData(const std::string &data) const noexcept;
        bool validateMessage(const std::string &message) const noexcept;

        std::optional<int> strToInt(const std::string &str) const noexcept;

    
    private:
        Socket m_sock;
        std::vector<Socket> m_clients;

        std::ostream &m_writeStream;


    private:
        Server() = delete;
};

#endif //SERVER_HPP