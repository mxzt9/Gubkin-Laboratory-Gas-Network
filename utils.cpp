#include <cstdlib>
#include <format>
#include <chrono>

#include "utils.hpp"

namespace Output {
    void clearConsole() {
        std::system("cls");
    }

    void Message(const std::string& message, const std::string& prefix) {
        std::cout << prefix << message;
    }

    void Error(const std::string& message) {
        Message(message, "[*] Ошибка: ");
    }

    void Warning(const std::string& message) {
        Message(message, "[!] Внимание: ");
    }

    void Success(const std::string& message) {
        Message(message, "[+] Успех: ");
    }

    void Info(const std::string& message) {
        Message(message, "[@] Информация: ");
    }
}

namespace Input {
    std::string String(const std::string& prompt, const std::string& default_value) {
        std::cout << prompt;

        if (!default_value.empty()) {
            std::cout << " [значение по умолчанию: " << default_value << "]";
        }

        std::cout << ": ";

        std::string value;
        if (!std::getline(std::cin, value)) {
            throw Command::Exit;
        }

        if (value == "exit" || value == "Exit" || value == "EXIT") {
            throw Command::Exit;
        }

        return value.empty() ? default_value : value;
    }

    int Int(const std::string& prompt, int min, int max, int default_value) {
        while (true) {
            std::string text = String(std::format("{} (от {} до {})", prompt, min, max), default_value == -1 ? "" : std::to_string(default_value));

            std::istringstream stream(text);
            int value;

            if (!(stream >> value) || !(stream >> std::ws).eof()) {
                Output::Error("Введите корректное число.\n");
                continue;
            }

            if (value < min || value > max) {
                Output::Error("Введите число в допустимом диапазоне.\n");
                continue;
            }

            return value;
        }
    }

    bool Bool(const std::string& prompt, bool default_value) {
        while (true) {
            std::string value = String(prompt + " (y/n)", default_value ? "y" : "n");

            if (value == "y" || value == "Y") return true;
            if (value == "n" || value == "N") return false;

            Output::Error("Введите y или n.\n");
        }
    }

    void Pause() {
        std::cout << "Нажмите Enter для продолжения: ";

        std::string value;
        if (!std::getline(std::cin, value)) {
            throw Command::Exit;
        }
    }
}

std::string getTimestamp() {
    return std::format("{:%d_%m_%Y %H_%M_%OS}", std::chrono::system_clock::now());
}
