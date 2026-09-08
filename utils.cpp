#include <ctime>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <stdexcept>

#include "utils.hpp"

 
std::string getTimestamp() {
    // Читает текущее время и возваращет его в виже строчки
    const std::time_t now = std::time(nullptr);
    std::tm localTime {};
    localtime_s(&localTime, &now);

    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", &localTime);
    return buffer;
};

std::string getLowerString(const std::string& string) {
    // Принимает строку и возвращает её же в нижнем регистре
    std::string outString {};
    outString.reserve(string.size());

    for (const unsigned char symbol : string) {
        outString += static_cast<char>(std::tolower(symbol));
    }

    return outString;
}

bool isValidName(const std::string& value) {
    // Название занимает одну строку текстового файла.
    if (value.empty()) {
        return false;
    }

    for (const unsigned char symbol : value) {
        if (std::iscntrl(symbol)) {
            return false;
        }
    }

    return true;
}
