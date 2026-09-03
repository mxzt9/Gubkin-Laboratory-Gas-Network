#pragma once

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
    // Вспомогательные
    int getNextPipeId();
    int getNextCStationId();
    
    // Чтение счётчика без инкрементации
    int getCurrentPipeId() const;
    int getCurrentCStationId() const;

    bool isPipeInArrayById(int id) const;
    bool isCStationInArrayById(int id) const;

    Pipe& getPipeById(int id);
    CompressorStation& getCStationById(int id);
    
    const std::vector<Pipe>& getPipeArray() const;
    const std::vector<CompressorStation>& getCStationArray() const;

    size_t getPipeArrayLen() const;
    size_t getCStationArrayLen() const;

    
    // Добавление
    void addPipe(int diameter, int length, const std::string& name, bool isRepair = false);
    void addCStation(int NW, int NAW, const std::string& name, StationType sType);

    // Удаление
    bool deletePipe(int id);
    bool deleteCStation(int id);

    // Редактирование
    bool editPipe(int id, int newDiameter, int newLength, const std::string& newName, bool newIsRepair);
    bool editCStation(int id, int newNumWorkers, int newNumActiveWorkers, const std::string& newName, StationType newType);

    // Работа с файлами
    bool saveToFile(const std::string& dirPath);
    bool loadFromFile(const std::string& pipePath, const std::string& cStationPath);

};
