#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>

#include "pipe.hpp"
#include "compressStation.hpp"
#include "utils.hpp"

class Network {
private:
    std::unordered_map<int, Pipe> pipeMap {};
    std::unordered_map<int, CompressorStation> cStationMap {};

    int nextPipeId {};
    int nextCStationId {};
    
    int generatePipeId() { return nextPipeId++; }
    int generateCStationId() { return nextCStationId++; }

public:

    void printNetwork() const;

    // Геттеры и чекеры
    int getNextPipeId() const;
    int getNextCStationId() const;
    
    const std::unordered_map<int, Pipe>& getPipeMap() const;
    const std::unordered_map<int, CompressorStation>& getCStationMap() const;

    // Добавление
    bool addPipe(int diameter, int length, const std::string& name, bool repair);
    bool addCStation(int numWorkshops, int numActiveWorkshops, const std::string& name, StationType stationType);

    // Удаление
    bool deletePipe(int id);
    bool deleteCStation(int id);

    // Редактирование
    bool editPipe(int id, bool newIsRepair);
    bool editCStation(int id, int newNumActiveWorkshops);

    // Присоединение трубы
    bool connectPipe(int pipeId, int cStationFromId, int cStationToId);
    int findFreePipe(int diameter) const;

    // Работа с файлами
    bool saveToFile(const std::filesystem::path& filePath);
    bool loadFromFile(const std::filesystem::path& filePath);

    // Поиск по фильтрам
    std::vector<int> searchPipesByName(const std::string& name) const;
    std::vector<int> searchPipesByRepair(bool repair) const;

    std::vector<int> searchCStationsByName(const std::string& name) const;
    std::vector<int> searchCStationsByActive(const std::vector<char>& condition) const;

    // Графовые операции
    bool topologicalSort(std::vector<int>& result) const;
    bool findShortestPath(int startId, int finishId, std::vector<Edge>& result) const;
};
