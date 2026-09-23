#pragma once

#include <ctime>
#include <sstream>
#include <string>
#include <vector>

struct Edge {
    int id {}, distance {};
};

std::string getTimestamp();

std::string getLowerString(const std::string& string);

bool isValidName(const std::string& value);
