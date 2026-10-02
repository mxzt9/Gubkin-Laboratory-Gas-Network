#pragma once

#include <ctime>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

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

    void printMessage(const std::string& message, const char* prefix = "", const char* color = "", std::ostream& output = std::cout);

    void printError(const std::string& message);
    void printWarning(const std::string& message);
    void printSuccess(const std::string& message);
    void printInfo(const std::string& message);
}

struct Edge {
    int id {}, distance {};
};

std::string getTimestamp();

std::string getLowerString(const std::string& string);

bool isValidName(const std::string& value);
