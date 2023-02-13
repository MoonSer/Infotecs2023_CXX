#ifndef SERVERCONTROLLER_HPP
#define SERVERCONTROLLER_HPP

#include "Buffer.hpp"
#include <thread>

class ServerController {
    public:
        ServerController(std::reference_wrapper<Buffer> buffer);

        void start();

        void stop();

    private:
        void _start();
    
    private:
        std::thread m_workThread;
        std::reference_wrapper<Buffer> m_buffer;
        bool m_inWork;
};

#endif //SERVERCONTROLLER_HPP