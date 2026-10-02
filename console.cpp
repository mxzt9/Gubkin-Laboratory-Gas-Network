#include <iostream>
#include <vector>
#include <format>

#include "console.hpp"
#include "utils.hpp"

void Console::waitForEnter() const {
    Input::String("\nНажмите Enter, чтобы продолжить");
}


// Добавление
void Console::handleAddPipe() {
    // Чтение значений из консоли
    const int diameter = Input::Int("Диаметр (мм)", 500, 1);
    const int length = Input::Int("Длина (км)", 100, 1);
    const std::string name = Input::String("Название", "Pipe_");
    const bool repair = Input::Bool("В ремонте?", false);
    const int amount = Input::Int("Сколько труб добавить", 1, 1);


    // Добавление труб
    const std::size_t added = network.addPipe(diameter, length, name, repair, amount);
    Output::Info("Добавлено труб: " + std::to_string(added) + " из " + std::to_string(amount) + "\n");
 
    waitForEnter();
}

void Console::handleAddCStation() {
    // Чтение значений из консоли
    const int numWorkshops = Input::Int("Количество цехов", 10, 1);
    const int numActiveWorkshops = Input::Int("Количество цехов в работе", numWorkshops, 0, numWorkshops);

    const std::string name = Input::String("Название", "CStation_");

    const StationType type = static_cast<StationType>(Input::Int("Класс станции (0 - Light, 1 - Medium, 2 - Heavy)", static_cast<int>(StationType::Light), 0, 2));
    const int amount = Input::Int("Сколько КС добавить", 1, 1);


    // Добавление КС
    const std::size_t added =  network.addCStation(numWorkshops, numActiveWorkshops, name, type, amount);
    Output::Info("Добавлено КС: " + std::to_string(added) + " из " + std::to_string(amount) + "\n");
    
    waitForEnter();
}

// Принт меню
void Console::handlePrintNetwork() {
    // Вывод труб и КС
    network.printNetwork();

    // Выбор фильтра для поиска
    printMenuViewAll();
    switch (Input::Int("Выбор")) {
        case 1: {
            Output::clearConsole();
            handleSearchPipesByName();
            waitForEnter();
            break;
        }
        case 2: {
            Output::clearConsole();
            handleSearchPipesByRepair();
            waitForEnter();
            break;
        }
        case 3: {
            Output::clearConsole();
            handleSearchCStationsByName();
            waitForEnter();
            break;
        }
        case 4: {
            Output::clearConsole();
            handleSearchCStationsByActive();
            waitForEnter();
            break;
        }
        case 0:
            return;
        default: {
            Output::Error("Нет такого пункта меню.\n");
            waitForEnter();
            break;
        }
    }
}


// Редактирование
void Console::handleEditPipe() {
    // Проверка на наличие труб
    if (network.getPipeMap().empty()) {
        Output::Warning("Нет труб для редактирования.\n");
        waitForEnter();
        return;
    }

    const std::vector<int> ids = Input::MultipleInt("ID труб через пробел");
    const bool newRepairStatus = Input::Bool("В ремонте?", false);
    std::size_t edited = network.editPipe(ids, newRepairStatus);

    Output::Info("Обработано труб: " + std::to_string(edited) + " из " + std::to_string(ids.size()) + "\n");
    waitForEnter();
}

void Console::handleEditCStation() {
    // Проверка на наличие КС
    if (network.getCStationMap().empty()) {
        Output::Warning("Нет КС для редактирования.\n");
        waitForEnter();
        return;
    }

    const std::vector<int> ids = Input::MultipleInt("ID КС через пробел");
    const int newNumActiveWorkshops = Input::Int("Количество цехов в работе", 0, 0);
    std::size_t edited = network.editCStation(ids, newNumActiveWorkshops);

    Output::Info("Обработано КС: " + std::to_string(edited) + " из " + std::to_string(ids.size()) + "\n");
    waitForEnter();
}

// Удаление
void Console::handleDeletePipe() {
    // Проверка на наличие труб
    if (network.getPipeMap().empty()) {
        Output::Warning("Нет труб для удаления.\n");
        waitForEnter();
        return;
    }
    
    // Чтение ID труб для удаления
    const auto ids = Input::MultipleInt("ID труб для удаления через пробел");
    const std::size_t deleted = network.deletePipe(ids);
    Output::Info("Удалено труб: " + std::to_string(deleted) + " из " + std::to_string(ids.size()) + "\n");

    waitForEnter();
}

void Console::handleDeleteCStation() {
    // Проверка на наличие КС
    if (network.getCStationMap().empty()) {
        Output::Warning("Нет КС для удаления.\n");
        waitForEnter();
        return;
    }
    
    // Чтение ID КС для удаления
    const auto ids = Input::MultipleInt("ID КС для удаления через пробел");
    const std::size_t deleted = network.deleteCStation(ids);
    Output::Info("Удалено КС: " + std::to_string(deleted) + " из " + std::to_string(ids.size()) + "\n");

    waitForEnter();
}

