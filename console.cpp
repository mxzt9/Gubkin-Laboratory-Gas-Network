#include <iostream>
#include <format>

#include "console.hpp"
#include "pipe.hpp"
#include "cstation.hpp"
#include "utils.hpp"



void Console::printMenu() const {
    for (std::size_t i = 0; i != mainMenuItems.size(); ++i) {
        std::cout << std::format("{:<4}{}\n", i + 1, mainMenuItems[i]);
    }
    
    Output::Message(std::format("{:<4}{}\n", "0", "Выход"));
}

void Console::printSearchMenu() const {
    for (std::size_t i = 0; i != searchMenuItems.size(); ++i) {
        std::cout << std::format("{:<4}{}\n", i + 1, searchMenuItems[i]);
    }
    
    Output::Message(std::format("{:<4}{}\n", "0", "Назад"));
}

void Console::handlePrintNetwork() {
    network.print();
    
    printSearchMenu();

    switch (Input::Int("Выбор", -1, 0, 4, false)) {
        case 1: {
            Output::clearConsole();
            handleSearchPipesByName();
            break;
        }
        case 2: {
            Output::clearConsole();
            handleSearchPipesByRepair();
            break;
        }
        case 3: {
            Output::clearConsole();
            handleSearchCStationsByName();
            break;
        }
        case 4: {
            Output::clearConsole();
            handleSearchCStationsByActive();
            break;
        }
        case 0:
            return;
        default: {
            Output::Error("Нет такого пункта меню.\n");
            Input::String("Нажмите Enter для продолжения", "");;
            break;
        }
    }
};

void Console::handleCreatePipe() {
    int diameter = Input::Int("Диаметр (мм)", 500, 1);
    int length = Input::Int("Длина (км)", 100, 1);
    std::string name = Input::String("Название", "Pipe");
    bool repair = Input::Bool("В ремонте?", false);
    int amount = Input::Int("Сколько труб добавить (ед)", 1, 1);

    if (network.createPipe(diameter, length, name, repair, amount)) {
        Output::Success(std::format("Было добавлено труб: {}.\n", amount));
        return;
    }

    Output::Error(std::format("Не удалось добавить труб: {}.\n", amount));
};

void Console::handleCreateCStation() {
    int unit_amount = Input::Int("Количество цехов (ед)", 10, 1);
    int active_unit_amount = Input::Int("Количество цехов в работе (ед)", unit_amount, 1, unit_amount);
    std::string name = Input::String("Название", "CStation");
    int station_type = Input::Int("Класс станции", 0, 0, 2);
    int amount = Input::Int("Сколько КС добавить", 1, 1);

    if (network.createCStation(unit_amount, active_unit_amount, name, station_type, amount)) {
        Output::Success(std::format("Было добавлено КС: {}.\n", amount));
        return;
    }

    Output::Error(std::format("Не удалось добавить КС: {}.\n", amount));
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

    for (const auto pipe : result) {
        pipe.print();
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

    for (const auto station : result) {
        station.print();
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

    for (const auto pipe : result) {
        pipe.print();
    }
}

void Console::handleSearchCStationsByActive() const {
    std::string comparison = Input::String("Знак сравнения (<, =, >)", ">");
    int ratio = Input::Int("Процент активных цехов", 50, 0, 100);
    const auto result = network.searchCStationsByActive(comparison, ratio);

    if (result.empty()) {
        Output::Warning("КС с таким именем не найдены.\n");
        return;
    }

    CStation::print_table_header();

    for (const auto station : result) {
        station.print();
    }
}

// Редактирование
void Console::handleEditPipe() {
    // Проверка на наличие труб
    if (network.emptyPipes()) {
        Output::Warning("Нет труб для редактирования.\n");
        return;
    }

    const auto ids = Input::Vector<std::size_t>("ID труб через пробел");
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

    const auto ids = Input::Vector<std::size_t>("ID КС через пробел");
    const int new_active_unit_amount = Input::Int("Количество цехов в работе", 0, 0);
  
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

    const auto ids = Input::Vector<std::size_t>("ID труб через пробел");

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

    const auto ids = Input::Vector<std::size_t>("ID КС через пробел");

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
        printMenu();

        // Чтение команды и запуск выбранного действия
        try {
            switch (Input::Int("\nExit - в любом действии, чтобы вернуться в меню.\nВыбор", -1, 0, 12, false)) {
                case 1:
                    Output::clearConsole();
                    handleCreatePipe();
                    Input::String("Нажмите Enter для продолжения", "");
                    break;

                case 2:
                    Output::clearConsole();
                    handleCreateCStation();
                    Input::String("Нажмите Enter для продолжения", "");
                    break;

                case 3:
                    Output::clearConsole();
                    handlePrintNetwork();
                    Input::String("Нажмите Enter для продолжения", "");
                    break;

                case 4:
                    Output::clearConsole();
                    handleEditPipe();
                    Input::String("Нажмите Enter для продолжения", "");
                    break;

                case 5:
                    Output::clearConsole();
                    handleEditCStation();
                    Input::String("Нажмите Enter для продолжения", "");
                    break;

                case 6:
                    Output::clearConsole();
                    handleDeletePipe();
                    Input::String("Нажмите Enter для продолжения", "");
                    break;

                case 7:
                    Output::clearConsole();
                    handleDeleteCStation();
                    Input::String("Нажмите Enter для продолжения", "");
                    break;

                case 8:
                    Output::clearConsole();
                    handleSave();
                    Input::String("Нажмите Enter для продолжения", "");
                    break;

                case 9:
                    Output::clearConsole();
                    handleLoad();
                    Input::String("Нажмите Enter для продолжения", "");
                    break;

                case 10:
                    Output::clearConsole();
                    //handleConnectPipe();
                    break;

                case 11:
                    Output::clearConsole();
                    //handleTopologicalSort();
                    break;

                case 12:
                    Output::clearConsole();
                    //handleFindShortestPath();
                    break;

                case 0:
                    running = false;
                    break;

                default:
                    Output::Error("Нет такого пункта меню.\n");
                    Input::String("Нажмите Enter для продолжения", "");
                    break;
            }
        } catch (Input::Command) {
            // Команда exit возвращает в меню, закрытый поток завершает программу
            if (!std::cin) {
                running = false;
            }
        }
    }
}
