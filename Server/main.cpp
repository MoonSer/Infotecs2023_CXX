#include <iostream>
#include "Socket.hpp"
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>

class Server {
    public:
        Server(int port = 3000) {
            this->m_sock.setNonBlocking();
            this->m_sock.bindTo(port);
            this->m_sock.startListen();
        }

        void loop() {
            while (true){
                this->tryAcceptConnection();

                for (auto &clientSock : this->m_clients)
                    this->tryReadFrom(clientSock);
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
            }
        }

        void tryAcceptConnection() {
            auto accepted = this->m_sock.tryAccept();
            if (accepted.has_value())
                this->m_clients.push_back(std::move(accepted.value()));
        }

        void tryReadFrom(Socket &socket) {
            auto data = socket.readAll();
                    
            if (data.has_value()) {
                if (this->validateMessage(data.value()) == true)
                    std::cout << "Message validated: \"" + data.value() + "\"\n";
            }

            if (socket.isConnected() == false)  
                this->m_clients.erase(std::remove(this->m_clients.begin(), this->m_clients.end(), socket));
        }

        bool validateMessage(const std::string &message) {
            auto convertedToInt = this->strToInt(message);
            if (message.size() > 2 && convertedToInt.has_value() && convertedToInt.value() % 32 == 0)
                return true;
            return false; 
        }

        std::optional<int> strToInt(const std::string &str) {
            try{
                return std::stoi(str);
            }catch(...) {
                return std::nullopt;
            }
        }

    private:
        Socket m_sock;
        std::vector<Socket> m_clients;
};



int main() {
#ifdef WIN32
    Socket::WSAInitialize();
#endif
    
    Server serv;
    serv.loop();
    
#ifdef WIN32
    Socket::WSAClean();
#endif
}
