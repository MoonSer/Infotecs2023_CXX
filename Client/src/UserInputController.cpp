#include "Converter.hpp"
#include "UserInputController.hpp"

UserInputController::UserInputController(std::reference_wrapper<Buffer> buffer, std::istream &readDevice, std::ostream &writeDevice, const std::vector<ValidatorType> &validators) noexcept  
            : m_buffer(buffer), m_readDevice(readDevice), m_writeDevice(writeDevice), m_validators(validators) {}

void UserInputController::start() {
    while (true) {
        this->m_writeDevice << ">";

        std::string userInput;
        std::getline(this->m_readDevice, userInput);
        
        // Нужно для правильной работы Ctrl+D (EOF)
        if (this->m_readDevice.eof())
            break;

        this->processString(userInput);
    }
}

void UserInputController::processString(const std::string &userInput) {
    // Сперва проверим валидность строки (пока только размер)
    if (!this->validateString(userInput)) {
        this->m_writeDevice << "String is not valid.\n";
        return;
    }

    // Конвертируем нашу строку в вектор чисел
    auto convertedToInts = Converter::safeStrToNumbers(userInput);
    if (!convertedToInts.has_value()) {
        this->m_writeDevice << "String is not valid.\n";
        return;
    }
    
    std::sort(convertedToInts.value().rbegin(), convertedToInts.value().rend());

    // Конвертируем обратно в строку, !встроена замена чётных цифр!
    std::string convertedToStr = Converter::intsToStr(convertedToInts.value());
    
    // Чистим массив от чётных цифр
    convertedToInts.value().erase(std::remove_if(convertedToInts.value().begin(), convertedToInts.value().end(), [] (short number) { return number%2 == 0; }), convertedToInts.value().end());


    this->m_buffer.get().pushData(std::move(convertedToStr), std::move(convertedToInts.value()));
}

bool UserInputController::validateString(const std::string &userInput) const {
    for (auto &validator : this->m_validators)
        if (!validator(userInput))
            return false;
    return true;
}

void UserInputController::addValidator(ValidatorType validator) noexcept {
    this->m_validators.emplace_back(validator);
}
