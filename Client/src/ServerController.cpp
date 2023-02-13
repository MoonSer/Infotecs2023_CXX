#include "ServerController.hpp"

ServerController::ServerController(std::reference_wrapper<Buffer> buffer) : m_buffer(buffer) {}

void ServerController::start() {
    // Только один поток ServerController::_start возможен!
    if (this->m_workThread.joinable())
        throw std::runtime_error("ServerController thread already in work!");
    
    this->m_workThread = std::thread(&ServerController::_start, this);
}

void ServerController::stop() noexcept {
    // Для остановки - устанавливаем флаг и пробуждаем поток через буфер (костыльненько это делать через буфер).
    this->m_inWork = false;
    this->m_buffer.get().wakeUpOne();
}

void ServerController::_start() {

    Buffer &buffer = this->m_buffer.get();
    while (true) {
        buffer.waitData();

        // inWork - флаг завершения программы, нужен для корректного окончания потока
        if (!this->m_inWork)
            break;

        while (buffer.size() > 0) {
            UserInputData data = buffer.pullData();

            // TODO: SendData
        }
    }
}
