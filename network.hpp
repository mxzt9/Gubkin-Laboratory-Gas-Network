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
    std::unordered_map<int, CompressorStation> CStationMap {};

    int nextPipeId { 0 };
    int nextCStationId { 0 };

public:

    // Геттеры и чекеры
    int getNextPipeId() const;
    int getNextCStationId() const;
    
    const std::unordered_map<int, Pipe>& getPipeMap() const;
    const std::unordered_map<int, CompressorStation>& getCStationMap() const;

    // Добавление
    std::size_t addPipe(int diameter, int length, const std::string& name, bool repair, int amount = 1);
    std::size_t addCStation(int numWorkshops, int numActiveWorkshops, const std::string& name, StationType stationType, int amount = 1);

    void printNetwork() const;

    // Поиск по фильтрам
    std::vector<int> searchPipesByName(const std::string& name) const;
    std::vector<int> searchPipesByRepair(bool repair) const;

    std::vector<int> searchCStationsByName(const std::string& name) const;
    std::vector<int> searchCStationsByActive(const std::vector<char>& condition) const;

    // Редактирование
    std::size_t editPipe(std::vector<int> ids, bool newIsRepair);
    std::size_t editCStation(std::vector<int> ids, int newNumActiveWorkshops);

    // Удаление
    std::size_t deletePipe(std::vector<int> ids);
    std::size_t deleteCStation(std::vector<int> ids);

    // Работа с файлами
    bool saveToFile(const std::filesystem::path& filePath);
    bool loadFromFile(const std::filesystem::path& filePath);

    // Присоединение трубы
    bool connectPipe(int pipeId, int CStationFromId, int CStationToId);
    int findFreePipe(int diameter) const;

    // Графовые операции
    bool topologicalSort(std::vector<int>& result) const;
    bool findShortestPath(int startId, int finishId, std::vector<Edge>& result) const;
};
