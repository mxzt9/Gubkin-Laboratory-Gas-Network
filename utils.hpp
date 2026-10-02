#pragma once

#include <ctime>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <limits>

namespace Format {
    inline constexpr const char* RESET   = "\x1b[0m";

    inline constexpr const char* BRIGHT_RED    = "\x1b[91m";
    inline constexpr const char* BRIGHT_GREEN  = "\x1b[92m";
    inline constexpr const char* BRIGHT_YELLOW = "\x1b[93m";
    inline constexpr const char* BRIGHT_BLUE   = "\x1b[94m";
    inline constexpr const char* BRIGHT_MAGENTA= "\x1b[95m";
    inline constexpr const char* BRIGHT_CYAN   = "\x1b[96m";
    inline constexpr const char* BRIGHT_WHITE  = "\x1b[97m";

    inline constexpr const char* BOLD      = "\x1b[1m";
    inline constexpr const char* UNDERLINE = "\x1b[4m";

    inline constexpr const char* ITALIC = "\x1b[3m";
}


namespace Output {
    void clearConsole();
    
    void Message(const std::string& message, const char* prefix = "", const char* color = "", std::ostream& output = std::cout);
    
    void Error(const std::string& message);
    void Warning(const std::string& message);
    void Success(const std::string& message);
    void Info(const std::string& message);
}

namespace Input {
    enum class Command {
        Exit
    };

    std::string String(const std::string& prompt, std::string defaultParam = "");
    int Int(const std::string& prompt, int defaultParam = -1, int min = (std::numeric_limits<int>::min)(), int max = (std::numeric_limits<int>::max)());
    std::vector<int> MultipleInt(const std::string& prompt);
    bool Bool(const std::string& prompt, bool defaultParam);

    std::vector<char> Comparison(const std::string& prompt);
}

struct Edge {
    int id {}, distance {};
};

std::string getTimestamp();

std::string getLowerString(const std::string& string);