// Работа с файлами
void Console::handleSave() {
    Output::clearConsole();

    while (true) {
        // Чтение пути с именем файла по умолчанию
        const std::string path = Input::String("Путь к файлу сети", "data/network_" + getTimestamp() + ".txt");

        // Сохранение сети, при ошибке повторяем ввод пути
        if (network.saveToFile(path)) {
            Output::Success("Трубы и КС сохранены в " + path + "\n");
            break;
        }

        Output::Error("Не удалось сохранить сеть. Проверьте путь и доступ к файлу.\n");
    }

    waitForEnter();
}

void Console::handleLoad() {
    Output::clearConsole();

    while (true) {
        // Чтение пути к файлу сети
        const std::string path = Input::String("Путь к файлу сети");

        // Замена текущей сети данными из файла при успешной загрузке
        if (network.loadFromFile(path)) {
            Output::Success("Текущая сеть заменена данными из " + path + "\n");
            break;
        }

        Output::Error("Проверьте путь и формат файла. Текущая сеть не изменена.\n");
    }

    waitForEnter();
}

// Присоединение
void Console::handleConnectPipe() {
    Output::clearConsole();

    // Чтение диаметра и поиск свободной трубы
    const int targetDiameter = Input::Int("Диаметр трубы (мм)", 500, 1);
    int selectedPipeId = -1;

    while (selectedPipeId == -1) {
        selectedPipeId = network.findFreePipe(targetDiameter);

        if (selectedPipeId != -1) {
            break;
        }

        // Если подходящей трубы нет, предлагаем создать новую
        Output::Warning("Свободная труба нужного диаметра не найдена.\n");
        if (!Input::Bool("Создать и присоединить?", false)) {
            waitForEnter();
            return;
        }

        const std::string defaultName = "Pipe_" + std::to_string(network.getNextPipeId());
        const int length = Input::Int("Длина (км)", 100, 1);
        const std::string name = Input::String("Название", defaultName);

        if (!network.addPipe(targetDiameter, length, name, false)) {
            Output::Error("Не удалось создать трубу.\n");
            waitForEnter();
            return;
        }

        // После создания повторяем поиск по диаметру.
    }

    // Чтение ID станций начала и конца трубы
    const int CStationIdFrom = Input::Int("ID КС начала трубы");
    const int CStationIdTo = Input::Int("ID КС конца трубы");

    // Проверка на наличие таких Id
    if (!network.getCStationMap().contains(CStationIdFrom) || !network.getCStationMap().contains(CStationIdTo)) {
        Output::Error("Одна или обе компрессорные станции не найдены.\n");

        waitForEnter();
        return;
    }

    // Проверка на то, что Id КС не совпадают или равны -1
    if (CStationIdFrom == CStationIdTo || CStationIdFrom == -1 || CStationIdTo == -1) {
        Output::Error("Начало и конец трубы должны быть разными КС.\n");

        waitForEnter();
        return;
    }

    // Присоединение трубы к выбранным КС
    if (!network.connectPipe(selectedPipeId, CStationIdFrom, CStationIdTo)) {
        Output::Error("Выбранную трубу не удалось присоединить.\n");
        waitForEnter();
        return;
    }

    Output::Success("Труба ID=" + std::to_string(selectedPipeId) + " соединяет КС " + std::to_string(CStationIdFrom) + " -> КС " + std::to_string(CStationIdTo) + "\n");
    waitForEnter();
}

// Графовые операции
void Console::handleTopologicalSort() {
    std::vector<int> result;

    // Сортировка КС, при наличии цикла выводим ошибку
    if (!network.topologicalSort(result)) {
        Output::Error("В графе присутствует цикл - сортировка невозможна.\n");
        waitForEnter();
        return;
    }

    // Вывод КС в полученном порядке
    Output::Message("Топологически отсортированный массив:\n");
    for (const auto& id : result) {
        Output::Message(std::format("{} ", id));
    }

    waitForEnter();
}

void Console::handleFindShortestPath() {
    std::vector<Edge> result;

    // Чтение ID начальной и конечной КС
    int startId = Input::Int("Введите ID стартовой КС");
    int finishId = Input::Int("Введите ID конечной КС");

    // Проверка на наличие КС и разные ID
    if (!network.getCStationMap().contains(startId) || !network.getCStationMap().contains(finishId)) {
        Output::Error("Одна или обе компрессорные станции не найдены.\n");
        waitForEnter();
        return;
    } else if (startId == finishId) {
        Output::Error("ID КС совпадают.\n");
        waitForEnter();
        return;
    }

    // Поиск кратчайшего пути между выбранными КС
    if (!network.findShortestPath(startId, finishId, result)) {
        Output::Error("Не удалось построить кратчайший путь.\n");
        waitForEnter();
        return;
    }

    int totalDistance = 0;

    // Вывод последовательности КС в маршруте
    Output::Message("[Маршрут]\n");
    for (std::size_t i = 0; i != result.size(); ++i) {
        Output::Message(std::to_string(result[i].id));

        if (i + 1 < result.size()) {
            Output::Message(" -> ");
        }
    }

    // Вывод участков маршрута и подсчёт общей длины
    Output::Message("\n\n[Участки маршрута]\n");
    for (std::size_t i = 1; i != result.size(); ++i) {
        Output::Message(std::format("КС {} -> КС {:<4} |   Длина: {} км\n", result[i - 1].id, result[i].id, result[i].distance));

        totalDistance += result[i].distance;
    }

    Output::Message(std::format("\nОбщая длина пути: {} км\n", totalDistance));
    waitForEnter();
}

