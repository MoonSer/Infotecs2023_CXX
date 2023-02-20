/**
 * @file Socket.hpp
 * @author MoonSer
 * @brief Содержит описание класса Socket
 * @date 2023-02-21
 */
#ifndef SOCKET_HPP
#define SOCKET_HPP

#ifdef WIN32
    #include <Winsock2.h>
    #include <WS2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
#endif

#include <string>
#include <stdexcept>
#include <optional>


/// @brief Пространство имён, в котором содержатся спец. функции для выявления ошибок
namespace SocketError {
        /**
         * @brief Выбрасывает исключение с текстом ошибки
         * @throw std::runtime_error ВСЕГДА.
         * 
         * @details Функция просто выбрасывает исключение типа std::runtime_error, 
         *  передавая аргументом для исключения  текст последней ошибки сокета.
         */
        void throwError();

        
        /**
         * @brief Выбрасывает исключение с текстом ошибки
         * @param helpStr Строка, конкатенируется перед текстом ошибки.
         * @throw std::runtime_error ВСЕГДА.
         *  
         * @details Функция просто выбрасывает исключение типа std::runtime_error, 
         * передавая аргументом для исключения текст последней ошибки сокета.
         */
        void throwError(const std::string &helpStr);
        
        
        /**
         * @brief Возвращает текст последней ошибки
         * @return const std::string Строку, которая содержит текст ошибки
         * 
         * @details Возвращает текст последней ошибки работы сокета.
         */
        const std::string getStrError() noexcept;

        /**
         * @brief Возвращает номер последней ошибки
         * @return Номер послдней ошибки
         * 
         * @details Возвращает номер последней ошибки работы с сокетом
         */
        int getLastErrno() noexcept;
        
        /**
         * @brief Возвращает статус ошибки
         * @return true, если последняя операция с сокетом выполнена успешна
         * 
         * @details Возвращает статус выполнения последней операции работы с сокетом
         */
        bool isSuccess() noexcept;

        /**
         * @brief Возвращает статус ошибки
         * @return Возвращает true, если возникла ошибка EISCONN
         * 
         * @details Возвращает true, если последняя операция с сокетом завершилась неуспешно. 
         * Указывает на то, что пытались выполнить соединения сокетом, который до этого уже
         * совершал подключение.
         */
        bool isAlreadyConnected() noexcept;

        /**
         * @brief Возвращает статус ошибки
         * @return Возвращает true, если возникла ошибка EAGAIN
         * 
         * @details Возвращает true, если последняя операция с сокетом завершилась нейтрально 
         * (у неблокирующего сокета пока нет данных для использования).
         */
        bool isEAgain() noexcept;

        /**
         * @brief Возвращает статус ошибки
         * @return Возвращает true, если возникла ошибка ECONNREFUSED или ECONNABORTED
         * 
         * @details Возвращает true, если последняя операция с сокетом завершилась неуспешно, так как потеряно соединение с сервером(клиентом).
         */
        bool isConnectionDroped() noexcept;
};


/**
 * @brief Класс ТСP/IPv4 сокета
 * 
 * @details Класс сокета. Реализованы основные функции, необходимые для работы.
 */
class Socket {
    public:
        /**
         * @brief Создаёт новый экземпляр сокета
         * 
         * @details При создании, производится инициализация (создание нового сокета). Поэтому при использовании на ОС Windows, необходимо сперва вызвать WSAStartup().
         * Если не сделать этого, будет выброшено исключение с соответствующим текстом.
         */
        Socket();

        Socket(Socket &&sock);
        Socket &operator=(Socket &&);

#ifdef WIN32
        /**
         * @brief Создаёт новый экземпляр сокета
         * @param socket Идентификатор сокета
         * 
         * @details Инициализация нового сокета не происходит. Будет использоваться сокет, переданный аргуметом. 
         */
        Socket(SOCKET socket);

        /**
         * @brief Инициализация Winsock's DLL
         */
        static void WSAStartup();

        /**
         * @brief Очистка Winsock's DLL
         */
        static void WSACleanup();
#else
        /**
         * @brief Создаёт новый экземпляр сокета
         * 
         * @param socket Идентификатор сокета
         * 
         * @details Инициализация нового сокета не происходит. Будет использоваться сокет, переданный аргуметом. 
         */
        Socket(int socket);
#endif

        /**
         * @brief Включает/отключает Keep-Alive режим
         * 
         * @param status включить/отключить Keep-Alive
         * @return возвращает true, если режим успешно включен
         */
        bool setKeepAlive(bool status = true) noexcept;

        /**
         * @brief Включает/отключает неблокирующий режим
         * 
         * @param status включить/отключить неблокирующий режим
         * @return возвращает true, если режим успешно включен
         */
        bool setNonBlocking(bool status = true) noexcept;

