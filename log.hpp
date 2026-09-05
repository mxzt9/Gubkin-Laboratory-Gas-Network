#pragma once

#include <string>
#include <fstream>
#include <queue>

class Logger {
private:
    const std::string logOutputPath;
    std::queue<std::string> logQueue;
    std::ofstream fileStream;

public:
    Logger();
    ~Logger();

    bool open();          
    void close();         

    void addLine(const std::string& timestamp, std::string logText);
    bool saveToFile();    
};