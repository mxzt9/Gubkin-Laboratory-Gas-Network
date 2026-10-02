#include <format>
#include <iostream>
#include <fstream>

#include "compressStation.hpp"
#include "utils.hpp"

CompressorStation::CompressorStation(int id, int numWorkshops, int numActiveWorkshops, const std::string& name, StationType stationType) : id(id), numWorkshops(numWorkshops), numActiveWorkshops(numActiveWorkshops), name(name), stationType(stationType) {}

int CompressorStation::getNumWorkshops() const { return numWorkshops; }
int CompressorStation::getNumActiveWorkshops() const { return numActiveWorkshops; }
const std::string& CompressorStation::getName() const { return name; }
StationType CompressorStation::getStationType() const { return stationType; }

bool CompressorStation::setNumActiveWorkshops(int newNumActiveWorkshops) {
    if (newNumActiveWorkshops < 0 || newNumActiveWorkshops > numWorkshops) {
        return false;
    }

    numActiveWorkshops = newNumActiveWorkshops;
    return true;
}

void CompressorStation::printCStationTableHeader() {
    Output::Message("\n                 [Компрессорные станции]\n");
    Output::Message("──────┬────────────────┬───────────┬─────────┬────────────────────────────────────\n");
    Output::Message(std::format("{:<5} │ {:<14} │ {:<9} │ {:<7} │ {:<8}\n",
                                     "ID", "Name", "Workshops", "Active", "Type"));
    Output::Message("──────┼────────────────┼───────────┼─────────┼────────────────────────────────────\n");
}

void CompressorStation::printCStation() const {
    std::string type;

    if (getStationType() == StationType::Light) { type = "Light"; }
    else if (getStationType() == StationType::Medium) { type = "Medium"; }
    else { type = "Heavy"; }

    Output::Message(std::format("{:<5} │ {:<14.14} │ {:<9} │ {:<7} │ {:<8}\n",
        id,
        name,
        numWorkshops,
        numActiveWorkshops,
        type
    ));
}

bool CompressorStation::save(std::ostream& file) const {
    file << id << '\n'
         << name << '\n'
         << numWorkshops << '\n'
         << numActiveWorkshops << '\n'
         << static_cast<int>(stationType) << '\n';

    return file.good();
}