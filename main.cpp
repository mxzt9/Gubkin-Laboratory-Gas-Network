#include <filesystem>
#include <windows.h>

#include "console.hpp"
#include "network.hpp"
#include "log.hpp"


int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    
    std::filesystem::create_directories("data");
    std::filesystem::create_directories("data/log");

    Network network;
    Logger logger;

    Console console(network, logger);
    console.run();

    return 0;
}
