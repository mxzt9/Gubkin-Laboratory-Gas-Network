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

int Network::getNextPipeId() {
    return currentPipeId++;
}

int Network::getNextCStationId() {
    return currentCStationId++;
}

int Network::getCurrentPipeId() const {
    return currentPipeId;
}

int Network::getCurrentCStationId() const {
    return currentCStationId;
}

bool Network::isPipeInArrayById(int id) const {
    auto it = std::find_if(pipeArray.begin(), pipeArray.end(), [id](const auto& pipe) { return pipe.getId() == id; });

    return it != pipeArray.end();
}

bool Network::isCStationInArrayById(int id) const {
    auto it = std::find_if(CStationArray.begin(), CStationArray.end(), [id](const auto& CStation) { return CStation.getId() == id; });

    return it != CStationArray.end();
}

Pipe& Network::getPipeById(int id) {
    auto it = std::find_if(pipeArray.begin(), pipeArray.end(), [id](const auto& pipe) { return pipe.getId() == id; });

    if (it == pipeArray.end()) { 
        throw std::out_of_range("[*] Ошибка: Труба с ID= " + std::to_string(id) + " не найдена"); 
    }

    return *it;
}


CompressorStation& Network::getCStationById(int id) {
    auto it = std::find_if(CStationArray.begin(), CStationArray.end(), [id](const auto& station) { return station.getId() == id; });

    if (it == CStationArray.end()) {
        throw std::out_of_range("[*] Ошибка: КС с ID= " + std::to_string(id) + " не найдена");
    }

    return *it;
}


const std::vector<Pipe>& Network::getPipeArray() const {
    return pipeArray;
}

const std::vector<CompressorStation>& Network::getCStationArray() const {
    return CStationArray;
}

std::size_t Network::getPipeArrayLen() const {
    return pipeArray.size();
}

std::size_t Network::getCStationArrayLen() const {
    return CStationArray.size();
}

/*



Методы с действиями на объектами в сети



*/


bool Network::addPipe(int diameter, int length, const std::string& name, bool isRepair, int id) {
    // Проверка на то, есть переданный ID в массиве
    if (id != -1 && isPipeInArrayById(id)) {
        return false;
    } 

    if (id == -1) {
        pipeArray.emplace_back(getNextPipeId(), diameter, length, name, isRepair);
    } else {
        pipeArray.emplace_back(id, diameter, length, name, isRepair);
    }
    return true;
}

bool Network::addCStation(int numWorkers, int numActiveWorkers, const std::string& name, StationType stationType) {
    CStationArray.emplace_back(getNextCStationId(), numWorkers, numActiveWorkers, name, stationType);
    return true;
}

bool Network::connectPipe(int pipeId, int CStationIdFrom, int CStationIdTo) {
    if (!isCStationInArrayById(CStationIdFrom) || !isCStationInArrayById(CStationIdTo) || CStationIdFrom == CStationIdTo) {
        return false;
    }

    Pipe& pipe = getPipeById(pipeId);
    if (pipe.hasConnectedCStation()) {
        return false;
    }

    pipe.setCStationFromId(CStationIdFrom);
    pipe.setCStationToId(CStationIdTo);

    return true;
}


bool Network::deletePipe(int id) {
    return std::erase_if(pipeArray, [id](const auto& pipe) { return pipe.getId() == id; }) == 1;
}

bool Network::deleteCStation(int id) {
    // Отсоединяем трубы от КС
    for (std::size_t i = 0; i != pipeArray.size(); ++i) {
        if (pipeArray[i].isConnectedToCStationById(id)) {
            pipeArray[i].disconnect();
        }
    }

    return std::erase_if(CStationArray, [id](const auto& CStation) { return CStation.getId() == id; }) == 1;
}



bool Network::editPipe(int id, int newDiameter, int newLength, const std::string& newName, bool newIsRepair) {
    Pipe& pipe = getPipeById(id);

    pipe.setDiameter(newDiameter);
    pipe.setLength(newLength);
    pipe.setName(newName);
    pipe.setRepair(newIsRepair);

    return true;
}

bool Network::editCStation(int id, int newNumWorkers, int newNumActiveWorkers, const std::string& newName, StationType newType) {
    if (newNumActiveWorkers > newNumWorkers || newNumActiveWorkers < 0) {
        return false;
    }

    CompressorStation& station = getCStationById(id);

    station.setNumWorkers(newNumWorkers);
    station.setNumActiveWorkers(newNumActiveWorkers);
    station.setName(newName);
    station.setStationType(newType);

    return true;
}



bool Network::saveToFile(const std::filesystem::path& dirPath, const std::filesystem::path& pipeFileName, const std::filesystem::path& cStationFileName) {
    const std::filesystem::path pipePath = dirPath / pipeFileName;
    const std::filesystem::path cStationPath = dirPath / cStationFileName;

    std::ofstream pipeFile(pipePath);
    std::ofstream cStationFile(cStationPath);

    if (!pipeFile || !cStationFile) {
        return false;
    }

    pipeFile << "ID,Diameter,Length,Name,IsRepair,CStationFromId,CStationToId\n";

    for (const auto& pipe : pipeArray) {
        pipeFile << pipe.getId() << ','
                 << pipe.getDiameter() << ','
                 << pipe.getLength() << ','
                 << pipe.getName() << ','
                 << pipe.getRepair() << ','
                 << pipe.getCStationFromId() << ','
                 << pipe.getCStationToId() << '\n';
    }

    cStationFile << "ID,NumWorkers,NumActiveWorkers,Name,StationType\n";

    for (const auto& cStation : CStationArray) {
        cStationFile << cStation.getId() << ','
                     << cStation.getNumWorkers() << ','
                     << cStation.getNumActiveWorkers() << ','
                     << cStation.getName() << ','
                     << static_cast<int>(cStation.getStationType()) << '\n';
    }

    return pipeFile.good() && cStationFile.good();
}

