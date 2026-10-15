#include <climits>
#include <iostream>
#include <string>
#include <format>
#include <limits>

#include "cstation.hpp"
#include "utils.hpp"

int CStation::next_id = 0;

CStation::CStation(int unit_amount, int active_unit_amount, const std::string& name, int station_type) : id(next_id++), unit_amount(unit_amount), 
active_unit_amount(active_unit_amount), name(name), station_type(station_type) {}

void CStation::print_table_header() {
    Output::Message("\n                 [Компрессорные станции]\n");
    Output::Message("──────┬────────────────┬───────────┬─────────┬────────────────────────────────────\n");
    Output::Message(std::format("{:<5} │ {:<14} │ {:<9} │ {:<7} │ {:<8}\n",
                                     "ID", "Name", "Units", "Active", "Type"));
    Output::Message("──────┼────────────────┼───────────┼─────────┼────────────────────────────────────\n");
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

    if (unit_amount <= 0 || active_unit_amount < 0 || active_unit_amount > unit_amount || station_type < 0 || station_type > 2 || id < 0 || id == (std::numeric_limits<int>::max)()) {
        return false;
    }

    next_id = next_id > id + 1 ? next_id : id + 1;

    return true;
}

std::ostream& operator<<(std::ostream& stream, const CStation& station) {
    stream << std::format("{:<5} │ {:<14.14} │ {:<9} │ {:<7} │ {:<8}\n",
        station.id,
        station.name,
        station.unit_amount,
        station.active_unit_amount,
        station.station_type
    );

    return stream; 
}
std::istream& operator>>(std::istream& stream, CStation& station) {
    int unit_amount = Input::Int("Количество цехов (ед)", 1, std::numeric_limits<int>::max(), 10, stream);
    int active_unit_amount = Input::Int("Количество цехов в работе (ед)", 0, unit_amount, unit_amount, stream);
    std::string name = Input::String("Название", "CStation", stream);
    int station_type = Input::Int("Класс станции", 0, 2, 0, stream);

    station = CStation(unit_amount, active_unit_amount, name, station_type);

    return stream;
}
