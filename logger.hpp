#pragma once

#include <iostream>
#include <fstream>

class Logger {
private:
    std::ostream& stream;
    std::streambuf* const old_buffer;

public:
    Logger(std::ostream& source) : stream(source), old_buffer(source.rdbuf()) {};

    ~Logger() { stream.rdbuf(old_buffer); }

    void redirect (std::ofstream& destination) { stream.rdbuf(destination.rdbuf()); }
};