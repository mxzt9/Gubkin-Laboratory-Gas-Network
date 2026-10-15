#include <iostream>
#include <filesystem>

#include "windows.h"

#include "network.hpp"
#include "console.hpp"
#include "utils.hpp"
#include "logger.hpp"


int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    std::filesystem::create_directories("data");
    std::filesystem::create_directories("logs");

    std::ofstream logfile("logs/log_" + getTimestamp() + ".txt");
    Logger logger(std::cerr);

    if (logfile) {
        logger.redirect(logfile);
    }

    Network network;
    Console console(network);

    console.run();

    return 0;
}
