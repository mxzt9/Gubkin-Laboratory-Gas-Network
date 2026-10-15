#include <sstream>
#include <cstdlib>
#include <string>
#include <iostream>
#include <format>
#include <chrono>

#include "utils.hpp"


namespace Output {
    void clearConsole() {
        std::system("cls");
    }

    void Message(const std::string& message, const std::string prefix, std::ostream& output) {
        output << prefix << message;

        if (&output != &std::cerr) {
            std::cerr << prefix << message;
        }
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
    std::string String(const std::string& prompt, const std::string& default_value, std::istream& input) {
        std::string full_prompt {};

        if (default_value.empty()) {
            full_prompt = std::format("{}: ", prompt);
        } else {
            full_prompt = std::format("{} [значение по умолчанию: {}]: ", prompt, default_value);
        }

        Output::Message(full_prompt);

        std::string value;
        if (!std::getline(input, value)) {
            throw Command::Exit;
        }

        if (value.empty()) {
            std::cerr << default_value << '\n';
            return default_value;
        }

        std::cerr << value << '\n';

        if (value == "exit" || value == "Exit" || value == "EXIT") {
            throw Command::Exit;
        }

        return value;
    }

    static int readInt(const std::string& prompt, int min, int max, const std::string& default_value, std::istream& input) {
        const std::string full_prompt = std::format("{} (от {} до {})", prompt, min, max);

        while (true) {
            const std::string text = String(full_prompt, default_value, input);
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

    int Int(const std::string& prompt, int min, int max, std::istream& input) {
        return readInt(prompt, min, max, "", input);
    }

    int Int(const std::string& prompt, int min, int max, int default_value, std::istream& input) {
        return readInt(prompt, min, max, std::to_string(default_value), input);
    }

    void Pause(std::istream& input) {
        Output::Message("Нажмите Enter для продолжения: ");

        std::string value;
        if (!std::getline(input, value)) {
            throw Command::Exit;
        }

        std::cerr << value << '\n';
    }

    bool Bool(const std::string& prompt, bool default_value, std::istream& input) {
        std::string full_prompt = prompt + " (y/n)";

        while (true) {
            const std::string value = String(full_prompt, default_value ? "y" : "n", input);

            if (value == "y" || value == "Y") { return true; }
            if (value == "n" || value == "N") { return false; }

            Output::Error("Введите y или n.\n");
        }
    }

}

std::string getTimestamp() {
    std::string time = std::format("{:%d_%m_%Y %H_%M_%OS}", std::chrono::system_clock::now());
    return time;
}
