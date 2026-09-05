#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "units.hpp"

class Network {
private:
    int currentPipeId { 0 };
    int currentCStationId { 0 };

    std::vector<Pipe> pipeArray {};
    std::vector<CompressorStation> CStationArray {};

public:
    // Геттеры и чекеры
    int getNextPipeId();
    int getNextCStationId();
    
    
    int getCurrentPipeId() const;
    int getCurrentCStationId() const;

    bool isPipeInArrayById(int id) const;
    bool isCStationInArrayById(int id) const;

    Pipe& getPipeById(int id);
    CompressorStation& getCStationById(int id);
    
    const std::vector<Pipe>& getPipeArray() const;
    const std::vector<CompressorStation>& getCStationArray() const;

    std::size_t getPipeArrayLen() const;
    std::size_t getCStationArrayLen() const;

    
    // Добавление
    bool addPipe(int diameter, int length, const std::string& name, bool isRepair, int id = -1);
    bool addCStation(int NW, int NAW, const std::string& name, StationType sType);

    // Удаление
    bool deletePipe(int id);
    bool deleteCStation(int id);

    // Редактирование
    bool editPipe(int id, int newDiameter, int newLength, const std::string& newName, bool newIsRepair);
    bool editCStation(int id, int newNumWorkers, int newNumActiveWorkers, const std::string& newName, StationType newType);

    // Присоединение трубы

    bool connectPipe(int pipeId, int CStationFromId, int CStationToId);

    // Работа с файлами
    bool saveToFile(const std::filesystem::path& dirPath, const std::filesystem::path& pipeFileName,
                    const std::filesystem::path& cStationFileName);
    bool loadFromFile(const std::filesystem::path& pipePath, const std::filesystem::path& cStationPath);

    // Поиск по фильтрам
    std::vector<const Pipe*> searchPipesByName(const std::string& name) const;
    std::vector<const Pipe*> searchPipesByRepair(bool isRepair) const;

    std::vector<const CompressorStation*> searchCStationsByName(const std::string& name) const;
    std::vector<const CompressorStation*> searchCStationsByActive(const std::vector<char>& condition) const;

};
