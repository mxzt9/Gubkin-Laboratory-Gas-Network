#pragma once

#include "network.hpp"

class Console {
private:
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

    // Пункты меню просмотра
    const std::vector<std::string> viewAllMenuItems{
        "Поиск труб по названию",
        "Поиск труб по статусу ремонта",
        "Поиск КС по названию",
        "Поиск КС по проценту/числу активных цехов"
    };

    // Сообщения и ожидание продолжения
    void waitForEnter() const;

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
