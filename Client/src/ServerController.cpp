#include "ServerController.hpp"
#include <iostream>

ServerController::ServerController(std::ostream &writeStream, std::reference_wrapper<Buffer> buffer) : m_buffer(buffer), m_writeStream(writeStream) {}

ServerController::~ServerController() {
    this->stop();
    #ifdef WIN32
        Socket::WSAClean();
    #endif
}

void ServerController::start() {
    // Только один поток ServerController::_start возможен!
    if (this->m_workThread.joinable())
        throw std::runtime_error("ServerController thread already in work!");
    std::cout << "ServerController(): started\n";
    this->m_workThread = std::thread(&ServerController::_start, this);
}

void ServerController::stop() noexcept {
    // Для остановки - устанавливаем флаг и пробуждаем поток через буфер (костыльненько это делать через буфер).
    this->m_inWork = false;
    this->m_buffer.get().wakeUpOne();
}

void ServerController::_start() {
    std::cout << "ServerController(): initializeSocket\n";
    this->initializeSocket();

    Buffer &buffer = this->m_buffer.get();
    while (true) {
        std::cout << "ServerController(): NewIteration: check buffer size" << buffer.size() << "\n";
        if (buffer.size() == 0)
            buffer.waitData();
        std::cout << "ServerController(): Waked up!\n";
        // inWork - флаг завершения программы, нужен для корректного окончания потока
        if (this->m_inWork == false)
            break;
        
        if (this->m_inWork == true)
            std::cout << "SDdsadsadsa " << this->m_inWork << "\n";
        if (this->m_inWork == false)
            std::cout << "fffffffffff " << this->m_inWork << "\n";
        std::cout << this->m_inWork << "\n";
        while (buffer.size() > 0 && this->m_inWork == true) {
            std::cout << "=========================================\n";
            UserInputData data = buffer.pullData();
            std::cout << "NewEvent: " << data.string() << "\n";
            this->processEvent(data);
        }
    }
}

void ServerController::processEvent(const UserInputData &data) {
    this->printPulledData(data);

    // Выход из цикла через break, если сконнектился до сервера и отправил
    while (this->m_inWork == true) {
        std::cout << "Check connection...\n";
        while (this->m_sock.connectTo("127.0.0.1:3000") == false && this->m_inWork == true)
            std::this_thread::sleep_for(std::chrono::seconds(5));
        
        std::cout << "Sending...\n";
        if (this->m_sock.sendAll(std::to_string(data.getSum())) == true)
            break;
        
        std::cout << "ERROR...\n";
        std::cout << NetworkHelper::getStrError() << "\n";
    }
}

void ServerController::initializeSocket() {
    this->m_sock.setNonBlocking();
    this->m_sock.connectTo("127.0.0.1:3000");
}

void ServerController::printPulledData(const UserInputData &data) {
    this->m_writeStream << data.string() << "\n";
}

