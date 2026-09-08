#pragma once

#include "network.hpp"
#include "log.hpp"

class Console {
private:
    enum class InputCommand {
        Exit
    };

    Network& network;
    Logger& logger;

    void logAction(const std::string& message) const;

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
        "Присоединить трубу"
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

    // Чтение из термнила
    std::string readLine(const std::string& prompt, std::string defaultParam = "") const;
    std::string readValidName(const std::string& prompt, std::string defaultParam = "") const;
    int readInt(const std::string& prompt, int defaultParam = -1, bool isPositive = false) const;
    bool readBool(const std::string& prompt, bool defaultParam) const;
    StationType readStationType(StationType defaultParam) const;
    std::vector<int> readMultipleChoice(const std::string& prompt) const;
    std::vector<char> readActiveSearch(const std::string& prompt) const;

    // Хэндлеры
    void handleAddPipe();
    void handleAddCS();
    void handleViewAll();
    void handleEditPipe();
    void handleEditCStation();
    void handleDeletePipe();
    void handleDeleteCS();
    void handleSave();
    void handleLoad();
    void handleConnectPipe();

    // Хэндлеры поиска
    void handleSearchPipesByName() const;
    void handleSearchPipesByRepair() const;
    void handleSearchCStationsByName() const;
    void handleSearchCStationsByActive() const;
    
    // Принтеры
    void printMenu() const;
    void printMenuViewAll() const;

    void printPipeTableHeader() const;
    void printCStationTableHeader() const;
    void printPipe(int id, const Pipe& pipe) const;
    void printCS(int id, const CompressorStation& CStation) const;

public:
    Console(Network& network, Logger& logger) : network(network), logger(logger) {};
    void run();
};
