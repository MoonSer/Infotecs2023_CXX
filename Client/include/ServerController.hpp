#ifndef SERVERCONTROLLER_HPP
#define SERVERCONTROLLER_HPP

#include "Buffer.hpp"
#include "Socket.hpp"
#include <thread>
#include <ostream>

class ServerController {
    public:
        ServerController(std::ostream &outStream, std::reference_wrapper<Buffer> buffer);
        ~ServerController();

        void start();

        void stop() noexcept;

    private:
        void _start();
        void initializeSocket();
        void processEvent(const UserInputData &data);
        void printPulledData(const UserInputData &data);
    
    private:
        std::thread m_workThread;
        std::reference_wrapper<Buffer> m_buffer;
        std::ostream &m_writeStream;
        Socket m_sock;
        bool m_inWork;
};

#endif //SERVERCONTROLLER_HPP