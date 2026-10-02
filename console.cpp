#include <iostream>
#include <sstream>
#include <vector>
#include <set>
#include <format>
#include <stdexcept>
#include <limits>

#include "console.hpp"
#include "utils.hpp"

using namespace Output;

/*
Базовые методы работы с консолью
*/



void Console::waitForEnter() const {
    read_line("\nНажмите Enter, чтобы продолжить");
}

std::string Console::read_line(const std::string& prompt, std::string defaultParam) const {
    std::string value {};
    printMessage(prompt);

    if (!defaultParam.empty()) {
        printMessage(std::format(" [Значение по умолчанию: {}]", defaultParam), "", Format::ITALIC);
    }

    printMessage(": ");

    // Проверка потока ввода
    if (!std::getline(std::cin, value)) {
        throw InputCommand::Exit;
    }

    if (value == "exit" || value == "Exit" || value == "EXIT") {
        throw InputCommand::Exit;
    }

    if (value.empty() && !defaultParam.empty()) {
        return defaultParam;
    }

    return value;
}

std::string Console::read_name(const std::string& prompt, std::string defaultParam) const {
    while (true) {
        const std::string value = read_line(prompt, defaultParam);

        // Проверка названия, при ошибке повторяем ввод
        if (isValidName(value)) {
            return value;
        }

        printError("Название не должно быть пустым и содержать управляющие символы.\n");
    }
}

int Console::read_int(const std::string& prompt, int defaultParam, int min, int max) const {
    // Показываем только заданные ограничения, без крайних значений int
    const bool hasMin = min != (std::numeric_limits<int>::min)();
    const bool hasMax = max != (std::numeric_limits<int>::max)();
    std::string range;

    if (hasMin && hasMax) {
        range = "от " + std::to_string(min) + " до " + std::to_string(max);
    } else if (hasMin) {
        range = "от " + std::to_string(min);
    } else if (hasMax) {
        range = "до " + std::to_string(max);
    }

    while (true) {
        printMessage(prompt);

        if (!range.empty()) {
            printMessage(std::format(" ({})", range));
        }

        // Если есть значение по умолчанию - выводим
        if (defaultParam != -1) {
            printMessage(std::format(" [Значение по умолчанию: {}]", defaultParam), "", Format::ITALIC);
        }

        printMessage(": ");

        int value;

        // Если есть значение по умолчанию и был нажат Enter - устанавливаем значение по умолчанию
        if (defaultParam != -1 && std::cin.peek() == '\n') {
            value = defaultParam;
        } else {
            std::cin >> value;
        }

        // Если поток упал проверяем на то мог ли это быть exit, если не exit - сообщение об ошибке
        if (std::cin.fail()) {
            std::cin.clear();

            std::string input;
            std::getline(std::cin, input);

            if (input == "exit" || input == "Exit" || input == "EXIT") {
                throw InputCommand::Exit;
            }

            printError("Введите целое число.\n");
            continue;
        }

        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

        // Проверка числа на допустимый диапазон
        if (value < min || value > max) {
            printError("Введите целое число " + range + ".\n");
            continue;
        }

        return value;
    }
}

std::vector<int> Console::read_multiple_int(const std::string& prompt) const {
    while (true) {
        const std::string value = read_line(prompt);

        std::stringstream stringStream(value);

        std::set<int> ids;
        int id;

        // Чтение ID через пробел
        while (stringStream >> id) {
            ids.insert(id);
        }

        if (!ids.empty()) {
            return std::vector<int>(ids.begin(), ids.end());
        }

        printError("Введите числа через пробел.\n");
    }
}

bool Console::read_bool(const std::string& prompt, bool defaultParam) const {
    const std::string defaultStr = defaultParam ? "y" : "n";
    const std::string formattedPrompt = prompt + " (y/n)";

    while (true) {
        const std::string value = read_line(formattedPrompt, defaultStr);

        if (value == "y" || value == "Y") { return true; }
        if (value == "n" || value == "N") { return false; }

        printError("Введите y или n.\n");
    }
}

StationType Console::read_station_type(StationType defaultParam) const {
    int defaultInt = static_cast<int>(defaultParam);
    return static_cast<StationType>(read_int("Класс станции (0 - Light, 1 - Medium, 2 - Heavy)", defaultInt, 0, 2));
}

