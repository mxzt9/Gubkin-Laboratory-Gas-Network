#include <string>
#include <cstddef>
#include <climits>
#include <iostream>
#include <format>

#include "console.hpp"
#include "pipe.hpp"
#include "cstation.hpp"
#include "utils.hpp"



void Console::printMenu() const {
    for (std::size_t i = 0; i != mainMenuItems.size(); ++i) {
        Output::Message(std::format("{:<4}{}\n", i + 1, mainMenuItems[i]));
    }
    
    Output::Message(std::format("{:<4}{}\n", "0", "Выход"));
}

void Console::printSearchMenu() const {
    for (std::size_t i = 0; i != searchMenuItems.size(); ++i) {
        Output::Message(std::format("{:<4}{}\n", i + 1, searchMenuItems[i]));
    }
    
    Output::Message(std::format("{:<4}{}\n", "0", "Назад"));
}

void Console::handlePrintNetwork() {
    network.print();
    
    printSearchMenu();

    int action = Input::Int("Выбор", 0, static_cast<int>(searchMenuItems.size()));
    Output::clearConsole();

    switch (action) {
        case 1: {
            handleSearchPipesByName();
            break;
        }
        case 2: {
            handleSearchPipesByRepair();
            break;
        }
        case 3: {
            handleSearchCStationsByName();
            break;
        }
        case 4: {
            handleSearchCStationsByActive();
            break;
        }
        case 0:
            return;
        default: {
            Output::Error("Нет такого пункта меню.\n");
            Input::Pause();
            break;
        }
    }
};

void Console::handleCreatePipe() {
    Pipe pipe;
    std::cin >> pipe;

    if (network.createPipe(pipe)) {
        Output::Success("Труба добавлена.\n");
        return;
    }

    Output::Error("Не удалось добавить трубу.\n");
};

void Console::handleCreateCStation() {
    CStation station;
    std::cin >> station;

    if (network.createCStation(station)) {
        Output::Success("КС добавлена.\n");
        return;
    }

    Output::Error("Не удалось добавить КС.\n");
};

// Поиск по фильтрам 
void Console::handleSearchPipesByName() const {
    std::string name = Input::String("Название трубы");
    const auto result = network.searchPipesByName(name);

    if (result.empty()) {
        Output::Warning("Трубы с таким именем не найдены.\n");
        return;
    }

    Pipe::print_table_header();

    for (const auto id : result) {
        std::cout << network.get_pipe_by_id(id);
    }
}

void Console::handleSearchCStationsByName() const {
    std::string name = Input::String("Название КС");
    const auto result = network.searchCStationsByName(name);

    if (result.empty()) {
        Output::Warning("КС с таким именем не найдены.\n");
        return;
    }

    CStation::print_table_header();

    for (const auto id : result) {
        std::cout << network.get_station_by_id(id);
    }
}


void Console::handleSearchPipesByRepair() const {
    bool repair = Input::Bool("В ремонте?", false);
    const auto result = network.searchPipesByRepair(repair);

    if (result.empty()) {
        Output::Warning("Трубы с таким статусом ремонта не найдены.\n");
        return;
    }

    Pipe::print_table_header();

    for (const auto id : result) {
        std::cout << network.get_pipe_by_id(id);
    }
}

void Console::handleSearchCStationsByActive() const {
    std::string comparison;

    while (true) {
        comparison = Input::String("Знак сравнения (<, <=, =, >=, >)", ">");

        if (comparison == "<" || comparison == "<=" || comparison == "=" || comparison == ">=" || comparison == ">") {
            break;
        }

        Output::Error("Введите <, <=, =, >= или >.\n");
    }
    
    int ratio = Input::Int("Процент активных цехов", 0, 100, 50);
    const auto result = network.searchCStationsByActive(comparison, ratio);

    if (result.empty()) {
        Output::Warning("КС с таким процентом активных цехов не найдены.\n");
        return;
    }

    CStation::print_table_header();

    for (const auto id : result) {
        std::cout << network.get_station_by_id(id);
    }
}

