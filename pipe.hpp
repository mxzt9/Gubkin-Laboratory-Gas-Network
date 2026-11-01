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

        int CStationOut_id_ { -1 }, CStationIn_id_ { -1 };
    
    public:
        Pipe() = default; // Пустой объект для загрузки, без выдачи нового ID.
        Pipe(int diameter, int length, const std::string& name, bool repair);

        static void reset_next_id() { next_id = 1; }

        // Геттеры
        int  get_id() const { return id; }
        const std::string& get_name() const { return name; }
        int  get_diameter() const { return diameter; }
        int  get_length() const { return length; }
        bool get_repair() const { return repair; }
        
        int get_CStationOut_id() const { return CStationOut_id_; }
        int get_CStationIn_id() const { return CStationIn_id_; }

        // Принтеры 
        static void printTableHeader();
        friend std::ostream& operator<<(std::ostream& stream, const Pipe& pipe);
        friend std::istream& operator>>(std::istream& stream, Pipe& pipe);

        void edit(bool new_repair);

        bool is_free() const { return CStationOut_id_ == -1 && CStationIn_id_ == -1; }

        void connect(int CStationOut_id, int CStationIn_id) { CStationOut_id_ = CStationOut_id; CStationIn_id_ = CStationIn_id; }
        void disconnect() { CStationOut_id_ = -1; CStationIn_id_ = -1; }

        bool save(std::ostream& file) const;
        bool load(std::istream& file);

};
