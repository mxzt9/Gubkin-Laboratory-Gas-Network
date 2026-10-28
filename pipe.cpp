#include <climits>
#include <iostream>
#include <string>
#include <format>
#include <limits>

#include "pipe.hpp"
#include "utils.hpp"

int Pipe::next_id = 1;

Pipe::Pipe(int diameter, int length, const std::string& name, bool repair) : id(next_id++), diameter(diameter), 
length(length), name(name), repair(repair) {}

void Pipe::printTableHeader() {
    Output::Message("\n                                     [Трубы]\n");
    Output::Message("──────┬────────────────┬──────────┬──────────┬──────────┬──────────────┬──────────\n");
    Output::Message(std::format("{:<5} │ {:<14} │ {:<8} │ {:<8} │ {:<8} │ {:<12} │ {:<9}\n",
        "ID", "Name", "Diameter", "Length", "Repair", "ID CS From", "ID CS To"));
    Output::Message("──────┼────────────────┼──────────┼──────────┼──────────┼──────────────┼──────────\n");
}

void Pipe::edit(bool new_repair) {
    repair = new_repair;
}

bool Pipe::save(std::ostream& file) const {
    file << id << '\n'
         << name << '\n'
         << diameter << '\n'
         << length << '\n'
         << (repair ? 1 : 0) << '\n';

    return file.good();
}

bool Pipe::load(std::istream& file) {
    if (!(file >> id)) {
        return false;
    }

    file.ignore();

    if (!std::getline(file, name) || !(file >> diameter >> length >> repair)) {
        return false;
    }

    if (diameter <= 0 || length <= 0 || id < 0 || id == (std::numeric_limits<int>::max)()) {
        return false;
    }

    next_id = next_id > id + 1 ? next_id : id + 1;

    return true;
}

std::ostream& operator<<(std::ostream& stream, const Pipe& pipe) {
    stream << std::format("{:<5} │ {:<14.14} │ {:<8} │ {:<8} │ {:<8} │ {:<12} │ {:<9}\n",
        pipe.id,
        pipe.name,
        pipe.diameter,
        pipe.length,
        pipe.repair ? "yes" : "no",
        "-",
        "-"
    );

    return stream;
}

std::istream& operator>>(std::istream& stream, Pipe& pipe) {
    int diameter = Input::Int("Диаметр (мм)", 1, std::numeric_limits<int>::max(), 500);
    int length = Input::Int("Длина (км)", 1, std::numeric_limits<int>::max(), 100);
    std::string name = Input::String("Название", "Pipe");
    bool repair = Input::Bool("В ремонте?", false);

    pipe = Pipe(diameter, length, name, repair);

    return stream;
}
