#pragma once

#include <string>
#include <iosfwd>

// Класс КС
class CStation {
private:
    static int next_id;

    static constexpr int max_unit_amount { 1000 };

    int id {};
    int unit_amount {}, active_unit_amount {};
    std::string name {};
    int station_type {};

public:
    CStation() = default; // Пустой объект для загрузки, без выдачи нового ID.
    CStation(int unit_amount, int active_unit_amount, const std::string& name, int station_type);

    static void reset_next_id() { next_id = 1; }

    // Геттеры
    int get_id() const { return id; }
    const std::string& get_name() const { return name; }
    int get_unit_amount() const { return unit_amount; }
    int get_active_unit_amount() const { return active_unit_amount; }
    int get_station_type() const { return station_type; }
    int get_unit_ratio() const { return unit_amount != 0 ? active_unit_amount * 100 / unit_amount : 0; }
    
    // Принтеры
    static void printTableHeader();
    friend std::ostream& operator<<(std::ostream& stream, const CStation& station);
    friend std::istream& operator>>(std::istream& stream, CStation& station);

    bool edit(int new_active_unit_amount);
    bool save(std::ostream& file) const;
    bool load(std::istream& file);
        
};
