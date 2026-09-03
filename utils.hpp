#pragma once

#include <ctime>
#include <sstream>
#include <string>
#include <vector>

inline std::string getTimestamp() {
    // Читает текущее время и возваращет его в виже строчки
    const std::time_t now = std::time(nullptr);
    std::tm localTime {};
    localtime_s(&localTime, &now);

    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", &localTime);
    return buffer;
}

inline std::string getLowerString(const std::string& string) {
    // Принимает строку и возвращает её же в нижнем регистре
    std::string outString {};
    outString.reserve(string.size());

    for (const unsigned char symbol : string) {
        outString += static_cast<char>(std::tolower(symbol));
    }

    return outString;
}

inline std::vector<std::string> splitCsvLine(const std::string& line) {
    // Разделяет строчку CSV-формата (1212,1212) на массив строчек
    std::vector<std::string> tokens;
    std::stringstream stream(line);
    std::string token;

    while (std::getline(stream, token, ',')) {
        tokens.push_back(token);
    }
 
    return tokens;
}

inline bool isValidName(const std::string& value) {
    // Проверяет корректность имени, исключает дежурные символы и запятые
    if (value.empty()) {
        return false;
    }

    for (const unsigned char symbol : value) {
        if (symbol == ',' || std::iscntrl(symbol)) {
            return false;
        }
    }

    return true;
}