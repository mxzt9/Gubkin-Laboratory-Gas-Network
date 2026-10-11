#include <iostream>
#include <string>
#include <format>
#include <algorithm>
#include <limits>

#include "pipe.hpp"
#include "utils.hpp"

std::size_t Pipe::next_id = 0;

Pipe::Pipe(int diameter, int length, const std::string& name, bool repair) : id(next_id++), diameter(diameter), 
length(length), name(name), repair(repair) {}

void Pipe::print_table_header() {
    Output::Message("\n                                     [Трубы]\n");
    Output::Message("──────┬────────────────┬──────────┬──────────┬──────────┬──────────────┬──────────\n");
    Output::Message(std::format("{:<5} │ {:<14} │ {:<8} │ {:<8} │ {:<8} │ {:<12} │ {:<9}\n",
        "ID", "Name", "Diameter", "Length", "Repair", "ID CS From", "ID CS To"));
    Output::Message("──────┼────────────────┼──────────┼──────────┼──────────┼──────────────┼──────────\n");
}


void Pipe::print() const {
    Output::Message(std::format("{:<5} │ {:<14.14} │ {:<8} │ {:<8} │ {:<8} │ {:<12} │ {:<9}\n",
        id,
        name,
        diameter,
        length,
        repair ? "yes" : "no",
        "-",
        "-"
    ));
}

bool Pipe::edit(bool new_repair) {
    repair = new_repair;
    return true;
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

    if (diameter <= 0 || length <= 0 || id == (std::numeric_limits<std::size_t>::max)()) {
        return false;
    }

    next_id = std::max(next_id, id + 1);

    return true;
}
