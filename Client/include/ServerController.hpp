#ifndef SERVERCONTROLLER_HPP
#define SERVERCONTROLLER_HPP

#include "Buffer.hpp"
#include "Socket.hpp"

#include <thread>
#include <ostream>

struct ServerAddress {
    std::string ip;
    unsigned short port;
};

class ServerController {
    public:
        ServerController(std::ostream &writeStream, 
                         std::reference_wrapper<Buffer> buffer, 
                         const std::string &ip = "127.0.0.1",
                         unsigned short port = 3000, 
                         std::chrono::milliseconds reconnectTimeout = std::chrono::seconds(5));
        ~ServerController();

        void start();
        void stop() noexcept;

        bool inWork() const noexcept;
        void setInWork(bool inWork) noexcept;

    private:
        void _start();
        bool _processEvent(const UserInputData &data);
    
    private:
        std::thread m_workThread;
        std::ostream &m_writeStream;
        std::reference_wrapper<Buffer> m_buffer;
        ServerAddress m_serverAddress;
        std::chrono::milliseconds m_reconnectTimeout;
  
        Socket m_sock;
        bool m_inWork;
};

#endif //SERVERCONTROLLER_HPP