std::vector<char> Console::read_comparison(const std::string& prompt) const {
    while (true) {
        const std::string input = read_line(prompt);
        std::stringstream stringStream(input);

        char comparison;
        int value;
        std::string suffix;

        // Получаем знак сравнения и значение
        if (stringStream >> comparison >> value) {
            // Пропускаем пробелы и читаем остаток строки: пустую строку или %
            std::getline(stringStream >> std::ws, suffix);

            // Проверка на корректные значения
            if ((comparison == '>' || comparison == '<' || comparison == '=') && value >= 0 && (suffix.empty() || (suffix == "%" && value <= 100))) {
                const std::string condition = comparison + std::to_string(value) + suffix;
                return std::vector<char>(condition.begin(), condition.end());
            }
        }

        printError("Введите условие вида >50%, <5 или =10.\n");
    }
}

/*
Хэндлеры
*/

// Добавление
void Console::handleAddPipe() {
    // Чтение значений из консоли
    const int diameter = read_int("Диаметр (мм)", 500, 1);
    const int length = read_int("Длина (км)", 100, 1);
    const std::string name = read_name("Название", "Pipe_");
    const bool repair = read_bool("В ремонте?", false);
    const int amount = read_int("Сколько труб добавить", 1, 1);


    // Добавление труб
    const std::size_t added = network.addPipe(diameter, length, name, repair, amount);
    printInfo("Добавлено труб: " + std::to_string(added) + " из " + std::to_string(amount) + "\n");
 
    waitForEnter();
}

void Console::handleAddCStation() {
    // Чтение значений из консоли
    const int numWorkshops = read_int("Количество цехов", 10, 1);
    const int numActiveWorkshops = read_int("Количество цехов в работе", numWorkshops, 0, numWorkshops);

    const std::string name = read_name("Название", "CStation_");

    const StationType type = read_station_type(StationType::Light);
    const int amount = read_int("Сколько КС добавить", 1, 1);


    // Добавление КС
    const std::size_t added =  network.addCStation(numWorkshops, numActiveWorkshops, name, type, amount);
    printInfo("Добавлено КС: " + std::to_string(added) + " из " + std::to_string(amount) + "\n");
    
    waitForEnter();
}

void Console::handlePrintNetwork() {
    // Вывод труб и КС
    network.printNetwork();

    // Выбор фильтра для поиска
    printMenuViewAll();
    switch (read_int("Выбор")) {
        case 1: {
            clearConsole();
            handleSearchPipesByName();
            waitForEnter();
            break;
        }
        case 2: {
            clearConsole();
            handleSearchPipesByRepair();
            waitForEnter();
            break;
        }
        case 3: {
            clearConsole();
            handleSearchCStationsByName();
            waitForEnter();
            break;
        }
        case 4: {
            clearConsole();
            handleSearchCStationsByActive();
            waitForEnter();
            break;
        }
        case 0:
            return;
        default: {
            printError("Нет такого пункта меню.\n");
            waitForEnter();
            break;
        }
    }
}


// Редактирование
void Console::handleEditPipe() {
    // Проверка на наличие труб
    if (network.getPipeMap().empty()) {
        printWarning("Нет труб для редактирования.\n");
        waitForEnter();
        return;
    }

    const std::vector<int> ids = read_multiple_int("ID труб через пробел");
    const bool newRepairStatus = read_bool("В ремонте?", false);
    std::size_t edited = network.editPipe(ids, newRepairStatus);

    printInfo("Обработано труб: " + std::to_string(edited) + " из " + std::to_string(ids.size()) + "\n");
    waitForEnter();
}

void Console::handleEditCStation() {
    // Проверка на наличие КС
    if (network.getCStationMap().empty()) {
        printWarning("Нет КС для редактирования.\n");
        waitForEnter();
        return;
    }

    const std::vector<int> ids = read_multiple_int("ID КС через пробел");
    const int newNumActiveWorkshops = read_int("Количество цехов в работе", 0, 0);
    std::size_t edited = network.editCStation(ids, newNumActiveWorkshops);

    printInfo("Обработано КС: " + std::to_string(edited) + " из " + std::to_string(ids.size()) + "\n");
    waitForEnter();
}

void Console::handleDeletePipe() {
    // Проверка на наличие труб
    if (network.getPipeMap().empty()) {
        printWarning("Нет труб для удаления.\n");
        waitForEnter();
        return;
    }
    
    // Чтение ID труб для удаления
    const auto ids = read_multiple_int("ID труб для удаления через пробел");
    const std::size_t deleted = network.deletePipe(ids);
    printInfo("Удалено труб: " + std::to_string(deleted) + " из " + std::to_string(ids.size()) + "\n");

    waitForEnter();
}

