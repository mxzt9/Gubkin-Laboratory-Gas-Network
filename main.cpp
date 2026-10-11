#include <iostream>
#include <filesystem>

#include "windows.h"

#include "network.hpp"
#include "console.hpp"


int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    std::filesystem::create_directories("data");

    Network network;
    Console console(network);

    console.run();

    return 0;
}
