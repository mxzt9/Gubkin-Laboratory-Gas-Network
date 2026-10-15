#pragma once

#include <string>
#include <iosfwd>



class Pipe {
    private:
        static int next_id;
        
        int id {};
        int diameter {}, length {};
        std::string name {};
        bool repair {};
    
    public:
        Pipe() = default; // Пустой объект для загрузки, без выдачи нового ID.
        Pipe(int diameter, int length, const std::string& name, bool repair);

        // Геттеры
        int get_id() const { return id; }
        const std::string& get_name() const { return name; }
        int get_diameter() const { return diameter; }
        int get_length() const { return length; }
        bool get_repair() const { return repair; }
        
        // Принтеры 
        static void print_table_header();
        friend std::ostream& operator<<(std::ostream& stream, const Pipe& pipe);
        friend std::istream& operator>>(std::istream& stream, Pipe& pipe);

        void edit(bool new_repair);
        bool save(std::ostream& file) const;
        bool load(std::istream& file);
};
