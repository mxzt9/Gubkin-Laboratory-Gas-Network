#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>
#include <cctype>
#include <stdexcept>
#include <format>
#include <set>
#include <cstdlib>

#include "utils.hpp"

namespace Output {
    void clearConsole() {
        std::system("cls");
    }

    void Message(const std::string& message, const char* prefix, const char* color, std::ostream& output) {
        output << color << prefix << message;

        if (*color != '\0') {
            output << Format::RESET;
        }
    }

    void Error(const std::string& message) {
        Message(message, "[*] Ошибка: ", Format::BRIGHT_RED, std::cerr);
    }

    void Warning(const std::string& message) {
        Message(message, "[!] Внимание: ", Format::BRIGHT_YELLOW);
    }

    void Success(const std::string& message) {
        Message(message, "[+] Успех: ", Format::BRIGHT_GREEN);
    }

    void Info(const std::string& message) {
        Message(message, "[@] Информация: ", Format::BRIGHT_CYAN);
    }
}

namespace Input {
    std::string String(const std::string& prompt, std::string defaultParam) {
        std::string value {};
        Output::Message(prompt);

        if (!defaultParam.empty()) {
            Output::Message(std::format(" [Значение по умолчанию: {}]", defaultParam), "", Format::ITALIC);
        }

        Output::Message(": ");

        // Проверка потока ввода
        if (!std::getline(std::cin, value)) {
            throw Command::Exit;
        }

        if (value == "exit" || value == "Exit" || value == "EXIT") {
            throw Command::Exit;
        }

        if (value.empty() && !defaultParam.empty()) {
            return defaultParam;
        }

        return value;
    }

    int Int(const std::string& prompt, int defaultParam, int min, int max) {
        // Показываем только заданные ограничения, без крайних значений int
        const bool hasMin = min != (std::numeric_limits<int>::min)();
        const bool hasMax = max != (std::numeric_limits<int>::max)();
        std::string range;

        if (hasMin && hasMax) {
            range = "от " + std::to_string(min) + " до " + std::to_string(max);
        } else if (hasMin) {
            range = "от " + std::to_string(min);
        } else if (hasMax) {
            range = "до " + std::to_string(max);
        }

        while (true) {
            Output::Message(prompt);

            if (!range.empty()) {
                Output::Message(std::format(" ({})", range));
            }

            // Если есть значение по умолчанию - выводим
            if (defaultParam != -1) {
                Output::Message(std::format(" [Значение по умолчанию: {}]", defaultParam), "", Format::ITALIC);
            }

            Output::Message(": ");

            int value;

            // Если есть значение по умолчанию и был нажат Enter - устанавливаем значение по умолчанию
            if (defaultParam != -1 && std::cin.peek() == '\n') {
                value = defaultParam;
            } else {
                std::cin >> value;
            }

            // Если поток упал проверяем на то мог ли это быть exit, если не exit - сообщение об ошибке
            if (std::cin.fail()) {
                if (std::cin.eof()) {
                    throw Command::Exit;
                }

                std::cin.clear();

                std::string input;
                std::getline(std::cin, input);

                if (input == "exit" || input == "Exit" || input == "EXIT") {
                    throw Command::Exit;
                }

                Output::Error("Введите целое число.\n");
                continue;
            }

            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

            // Проверка числа на допустимый диапазон
            if (value < min || value > max) {
                Output::Error("Введите целое число " + range + ".\n");
                continue;
            }

            return value;
        }
    }

    std::vector<int> MultipleInt(const std::string& prompt) {
        while (true) {
            const std::string value = String(prompt);

            std::stringstream stringStream(value);

            std::set<int> ids;
            int id;

            // Чтение ID через пробел
            while (stringStream >> id) {
                ids.insert(id);
            }

            if (!ids.empty()) {
                return std::vector<int>(ids.begin(), ids.end());
            }

            Output::Error("Введите числа через пробел.\n");
        }
    }

    bool Bool(const std::string& prompt, bool defaultParam) {
        const std::string defaultStr = defaultParam ? "y" : "n";
        const std::string formattedPrompt = prompt + " (y/n)";

        while (true) {
            const std::string value = String(formattedPrompt, defaultStr);

            if (value == "y" || value == "Y") { return true; }
            if (value == "n" || value == "N") { return false; }

            Output::Error("Введите y или n.\n");
        }
    }

    std::vector<char> Comparison(const std::string& prompt) {
        while (true) {
            const std::string input = String(prompt);
            std::stringstream stringStream(input);

            char comparison;
            int value;
            std::string suffix;

            // Получаем знак сравнения и значение
            if (stringStream >> comparison >> value) {
                // Пропускаем пробелы и читаем остаток строки: пустую строку или %
                std::getline(stringStream >> std::ws, suffix);

                // Проверка на корректные значения
                if ((comparison == '>' || comparison == '<' || comparison == '=') && value >= 0 && (suffix.empty() || (suffix == "%" && value <= 100))) {
                    const std::string condition = comparison + std::to_string(value) + suffix;
                    return std::vector<char>(condition.begin(), condition.end());
                }
            }

            Output::Error("Введите условие вида >50%, <5 или =10.\n");
        }
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