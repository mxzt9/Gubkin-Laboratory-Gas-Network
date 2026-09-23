#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <queue>
#include <set>

#include "network.hpp"
#include "utils.hpp"

void Network::printNetwork() const {
    Pipe::printPipeTableHeader();
    for (const auto& [id, pipe] : pipeMap) {
        pipe.printPipe();
    }
    
    CompressorStation::printCStationTableHeader();
    for (const auto& [id, station] : cStationMap) {
        station.printCStation();
    }
    
    std::cout << "\n\n";
}

/*
Вспомогательные методы
*/

int Network::getNextPipeId() const { return nextPipeId; }
int Network::getNextCStationId() const { return nextCStationId; }

const std::unordered_map<int, Pipe>& Network::getPipeMap() const {
    return pipeMap;
}

const std::unordered_map<int, CompressorStation>& Network::getCStationMap() const {
    return cStationMap;
}

/*
Методы с действиями на объектами в сети
*/


bool Network::addPipe(int diameter, int length, const std::string& name, bool repair) {
    const int id = generatePipeId();
    return pipeMap.try_emplace(id, id, diameter, length, name, repair).second;
}

bool Network::addCStation(int numWorkshops, int numActiveWorkshops, const std::string& name, StationType stationType) {
    const int id = generateCStationId();
    return cStationMap.try_emplace(id, id, numWorkshops, numActiveWorkshops, name, stationType).second;
}

bool Network::connectPipe(int pipeId, int fromId, int toId) {
    if (!pipeMap.contains(pipeId) || !cStationMap.contains(fromId) || !cStationMap.contains(toId) || fromId == toId) return false;

    Pipe& pipe = pipeMap.at(pipeId);
    if (!pipe.isFree()) {
        return false;
    }

    pipe.connect(fromId, toId);
    return true;
}

bool Network::deletePipe(int id) {
    return pipeMap.erase(id) != 0;
}

int Network::findFreePipe(int diameter) const {
    for (const auto& [pipeId, pipe] : pipeMap) {
        if (pipe.getDiameter() == diameter && pipe.isFree()) {
            return pipeId;
        }
    }

    return -1;
}

bool Network::deleteCStation(int id) {
    if (!cStationMap.contains(id)) {
        return false;
    }

    for (auto& [pipeId, pipe] : pipeMap) {
        if (pipe.getCStationFromId() == id || pipe.getCStationToId() == id) {
            pipe.disconnect();
        }
    }

    cStationMap.erase(id);
    return true;
}

bool Network::editPipe(int id, bool newIsRepair) {
    if (!pipeMap.contains(id)) {
        return false;
    }

    Pipe& pipe = pipeMap.at(id);
    pipe.setRepair(newIsRepair);
    return true;
}

bool Network::editCStation(int id, int newNumActiveWorkshops) {
    if (!cStationMap.contains(id)) {
        return false;
    }


    CompressorStation& station = cStationMap.at(id);

    return station.setNumActiveWorkshops(newNumActiveWorkshops);

}

bool Network::saveToFile(const std::filesystem::path& filePath) {
    std::ofstream file(filePath);

    if (!file){
        return false;
    }

    file << cStationMap.size() << '\n';

    for (auto& [id, cStation] : cStationMap) {
        cStation.save(file);
    }

    file << pipeMap.size() << '\n';

    for (auto& [id, pipe] : pipeMap) {
        pipe.save(file);
    }

    return file.good();
}

bool Network::loadFromFile(const std::filesystem::path& filePath) {
    std::ifstream file(filePath);
    int count;

    if (!(file >> count) || count < 0) {
        return false;
    }

    std::unordered_map<int, Pipe> newPipeMap;
    std::unordered_map<int, CompressorStation> newCStationMap;

    int newPipeId = 0;
    int newCStationId = 0;

    // Сначала читаем КС, чтобы затем присоединить к ним трубы.
    for (int i = 0; i < count; ++i) {
        int id, workshops, active, type;
        std::string name;

        if (!(file >> id)) {
            return false;
        }
        newCStationId = std::max(newCStationId, id + 1);

        file.ignore(); // Перенос строки после ID.

        if (!std::getline(file, name) || !(file >> workshops >> active >> type)) {
            return false;
        }

        if (!newCStationMap.try_emplace(id, id, workshops, active, name, static_cast<StationType>(type)).second) {
            return false;
        }

    }

    if (!(file >> count) || count < 0) {
        return false;
    }

    for (int i = 0; i < count; ++i) {
        int id, diameter, length, from, to;
        bool repair;
        std::string name;

        if (!(file >> id)) {
            return false;
        }
        newPipeId = std::max(newPipeId, id + 1);

        file.ignore();

        if (!std::getline(file, name) || !(file >> diameter >> length >> repair >> from >> to)) {
            return false;
        }
        if (!newPipeMap.try_emplace(id, id, diameter, length, name, repair).second) {
            return false;
        }
        if (from != -1 || to != -1) {
            if (!newCStationMap.contains(from) || !newCStationMap.contains(to) || from == to) {
                return false;
            }
            newPipeMap.at(id).connect(from, to);
        }
    }


    pipeMap = newPipeMap;
    cStationMap = newCStationMap;
    nextPipeId = newPipeId;
    nextCStationId = newCStationId;

    return true;
}

