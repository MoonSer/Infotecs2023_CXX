#include "ServerController.hpp"
#include <iostream>

ServerController::ServerController(std::ostream &writeStream, 
                                   std::reference_wrapper<Buffer> buffer, 
                                   const std::string &serverAddress, 
                                   std::chrono::milliseconds reconnectTimeout) 
    : m_writeStream(writeStream), m_buffer(buffer), 
      m_serverAddress(serverAddress), m_reconnectTimeout(reconnectTimeout), 
      m_inWork(false) {}

ServerController::~ServerController() {
    this->stop();
    #ifdef WIN32
        Socket::WSAClean();
    #endif
}

void ServerController::start() {
    // Только один поток ServerController::_start возможен!
    if (this->m_workThread.joinable())
        this->stop();
    
    this->m_workThread = std::thread(&ServerController::_start, this);
    this->setInWork(true);
}

void ServerController::stop() noexcept {
    // Для остановки - устанавливаем флаг и пробуждаем поток через буфер (правда костыльненько это делать через буфер).
    this->setInWork(false);
    this->m_buffer.get().wakeUpOne();
    
    if (this->m_workThread.joinable())
        this->m_workThread.join();
}

bool ServerController::inWork() const noexcept
    { return this->m_inWork; }

void ServerController::setInWork(bool inWork) noexcept {
    this->m_inWork = inWork;
}

void ServerController::_start() {
    this->initializeSocket();

    Buffer &buffer = this->m_buffer.get();
    while (true) {
        if (buffer.size() == 0)
            buffer.waitData();

        while (buffer.size() > 0 && this->inWork()) {
            auto data = buffer.front();
            if (this->processEvent(data))
                buffer.pop();
        }

        // inWork - флаг завершения программы, нужен для корректного окончания потока
        if (!this->inWork())
            break;
    }
}

bool ServerController::processEvent(const UserInputData &data) {
    this->printProcessingData(data);

    // Выход из цикла через break, если сконнектился до сервера и отправил
    while (this->inWork()) {
             
        while (!this->m_sock.isConnected() && this->inWork()) {
            std::this_thread::sleep_for(this->m_reconnectTimeout);
            if (this->m_sock.connectTo(this->m_serverAddress))
                break;
        }
        this->m_writeStream << "Sending..." << this->m_sock.isConnected() << " " << errno << " " << strerror(errno) << "\n";    
        if (this->m_sock.isConnected() && this->m_sock.sendAll(std::to_string(data.getSum()) + "\n"))
            return true;
    }
    return false;
}

void ServerController::initializeSocket() {
    this->m_sock.setNonBlocking();
    this->m_sock.connectTo(this->m_serverAddress);
}

void ServerController::printProcessingData(const UserInputData &data) const noexcept {
    this->m_writeStream << "\r" << data.string() << "\n >";
}

