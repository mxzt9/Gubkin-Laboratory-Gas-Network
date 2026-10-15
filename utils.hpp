#pragma once 

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <sstream>

namespace Output {
    void clearConsole();
    
    void Message(const std::string& message, const std::string prefix = "", std::ostream& output = std::cout);
    
    void Error(const std::string& message);
    void Warning(const std::string& message);
    void Success(const std::string& message);
    void Info(const std::string& message);
}


namespace Input {
    enum class Command {
        Exit
    };

    std::string String(const std::string& prompt, const std::string& default_value = "", std::istream& input = std::cin);

    int Int(const std::string& prompt, int min, int max, std::istream& input = std::cin);
    int Int(const std::string& prompt, int min, int max, int default_value, std::istream& input = std::cin);

    void Pause(std::istream& input = std::cin);

    template<typename T>
    std::vector<T> Vector(const std::string& prompt) {
        while (true) {
            const std::string value = String(prompt);

            std::stringstream stringStream(value);

            std::set<T> ids;
            T id {};

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
    
    bool Bool(const std::string& prompt, bool default_value, std::istream& input = std::cin);
    
}


std::string getTimestamp();
