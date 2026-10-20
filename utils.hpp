#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <sstream>

#include "pipe.hpp"
#include "cstation.hpp"

namespace Output {
    void clearConsole();
    void Message(const std::string& message, const std::string& prefix = "");
    void Error(const std::string& message);
    void Warning(const std::string& message);
    void Success(const std::string& message);
    void Info(const std::string& message);
}

namespace Input {
    enum class Command { Exit };

    std::string String(const std::string& prompt, const std::string& default_value = "");

    int Int(const std::string& prompt, int min, int max, int default_value = -1);

    bool Bool(const std::string& prompt, bool default_value = false);
    void Pause();

    template<typename T>
    std::vector<T> Vector(const std::string& prompt) {
        while (true) {
            std::istringstream stream(String(prompt));
            std::set<T> ids;
            T id;

            while (stream >> id) {
                ids.insert(id);
            }

            if (!ids.empty() && stream.eof()) {
                return {ids.begin(), ids.end()};
            }

            Output::Error("Введите числа через пробел.\n");
        }
    }
}

namespace Filter {
    inline constexpr auto Name = [](const auto& object, const std::string& name) {
        return object.get_name().find(name) != std::string::npos;
    };

    inline constexpr auto Repair = [](const Pipe& object, bool repair) {
        return object.get_repair() == repair;
    };

    inline constexpr auto Active = [](const CStation& object, const std::string& comparison, int ratio) {
        const auto value = object.get_unit_ratio();

        if (comparison == "<") return value < ratio;
        if (comparison == "<=") return value <= ratio;
        if (comparison == "=") return value == ratio;
        if (comparison == ">") return value > ratio;
        if (comparison == ">=") return value >= ratio;

        return false;
    };
}

template<typename T, typename F, typename... Args>
bool filter(const T& object, F function, const Args&... args) {
    return function(object, args...);
}

std::string getTimestamp();

