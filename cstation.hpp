#pragma once

#include <string>
#include <cstddef>
#include <iosfwd>

// Класс КС
class CStation {
private:
    static std::size_t next_id;

    std::size_t id {};
    int unit_amount {}, active_unit_amount {};
    std::string name {};
    int station_type {};

public:
    CStation() = default; // Пустой объект для загрузки, без выдачи нового ID.
    CStation(int unit_amount, int active_unit_amount, const std::string& name, int station_type);

    // Геттеры
    std::size_t get_id() const { return id; }
    std::string get_name() const { return name; }
    int get_unit_amount() const { return unit_amount; }
    int get_unit_ratio() const { return active_unit_amount * 100 / unit_amount; }
    
    // Принтеры
    static void print_table_header();
    void print() const;

    bool edit(int new_active_unit_amount);
    bool save(std::ostream& file) const;
    bool load(std::istream& file);
        
};