// Редактирование
void Console::handleEditPipe() {
    // Проверка на наличие труб
    if (network.emptyPipes()) {
        Output::Warning("Нет труб для редактирования.\n");
        return;
    }

    const auto ids = Input::Vector<int>("ID труб через пробел");
    const bool new_repair = Input::Bool("В ремонте?", false);

    if (network.editPipe(ids, new_repair)) {
        Output::Success(std::string("Все трубы смогли получить новый статус ремонта: ") + (new_repair ? "yes.\n" : "no.\n"));
        return;
    };

    Output::Error("Не удалось отредактировать трубы.\n");
}


void Console::handleEditCStation() {
    // Проверка на наличие КС
    if (network.emptyCStations()) {
        Output::Warning("Нет КС для редактирования.\n");
        return;
    }

    const auto ids = Input::Vector<int>("ID КС через пробел");
    const int new_active_unit_amount = Input::Int("Количество цехов в работе", 0, std::numeric_limits<int>::max(), 0);
  
    if (network.editCStation(ids, new_active_unit_amount)) {
        Output::Success("Всем КС было успешно выставленно кол-во активных цехов: " + std::to_string(new_active_unit_amount) + ".\n");
        return;
    };

    Output::Error("Не удалось отредактировать КС, скорее всего некорректно введено число активных цехов.\n");
}

void Console::handleDeletePipe() {
    // Проверка на наличие труб
    if (network.emptyPipes()) {
        Output::Warning("Нет труб для удаления.\n");
        return;
    }

    const auto ids = Input::Vector<int>("ID труб через пробел");

    if (network.deletePipe(ids)) {
        Output::Success("Все трубы были успешно удалены.\n");
        return;
    }

    Output::Error("Не удалось удалить трубы.\n");
}

void Console::handleDeleteCStation() {
    // Проверка на наличие КС
    if (network.emptyCStations()) {
        Output::Warning("Нет КС для удаления.\n");
        return;
    }

    const auto ids = Input::Vector<int>("ID КС через пробел");

    if (network.deleteCStation(ids)) {
        Output::Success("Все КС были успешно удалены.\n");
        return;
    }

    Output::Error("Не удалось удалить КС.\n");
}


void Console::handleSave() {
    while (true) {
        const auto path = Input::String("Путь к файлу сети", "data/network_" + getTimestamp() + ".txt");

        if (network.save(path)) {
            Output::Success("Трубы и КС сохранены в " + path + "\n");
            return;
        }

        Output::Error("Не удалось сохранить сеть. Проверьте путь и доступ к файлу.\n");
    }
}

void Console::handleLoad() {
    while (true) {
        const auto path = Input::String("Путь к файлу сети");

        if (network.load(path)) {
            Output::Success("Текущая сеть заменена данными из " + path + "\n");
            return;
        }

        Output::Error("Проверьте путь и формат файла. Текущая сеть не изменена.\n");
    }
}

void Console::run() {
    bool running = true;

    while (running) {
        Output::clearConsole();
        printMenu();

        // Чтение команды и запуск выбранного действия
        try {
            Output::Message("\nExit - в любом действии, чтобы вернуться в меню.\n");
            int action = Input::Int("Выбор", 0, static_cast<int>(mainMenuItems.size()));
            Output::clearConsole();

            switch (action) {
                case 1:
                    handleCreatePipe();
                    break;

                case 2:
                    handleCreateCStation();
                    break;

                case 3:
                    handlePrintNetwork();
                    break;

                case 4:
                    handleEditPipe();
                    break;

                case 5:
                    handleEditCStation();
                    break;

                case 6:
                    handleDeletePipe();
                    break;

                case 7:
                    handleDeleteCStation();
                    break;

                case 8:
                    handleSave();
                    break;

                case 9:
                    handleLoad();
                    break;

                case 10:
                    //handleConnectPipe();
                    break;

                case 11:
                    //handleTopologicalSort();
                    break;

                case 12:
                    //handleFindShortestPath();
                    break;

                case 0:
                    running = false;
                    break;

                default:
                    Output::Error("Нет такого пункта меню.\n");
                    break;
            }
            
            if (running) {
                Input::Pause();
            }
        } catch (Input::Command) {
            // Команда exit возвращает в меню, закрытый поток завершает программу
            if (!std::cin) {
                running = false;
            }
        }
    }
}
