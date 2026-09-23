#pragma once

#include <string>
#include <fstream>

enum class StationType {
    Light,
    Medium,
    Heavy,
};

// Класс КС
class CompressorStation {
private:
    int id {}, numWorkshops {}, numActiveWorkshops {};
    std::string name {};
    StationType stationType {};

public:
    CompressorStation(int id, int numWorkshops, int numActiveWorkshops, const std::string& name, StationType stationType);

    // Геттеры КС
    int getNumWorkshops() const;
    int getNumActiveWorkshops() const;
    const std::string& getName() const;
    StationType getStationType() const;

    // Сеттеры КС
    bool setNumActiveWorkshops(int newNumActiveWorkshops);

    static void printCStationTableHeader();
    void printCStation() const;
    bool save(std::ostream& file) const;
};
