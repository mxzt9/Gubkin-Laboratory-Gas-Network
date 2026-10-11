#include <iostream>
#include <string>
#include <algorithm>

#include "network.hpp"
#include "utils.hpp"


// Добавление
bool Network::createPipe(int diameter, int length, const std::string& name, bool repair, int amount) {
    for (int i = 0; i != amount; ++i) {
        Pipe pipe {diameter, length, name, repair};

        if (!PipeMap.emplace(pipe.get_id(), pipe).second) {
            return false;
        }
    }

    return true;
}

bool Network::createCStation(int unit_amount, int active_unit_amount, const std::string& name, int station_type, int amount) {
    for (int i = 0; i != amount; ++i) {
        CStation station {unit_amount, active_unit_amount, name, station_type};

        if (!CStationMap.emplace(station.get_id(), station).second) {
            return false;
        }
    }

    return true;
}

void Network::print() {
    std::vector<std::size_t> pipeIds;
    std::vector<std::size_t> CStationIds;

    for (const auto& [id, pipe] : PipeMap) {
        pipeIds.push_back(id);
    }

    for (const auto& [id, station] : CStationMap) {
        CStationIds.push_back(id);
    }

    std::sort(pipeIds.begin(), pipeIds.end());
    std::sort(CStationIds.begin(), CStationIds.end());

    Pipe::print_table_header();

    for (const std::size_t id : pipeIds) {
        PipeMap.at(id).print();
    }

    CStation::print_table_header();

    for (const std::size_t id : CStationIds) {
        CStationMap.at(id).print();
    }

    Output::Message("\n\n");
}

std::vector<Pipe> Network::searchPipesByName(const std::string& name) const {
    std::vector<Pipe> result;

    for (const auto& [id, pipe] : PipeMap) {
        if (pipe.get_name() == name) {
            result.push_back(pipe);
        }
    }

    return result;
}

std::vector<CStation> Network::searchCStationsByName(const std::string& name) const {
    std::vector<CStation> result;

    for (const auto& [id, station] : CStationMap) {
        if (station.get_name() == name) {
            result.push_back(station);
        }
    }

    return result;
}

std::vector<Pipe> Network::searchPipesByRepair(bool repair) const {
    std::vector<Pipe> result;

    for (const auto& [id, pipe] : PipeMap) {
        if (pipe.get_repair() == repair) {
            result.push_back(pipe);
        }
    }

    return result;
}


std::vector<CStation> Network::searchCStationsByActive(std::string comparison, int ratio) const {
    if (!(comparison == "<" || comparison == "=" || comparison == ">")) {
        return {};
    }
    
    std::vector<CStation> result;

    for (const auto& [id, station] : CStationMap) {
        if (comparison == "<" && station.get_unit_ratio() < ratio) {
            result.push_back(station);
        }  else if (comparison == "=" && station.get_unit_ratio() == ratio) {
            result.push_back(station);
        } else if (comparison == ">" && station.get_unit_ratio() > ratio) {
            result.push_back(station);
        }
    }

    return result;
}


bool Network::editPipe(const std::vector<std::size_t>& ids, bool new_repair) {
    for (const auto id : ids) {
        if (auto it = PipeMap.find(id); it != PipeMap.end()) {
            // При ошибке редактирования выходим
            if (!it->second.edit(new_repair)) {
                return false;
            };
        }
    }

    return true;
}

bool Network::editCStation(const std::vector<std::size_t>& ids, int new_active_unit_amount) {
    if (new_active_unit_amount < 0) {
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

bool Network::deletePipe(const std::vector<std::size_t>& ids) {
    for (const auto id : ids) {
        PipeMap.erase(id);
    }

    return true;
}

bool Network::deleteCStation(const std::vector<std::size_t>& ids) {
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

    
    std::vector<std::size_t> ids;

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

    std::unordered_map<std::size_t, CStation> stations;
    std::unordered_map<std::size_t, Pipe> pipes;
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
