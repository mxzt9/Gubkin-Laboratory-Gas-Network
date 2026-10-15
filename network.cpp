#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>

#include "network.hpp"
#include "utils.hpp"


// Добавление
bool Network::createPipe(const Pipe& pipe) {
    return PipeMap.emplace(pipe.get_id(), pipe).second;
}

bool Network::createCStation(const CStation& station) {
    return CStationMap.emplace(station.get_id(), station).second;
}


void Network::print() {
    std::vector<int> pipeIds;
    std::vector<int> CStationIds;

    for (const auto& [id, pipe] : PipeMap) {
        pipeIds.push_back(id);
    }

    for (const auto& [id, station] : CStationMap) {
        CStationIds.push_back(id);
    }

    std::sort(pipeIds.begin(), pipeIds.end());
    std::sort(CStationIds.begin(), CStationIds.end());

    Pipe::print_table_header();

    for (const int id : pipeIds) {
        std::cout << PipeMap.at(id);
    }

    CStation::print_table_header();

    for (const int id : CStationIds) {
        std::cout << CStationMap.at(id);
    }

    Output::Message("\n\n");
}

std::vector<int> Network::searchPipesByName(const std::string& name) const {
    std::vector<int> result;

    for (const auto& [id, pipe] : PipeMap) {
        if (pipe.get_name().find(name) != std::string::npos) {
            result.push_back(id);
        }
    }

    return result;
}

std::vector<int> Network::searchCStationsByName(const std::string& name) const {
    std::vector<int> result;

    for (const auto& [id, station] : CStationMap) {
        if (station.get_name().find(name) != std::string::npos) {
            result.push_back(id);
        }
    }

    return result;
}

std::vector<int> Network::searchPipesByRepair(bool repair) const {
    std::vector<int> result;

    for (const auto& [id, pipe] : PipeMap) {
        if (pipe.get_repair() == repair) {
            result.push_back(id);
        }
    }

    return result;
}


std::vector<int> Network::searchCStationsByActive(const std::string& comparison, int ratio) const {
    if (!(comparison == "<" || comparison == "<=" || comparison == "=" || comparison == ">=" || comparison == ">")) {
        return {};
    }
    
    std::vector<int> result {};

    for (const auto& [id, station] : CStationMap) {
        if (comparison == "<" && station.get_unit_ratio() < ratio) {
            result.push_back(id);
        } else if (comparison == "<=" && station.get_unit_ratio() <= ratio) {
            result.push_back(id);
        } else if (comparison == "=" && station.get_unit_ratio() == ratio) {
            result.push_back(id);
        } else if (comparison == ">" && station.get_unit_ratio() > ratio) {
            result.push_back(id);
        } else if (comparison == ">=" && station.get_unit_ratio() >= ratio) {
            result.push_back(id);
        }
    }

    return result;
}


bool Network::editPipe(const std::vector<int>& ids, bool new_repair) {
    if (ids.empty()) {
        return false;
    }

    for (const auto id : ids) {
        if (PipeMap.find(id) == PipeMap.end()) {
            return false;
        }
    }

    for (const auto id : ids) {
        PipeMap.at(id).edit(new_repair);
    }

    return true;
}

bool Network::editCStation(const std::vector<int>& ids, int new_active_unit_amount) {
    if (ids.empty() || new_active_unit_amount < 0) {
        return false;
    }

    // Проверяем есть ли все трубы и у всех кол-во цехов больше чем новых активных
    for (const auto id : ids) {
        auto it = CStationMap.find(id);
        if (it == CStationMap.end() || it->second.get_unit_amount() < new_active_unit_amount) {
            return false;
        }
    }

    for (const auto id : ids) {
        CStationMap.at(id).edit(new_active_unit_amount);
    }

    return true;
}

bool Network::deletePipe(const std::vector<int>& ids) {
    if (ids.empty()) {
        return false;
    }

    for (const auto id : ids) {
        if (PipeMap.find(id) == PipeMap.end()) {
            return false;
        }
    }

    for (const auto id : ids) {
        PipeMap.erase(id);
    }

    return true;
}

bool Network::deleteCStation(const std::vector<int>& ids) {
    if (ids.empty()) {
        return false;
    }

    for (const auto id : ids) {
        if (CStationMap.find(id) == CStationMap.end()) {
            return false;
        }
    }

    for (const auto id : ids) {
        CStationMap.erase(id);
    }

    return true;
}

bool Network::save(const std::filesystem::path& path) const {
    std::ofstream file(path);

    if (!file) {
        return false;
    }

    
    std::vector<int> ids;

    file << CStationMap.size() << '\n';

    for (const auto& [id, station] : CStationMap) {
        ids.push_back(id);
    }

    std::sort(ids.begin(), ids.end());

    for (const auto id : ids) {
        if (!CStationMap.at(id).save(file)) {
            return false;
        }
    }

    ids.clear();

    file << PipeMap.size() << '\n';

    for (const auto& [id, pipe] : PipeMap) {
        ids.push_back(id);
    }

    std::sort(ids.begin(), ids.end());

    for (const auto id : ids) {
        if (!PipeMap.at(id).save(file)) {
            return false;
        }
    }

    file.close();

    return !file.fail();
}

bool Network::load(const std::filesystem::path& path) {
    std::ifstream file(path);

    if (!file) {
        return false;
    }

    std::unordered_map<int, CStation> stations;
    std::unordered_map<int, Pipe> pipes;
    int count {};

    if (!(file >> count) || count < 0) {
        return false;
    }

    for (int i = 0; i < count; ++i) {
        CStation station {};

        if (!station.load(file)) {
            return false;
        }

        const auto id = station.get_id();

        if (!stations.emplace(id, station).second) {
            return false;
        }
    }

    if (!(file >> count) || count < 0) {
        return false;
    }

    for (int i = 0; i < count; ++i) {
        Pipe pipe {};

        if (!pipe.load(file)) {
            return false;
        }

        const auto id = pipe.get_id();

        if (!pipes.emplace(id, pipe).second) {
            return false;
        }
    }

    file >> std::ws;

    if (!file.eof() || file.bad()) {
        return false;
    }

    CStationMap.swap(stations);
    PipeMap.swap(pipes);

    return true;
}
