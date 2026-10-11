#pragma once

#include <vector>
#include <string>

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
        //"Присоединить трубу",
        //"Топологическая сортировка",
        //"Найти кратчайший путь между КС"
    };

    // Пункты меню просмотра
    const std::vector<std::string> searchMenuItems{
        "Поиск труб по названию",
        "Поиск труб по статусу ремонта",
        "Поиск КС по названию",
        "Поиск КС по проценту/числу активных цехов"
    };

    // Хендлеры
    void handleCreatePipe();
    void handleCreateCStation();
    void handlePrintNetwork();
    void handleEditPipe();
    void handleEditCStation();
    void handleDeletePipe();
    void handleDeleteCStation();
    void handleSave();
    void handleLoad();

    // Хендлеры поиска
    void handleSearchPipesByName() const;
    void handleSearchPipesByRepair() const;
    void handleSearchCStationsByName() const;
    void handleSearchCStationsByActive() const;

    // Принтеры
    void printMenu() const;
    void printSearchMenu() const;

public:
    Console(Network& network) : network(network) {} 

    // Запуск
    void run();
};