        /**
         * @brief Выполняет присоединение к серверу
         * 
         * @param ip IP адрес сервера
         * @param port  Порт сервера
         * @return Возвращает true, если соединение успешно установлено
         * 
         * @details Метод выполняет соединение с сервером. Причём, если сокет уже до этого использовался для соединения - будет выполнена переинициализация.
         */
        bool connect(const std::string &ip, unsigned short port) noexcept;


        /**
         * @brief Связывает локальный адрес с сокетом.
         * 
         * @param ip  IP локального адреса для связывания
         * @param port port локального адреса для связываения
         * 
         * @details В случае неудачи выбрасывает исключение с текстом ошибки.
         */
        void bindOrThrow(const std::string &ip, unsigned short port);
        
        /**
         * @brief Связывает локальный адрес с сокетом.
         * 
         * @param ip  IP локального адреса для связывания
         * @param port port локального адреса для связываения
         * @return true в случае удачного связываения
         */
        bool bind(const std::string &ip, unsigned short port) noexcept;
        
        /**
         * @brief Связывает локальный адрес с сокетом.
         * @param port port локального адреса для связываения
         * 
         * @details Для связывания использует любой локальный адрес (INADDR_ANY). В случае неудачи выбрасывает исключение с текстом ошибки.
         */
        void bindOrThrow(unsigned short port);

        /**
         * @brief Связывает локальный адрес с сокетом.
         * 
         * @param port port локального адреса для связываения
         * @return true в случае удачного связываения
         * 
         * @details Для связывания использует любой локальный адрес (INADDR_ANY).
         */
        bool bind(unsigned short port) noexcept;

        /**
         * @brief Переводит сокет в состояние прослушивания 
         * 
         * @param count Максимальная очередь для подключения
         * @return true, если удалось изменить состояние сокета
         */
        bool listen(int count = 5) noexcept;
        
        /**
         * @brief Переводит сокет в состояние прослушивания 
         * @param count Максимальная очередь для подключения
         * 
         * @details В случае неудачи выбрасывает исключение с текстом ошибки.
         */
        void listenOrThrow(int count = 5);


        /**
         * @brief Выполняет попытку присоединения входящего подключения
         * 
         * @return Экземпляр подключенного сокета, или std::nullopt, если не удалось ответить на подключение.
         * 
         * @details В случае удачного присоединения, возвращает экземпляр сокета в обёртке std::optional. Иначе - std::nullopt
         */
        std::optional<Socket> accept() noexcept;

        /**
         * @brief Выполняет попытку присоединения входящего подключения
         * 
         * @return Экземпляр подключенного сокета 
         * 
         * * @details В случае удачного присоединения, возвращает экземпляр сокета. Иначе - выбрасывает исключение с соответствующиим текстом ошибки
         */
        Socket acceptOrThrow();
        

        /**
         * @brief Пересылает данные по подключенному сокету
         * 
         * @param message текст, который нужно отправить
         * @return Возвращает true, если данные успешно переданы
         * 
         * @details Функция пытается отправить текстовые данные по подключенному сокету. Если операция прошла успешно - возвращает true. В противном случае - false.
         */
        bool send(const std::string &message) noexcept;

        /**
         * @brief Считывает данные по подключенному сокету
         * 
         * @return В случае успешного считывания - текст полученных данных. Иначе - std::nullopt
         */
        std::optional<std::string> recv() noexcept;
        
        /**
         * @brief Выполняет закрытие сокета, если он валиден ( != -1 )
         */
        void cleanup() noexcept;

        /**
         * @brief Возвращает статус сокета
         * @return true, если сокет валиден (!= -1)
         */
        bool isValid() const noexcept;

#ifdef WIN32
        /**
         * @brief Возвращает нативный идентификатор сокета
         * 
         * @return SOCKET - нативный идентификатор
         */
        SOCKET getRaw() const noexcept;
#else
        /**
         * @brief Возвращает нативный идентификатор сокета
         * 
         * @return int - нативный идентификатор
         */
        int getRaw() const noexcept;
#endif
        
        /**
         * @brief Оператор сравнения
         * 
         * @return Возвращает true, если совпадают идентификаторы сокетов.
         */
        friend bool operator==(const Socket &l, const Socket &r);


    private:
        /**
         * @brief Выполняет инициализацию (создание TCP/IPv4 сокета)
         */
        void _initialize();

        Socket(const Socket &) = delete;
        Socket &operator=(const Socket &) = delete;


    private:
#ifdef WIN32
        SOCKET m_sock;
        static bool m_WSAInitialized;
#else 
        int m_sock;
#endif
};

#endif //SOCKET_HPP