#ifndef USERINPUTCONTROLLER_HPP
#define USERINPUTCONTROLLER_HPP

#include <istream>
#include <functional>
#include <optional>
#include "Buffer.hpp"

class UserInputController {
    using ValidatorType = std::function<bool(const std::string&)>;
    
    public:
        UserInputController(std::reference_wrapper<Buffer> buffer, std::istream &readDevice, std::ostream &writeDevice, const std::vector<ValidatorType> &validators = {}) noexcept;

        void addValidator(ValidatorType validator) noexcept;

        void start();

        bool validateString(const std::string &userInput) const;
        
    
    private:
        void processString(const std::string &userInput);
        std::optional<std::vector<short>> convertToNumbers(const std::string &userInput) const noexcept;
        

    private:
        std::reference_wrapper<Buffer> m_buffer;
        std::vector<ValidatorType> m_validators;
        
        std::istream &m_readDevice;
        std::ostream &m_writeDevice;

    
    private:
        UserInputController() = delete;
        UserInputController(UserInputController &) = delete;
        UserInputController &operator=(const UserInputController&) = delete;
        UserInputController(UserInputController &&) = delete;
        UserInputController &operator=(UserInputController&&) = delete;
};

#endif // USERINPUTCONTROLLER_HPP