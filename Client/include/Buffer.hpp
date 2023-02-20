#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <mutex>
#include <condition_variable>
#include <queue>

// Представляет собой "фрейм" буфера
class UserInputData {
    public:
        UserInputData(std::string && str, std::vector<short> &&numbers) noexcept 
            : m_stringData(std::move(str)), m_numberedData(std::move(numbers)) {}
        
        UserInputData(UserInputData && userData) {
            this->m_stringData.swap(userData.m_stringData);
            this->m_numberedData.swap(userData.m_numberedData);
        }

        
        inline const std::string &string() const noexcept
            { return this->m_stringData; }
        
        inline const std::vector<short> &numbers() const noexcept
            { return this->m_numberedData; }
        
        inline const int getSum() const noexcept{
            int sum = 0;
            for (const auto &value : this->m_numberedData)
                sum += value;
            return sum;
        }

    private:
        std::string m_stringData;
        std::vector<short> m_numberedData;


    private:
        UserInputData(const UserInputData &) = delete;
};




class Buffer {
    public:
        Buffer() = default;
        void waitData() noexcept;

        void pushData(std::string &&strData, std::vector<short> &&numbersData, bool wakeUpHandler = true) noexcept;
        void pushData(UserInputData &&data, bool wakeUpHandler = true) noexcept;
        
        const std::reference_wrapper<const UserInputData> front() const;

        void pop() noexcept;

        std::size_t size() const noexcept;
        void wakeUpOne() noexcept;

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