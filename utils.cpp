#include <string>
#include <vector>
#include <iostream>
#include <format>
#include <set>
#include <sstream>
#include <chrono>

#include "utils.hpp"


namespace Output {
    void clearConsole() {
        std::system("cls");
    }

    void Message(const std::string& message, const std::string prefix, std::ostream& output) {
        output << prefix << message;
    }

    void Error(const std::string& message) {
        Message(message, "[*] Ошибка: ", std::cerr);
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
        std::string full_prompt {};

        if (default_value.empty()) {
            full_prompt = std::format("{}: ", prompt);
        } else {
            full_prompt = std::format("{} [значение по умолчанию: {}]: ", prompt, default_value);
        }

        while (true) {
            Output::Message(full_prompt);

            std::string value;
            std::getline(std::cin, value);

            if (value.empty()) {
                return default_value;
            }

            if (value == "exit" || value == "Exit" || value == "EXIT") {
                throw Command::Exit;
            }

            return value;
        }
    }

    int Int(const std::string& prompt, int default_value, int min, int max, bool show_default) {
        std::string full_prompt {};

        if (show_default) {
            full_prompt = std::format("{} [значение по умолчанию: {}] (от {} до {}): ", prompt, default_value, min, max);
        } else {
            full_prompt = std::format("{}: ", prompt);
        }

        while (true) {
            Output::Message(full_prompt);

            int value;

            if (std::cin.peek() == '\n') {
                value = default_value;
            } else {
                std::cin >> value;
            }

            if (std::cin.fail()) {
                std::cin.clear();

                std::string input;
                std::getline(std::cin, input);

                if (input == "exit" || input == "Exit" || input == "EXIT") {
                    throw Command::Exit;
                }

                Output::Error("Введите корректное число.\n");
                continue;
            }

            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

            if (value < min || value > max) {
                Output::Error("Введите число в допустимом диапазоне.\n");
                continue;
            }

            return value;
        }
    }

    template<typename T>
    std::vector<T> Vector(const std::string& prompt) {
        while (true) {
            const std::string value = String(prompt);

            std::stringstream stringStream(value);

            std::set<T> ids;
            T id;

            // Чтение ID через пробел
            while (stringStream >> id) {
                ids.insert(id);
            }

            if (!ids.empty() && stringStream.eof()) {
                return std::vector<T>(ids.begin(), ids.end());
            }

            Output::Error("Введите числа через пробел.\n");
        }
    }

    bool Bool(const std::string& prompt, bool default_value) {
        std::string full_prompt = prompt + " (y/n)";

        while (true) {
            const std::string value = String(full_prompt, default_value ? "y" : "n");

            if (value == "y" || value == "Y") { return true; }
            if (value == "n" || value == "N") { return false; }

            Output::Error("Введите y или n.\n");
        }
    }

    template std::vector<std::size_t> Vector<std::size_t>(const std::string& prompt);
}

std::string getTimestamp() {
    std::string time = std::format("{:%d_%m_%Y %H_%M_%OS}", std::chrono::system_clock::now());
}