bool Network::loadFromFile(const std::filesystem::path& pipePath, const std::filesystem::path& cStationPath) {

    // Открытие потоков чтения
    std::ifstream pipeFile { pipePath };
    std::ifstream cStationFile { cStationPath };

    if (!pipeFile || !cStationFile) {
        return false;
    }

    Network loadedNetwork;
    std::string line;

    try {
        // Чтение заголовка
        if (!std::getline(cStationFile, line)) {
            return false;
        }

        // Чтение КС
        while (std::getline(cStationFile, line)) {
            if (line.empty()) {
                continue;
            }

            // Массив значений
            const auto tokens = splitCsvLine(line);
            if (tokens.size() != 5) {
                return false;
            }

            // Преобразование и добавление в массив
            const int id = std::stoi(tokens[0]);
            const int workers = std::stoi(tokens[1]);
            const int activeWorkers = std::stoi(tokens[2]);
            const int type = std::stoi(tokens[4]);

            loadedNetwork.CStationArray.emplace_back(id, workers, activeWorkers, tokens[3], static_cast<StationType>(type));
        }

        // Чтение заголовка
        if (!std::getline(pipeFile, line)) {
            return false;
        }

        // Чтение труб
        while (std::getline(pipeFile, line)) {
            if (line.empty()) {
                continue;
            }

            // Массив значений
            const auto tokens = splitCsvLine(line);
            if (tokens.size() != 7) {
                return false;
            }

            // Преобразование и добавление в массив
            const int id = std::stoi(tokens[0]);
            const int diameter = std::stoi(tokens[1]);
            const int length = std::stoi(tokens[2]);
            const int repair = std::stoi(tokens[4]);
            const int fromId = std::stoi(tokens[5]);
            const int toId = std::stoi(tokens[6]);

            loadedNetwork.pipeArray.emplace_back(id, diameter, length, tokens[3], repair == 1);

            // Ссылка на только что добавленную трубу
            Pipe& pipe = loadedNetwork.pipeArray.back();

            // Проверка на то, есть ли у трубы есть подключенные КС
            if (fromId != -1 && toId != -1) {
                // Проверка на наличие КС с такими id
                if (!loadedNetwork.isCStationInArrayById(fromId)|| !loadedNetwork.isCStationInArrayById(toId)) {
                    return false;
                }

                pipe.setCStationFromId(fromId);
                pipe.setCStationToId(toId);
            }
        }
    } catch (const std::exception&) {
        return false;
    }
    
    CStationArray = std::move(loadedNetwork.CStationArray);
    pipeArray = std::move(loadedNetwork.pipeArray);

    // Сопостовление Id
    if (!pipeArray.empty()) {
        currentPipeId = pipeArray.back().getId() + 1;
    } else {
        currentPipeId = 0;
    }

    if (!CStationArray.empty()) {
        currentCStationId = CStationArray.back().getId() + 1;
    } else {
        currentCStationId = 0;
    }

    return true;
}

/*



Поиск и фильтры



*/


std::vector<const Pipe*> Network::searchPipesByName(const std::string& name) const {
    std::vector<const Pipe*> result;
    const std::string lowerName = getLowerString(name);

    for (const auto& pipe : pipeArray) {
        if (getLowerString(pipe.getName()).starts_with(lowerName)) {
            result.push_back(&pipe);
        }
    }

    return result;
}


std::vector<const Pipe*> Network::searchPipesByRepair(bool isRepair) const {
    std::vector<const Pipe*> result {};

    for (const Pipe& pipe : getPipeArray()) {
        const bool pipeRepairStatus = pipe.getRepair();

        if (pipeRepairStatus == isRepair) {
            result.push_back(&pipe);
        }
    }

    return result;
}

std::vector<const CompressorStation*> Network::searchCStationsByName(const std::string& name) const {
    std::vector<const CompressorStation*> result {};
    const std::string lowerName = getLowerString(name);

    for (const auto& CStation : CStationArray) {
        if (getLowerString(CStation.getName()).starts_with(lowerName)) {
            result.push_back(&CStation);
        }
    }

    return result;

};

std::vector<const CompressorStation*> Network::searchCStationsByActive(const std::vector<char>& condition) const {
    std::vector<const CompressorStation*> result {};

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

    for (const CompressorStation& CStation : getCStationArray()) {

        const int totalWorkers = CStation.getNumWorkers();
        const int activeWorkers = CStation.getNumActiveWorkers();

        long long currentValue;
        long long comparisonValue;

        // Для сравнения процентов используем перекрёстное умножение:
        //
        // activeWorkers / totalWorkers * 100 <=> requestedValue
        //
        // activeWorkers * 100 / totalWorkers <=> requestedValue
        //
        // activeWorkers * 100 <=> requestedValue * totalWorkers
        //
        // currentValue = activeWorkers * 100
        // comparisonValue = requestedValue * totalWorkers

        // Иначе оставляем как есть
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
            result.push_back(&CStation);
        }
    }

    return result;
};
