#include <iostream>
#include <string>
#include <format>
#include <algorithm>
#include <limits>

#include "cstation.hpp"
#include "utils.hpp"

std::size_t CStation::next_id = 0;

CStation::CStation(int unit_amount, int active_unit_amount, const std::string& name, int station_type) : id(next_id++), unit_amount(unit_amount), 
active_unit_amount(active_unit_amount), name(name), station_type(station_type) {}

void CStation::print_table_header() {
    Output::Message("\n                 [Компрессорные станции]\n");
    Output::Message("──────┬────────────────┬───────────┬─────────┬────────────────────────────────────\n");
    Output::Message(std::format("{:<5} │ {:<14} │ {:<9} │ {:<7} │ {:<8}\n",
                                     "ID", "Name", "Units", "Active", "Type"));
    Output::Message("──────┼────────────────┼───────────┼─────────┼────────────────────────────────────\n");
}

void CStation::print() const {
    Output::Message(std::format("{:<5} │ {:<14.14} │ {:<9} │ {:<7} │ {:<8}\n",
        id,
        name,
        unit_amount,
        active_unit_amount,
        station_type
    ));
}

bool CStation::edit(int new_active_unit_amount) {
    if (new_active_unit_amount > unit_amount) {
        return false;
    }

    active_unit_amount = new_active_unit_amount;
    return true;
}

bool CStation::save(std::ostream& file) const {
    file << id << '\n'
         << name << '\n'
         << unit_amount << '\n'
         << active_unit_amount << '\n'
         << station_type << '\n';

    return file.good();
}

bool CStation::load(std::istream& file) {
    if (!(file >> id)) {
        return false;
    }

    file.ignore();

    if (!std::getline(file, name) || !(file >> unit_amount >> active_unit_amount >> station_type)) {
        return false;
    }

    if (unit_amount <= 0 || active_unit_amount < 0 || active_unit_amount > unit_amount || station_type < 0 || station_type > 2 || id == (std::numeric_limits<std::size_t>::max)()) {
        return false;
    }

    next_id = std::max(next_id, id + 1);

    return true;
}
