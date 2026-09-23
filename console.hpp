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
        "Присоединить трубу",
        "Топологическая сортировка",
        "Найти кратчайший путь между КС"
    };

    // Методы меню просмотра
    const std::vector<std::string> viewAllMenuItems{
        "Поиск труб по названию",
        "Поиск труб по статусу ремонта",
        "Поиск КС по названию",
        "Поиск КС по проценту/числу активных цехов"
    };

    // Очистка консоли
    void clearConsole() const;

    // Чтение из терминала
    std::string read_line(const std::string& prompt, std::string defaultParam = "") const;
    std::string read_valid_name(const std::string& prompt, std::string defaultParam = "") const;
    int read_int(const std::string& prompt, int defaultParam = -1, bool isPositive = false) const;
    bool read_bool(const std::string& prompt, bool defaultParam) const;
    StationType read_station_type(StationType defaultParam) const;
    std::vector<int> read_multiple_int(const std::string& prompt) const;
    std::vector<char> read_comparison(const std::string& prompt) const;

    // Хэндлеры
    void handleAddPipe();
    void handleAddCStation();
    void handlePrintNetwork();
    void handleEditPipe();
    void handleEditCStation();
    void handleDeletePipe();
    void handleDeleteCStation();
    void handleSave();
    void handleLoad();
    void handleConnectPipe();
    void handleTopologicalSort();
    void handleFindShortestPath();

    // Хэндлеры поиска
    void handleSearchPipesByName() const;
    void handleSearchPipesByRepair() const;
    void handleSearchCStationsByName() const;
    void handleSearchCStationsByActive() const;
    
    // Принтеры
    void printMenu() const;
    void printMenuViewAll() const;


public:
    Console(Network& network) : network(network) {};
    void run();
};
