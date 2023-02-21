#include "ServerController.hpp"

ServerController::ServerController(std::ostream &writeStream, 
                                   std::reference_wrapper<Buffer> buffer, 
                                   const std::string &ip,
                                   unsigned short port,
                                   std::chrono::milliseconds reconnectTimeout) 
    : m_writeStream(writeStream), m_buffer(buffer), 
      m_serverAddress{ip, port}, m_reconnectTimeout(reconnectTimeout), 
      m_inWork(false) {}

ServerController::~ServerController() {
    this->stop();
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

void ServerController::setInWork(bool inWork) noexcept 
    { this->m_inWork = inWork; }

void ServerController::_start() {
    Buffer &buffer = this->m_buffer.get();
    while (true) {
        if (buffer.size() == 0)
            buffer.waitData();

        while (buffer.size() > 0 && this->inWork())
            if (this->_processEvent(buffer.front()))
                buffer.pop();

        // inWork - флаг завершения программы, нужен для корректного окончания потока
        if (!this->inWork())
            break;
    }
}

bool ServerController::_processEvent(const UserInputData &data) {
    this->m_writeStream << "\r" << data.string() << "\n>" << std::flush;

    // Выход из цикла через break, если сконнектился до сервера и отправил
    while (this->inWork())  {
        if (this->m_sock.connect(this->m_serverAddress.ip, this->m_serverAddress.port))
            break;
        std::this_thread::sleep_for(this->m_reconnectTimeout);
    }
        
    if (this->m_sock.send(std::to_string(data.getSum()) + "\n"))
        return true;

    return false;
}