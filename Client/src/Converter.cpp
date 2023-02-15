#include "Converter.hpp"

#include <sstream>

std::vector<short> Converter::strToInts(const std::string &str) {
    std::vector<short> returnVector;
    // Пользуемся тем, что до 127-го символов, ASCII-таблица везде одинаковая
    for (const auto &c : str )
        if (c < 58 && c > 47 )
           returnVector.emplace_back(int(c)-48);
        else
            throw std::invalid_argument("Incorrect symbol \"" + std::string{c} + "\" in string: \"" + str + "\"");
    
    return returnVector;
}

std::optional<std::vector<short>> Converter::safeStrToNumbers(const std::string &userInput) noexcept {
    try {
        return Converter::strToInts(userInput);
    }catch (std::exception &e) {
        return std::nullopt;
    }
}



std::string Converter::intsToStr(const std::vector<short> &numbers) noexcept {
    std::stringstream stream;
    for (const auto &n : numbers)
        if (n % 2)
            stream << n;
        else
            stream << "KB";
    return stream.str();
}