void Console::handleDeleteCStation() {
    // Проверка на наличие КС
    if (network.getCStationMap().empty()) {
        printWarning("Нет КС для удаления.\n");
        waitForEnter();
        return;
    }
    
    // Чтение ID КС для удаления
    const auto ids = read_multiple_int("ID КС для удаления через пробел");
    const std::size_t deleted = network.deleteCStation(ids);
    printInfo("Удалено КС: " + std::to_string(deleted) + " из " + std::to_string(ids.size()) + "\n");

    waitForEnter();
}

void Console::handleSave() {
    clearConsole();

    while (true) {
        // Чтение пути с именем файла по умолчанию
        const std::string path = read_line("Путь к файлу сети", "data/network_" + getTimestamp() + ".txt");

        // Сохранение сети, при ошибке повторяем ввод пути
        if (network.saveToFile(path)) {
            printSuccess("Трубы и КС сохранены в " + path + "\n");
            break;
        }

        printError("Не удалось сохранить сеть. Проверьте путь и доступ к файлу.\n");
    }

    waitForEnter();
}

void Console::handleLoad() {
    clearConsole();

    while (true) {
        // Чтение пути к файлу сети
        const std::string path = read_line("Путь к файлу сети");

        // Замена текущей сети данными из файла при успешной загрузке
        if (network.loadFromFile(path)) {
            printSuccess("Текущая сеть заменена данными из " + path + "\n");
            break;
        }

        printError("Проверьте путь и формат файла. Текущая сеть не изменена.\n");
    }

    waitForEnter();
}

void Console::handleConnectPipe() {
    clearConsole();

    // Чтение диаметра и поиск свободной трубы
    const int targetDiameter = read_int("Диаметр трубы (мм)", 500, 1);
    int selectedPipeId = -1;

    while (selectedPipeId == -1) {
        selectedPipeId = network.findFreePipe(targetDiameter);

        if (selectedPipeId != -1) {
            break;
        }

        // Если подходящей трубы нет, предлагаем создать новую
        printWarning("Свободная труба нужного диаметра не найдена.\n");
        if (!read_bool("Создать и присоединить?", false)) {
            waitForEnter();
            return;
        }

        const std::string defaultName = "Pipe_" + std::to_string(network.getNextPipeId());
        const int length = read_int("Длина (км)", 100, 1);
        const std::string name = read_name("Название", defaultName);

        if (!network.addPipe(targetDiameter, length, name, false)) {
            printError("Не удалось создать трубу.\n");
            waitForEnter();
            return;
        }

        // После создания повторяем поиск по диаметру.
    }

    // Чтение ID станций начала и конца трубы
    const int CStationIdFrom = read_int("ID КС начала трубы");
    const int CStationIdTo = read_int("ID КС конца трубы");

    // Проверка на наличие таких Id
    if (!network.getCStationMap().contains(CStationIdFrom) || !network.getCStationMap().contains(CStationIdTo)) {
        printError("Одна или обе компрессорные станции не найдены.\n");

        waitForEnter();
        return;
    }

    // Проверка на то, что Id КС не совпадают или равны -1
    if (CStationIdFrom == CStationIdTo || CStationIdFrom == -1 || CStationIdTo == -1) {
        printError("Начало и конец трубы должны быть разными КС.\n");

        waitForEnter();
        return;
    }

    // Присоединение трубы к выбранным КС
    if (!network.connectPipe(selectedPipeId, CStationIdFrom, CStationIdTo)) {
        printError("Выбранную трубу не удалось присоединить.\n");
        waitForEnter();
        return;
    }

    printSuccess("Труба ID=" + std::to_string(selectedPipeId) + " соединяет КС " + std::to_string(CStationIdFrom) + " -> КС " + std::to_string(CStationIdTo) + "\n");
    waitForEnter();
}

void Console::handleTopologicalSort() {
    std::vector<int> result;

    // Сортировка КС, при наличии цикла выводим ошибку
    if (!network.topologicalSort(result)) {
        printError("В графе присутствует цикл - сортировка невозможна.\n");
        waitForEnter();
        return;
    }

    // Вывод КС в полученном порядке
    printMessage("Топологически отсортированный массив:\n");
    for (const auto& id : result) {
        printMessage(std::format("{} ", id));
    }

    waitForEnter();
}

