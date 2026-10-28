#pragma once

#include <set>
#include <vector>
#include <string>

#include "network.hpp"

class Console {
private:
    Network& network;

    // Пункты главного меню
    const std::vector<std::string> mainMenuItems {
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
    const std::vector<std::string> searchMenuItems {
        "Поиск труб по названию",
        "Поиск труб по статусу ремонта",
        "Поиск КС по названию",
        "Поиск КС по проценту/числу активных цехов"
    };

    // Пункты меню действий с найденным подмножеством
    const std::vector<std::string> searchInSubsetMenuItems {
        "Редактировать все найденные",
        "Удалить все найденные",
        "Редактировать выбранные ID",
        "Удалить выбранные ID"
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
    void handlePipeSearchResult(const std::set<int>& ids);
    void handleCStationSearchResult(const std::set<int>& ids);
    void handleSearchPipesByName();
    void handleSearchPipesByRepair();
    void handleSearchCStationsByName();
    void handleSearchCStationsByActive();

    // Принтеры
    void printMenu() const;
    void printNetworkMenu() const;
    void printSearchMenu() const;

public:
    Console(Network& network) : network(network) {} 

    // Запуск
    void run();
};
