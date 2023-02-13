#include "Buffer.hpp"

void Buffer::waitData() {
    // Не боимся юзать один мьютекс для чтения, записи и ожидания, т.к. выйдя из функции Buffer::waitData(), мьютекс будет разблокирован
    std::unique_lock lock(this->m_mutex);
    this->m_conditionVariable.wait(lock);
}

void Buffer::pushData(std::string &&strData, std::vector<short> &&numbersData, bool wakeUpHandler) noexcept {
    std::unique_lock lock(this->m_mutex);
    this->m_data.emplace(std::move(strData), std::move(numbersData));
    if (wakeUpHandler)
        this->wakeUpOne();
}

void Buffer::pushData(UserInputData &&data, bool wakeUpHandler) noexcept {
    std::unique_lock lock(this->m_mutex);
    this->m_data.push(std::move(data));
    if (wakeUpHandler)
        this->wakeUpOne();
}

UserInputData Buffer::pullData() {
    if (this->m_data.size() == 0)
        throw std::logic_error("No one event!");
    std::unique_lock lock(this->m_mutex);
    
    UserInputData data = std::move(this->m_data.front());
    this->m_data.pop();
    return data;
}

std::size_t Buffer::size() const noexcept {
    return this->m_data.size();
}

inline void Buffer::wakeUpOne() {
    this->m_conditionVariable.notify_all();
}
