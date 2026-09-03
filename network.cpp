#include <fstream>
#include <stdexcept>

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
    for (size_t i = 0; i < pipeArray.size(); ++i) {
        if (pipeArray[i].getId() == id) {
            return true;
        }
    }

    return false;
}

bool Network::isCStationInArrayById(int id) const {
    for (size_t i = 0; i < CStationArray.size(); ++i) {
        if (CStationArray[i].getId() == id) {
            return true;
        }
    }

    return false;
}

Pipe& Network::getPipeById(int id) {
    for (size_t i = 0; i < pipeArray.size(); ++i) {
        if (pipeArray[i].getId() == id) {
            return pipeArray[i];
        }
    }

    std::string message_error { "Ошибка: Труба с ID= " + std::to_string(id) + " не найдена" };
    throw std::out_of_range(message_error);
}


CompressorStation& Network::getCStationById(int id) {
    for (size_t i = 0; i < CStationArray.size(); ++i) {
        if (CStationArray[i].getId() == id) {
            return CStationArray[i];
        }
    }
    std::string message_error { "Ошибка: КС с ID= " + std::to_string(id) + " не найдена" };
    throw std::out_of_range(message_error);

}


const std::vector<Pipe>& Network::getPipeArray() const {
    return pipeArray;
}

const std::vector<CompressorStation>& Network::getCStationArray() const {
    return CStationArray;
}

size_t Network::getPipeArrayLen() const {
    return pipeArray.size();
}

size_t Network::getCStationArrayLen() const {
    return CStationArray.size();
}

/*



Методы с действиями на объектами в сети



*/


void Network::addPipe(int diameter, int length, const std::string& name, bool isRepair) {
    pipeArray.emplace_back(getNextPipeId(), diameter, length, name, isRepair);
}

void Network::addCStation(int numWorkers, int numActiveWorkers, const std::string& name, StationType stationType) {
    CStationArray.emplace_back(getNextCStationId(), numWorkers, numActiveWorkers, name, stationType);
}

bool Network::deletePipe(int id) {
    if (!isPipeInArrayById(id)) {
        return false;
    }

    for (size_t i = 0; i < pipeArray.size(); ++i) {
        if (pipeArray[i].getId() == id) {
            pipeArray.erase(pipeArray.begin() + i);
            return true;
        }
    }

    return false;
}

bool Network::deleteCStation(int id) {
    if (!isCStationInArrayById(id)) {
        return false;
    }

    for (size_t i = 0; i < pipeArray.size(); ++i) {
        if (pipeArray[i].isConnectedToCStation(id)) {
            pipeArray[i].disconnect();
        }
    }

    for (size_t i = 0; i < CStationArray.size(); ++i) {
        if (CStationArray[i].getId() == id) {
            CStationArray.erase(CStationArray.begin() + i);
            return true;
        }
    }

    return false;
}



bool Network::editPipe(int id, int newDiameter, int newLength, const std::string& newName, bool newIsRepair) {
    if (!isPipeInArrayById(id)) {
        return false;
    }

    Pipe& pipe = getPipeById(id);
    pipe.setDiameter(newDiameter);
    pipe.setLength(newLength);
    pipe.setName(newName);
    pipe.setRepair(newIsRepair);

    return true;
}

bool Network::editCStation(int id, int newNumWorkers, int newNumActiveWorkers, const std::string& newName, StationType newType) {
    if (!isCStationInArrayById(id) || newNumActiveWorkers > newNumWorkers || newNumActiveWorkers < 0) {
        return false;
    }

    CompressorStation& station = getCStationById(id);
    station.setNumWorkers(newNumWorkers);
    station.setNumActiveWorkers(newNumActiveWorkers);
    station.setName(newName);
    station.setStationType(newType);

    return true;
}



bool Network::saveToFile() {
    // Получаем время
    const auto timestamp = getTimestamp();

    // Открытие потоков записи
    std::ofstream pipeFile("pipes_" + timestamp + ".csv");
    std::ofstream cStationFile("cstations_" + timestamp + ".csv");

    if (!pipeFile || !cStationFile) {
        return false;
    }

    // Запись труб в файл
    pipeFile << "ID,Diameter,Length,Name,IsRepair,CStationFromId,CStationToId\n";
    for (const auto& pipe : pipeArray) {
        pipeFile << pipe.getId() << ',' << pipe.getDiameter() << ','
                 << pipe.getLength() << ',' << pipe.getName() << ','
                 << pipe.getRepair() << ',' << pipe.getCStationFromId() << ','
                 << pipe.getCStationToId() << '\n';
    }

    // Запись КС в файл
    cStationFile << "ID,NumWorkers,NumActiveWorkers,Name,StationType\n";
    for (const auto& cStation : CStationArray) {
        cStationFile << cStation.getId() << ',' << cStation.getNumWorkers() << ','
                     << cStation.getNumActiveWorkers() << ',' << cStation.getName() << ','
                     << static_cast<int>(cStation.getStationType()) << '\n';
    }

    // Проверка на успех записи
    return pipeFile.good() && cStationFile.good();
}

bool Network::loadFromFile(const std::string& pipePath, const std::string& cStationPath) {

    // Открытие потоков чтения
    std::ifstream pipeFile(pipePath);
    std::ifstream cStationFile(cStationPath);
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
    currentPipeId = loadedNetwork.currentPipeId;
    currentCStationId = loadedNetwork.currentCStationId;

    return true;
}
