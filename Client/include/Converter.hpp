#ifndef CONVERTER_HPP
#define CONVERTER_HPP

#include <string>
#include <vector>


namespace Converter {
    std::vector<short> strToInts(const std::string &str);
    std::string intsToStr(const std::vector<short> &numbers) noexcept;
}


#endif //CONVERTER_HPP