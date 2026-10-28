#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include <filesystem>

#include "pipe.hpp"
#include "cstation.hpp"


class Network {
private:
    std::unordered_map<int, Pipe> PipeMap {};
    std::unordered_map<int, CStation> CStationMap {};

public:
    // Геттеры
    const Pipe& get_pipe_by_id(int id) const { return PipeMap.at(id); }
    const CStation& get_station_by_id(int id) const { return CStationMap.at(id); }

    // Чекеры
    bool emptyPipes() const { return PipeMap.empty(); }
    bool emptyCStations() const { return CStationMap.empty(); }

    // Добавление
    bool createPipe(const Pipe& pipe);
    bool createCStation(const CStation& station);

    // Принтер меню
    void print();

    // Поиск по фильтрам
    std::set<int> searchPipesByName(const std::string& name) const;
    std::set<int> searchCStationsByName(const std::string& name) const;

    std::set<int> searchPipesByRepair(bool repair) const;
    std::set<int> searchCStationsByActive(const std::string& comparison, int ratio) const;

    // Редактирование
    bool editPipe(const std::set<int>& ids, bool new_repair);
    bool editCStation(const std::set<int>& ids, int new_active_unit_amount);

    // Удаление
    bool deletePipe(const std::set<int>& ids);
    bool deleteCStation(const std::set<int>& ids);

    // Работа с файлам
    bool save(const std::filesystem::path& path) const;
    bool load(const std::filesystem::path& path);
};
