#ifndef SERVERCONTROLLER_HPP
#define SERVERCONTROLLER_HPP

#include "Buffer.hpp"
#include "Socket.hpp"

#include <thread>
#include <ostream>

class ServerController {
    public:
        ServerController(std::ostream &writeStream, 
                         std::reference_wrapper<Buffer> buffer, 
                         const std::string &serverAddress = "127.0.0.1:3000", 
                         std::chrono::milliseconds reconnectTimeout = std::chrono::seconds(2));
        ~ServerController();

        void start();

        void stop() noexcept;

        bool inWork() const noexcept;
        void setInWork(bool inWork) noexcept;

    private:
        void _start();
        void initializeSocket();
        bool processEvent(const UserInputData &data);
        void printProcessingData(const UserInputData &data) const noexcept;
    
    private:
        std::thread m_workThread;
        std::ostream &m_writeStream;
        std::reference_wrapper<Buffer> m_buffer;
        std::string m_serverAddress;
        std::chrono::milliseconds m_reconnectTimeout;
  
        Socket m_sock;
        bool m_inWork;
};

#endif //SERVERCONTROLLER_HPP