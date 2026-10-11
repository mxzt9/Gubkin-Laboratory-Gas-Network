#pragma once

#include <string>
#include <cstddef>
#include <iosfwd>


class Pipe {
    private:
        static std::size_t next_id;

        std::size_t id {};
        int diameter {}, length {};
        std::string name {};
        bool repair {};
    
    public:
        Pipe() = default; // Пустой объект для загрузки, без выдачи нового ID.
        Pipe(int diameter, int length, const std::string& name, bool repair);

        // Геттеры
        std::size_t get_id() const { return id; }
        std::string get_name() const { return name; }
        bool get_repair() const { return repair; }
        
        // Принтеры 
        static void print_table_header();
        void print() const;

        bool edit(bool new_repair);
        bool save(std::ostream& file) const;
        bool load(std::istream& file);
};
