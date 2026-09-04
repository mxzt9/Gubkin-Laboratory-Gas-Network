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
    int id;
    int numWorkers, numActiveWorkers;
    std::string name;
    StationType stationType;

public:
    CompressorStation(int id, int NW, int NAW, const std::string& name, StationType sType) : id(id), numWorkers(NW), numActiveWorkers(NAW), name(name), stationType(sType) {}

    // Геттеры КС
    int getId() const { return id; }
    int getNumWorkers() const { return numWorkers; }
    int getNumActiveWorkers() const { return numActiveWorkers; }
    const std::string& getName() const { return name; }
    StationType getStationType() const { return stationType; }

    // Сеттеры КС
    void setName(const std::string& newName) { name = newName; }
    void setStationType(StationType newType) { stationType = newType; }
    void setNumWorkers(int newNW) { numWorkers = newNW; }
    void setNumActiveWorkers(int newNAW) { numActiveWorkers = newNAW; }
};


// Класс трубы
class Pipe {
private:
    int id {};
    int d, length {};
    std::string name {};
    bool isRepair {};

    int cStationFromId { -1 };
    int cStationToId { -1 };

public:
    Pipe(int id, int d, int len, const std::string& name, bool isRepair) : id(id), d(d), length(len), name(name), isRepair(isRepair) {}

    // Геттеры трубы
    int getId() const { return id; }
    int getDiameter() const { return d; }
    int getLength() const { return length; }

    const std::string& getName() const { return name; }
    
    bool getRepair() const { return isRepair; }

    int getCStationFromId() const { return cStationFromId; }
    int getCStationToId() const { return cStationToId; }

    bool hasConnectedCStation() const { return cStationFromId != -1 || cStationToId != -1; }

    bool isConnectedToCStationById(int cStationId) const {
        return cStationFromId == cStationId || cStationToId == cStationId;
    }
    
    // Сеттеры трубы
    void setDiameter(int newD) { d = newD; }
    void setLength(int newL) { length = newL; }
    void setName(const std::string& newName) { name = newName; }
    void setRepair(bool newRepair) { isRepair = newRepair; }
    
    void setCStationFromId(int cStationId) { cStationFromId = cStationId; }
    void setCStationToId(int cStationId) { cStationToId = cStationId; }
    void disconnect() {
        cStationFromId = -1;
        cStationToId = -1;
    }
};
