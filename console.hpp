#pragma once

#include "network.hpp"

class Console {
private:
    enum class InputCommand {
        Exit
    };

    Network& network;

    // Пункты главного меню
    const std::vector<std::string> mainMenuItems{
        "Добавить трубу",
        "Добавить КС",
        "Просмотр",
        "Редактировать трубу",
        "Редактировать КС",
        "Удалить трубу",
        "Удалить КС",
        "Сохранить",
        "Загрузить",
    };

    // Очистка консоли
    void clearConsole() const;

    // Чтение из термнила
    std::string readLine(const std::string& prompt, std::string defaultParam = "") const;
    std::string readValidName(const std::string& prompt, std::string defaultParam = "") const;
    int readInt(const std::string& prompt, int defaultParam = -1, bool isPositive = false) const;
    bool readBool(const std::string& prompt, bool defaultParam) const;
    StationType readStationType(StationType defaultParam) const;
    std::vector<int> readMultipleChoice(const std::string& prompt) const;

    // Хэндлеры
    void handleAddPipe();
    void handleAddCS();
    void handleViewAll();
    void handleEditPipe();
    void handleEditCStation();
    void handleDeletePipe();
    void handleDeleteCS();
    //void handleSave();
    //void handleLoad();
    
    // Принтеры
    void printMenu() const;
    void printPipeTableHeader() const;
    void printCSTableHeader() const;
    void printPipe(const Pipe& pipe) const;
    void printCS(const CompressorStation& station) const;

public:
    Console(Network& network) : network(network) {};
    void run();
};