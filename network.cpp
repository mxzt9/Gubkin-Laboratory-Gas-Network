#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <queue>

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

    Pipe::printTableHeader();

    for (const int id : pipeIds) {
        std::cout << PipeMap.at(id);
    }

    CStation::printTableHeader();

    for (const int id : CStationIds) {
        std::cout << CStationMap.at(id);
    }

    Output::Message("\n\n");
}

std::set<int> Network::searchPipesByName(const std::string& name) const {
    std::set<int> result;

    for (const auto& [id, pipe] : PipeMap) {
        if (Filter::Name(pipe, name)) {
            result.insert(id);
        }
    }

    return result;
}

std::set<int> Network::searchCStationsByName(const std::string& name) const {
    std::set<int> result;

    for (const auto& [id, station] : CStationMap) {
        if (Filter::Name(station, name)) {
            result.insert(id);
        }
    }

    return result;
}

std::set<int> Network::searchPipesByRepair(bool repair) const {
    std::set<int> result;

    for (const auto& [id, pipe] : PipeMap) {
        if (Filter::Repair(pipe, repair)) {
            result.insert(id);
        }
    }

    return result;
}


std::set<int> Network::searchCStationsByActive(const std::string& comparison, int ratio) const {
    if (!(comparison == "<" || comparison == "<=" || comparison == "=" || comparison == ">=" || comparison == ">")) {
        return {};
    }
    
    std::set<int> result {};

    for (const auto& [id, station] : CStationMap) {
        if (Filter::Active(station, comparison, ratio)) {
            result.insert(id);
        }
    }

    return result;
}


bool Network::editPipe(const std::set<int>& ids, bool new_repair) {
    for (const auto id : ids) {
        if (!PipeMap.contains(id)) {
            return false;
        }
    }

    for (const auto id : ids) {
        PipeMap[id].edit(new_repair);
    }

    return true;
}

bool Network::editCStation(const std::set<int>& ids, int new_active_unit_amount) {
    if (new_active_unit_amount < 0) {
        return false;
    }

    // Проверяем есть ли все трубы и у всех кол-во цехов больше чем новых активных
    for (const auto id : ids) {
        if (!CStationMap.contains(id)) {
            return false;
        }

        if (CStationMap[id].get_unit_amount() < new_active_unit_amount) {
            return false;
        }
    }

    for (const auto id : ids) {
        CStationMap[id].edit(new_active_unit_amount);
    }

    return true;
}

bool Network::deletePipe(const std::set<int>& ids) {
    for (const auto id : ids) {
        PipeMap.erase(id);
    }

    return true;
}

bool Network::deleteCStation(const std::set<int>& ids) {
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

    PipeMap.clear();
    CStationMap.clear();

    Pipe::reset_next_id();
    CStation::reset_next_id();

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

        if (!CStationMap.emplace(id, station).second) {
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

        if (!PipeMap.emplace(id, pipe).second) {
            return false;
        }
    }

    file >> std::ws;

    if (!file.eof() || file.bad()) {
        return false;
    }

    return true;
}

int Network::findTargetPipe(int target_diameter) const {
    for (const auto& [id, pipe] : PipeMap) {
        if (pipe.get_diameter() == target_diameter && pipe.is_free()) {
            return id;
        }
    }

    return -1;
}

bool Network::connect(int pipe_id, int CStationOut_id, int CStationIn_id) {
    if (!PipeMap.contains(pipe_id)) return false;
    if (!CStationMap.contains(CStationOut_id) || !CStationMap.contains(CStationIn_id)) return false;
    if (CStationOut_id == CStationIn_id) return false;

    PipeMap[pipe_id].connect(CStationOut_id, CStationIn_id);
    return true;
}

bool Network::topologicalSort(std::vector<int>& result) const {
    // Очистка массива
    result.clear();

    // Массив вида {id КС: степень входящих в нее соединений}
    std::unordered_map<int, int> in_degree;

    // Массив вида {id КС: Id КС-соседей}
    std::unordered_map<int, std::vector<int>> graph;

    // Строим граф
    for (const auto& [id, pipe] : PipeMap) {
        // Получение id
        int out = pipe.get_CStationOut_id();
        int in = pipe.get_CStationIn_id();

        // Пропуск не подключенных труб
        if (out == -1 || in == -1) {
            continue;
        }

        // Добавляем вершины, если их ещё нет
        in_degree.try_emplace(out, 0);
        in_degree.try_emplace(in, 0);

        // Увеличиваем степень вершины
        in_degree[in]++;

        // Добавляем соединение
        graph[out].push_back(in);
    }


    std::queue<int> queue;

    // Добавляем в очередь вершины с отсутствующими входными подключениями
    for (const auto& [CStationId, degree] : in_degree) {
        if (degree == 0) {
            queue.push(CStationId);
        }
    }


    while (!queue.empty()) {

        // Получаем первый id
        int currentId = queue.front();

        // Удаляем его
        queue.pop();

        // Добавляем в результирующий массив
        result.push_back(currentId);

        // Удаляем исходящие рёбра текущей вершины
        for (int next_id : graph[currentId]) {
            in_degree[next_id]--;

            // Если у отсоединенной вершины количество ребер стало ноль добавляем в очередь
            if (in_degree[next_id] == 0) {
                queue.push(next_id);
            }
        }
    }

    // Проверка на наличие цикла, если размеры не совпадут -> false
    return result.size() == in_degree.size();
}