// Поиск

void Console::handleSearchPipesByName() const {
    // Чтение начала названия и поиск труб
    const std::string name = Input::String("\nВведите начало названия для поиска");
    const std::vector<int> pipeList = network.searchPipesByName(name);

    // Проверка на наличие результатов поиска
    if (pipeList.empty()) {
        Output::Warning("Трубы с таким именем не найдены.\n");
        return;
    }

    // Вывод найденных труб в таблице
    Pipe::printPipeTableHeader();

    for (const int id : pipeList) {
        network.getPipeMap().at(id).printPipe();
    }
}

void Console::handleSearchPipesByRepair() const {
    // Чтение статуса ремонта и поиск труб
    const bool repairStatus = Input::Bool("В ремонте?", false);
    const std::vector<int> pipeList = network.searchPipesByRepair(repairStatus);

    // Проверка на наличие результатов поиска
    if (pipeList.empty()) {
        Output::Warning("Трубы с указанным статусом не найдены.\n");
        return;
    }

    // Вывод найденных труб в таблице
    Pipe::printPipeTableHeader();

    for (const int id : pipeList) {
        network.getPipeMap().at(id).printPipe();
    }
}

void Console::handleSearchCStationsByName() const {
    // Чтение начала названия и поиск КС
    const std::string name = Input::String("\nВведите начало названия для поиска");
    const std::vector<int> CStationList = network.searchCStationsByName(name);

    // Проверка на наличие результатов поиска
    if (CStationList.empty()) {
        Output::Warning("КС с таким именем не найдены.\n");
        return;
    }

    // Вывод найденных КС в таблице
    CompressorStation::printCStationTableHeader();

    for (const int id : CStationList) {
        network.getCStationMap().at(id).printCStation();
    }
}

void Console::handleSearchCStationsByActive() const {
    // Чтение условия для числа или процента активных цехов
    const std::vector<char> condition = Input::Comparison("Введите условие поиска (>50%, <5, =10)");

    // Поиск КС по условию
    const std::vector<int> CStationList = network.searchCStationsByActive(condition);

    // Проверка на наличие результатов поиска
    if (CStationList.empty()) {
        Output::Warning("КС, подходящие под условие, не найдены.\n");
        return;
    }

    // Вывод найденных КС в таблице
    CompressorStation::printCStationTableHeader();

    for (const int id : CStationList) {
        network.getCStationMap().at(id).printCStation();
    }
}


// Принтеры айтемов
void Console::printMenu() const {
    Output::clearConsole();

    // Вывод пунктов главного меню
    for (std::size_t i = 0; i < mainMenuItems.size(); ++i) {
        Output::Message(std::format("{:<4}{}\n", std::format("{}.", i + 1), mainMenuItems[i]));
    }

    Output::Message(std::format("{:<4}{}\n", "0.", "Выход"));
}

void Console::printMenuViewAll() const {
    // Вывод фильтров поиска
    for (std::size_t i = 0; i < viewAllMenuItems.size(); ++i) {
        Output::Message(std::format("{}. {}\n", i + 1, viewAllMenuItems[i]));
    }

    Output::Message("0. Назад\n");
}


// Пуск
void Console::run() {
    bool running = true;

    while (running) {
        printMenu();

        // Чтение команды и запуск выбранного действия
        try {
            switch (Input::Int("\nExit - в любом действии, чтобы вернуться в меню.\nВыбор", -1)) {
                case 1:  Output::clearConsole(); handleAddPipe(); break;
                case 2:  Output::clearConsole(); handleAddCStation(); break;
                case 3:  Output::clearConsole(); handlePrintNetwork(); break;
                case 4:  Output::clearConsole(); handleEditPipe(); break;
                case 5:  Output::clearConsole(); handleEditCStation(); break;
                case 6:  Output::clearConsole(); handleDeletePipe(); break;
                case 7:  Output::clearConsole(); handleDeleteCStation(); break;
                case 8:  Output::clearConsole(); handleSave(); break;
                case 9:  Output::clearConsole(); handleLoad(); break;
                case 10: Output::clearConsole(); handleConnectPipe(); break;
                case 11: Output::clearConsole(); handleTopologicalSort(); break;
                case 12: Output::clearConsole(); handleFindShortestPath(); break;
                case 0:
                    running = false;
                    break;
                default:
                    Output::Error("Нет такого пункта меню.\n");
                    waitForEnter();
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
