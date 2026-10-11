#pragma once 

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <sstream>
#include <charconv>
#include <system_error>
#include <climits>
#include <limits>

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

    std::string String(const std::string& prompt, const std::string& default_value = "");

    int Int(const std::string& prompt, int default_value, int min = (std::numeric_limits<int>::min)(), int max = (std::numeric_limits<int>::max)(), bool show_info = true);

    template<typename T>
    std::vector<T> Vector(const std::string& prompt);
    
    bool Bool(const std::string& prompt, bool default_value);
    
}


std::string getTimestamp();
