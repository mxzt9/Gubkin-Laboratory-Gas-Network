#pragma once

#include <string>

enum class StationType {
    Light,
    Medium,
    Heavy,
};

// Класс КС
class CompressorStation {
private:
    int numWorkers, numActiveWorkers;
    std::string name;
    StationType stationType;

public:
    CompressorStation(int NW, int NAW, const std::string& name, StationType sType) : numWorkers(NW), numActiveWorkers(NAW), name(name), stationType(sType) {}

    // Геттеры КС
    int getNumWorkers() const { return numWorkers; }
    int getNumActiveWorkers() const { return numActiveWorkers; }
    const std::string& getName() const { return name; }
    StationType getStationType() const { return stationType; }

    // Сеттеры КС
    bool setNumActiveWorkers(int newNAW) {
        if (newNAW < 0 || newNAW > numWorkers) return false;
        numActiveWorkers = newNAW;
        return true;
    }
};


// Класс трубы
class Pipe {
private:
    int d {}, length {};
    std::string name {};
    bool isRepair {};

    int CStationFromId { -1 };
    int CStationToId { -1 };

public:
    Pipe(int d, int len, const std::string& name, bool isRepair) : d(d), length(len), name(name), isRepair(isRepair) {}

    // Геттеры трубы
    int getDiameter() const { return d; }
    int getLength() const { return length; }
    const std::string& getName() const { return name; }
    bool getRepair() const { return isRepair; }

    int getCStationFromId() const { return CStationFromId; }
    int getCStationToId() const { return CStationToId; }

    // Сеттеры трубы
    void setRepair(bool newRepair) { isRepair = newRepair; }

    void connect(int fromId, int toId) { CStationFromId = fromId; CStationToId = toId; }
    void disconnect() { CStationFromId = -1; CStationToId = -1; }
};
