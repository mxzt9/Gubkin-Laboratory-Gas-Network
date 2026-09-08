#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <map>

#include "units.hpp"

class Network {
private:

std::map<int, Pipe> pipeMap {};
std::map<int, CompressorStation> CStationMap {};

public:
    int currentPipeId {};
    int currentCStationId {};
    
    int getNextPipeId() { return currentPipeId++; }
    int getNextCStationId() { return currentCStationId++; }

    // Геттеры и чекеры
    const std::map<int, Pipe>& getPipeMap() const;
    const std::map<int, CompressorStation>& getCStationMap() const;

    // Добавление
    bool addPipe(int diameter, int length, const std::string& name, bool isRepair, int id = -1);
    bool addCStation(int NW, int NAW, const std::string& name, StationType sType);

    // Удаление
    bool deletePipe(int id);
    bool deleteCStation(int id);

    // Редактирование
    bool editPipe(int id, bool newIsRepair);
    bool editCStation(int id, int newNumActiveWorkers);

    // Присоединение трубы
    bool connectPipe(int pipeId, int CStationFromId, int CStationToId);

    // Работа с файлами
    bool saveToFile(const std::filesystem::path& filePath);
    bool loadFromFile(const std::filesystem::path& filePath);

    // Поиск по фильтрам
    std::vector<int> searchPipesByName(const std::string& name) const;
    std::vector<int> searchPipesByRepair(bool isRepair) const;

    std::vector<int> searchCStationsByName(const std::string& name) const;
    std::vector<int> searchCStationsByActive(const std::vector<char>& condition) const;

};
