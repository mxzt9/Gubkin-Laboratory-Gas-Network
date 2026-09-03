#include <windows.h>

#include "console.hpp"
#include "network.hpp"


int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    Network network;

    Console console(network);
    console.run();

    return 0;
}
