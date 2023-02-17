#include "Buffer.hpp"

void Buffer::waitData() noexcept {
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

const std::reference_wrapper<const UserInputData> Buffer::front() const {
    if (this->m_data.size() == 0)
        throw std::logic_error("Buffer::front() - Error: No one event!");
    
    return std::ref(this->m_data.front());
}

void Buffer::pop() noexcept {
    std::unique_lock lock(this->m_mutex);
    if (this->m_data.size() != 0)
        this->m_data.pop();
}

std::size_t Buffer::size() const noexcept {
    return this->m_data.size();
}

void Buffer::wakeUpOne() noexcept {
    this->m_conditionVariable.notify_all();
}
