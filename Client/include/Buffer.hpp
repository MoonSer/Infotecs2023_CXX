#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <mutex>
#include <condition_variable>
#include <queue>

// Представляет собой "фрейм" буфера
class UserInputData {
    public:
        UserInputData(std::string && str, std::vector<short> &&numbers) noexcept : stringData(std::move(str)), numberedData(std::move(numbers)) {}
        UserInputData(UserInputData && user) {
            this->stringData.swap(user.stringData);
            this->numberedData.swap(user.numberedData);
        }

        inline const std::string &string() const
            { return this->stringData; }
        
        inline const std::vector<short> &numbers() const
            { return this->numberedData; }

    private:
        std::string stringData;
        std::vector<short> numberedData;


    private:
        UserInputData(const UserInputData &) = delete;
};



class Buffer {
    public:
        Buffer() = default;
        void waitData();

        void pushData(std::string &&strData, std::vector<short> &&numbersData, bool wakeUpHandler = true) noexcept;
        void pushData(UserInputData &&data, bool wakeUpHandler = true) noexcept;
        
        UserInputData pullData();

        inline std::size_t size() const noexcept;
        inline void wakeUpOne();

    private:
        std::mutex m_mutex;
        std::condition_variable m_conditionVariable;
        std::queue<UserInputData> m_data;

    
    private:
        Buffer(const Buffer &) = delete;
        Buffer &operator=(const Buffer&) = delete;
        Buffer(Buffer &&) = delete;
        Buffer &operator=(Buffer&&) = delete;
};

#endif //BUFFER_HPP