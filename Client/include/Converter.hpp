#ifndef CONVERTER_HPP
#define CONVERTER_HPP

#include <string>
#include <vector>
#include <optional>

namespace Converter {
    std::vector<short> strToInts(const std::string &str);
    std::optional<std::vector<short>> safeStrToNumbers(const std::string &userInput) noexcept;

    std::string intsToStr(const std::vector<short> &numbers) noexcept;
}


#endif //CONVERTER_HPP