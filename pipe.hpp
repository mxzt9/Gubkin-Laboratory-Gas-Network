#pragma once

#include <string>
#include <iosfwd>

// Класс трубы
class Pipe {
private:
    int id {}, diameter {}, length {};
    std::string name {};
    bool repair {};

    int cStationFromId { -1 };
    int cStationToId { -1 };

public:
    Pipe(int id, int diameter, int length, const std::string& name, bool repair);

    // Геттеры трубы
    int getId() const;
    int getDiameter() const;
    int getLength() const;
    const std::string& getName() const;
    bool getRepair() const;
    bool isFree() const;

    int getCStationFromId() const;
    int getCStationToId() const;

    // Сеттеры трубы
    void setRepair(bool newRepair);

    void connect(int fromId, int toId);
    void disconnect();

    static void printPipeTableHeader();
    void printPipe() const;
    bool save(std::ostream& file) const;
};
