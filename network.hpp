#pragma once

#include <unordered_map>
#include <vector>
#include <cstddef>
#include <filesystem>

#include "pipe.hpp"
#include "cstation.hpp"


class Network {
private:
    std::unordered_map<std::size_t, Pipe> PipeMap {};
    std::unordered_map<std::size_t, CStation> CStationMap {};

public:
    // Чекеры
    bool emptyPipes() { return PipeMap.empty(); }
    bool emptyCStations() { return CStationMap.empty(); }

    // Добавление
    bool createPipe(int diameter, int length, const std::string& name, bool repair, int amount = 1);
    bool createCStation(int unit_amount, int active_unit_amount, const std::string& name, int station_type, int amount = 1);

    // Принтер меню
    void print();

    // Поиск по фильтрам
    std::vector<Pipe> searchPipesByName(const std::string& name) const;
    std::vector<CStation> searchCStationsByName(const std::string& name) const;

    std::vector<Pipe> searchPipesByRepair(bool repair) const;
    std::vector<CStation> searchCStationsByActive(std::string comparison, int ratio) const;

    // Редактирование
    bool editPipe(const std::vector<std::size_t>& ids, bool new_repair);
    bool editCStation(const std::vector<std::size_t>& ids, int new_active_unit_amount);

    // Удаление
    bool deletePipe(const std::vector<std::size_t>& ids);
    bool deleteCStation(const std::vector<std::size_t>& ids);

    // Работа с файлам
    bool save(const std::filesystem::path& path) const;
    bool load(const std::filesystem::path& path);
};
