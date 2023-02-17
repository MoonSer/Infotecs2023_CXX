#include "Server.hpp"

Server::Server(std::ostream &writeStream, int port) : m_writeStream(writeStream) {
    this->m_sock.setNonBlocking();
    this->m_sock.enableKeepAlive();
    if (!this->m_sock.bindTo(port))
        throw std::runtime_error(strerror(errno));
    this->m_sock.startListen();
}


void Server::loop() {
    while (true){
        this->tryAcceptConnection();
        // this->m_writeStream << "Base: " << this->m_sock.get() << "\n";
        // this->m_writeStream << "Clients:" << "\n";
        for (auto iter = this->m_clients.begin(); iter != this->m_clients.end();) {
            // this->m_writeStream << iter->get() << "\n";

            if (!this->tryReadFrom(std::ref(*iter))) {
                iter = this->m_clients.erase(iter);
            }else{
                ++iter;
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void Server::tryAcceptConnection() {
    auto accepted = this->m_sock.tryAccept();
    if (accepted.has_value()) {

        accepted.value().setNonBlocking();
        this->m_clients.push_back(std::move(accepted.value()));
    }
}

bool Server::tryReadFrom(Socket &socket) {
    auto readedFromSocket = socket.readAll();

    if (readedFromSocket.has_value())
        this->processData(readedFromSocket.value());
    
    if (!socket.isConnected())
        return false;
    
    return true;
}

void Server::processData(const std::string &data) const noexcept {
    std::size_t lastDelimIndex = -1;
    while (lastDelimIndex+1 != data.size()) {
        auto currentString = data.substr(lastDelimIndex+1, data.find('\n', lastDelimIndex+1)-lastDelimIndex-1);

        if (this->validateMessage(currentString))
            this->m_writeStream << "Message validated: \"" + currentString + "\"\n";
        
        lastDelimIndex = data.find('\n', lastDelimIndex+1);
    }

}

bool Server::validateMessage(const std::string &message) const noexcept {
    auto convertedToInt = this->strToInt(message);

    if (message.size() > 1 && convertedToInt.has_value() && convertedToInt.value() % 32 == 0)
        return true;
    return false;
}

std::optional<int> Server::strToInt(const std::string &str) const noexcept {
    try{
        return std::stoi(str);
    }catch(...) {
        return std::nullopt;
    }
}