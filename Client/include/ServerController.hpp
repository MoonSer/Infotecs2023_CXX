#ifndef SERVERCONTROLLER_HPP
#define SERVERCONTROLLER_HPP

#include "Buffer.hpp"
#include "Socket.hpp"
#include <thread>

class ServerController {
    public:
        ServerController(std::reference_wrapper<Buffer> buffer);
        ~ServerController();

        void start();

        void stop() noexcept;

    private:
        void _start();
    
    private:
        std::thread m_workThread;
        std::reference_wrapper<Buffer> m_buffer;
        Socket m_sock;
        bool m_inWork;
};

#endif //SERVERCONTROLLER_HPP