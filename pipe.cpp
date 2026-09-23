#include <format>
#include <iostream>

#include "pipe.hpp"

Pipe::Pipe(int id, int diameter, int length, const std::string& name, bool repair) : id(id), diameter(diameter), length(length), name(name), repair(repair) {}

int Pipe::getId() const { return id; }
int Pipe::getDiameter() const { return diameter; }
int Pipe::getLength() const { return length; }
const std::string& Pipe::getName() const { return name; }
bool Pipe::getRepair() const { return repair; }
bool Pipe::isFree() const { return cStationFromId == -1 && cStationToId == -1; }

int Pipe::getCStationFromId() const { return cStationFromId; }
int Pipe::getCStationToId() const { return cStationToId; }

void Pipe::setRepair(bool newRepair) { repair = newRepair; }

void Pipe::connect(int fromId, int toId) {
    cStationFromId = fromId;
    cStationToId = toId;
}

void Pipe::disconnect() {
    cStationFromId = -1;
    cStationToId = -1;
}

void Pipe::printPipeTableHeader() {
    std::cout << "\n                                    [Трубы]                                       ";
    std::cout << "\n───────┬────────────┬──────────┬──────────┬──────────┬──────────────┬───────────\n";
    std::cout << std::format("{:<6} │ {:<10.10} │ {:<8} │ {:<8} │ {:<8} │ {:<12} │ {}\n",
        "ID", "Name", "Diameter", "Length", "Repair", "ID CS From", "ID CS To");
    std::cout <<   "───────┼────────────┼──────────┼──────────┼──────────┼──────────────┼───────────\n";
    
}

void Pipe::printPipe() const {
    std::cout << std::format("{:<6} │ {:<10.10} │ {:<8} │ {:<8} │ {:<8} │ {:<12} │ {}\n",
        id,
        name,
        diameter,
        length,
        repair ? "yes" : "no",
        cStationFromId == -1 ? "None" : std::to_string(cStationFromId),
        cStationToId == -1 ? "None" : std::to_string(cStationToId)
    );
}

bool Pipe::save(std::ostream& file) const {
    file << id << '\n'
         << name << '\n'
         << diameter << '\n' 
         << length << '\n'
         << repair << '\n' 
         << cStationFromId << '\n'
         << cStationToId << '\n';
        
    return file.good();
}
