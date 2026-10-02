#include <format>
#include <iostream>

#include "pipe.hpp"
#include "utils.hpp"

Pipe::Pipe(int id, int diameter, int length, const std::string& name, bool repair) : id(id), diameter(diameter), length(length), name(name), repair(repair) {}

int Pipe::getId() const { return id; }
int Pipe::getDiameter() const { return diameter; }
int Pipe::getLength() const { return length; }
const std::string& Pipe::getName() const { return name; }
bool Pipe::getRepair() const { return repair; }
bool Pipe::isFree() const { return CStationFromId == -1 && CStationToId == -1; }

int Pipe::getCStationFromId() const { return CStationFromId; }
int Pipe::getCStationToId() const { return CStationToId; }

void Pipe::setRepair(bool newRepair) { repair = newRepair; }

void Pipe::connect(int fromId, int toId) {
    CStationFromId = fromId;
    CStationToId = toId;
}

void Pipe::disconnect() {
    CStationFromId = -1;
    CStationToId = -1;
}

void Pipe::printPipeTableHeader() {
    Output::Message("\n                                     [Трубы]\n");
    Output::Message("──────┬────────────────┬──────────┬──────────┬──────────┬──────────────┬──────────\n");
    Output::Message(std::format("{:<5} │ {:<14} │ {:<8} │ {:<8} │ {:<8} │ {:<12} │ {:<9}\n",
        "ID", "Name", "Diameter", "Length", "Repair", "ID CS From", "ID CS To"));
    Output::Message("──────┼────────────────┼──────────┼──────────┼──────────┼──────────────┼──────────\n");
}

void Pipe::printPipe() const {
    Output::Message(std::format("{:<5} │ {:<14.14} │ {:<8} │ {:<8} │ {:<8} │ {:<12} │ {:<9}\n",
        id,
        name,
        diameter,
        length,
        repair ? "yes" : "no",
        CStationFromId == -1 ? "None" : std::to_string(CStationFromId),
        CStationToId == -1 ? "None" : std::to_string(CStationToId)
    ));
}

bool Pipe::save(std::ostream& file) const {
    file << id << '\n'
         << name << '\n'
         << diameter << '\n' 
         << length << '\n'
         << repair << '\n' 
         << CStationFromId << '\n'
         << CStationToId << '\n';
        
    return file.good();
}
