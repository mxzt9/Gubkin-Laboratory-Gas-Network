#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>
#include <cctype>
#include <stdexcept>

#include "utils.hpp"

namespace Output {
    void clearConsole() {
        std::system("cls");
    }

    void printMessage(const std::string& message, const char* prefix, const char* color, std::ostream& output) {
        output << color << prefix << message;

        if (*color != '\0') {
            output << Format::RESET;
        }
    }

    void printError(const std::string& message) {
        printMessage(message, "[*] Ошибка: ", Format::BRIGHT_RED, std::cerr);
    }

    void printWarning(const std::string& message) {
        printMessage(message, "[!] Внимание: ", Format::BRIGHT_YELLOW);
    }

    void printSuccess(const std::string& message) {
        printMessage(message, "[+] Успех: ", Format::BRIGHT_GREEN);
    }

    void printInfo(const std::string& message) {
        printMessage(message, "[@] Информация: ", Format::BRIGHT_CYAN);
    }
}

std::string getTimestamp() {
    // Читает текущее время и возвращает его в виде строки
    const std::time_t now = std::time(nullptr);
    std::tm localTime {};
    localtime_s(&localTime, &now);

    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", &localTime);
    return buffer;
}

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