void Console::handleFindShortestPath() {
    std::vector<Edge> result;

    // Чтение ID начальной и конечной КС
    int startId = read_int("Введите ID стартовой КС");
    int finishId = read_int("Введите ID конечной КС");

    // Проверка на наличие КС и разные ID
    if (!network.getCStationMap().contains(startId) || !network.getCStationMap().contains(finishId)) {
        printError("Одна или обе компрессорные станции не найдены.\n");
        waitForEnter();
        return;
    } else if (startId == finishId) {
        printError("ID КС совпадают.\n");
        waitForEnter();
        return;
    }

    // Поиск кратчайшего пути между выбранными КС
    if (!network.findShortestPath(startId, finishId, result)) {
        printError("Не удалось построить кратчайший путь.\n");
        waitForEnter();
        return;
    }

    int totalDistance = 0;

    // Вывод последовательности КС в маршруте
    printMessage("[Маршрут]\n");
    for (std::size_t i = 0; i != result.size(); ++i) {
        printMessage(std::to_string(result[i].id));

        if (i + 1 < result.size()) {
            printMessage(" -> ");
        }
    }

    // Вывод участков маршрута и подсчёт общей длины
    printMessage("\n\n[Участки маршрута]\n");
    for (std::size_t i = 1; i != result.size(); ++i) {
        printMessage(std::format("КС {} -> КС {:<4} |   Длина: {} км\n", result[i - 1].id, result[i].id, result[i].distance));

        totalDistance += result[i].distance;
    }

    printMessage(std::format("\nОбщая длина пути: {} км\n", totalDistance));
    waitForEnter();
}

/*
Хэндлеры поиска
*/

void Console::handleSearchPipesByName() const {
    // Чтение начала названия и поиск труб
    const std::string name = read_line("\nВведите начало названия для поиска");
    const std::vector<int> pipeList = network.searchPipesByName(name);

    // Проверка на наличие результатов поиска
    if (pipeList.empty()) {
        printWarning("Трубы с таким именем не найдены.\n");
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
    const bool repairStatus = read_bool("В ремонте?", false);
    const std::vector<int> pipeList = network.searchPipesByRepair(repairStatus);

    // Проверка на наличие результатов поиска
    if (pipeList.empty()) {
        printWarning("Трубы с указанным статусом не найдены.\n");
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
    const std::string name = read_line("\nВведите начало названия для поиска");
    const std::vector<int> CStationList = network.searchCStationsByName(name);

    // Проверка на наличие результатов поиска
    if (CStationList.empty()) {
        printWarning("КС с таким именем не найдены.\n");
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
    const std::vector<char> condition = read_comparison("Введите условие поиска (>50%, <5, =10)");

    // Поиск КС по условию
    const std::vector<int> CStationList = network.searchCStationsByActive(condition);

    // Проверка на наличие результатов поиска
    if (CStationList.empty()) {
        printWarning("КС, подходящие под условие, не найдены.\n");
        return;
    }

    // Вывод найденных КС в таблице
    CompressorStation::printCStationTableHeader();

    for (const int id : CStationList) {
        network.getCStationMap().at(id).printCStation();
    }
}

/*
Принтеры
*/

void Console::printMenu() const {
    clearConsole();

    // Вывод пунктов главного меню
    for (std::size_t i = 0; i < mainMenuItems.size(); ++i) {
        printMessage(std::format("{:<4}{}\n", std::format("{}.", i + 1), mainMenuItems[i]));
    }

    printMessage(std::format("{:<4}{}\n", "0.", "Выход"));
}

void Console::printMenuViewAll() const {
    // Вывод фильтров поиска
    for (std::size_t i = 0; i < viewAllMenuItems.size(); ++i) {
        printMessage(std::format("{}. {}\n", i + 1, viewAllMenuItems[i]));
    }

    printMessage("0. Назад\n");
}

/*
Запуск
*/

void Console::run() {
    bool running = true;

    while (running) {
        printMenu();

        // Чтение команды и запуск выбранного действия
        try {
            switch (read_int("\nExit - в любом действии, чтобы вернуться в меню.\nВыбор", -1)) {
                case 1:  clearConsole(); handleAddPipe(); break;
                case 2:  clearConsole(); handleAddCStation(); break;
                case 3:  clearConsole(); handlePrintNetwork(); break;
                case 4:  clearConsole(); handleEditPipe(); break;
                case 5:  clearConsole(); handleEditCStation(); break;
                case 6:  clearConsole(); handleDeletePipe(); break;
                case 7:  clearConsole(); handleDeleteCStation(); break;
                case 8:  clearConsole(); handleSave(); break;
                case 9:  clearConsole(); handleLoad(); break;
                case 10: clearConsole(); handleConnectPipe(); break;
                case 11: clearConsole(); handleTopologicalSort(); break;
                case 12: clearConsole(); handleFindShortestPath(); break;
                case 0:
                    running = false;
                    break;
                default:
                    printError("Нет такого пункта меню.\n");
                    waitForEnter();
                    break;
            }
        } catch (InputCommand) {
            // Команда exit возвращает в меню, закрытый поток завершает программу
            if (!std::cin) {
                running = false;
            }
        }
    }
}