/*
Поиск и фильтры
*/


std::vector<int> Network::searchPipesByName(const std::string& name) const {
    std::vector<int> result;
    const std::string lowerName = getLowerString(name);

    for (const auto& [pipeId, pipe] : pipeMap) {
        if (getLowerString(pipe.getName()).starts_with(lowerName)) {
            result.push_back(pipeId);
        }
    }

    return result;
}


std::vector<int> Network::searchPipesByRepair(bool repair) const {
    std::vector<int> result;

    for (const auto& [pipeId, pipe] : pipeMap) {
        if (pipe.getRepair() == repair) {
            result.push_back(pipeId);
        }
    }

    return result;
}

std::vector<int> Network::searchCStationsByName(const std::string& name) const {
    std::vector<int> result;
    const std::string lowerName = getLowerString(name);

    for (const auto& [cStationId, cStation] : cStationMap) {
        if (getLowerString(cStation.getName()).starts_with(lowerName)) {
            result.push_back(cStationId);
        }
    }

    return result;

}

std::vector<int> Network::searchCStationsByActive(const std::vector<char>& condition) const {
    std::vector<int> result {};

    // Получаем первый и последний символ, определяем знак сравнения и условие сравнивания
    // Пример: >50% -> comparison == '>' ; searchByPercent == true
    const char comparison = condition.front();
    const bool searchByPercent = condition.back() == '%';

    // Определение конца индекса цифры
    const auto numberEnd = searchByPercent ? condition.end() - 1 : condition.end();

    // Строка, хранящая число для сравнения
    const std::string numberString(condition.begin() + 1, numberEnd);
    // Эквивалент строчки в int
    const int requestedValue = std::stoi(numberString);

    for (const auto& [cStationId, cStation] : cStationMap) {

        const int totalWorkshops = cStation.getNumWorkshops();
        const int activeWorkshops = cStation.getNumActiveWorkshops();

        long long currentValue;
        long long comparisonValue;

        // Для сравнения процентов используем перекрёстное умножение:
        //
        // activeWorkshops * 100 / totalWorkshops <=> requestedValue
        //
        // activeWorkshops * 100 <=> requestedValue * totalWorkshops
        //
        // currentValue = activeWorkshops * 100
        // comparisonValue = requestedValue * totalWorkshops
        if (searchByPercent) {
            currentValue = static_cast<long long>(activeWorkshops) * 100;
            comparisonValue = static_cast<long long>(requestedValue) * totalWorkshops;
        } else {
            currentValue = activeWorkshops;
            comparisonValue = requestedValue;
        }

        bool matches = false;

        // Сравнение
        if (comparison == '>') {
            matches = currentValue > comparisonValue;
        } else if (comparison == '<') {
            matches = currentValue < comparisonValue;
        } else if (comparison == '=') {
            matches = currentValue == comparisonValue;
        }

        if (matches) {
            result.push_back(cStationId);
        }
    }

    return result;
}

