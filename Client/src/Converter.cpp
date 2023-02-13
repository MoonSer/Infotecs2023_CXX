#include "Converter.hpp"

#include <sstream>

std::vector<short> Converter::strToInts(const std::string &str) {
    std::vector<short> returnVector;
    // Пользуемся тем, что до 127-го символо, ASCII-таблица везде одинаковая
    for (const auto &c : str )
        if (c < 58 && c > 47 )
           returnVector.emplace_back(int(c)-48);
        else
            throw std::logic_error("Incorrect symbol in string:" + std::string{c});
    
    return returnVector;
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