#include "Server.hpp"

Server::Server(std::ostream &writeStream, unsigned short port) 
    : m_writeStream(writeStream), m_inWork(false) {  this->_initializeSocket(port); }

Server::~Server() {
#ifdef WIN32
    Socket::WSACleanup();
#endif
}

void Server::loop() {
    this->m_inWork = true;
    while (this->m_inWork){
        std::vector<int> clientsToCloseConnection;
        int pollSatus = poll(this->m_sockets.data(), this->m_sockets.size(), -1);
        
        if (pollSatus == -1) {
            this->_cleanup();
            SocketError::throwError("Server::loop(). Poll errror");
        }

        // Новое соединение
        if (this->m_sockets.at(0).revents & POLLIN)
            this->_acceptConnection();

        // Какой-то из клинетских сокетов готов читать
        for (auto it = ++this->m_sockets.begin(); it != this->m_sockets.end(); ++it) {

            if (it->revents == 0){
                continue;

            }else if (it->revents & POLLIN) {
                if (!this->_readAndProcessFrom(Socket(it->fd))) 
                    clientsToCloseConnection.emplace_back(it->fd);
            }else{
                clientsToCloseConnection.emplace_back(it->fd);
            }
        }

        // Удаляем отключившихся клиентов
        this->m_sockets.erase(
            std::remove_if(this->m_sockets.begin(), this->m_sockets.end(), 
                [clientsToCloseConnection] (const auto &pollSocket) -> bool {
                    return std::find(clientsToCloseConnection.begin(), clientsToCloseConnection.end(), pollSocket.fd) != clientsToCloseConnection.end();
                }),
        this->m_sockets.end());
    }
}


void Server::addValidator(ValidatorType validator) noexcept {
    this->m_validators.emplace_back(validator);
}


void Server::_initializeSocket(unsigned short port) {
#ifdef WIN32
    Socket::WSAStartup();
#endif
    Socket serverSock;
    if (!serverSock.setNonBlocking(true) || !serverSock.setReuseAddr(true))
        SocketError::throwError("Server::_initializeSocket()");
    serverSock.bindOrThrow(3000);
    serverSock.listenOrThrow(3);
    this->m_sockets.emplace_back(pollfd{serverSock.getRaw(), POLLIN});
}


void Server::_acceptConnection() {
    Socket serverSock(this->m_sockets.front().fd);
    do {
        auto client = serverSock.accept();
        
        if (!client.has_value()) {
            if (SocketError::isEAgain())
                break;
            
            SocketError::throwError("Server::_acceptConnection()");
        }

        client.value().setNonBlocking(true);

        this->m_sockets.emplace_back(pollfd{client.value().getRaw(), POLLIN});
    }while (true);
}

bool Server::_readAndProcessFrom(Socket &&clientSock) {
    auto readedData = clientSock.recv();
    if (!readedData.has_value()) 
        return false;

    this->processData(readedData.value());
    return true;
}

void Server::_cleanup() {
    for (const auto &pollfdSock : this->m_sockets)
        Socket(pollfdSock.fd).cleanup();
#ifdef WIN32
    Socket::WSACleanup();
#endif
}

void Server::processData(const std::string &data) const noexcept {
    std::size_t lastDelimIndex = -1;
    while (lastDelimIndex+1 != data.size()) {
        auto currentString = data.substr(lastDelimIndex+1, data.find('\n', lastDelimIndex+1)-lastDelimIndex-1);

        if (this->validateMessage(currentString))
            this->m_writeStream << "Message validated: \"" + currentString + "\"\n";
        
        lastDelimIndex = data.find('\n', lastDelimIndex+1);
        break;
    }

}

bool Server::validateMessage(const std::string &message) const {
    for (auto &validator : this->m_validators)
        if (!validator(message))
            return false;
    return true;
}