bool Network::topologicalSort(std::vector<int>& result) const {
    // Очистка массива
    result.clear();

    // Массив вида {id КС: степень входящих в нее соединений}
    std::unordered_map<int, int> inDegree;

    // Массив вида {id КС: Id КС-соседей}
    std::unordered_map<int, std::vector<int>> graph;

    // Строим граф
    for (const auto& [pipeId, pipe] : pipeMap) {
        // Получение id
        int from = pipe.getCStationFromId();
        int to = pipe.getCStationToId();

        // Пропуск не подключенных труб
        if (from == -1 || to == -1) {
            continue;
        }

        // Добавляем вершины, если их ещё нет
        inDegree.try_emplace(from, 0);
        inDegree.try_emplace(to, 0);

        // Увеличиваем степень вершины
        inDegree[to]++;

        // Добавляем соединение
        graph[from].push_back(to);
    }


    std::queue<int> queue;

    // Добавляем в очередь вершины с отсутствующими входными подключениями
    for (const auto& [cStationId, degree] : inDegree) {
        if (degree == 0) {
            queue.push(cStationId);
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
        for (int nextId : graph[currentId]) {
            inDegree[nextId]--;

            // Если у отсоединенной вершины количество ребер стало ноль добавляем в очередь
            if (inDegree[nextId] == 0) {
                queue.push(nextId);
            }
        }
    }

    // Проверка на наличие цикла, если размеры не совпадут -> false
    return result.size() == inDegree.size();
}



bool Network::findShortestPath(int startId, int finishId, std::vector<Edge>& result) const {
    struct QueueNode {
        int id {};
        int distance {};

        bool operator<(const QueueNode& other) const {
            if (distance == other.distance) {
                return id < other.id;
            }

            return distance < other.distance;
        }
    };

    result.clear();

    // Массив вида {id КС: минимальное расстояние от начальной КС}
    std::unordered_map<int, int> distances;

    // Массив вида {id КС: id предыдущей КС}
    std::unordered_map<int, int> previousIds;

    // Массив вида {id КС: длина ребра от предыдущей КС}
    std::unordered_map<int, int> previousDistances;

    // Массив вида {id КС: Id КС-соседей, расстояние до них}
    std::unordered_map<int, std::vector<Edge>> graph;

    // Строим граф
    for (const auto& [pipeId, pipe] : pipeMap) {
        int from = pipe.getCStationFromId();
        int to = pipe.getCStationToId();

        // Пропуск неподключенных труб
        if (from == -1 || to == -1) {
            continue;
        }

        int distance = pipe.getLength();

        graph[from].push_back(Edge{to, distance});
    }

    // Заполняем расстояния до всех КС бесконечностью
    for (const auto& [cStationId, station] : cStationMap) {
        distances[cStationId] = INT_MAX;
    }

    // Проверка существования начальной КС
    if (distances.find(startId) == distances.end()) {
        return false;
    }

    // Проверка существования конечной КС
    if (distances.find(finishId) == distances.end()) {
        return false;
    }

    // Расстояние от начальной вершины до самой себя
    distances[startId] = 0;

    std::set<QueueNode> queue;

    queue.insert(QueueNode{startId, 0});

    while (!queue.empty()) {
        auto currentNode = queue.begin();

        int currentId = currentNode->id;
        int currentDistance = currentNode->distance;

        queue.erase(currentNode);

        // Если дошли до конечной КС
        if (currentId == finishId) {
            break;
        }

        // Если у КС нет соседей
        if (graph.find(currentId) == graph.end()) {
            continue;
        }

        // Перебираем соседей текущей КС
        for (const auto& edge : graph.at(currentId)) {
            int neighbourId = edge.id;
            int edgeDistance = edge.distance;

            int newDistance = currentDistance + edgeDistance;

            // Если нашли более короткий путь
            if (newDistance < distances[neighbourId]) {
                // Если вершина уже была в очереди,
                // удаляем её старое значение
                if (distances[neighbourId] != INT_MAX) {
                    queue.erase(QueueNode{neighbourId, distances[neighbourId]});
                }

                // Обновляем минимальное расстояние
                distances[neighbourId] = newDistance;

                // Запоминаем откуда пришли
                previousIds[neighbourId] = currentId;

                // Запоминаем длину ребра
                previousDistances[neighbourId] = edgeDistance;

                // Добавляем вершину в очередь
                queue.insert(QueueNode{neighbourId, newDistance});
            }
        }
    }

    // Если путь до конечной КС не найден
    if (distances[finishId] == INT_MAX) {
        return false;
    }

    // Восстанавливаем путь с конца
    int currentId = finishId;

    while (currentId != startId) {
        result.push_back(Edge{
            currentId,
            previousDistances[currentId]
        });

        currentId = previousIds[currentId];
    }

    // Добавляем начальную КС
    result.push_back(Edge{startId, 0});

    // Переворачиваем путь
    std::reverse(result.begin(), result.end());

    return true;
}
