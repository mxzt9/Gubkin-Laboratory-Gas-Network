#include <fstream>
#include <stdexcept>
#include <sstream>
#include <filesystem>
#include <algorithm>

#include "network.hpp"
#include "utils.hpp"
#include "units.hpp"



/*



Вспомогательные методы



*/

const std::map<int, Pipe>& Network::getPipeMap() const {
    return pipeMap;
}

const std::map<int, CompressorStation>& Network::getCStationMap() const {
    return CStationMap;
}

/*



Методы с действиями на объектами в сети



*/


bool Network::addPipe(int diameter, int length, const std::string& name, bool isRepair, int id) {
    if (id == -1) { 
        id = getNextPipeId();
    }
    return pipeMap.try_emplace(id, diameter, length, name, isRepair).second;
}

bool Network::addCStation(int numWorkers, int numActiveWorkers, const std::string& name, StationType stationType) {
    const int id = getNextCStationId();
    return CStationMap.try_emplace(id, numWorkers, numActiveWorkers, name, stationType).second;
}

bool Network::connectPipe(int pipeId, int fromId, int toId) {
    if (!pipeMap.contains(pipeId) || !CStationMap.contains(fromId) || !CStationMap.contains(toId) || fromId == toId) return false;

    Pipe& pipe = pipeMap.at(pipeId);
    if (pipe.getCStationFromId() != -1 && pipe.getCStationToId() != -1) {
        return false;
    }

    pipe.connect(fromId, toId);
    return true;
}

bool Network::deletePipe(int id) {
    return pipeMap.erase(id) != 0;
}

bool Network::deleteCStation(int id) {
    if (!CStationMap.contains(id)) { 
        return false; 
    }

    for (auto& [pipeId, pipe] : pipeMap) {
        if (pipe.getCStationFromId() != -1 && pipe.getCStationToId() != -1) { 
            pipe.disconnect();
        }
    }

    CStationMap.erase(id);
    return true;
}

bool Network::editPipe(int id, bool newIsRepair) {
    try {
        Pipe& pipe = pipeMap.at(id);
        pipe.setRepair(newIsRepair);
        return true;
    }
    catch (const std::out_of_range&) {
        return false;
    }
}

bool Network::editCStation(int id, int newNumActiveWorkers) {
    try {
        CompressorStation& station = CStationMap.at(id);

        if (newNumActiveWorkers > station.getNumWorkers() || newNumActiveWorkers < 0) {
            return false;
        }
        station.setNumActiveWorkers(newNumActiveWorkers);
        return true;

    }
    catch (const std::out_of_range&) {
        return false;
    }
}


bool Network::saveToFile(const std::filesystem::path& filePath) {
    std::ofstream file(filePath);
    if (!file) return false;

    file << CStationMap.size() << '\n';

    for (const auto& [id, station] : CStationMap) {
        file << id << '\n' << station.getName() << '\n'
            << station.getNumWorkers() << '\n' << station.getNumActiveWorkers() << '\n'
            << static_cast<int>(station.getStationType()) << '\n';
    }

    file << pipeMap.size() << '\n';

    for (const auto& [id, pipe] : pipeMap) {
        file << id << '\n' << pipe.getName() << '\n'
            << pipe.getDiameter() << '\n' << pipe.getLength() << '\n'
            << pipe.getRepair() << '\n' << pipe.getCStationFromId() << '\n'
            << pipe.getCStationToId() << '\n';
    }

    return file.good();
}

bool Network::loadFromFile(const std::filesystem::path& filePath) {
    std::ifstream file(filePath);
    int count;

    if (!(file >> count) || count < 0) {
        return false;
    }

    std::map<int, Pipe> newPipeMap;
    std::map<int, CompressorStation> newCStationMap;
    
    int newPipeId = 0;
    int newCStationId = 0;

    // Сначала читаем КС, чтобы затем присоединить к ним трубы.
    for (int i = 0; i < count; ++i) {
        int id, workers, active, type;
        std::string name;

        if (!(file >> id)) {
            return false;
        }
        newCStationId = id + 1;

        file.ignore(); // Перенос строки после ID.

        if (!std::getline(file, name) || !(file >> workers >> active >> type)) {
            return false;
        }

        if (!newCStationMap.try_emplace(id, workers, active, name, static_cast<StationType>(type)).second) {
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
        newPipeId = id + 1;

        file.ignore();

        if (!std::getline(file, name) || !(file >> diameter >> length >> repair >> from >> to)) {
            return false;
        }
        if (!newPipeMap.try_emplace(id, diameter, length, name, repair).second) {
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
    CStationMap = newCStationMap;
    currentPipeId = newPipeId;
    currentCStationId = newCStationId;

    return true;
}

/*



Поиск и фильтры



*/


std::vector<int> Network::searchPipesByName(const std::string& name) const {
    std::vector<int> result;
    const std::string lowerName = getLowerString(name);

    for (const auto& [pipe_id, pipe] : pipeMap) {
        if (getLowerString(pipe.getName()).starts_with(lowerName)) {
            result.push_back(pipe_id);
        }
    }

    return result;
}


std::vector<int> Network::searchPipesByRepair(bool isRepair) const {
    std::vector<int> result;

    for (const auto& [pipe_id, pipe] : pipeMap) {
        if (pipe.getRepair() == isRepair) {
            result.push_back(pipe_id);
        }
    }

    return result;
}

std::vector<int> Network::searchCStationsByName(const std::string& name) const {
    std::vector<int> result;
    const std::string lowerName = getLowerString(name);

    for (const auto& [CStation_id, CStation] : CStationMap) {
        if (getLowerString(CStation.getName()).starts_with(lowerName)) {
            result.push_back(CStation_id);
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

    // Строчка хранящее число для сравнения
    const std::string numberString(condition.begin() + 1, numberEnd);
    // Эквивалент строчки в int
    const int requestedValue = std::stoi(numberString);

    for (const auto& [CStation_id, CStation] : CStationMap) {

        const int totalWorkers = CStation.getNumWorkers();
        const int activeWorkers = CStation.getNumActiveWorkers();

        long long currentValue;
        long long comparisonValue;

        // Для сравнения процентов используем перекрёстное умножение:
        //
        // activeWorkers * 100 / totalWorkers <=> requestedValue
        //
        // activeWorkers * 100 <=> requestedValue * totalWorkers
        //
        // currentValue = activeWorkers * 100
        // comparisonValue = requestedValue * totalWorkers
        if (searchByPercent) {
            currentValue = static_cast<long long>(activeWorkers) * 100;
            comparisonValue = static_cast<long long>(requestedValue) * totalWorkers;
        } else {
            currentValue = activeWorkers;
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
            result.push_back(CStation_id);
        }
    }

    return result;
};
