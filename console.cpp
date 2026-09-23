#include <iostream>
#include <sstream>
#include <vector>
#include <set>
#include <format>
#include <stdexcept>
#include <limits>

#include "console.hpp"
#include "utils.hpp"


/*



Базовые методы работы с консолью



*/

void Console::clearConsole() const {
    std::system("cls");
}

std::string Console::read_line(const std::string& prompt, std::string defaultParam) const {
    std::string value {};
    std::cout << prompt;

    if (!defaultParam.empty()) {
        std::cout << " [Значение по умолчанию: " << defaultParam << "]";
    }
    
    std::cout << ' ';

    // Проверки на поток ввода
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

std::string Console::read_valid_name(const std::string& prompt, std::string defaultParam) const {
    while (true) {
        const std::string value = read_line(prompt, defaultParam);

        if (isValidName(value)) {
            return value;
        }

        std::cerr << "[*] Ошибка: Название не должно быть пустым и содержать управляющие символы.\n";
    }
}

int Console::read_int(const std::string& prompt, int defaultParam, bool isPositive) const {
    while (true) {
        std::cout << prompt;
        if (defaultParam != -1) {
            std::cout << " [Значение по умолчанию: " << defaultParam << "] ";
        }

        int value;
        if (defaultParam != -1 && std::cin.peek() == '\n') {
            value = defaultParam;
        } else {
            std::cin >> value;
        }

        if (std::cin.fail()) {
            std::cin.clear();

            std::string input;
            std::getline(std::cin, input);

            if (input == "exit" || input == "Exit" || input == "EXIT") {
                throw InputCommand::Exit;
            }

            std::cout << "[*] Ошибка: Введите целое число.\n";
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (isPositive && value <= 0) {
            std::cerr << "[*] Ошибка: Введите число больше 0.\n";
            continue;
        }
        return value;
    }
}

bool Console::read_bool(const std::string& prompt, bool defaultParam) const {
    std::string defaultStr = defaultParam ? "y" : "n";
    std::string formattedPrompt = prompt + " (y/n)";

    while (true) {
        const std::string value = read_line(formattedPrompt, defaultStr);

        if (value == "y" || value == "Y") { return true; }
        if (value == "n" || value == "N") { return false; }

        std::cerr << "[*] Ошибка: Введите y или n.\n";
    }
}

StationType Console::read_station_type(StationType defaultParam) const {
    std::string prompt = "Класс станции (0 - Light, 1 - Medium, 2 - Heavy):";
    int defaultInt = static_cast<int>(defaultParam);

    while (true) {
        const int type = read_int(prompt, defaultInt);

        if (type >= 0 && type <= 2) { 
            return static_cast<StationType>(type); 
        }

        std::cerr << "[*] Ошибка: Введите 0, 1 или 2.\n";
    }
}

std::vector<int> Console::read_multiple_int(const std::string& prompt) const {
    while (true) {
        const std::string value = read_line(prompt);

        std::stringstream stringStream(value);

        std::set<int> ids;
        int id;

        while (stringStream >> id) {
            ids.insert(id);
        }

        if (!ids.empty()) {
            return std::vector<int>(ids.begin(), ids.end());
        }

        std::cout << "[*] Ошибка: Введите числа через пробел.\n";
    }
}

std::vector<char> Console::read_comparison(const std::string& prompt) const {
    while (true) {
        const std::string input = read_line(prompt);
        std::stringstream stringStream(input);

        char comparison;
        int value;
        std::string suffix;

        if (stringStream >> comparison >> value) {
            std::getline(stringStream >> std::ws, suffix);

            if ((comparison == '>' || comparison == '<' || comparison == '=') && value >= 0 && (suffix.empty() || (suffix == "%" && value <= 100))) {

                const std::string condition = comparison + std::to_string(value) + suffix;
                return std::vector<char>(condition.begin(), condition.end());

            }
        }

        std::cout << "[*] Ошибка: Введите условие вида >50%, <5 или =10.\n";
    }
}

/*
Хэндлеры
*/

// Добавление 
void Console::handleAddPipe() {
    const int defaultNameNum = network.getNextPipeId();
    const std::string defaultName = "Pipe_" + std::to_string(defaultNameNum);

    const int diameter = read_int("Диаметр (мм): ", 500, true);
    const int length = read_int("Длина (км): ", 100, true);
    const bool repair = read_bool("В ремонте?", false);
    const std::string name = read_valid_name("Название: ", defaultName);
    const int num = read_int("Сколько труб добавить: ", 1, true);

    int added = 0;

    for (int i = 0; i < num; ++i) {
        const int id = network.getNextPipeId();
        const std::string actualName = name == defaultName ? "Pipe_" + std::to_string(id) : name;
        if (!network.addPipe(diameter, length, actualName, repair)) {
            std::cerr << "[*] Ошибка: Не удалось добавить трубу.\n";
            break;
        }

        ++added;

    }
    std::cout << "Добавлено труб: " << added << " из " << num << "\n";

    read_line("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleAddCStation() {
    const int defaultNameNum = network.getNextCStationId();
    const std::string defaultName = "CStation_" + std::to_string(defaultNameNum);

    const int numWorkshops = read_int("Количество цехов: ", 10, true);
    int numActiveWorkshops = read_int("Количество цехов в работе: ", numWorkshops);

    while (numActiveWorkshops < 0 || numActiveWorkshops > numWorkshops) {
        std::cout << "[*] Ошибка: Количество активных цехов должно быть от 0 до общего числа цехов.\n";
        numActiveWorkshops = read_int("Количество цехов в работе: ", numWorkshops);
    }

    const std::string name = read_valid_name("Название: ", defaultName);
    const StationType type = read_station_type(StationType::Light);
    const int num = read_int("Сколько КС добавить: ", 1, true);

    int added = 0;
    for (int i = 0; i < num; ++i) {
        const int id = network.getNextCStationId();

        const std::string actualName = name == defaultName ? "CStation_" + std::to_string(id) : name;

        if (!network.addCStation(numWorkshops, numActiveWorkshops, actualName, type)) {
            std::cerr << "[*] Ошибка: Не удалось добавить КС.\n";
            break;
        }

        ++added;

    }
    std::cout << "Добавлено КС: " << added << " из " << num << "\n";
    read_line("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handlePrintNetwork() {

    network.printNetwork();

    printMenuViewAll();
    switch (read_int("Выбор: ")) {
        case 1: {
            clearConsole();
            handleSearchPipesByName();
            read_line("\nНажмите Enter, чтобы продолжить...");
            break;
        }
        case 2: {
            clearConsole();
            handleSearchPipesByRepair();
            read_line("\nНажмите Enter, чтобы продолжить...");
            break;
        }
        case 3: {
            clearConsole();
            handleSearchCStationsByName();
            read_line("\nНажмите Enter, чтобы продолжить...");
            break;
        }
        case 4: {
            clearConsole();
            handleSearchCStationsByActive();
            read_line("\nНажмите Enter, чтобы продолжить...");
            break;
        }
        case 0:
            return;
        default: {
            std::cout << "[*] Ошибка: Нет такого пункта меню.\n";
            read_line("\nНажмите Enter, чтобы продолжить...");
            break;
        }
    }
}


// Редактирование
void Console::handleEditPipe() {
    if (network.getPipeMap().empty()) {
        read_line("Нет труб для редактирования. Нажмите Enter, чтобы вернуться в меню...");
        return;
    }

    const auto ids = read_multiple_int("ID труб через пробел: ");

    int edited = 0;

    for (int id : ids) {
        if (!network.getPipeMap().contains(id)) {
            std::cout << "Труба ID=" << id << " не найдена, пропущена.\n";

            continue;
        }

        const bool repair = read_bool("Труба ID=" + std::to_string(id) + " в ремонте?", network.getPipeMap().at(id).getRepair());
        
        if (network.editPipe(id, repair)) {
            ++edited;

            std::cout << "Труба ID=" << id << ": статус ремонта установлен - "
                      << (repair ? "в ремонте" : "не в ремонте") << '\n';
        }
    }
    std::cout << "Обработано труб: " << edited << " из " << ids.size() << '\n';
    read_line("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleEditCStation() {
    if (network.getCStationMap().empty()) {
        read_line("Нет КС для редактирования. Нажмите Enter, чтобы вернуться в меню...");
        return;
    }

    const auto ids = read_multiple_int("ID КС через пробел: ");

    int edited = 0;
    
    for (int id : ids) {
        if (!network.getCStationMap().contains(id)) {
            std::cout << "КС ID=" << id << " не найдена, пропущена.\n";
            continue;
        }

        const auto& station = network.getCStationMap().at(id);
        int active;

        do {
            active = read_int("КС ID=" + std::to_string(id) + ": активных цехов (от 0 до " + std::to_string(station.getNumWorkshops()) + "):", station.getNumActiveWorkshops());
            
            if (active < 0 || active > station.getNumWorkshops()) {
                std::cerr << "[*] Ошибка: Количество активных цехов вне допустимого диапазона.\n";
            }

        } while (active < 0 || active > station.getNumWorkshops());
        
        if (network.editCStation(id, active)) {
            ++edited;

            std::cout << "КС ID=" << id << ": установлено активных цехов - " << active << '\n';
        }
    }
    std::cout << "Обработано КС: " << edited << " из " << ids.size() << '\n';
    read_line("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleDeletePipe() {
    if (network.getPipeMap().empty()) {
        read_line("Нет труб для удаления. Нажмите Enter, чтобы вернуться в меню...");
        return;
    }

    const auto ids = read_multiple_int("ID труб для удаления через пробел: ");

    for (int id : ids) {
        const bool removed = network.deletePipe(id);
        
        std::cout << "Труба ID=" << id
                  << (removed ? " удалена" : " не найдена, удаление пропущено") << '\n';
    }
    read_line("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleDeleteCStation() {
    if (network.getCStationMap().empty()) {
        read_line("Нет КС для удаления. Нажмите Enter, чтобы вернуться в меню...");
        return;
    }

    const auto ids = read_multiple_int("ID КС для удаления через пробел: ");

    for (int id : ids) {
        const bool removed = network.deleteCStation(id);
        std::cout << "КС ID=" << id
                  << (removed ? " удалена; связанные трубы отсоединены" : " не найдена, удаление пропущено") << '\n';
    }
    read_line("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleSave() {
    clearConsole();

    while (true) {
        const std::string path = read_line("Путь к файлу сети:", "data/network_" + getTimestamp() + ".txt");

        if (network.saveToFile(path)) {
            std::cout << "Трубы и КС сохранены в " << path << '\n';
            break;
        }

        std::cerr << "[*] Ошибка: Не удалось сохранить сеть. Проверьте путь и доступ к файлу.\n";
    }
    read_line("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleLoad() {
    clearConsole();

    while (true) {
        const std::string path = read_line("Путь к файлу сети:");

        if (network.loadFromFile(path)) {
            std::cout << "Текущая сеть заменена данными из " << path << '\n';
            break;
        }
        
        std::cerr << "[*] Ошибка: Проверьте путь и формат файла. Текущая сеть не изменена.\n";
    }
    read_line("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleConnectPipe() {
    clearConsole();

    const int targetDiameter = read_int("Диаметр трубы (мм): ", 500, true);
    int selectedPipeId = -1;

    while (selectedPipeId == -1) {
        selectedPipeId = network.findFreePipe(targetDiameter);

        if (selectedPipeId != -1){
            break;
        }

        if (!read_bool("[!] Внимание: Свободная труба нужного диаметра не найдена. Создать и присоединить? ", false)) {
            read_line("\nНажмите Enter, чтобы вернуться в меню...");
            return;
        }

        const std::string defaultName = "Pipe_" + std::to_string(network.getNextPipeId());
        const int length = read_int("Длина (км): ", 100, true);
        const std::string name = read_valid_name("Название: ", defaultName);

        if (!network.addPipe(targetDiameter, length, name, false)) {
            std::cerr << "[*] Ошибка: Не удалось создать трубу.\n";
            read_line("\nНажмите Enter, чтобы вернуться в меню...");
            return;
        }

        // После создания повторяем поиск по диаметру.
    }

    const int cStationIdFrom = read_int("ID КС начала трубы: ");
    const int cStationIdTo = read_int("ID КС конца трубы: ");

    // Проверка на наличие таких Id
    if (!network.getCStationMap().contains(cStationIdFrom) || !network.getCStationMap().contains(cStationIdTo)) {
        std::cerr << "[*] Ошибка: Одна или обе компрессорные станции не найдены.\n";
        
        read_line("\nНажмите Enter, чтобы вернуться в меню...");
        return;
    } 

    // Проверка на то, что Id КС не совпадают или равны -1
    if (cStationIdFrom == cStationIdTo || cStationIdFrom == -1 || cStationIdTo == -1) {
        std::cerr << "[*] Ошибка: Начало и конец трубы должны быть разными КС.\n";
        
        read_line("\nНажмите Enter, чтобы вернуться в меню...");
        return;
    }

    if (!network.connectPipe(selectedPipeId, cStationIdFrom, cStationIdTo)) {
        std::cerr << "[*] Ошибка: Выбранную трубу не удалось присоединить.\n";
        read_line("\nНажмите Enter, чтобы вернуться в меню...");
        return;
    }

    std::cout << "Труба ID=" << selectedPipeId << " соединяет КС " << cStationIdFrom << " -> КС " << cStationIdTo << "\n";
    read_line("\nНажмите Enter, чтобы вернуться в меню...");
}

/*
Хэндлеры поиска
*/

void Console::handleSearchPipesByName() const {
    const std::string name = read_line("\nВведите начало названия для поиска: ");
    const std::vector<int> pipeList = network.searchPipesByName(name);

        
    if (pipeList.empty()) {
        std::cout << "[!] Внимание: Трубы с таким именем не найдены.\n";
        return;
    }

    Pipe::printPipeTableHeader();

    for (const int id : pipeList) {
        network.getPipeMap().at(id).printPipe();
    }
}

void Console::handleSearchPipesByRepair() const {
    const bool repairStatus = read_bool("В ремонте?", false);
    const std::vector<int> pipeList = network.searchPipesByRepair(repairStatus);

        
    if (pipeList.empty()) {
        std::cout << "[!] Внимание: Трубы с указанным статусом не найдены.\n";
        return;
    }

    Pipe::printPipeTableHeader();

    for (const int id : pipeList) {
        network.getPipeMap().at(id).printPipe();
    }
}

void Console::handleSearchCStationsByName() const {
    const std::string name = read_line("\nВведите начало названия для поиска: ");
    const std::vector<int> cStationList = network.searchCStationsByName(name);

        
    if (cStationList.empty()) {
        std::cout << "[!] Внимание: КС с таким именем не найдены.\n";
        return;
    }

    CompressorStation::printCStationTableHeader();

    for (const int id : cStationList) {
        network.getCStationMap().at(id).printCStation();
    }

}

void Console::handleSearchCStationsByActive() const {
    const std::vector<char> condition = read_comparison(
        "Введите условие поиска (>50%, <5, =10): "
    );

    const std::vector<int> cStationList = network.searchCStationsByActive(condition);

    if (cStationList.empty()) {
        std::cout << "[!] Внимание: КС, подходящие под условие, не найдены.\n";
        return;
    }
    

    CompressorStation::printCStationTableHeader();


    for (const int id : cStationList) {
        network.getCStationMap().at(id).printCStation();
    }
}

void Console::handleTopologicalSort() {
    std::vector<int> result;

    if (!network.topologicalSort(result)) {
        std::cerr << "[*] Ошибка: В графе присутствует цикл - сортировка невозможна.\n";
        read_line("\nНажмите Enter, чтобы вернуться в меню...");
        return;
    }

    std::cout << "Топологически отсортированный массив:\n";
    for (const auto& id : result) {
        std::cout << id << " ";
    }
    read_line("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleFindShortestPath() {
    std::vector<Edge> result;

    int startId = read_int("Введите ID стартовой КС: ");
    int finishId = read_int("Введите ID конечной КС: ");

    if (!network.getCStationMap().contains(startId) || !network.getCStationMap().contains(finishId)) {
        std::cerr << "[*] Ошибка: Одна или обе компрессорные станции не найдены.\n";
        read_line("\nНажмите Enter, чтобы вернуться в меню...");
        return;
    } else if (startId == finishId) {
        std::cerr << "[*] Ошибка: ID КС совпадают.\n";
        read_line("\nНажмите Enter, чтобы вернуться в меню...");
        return;
    }


    if (!network.findShortestPath(startId, finishId, result)) {
        std::cerr << "[*] Ошибка: Не удалось построить кратчайший путь.\n";
        read_line("\nНажмите Enter, чтобы вернуться в меню...");
        return;
    }

    int totalDistance = 0;

    std::cout << "[Маршрут]\n";
    for (size_t i = 0; i != result.size(); ++i) {
        std::cout << result[i].id;

        if (i + 1 < result.size()) {
            std::cout << " -> ";
        }
    }

    std::cout << "\n\n[Участки маршрута]\n";
    for (size_t i = 1; i != result.size(); ++i) {
        std::cout << std::format("КС {} -> КС {:<4} |   Длина: {} км\n",
            result[i - 1].id,
            result[i].id,
            result[i].distance
        );

        totalDistance += result[i].distance;
    }

    std::cout << "\nОбщая длина пути: " << totalDistance << " км\n";
    read_line("\nНажмите Enter, чтобы вернуться в меню...");
}

/*
Принтеры
*/

void Console::printMenu() const {
    clearConsole();

    for (std::size_t i = 0; i < mainMenuItems.size(); ++i) {
        std::cout << std::format("{:<4}{}\n", std::format("{}.", i + 1), mainMenuItems[i]);
    }

    std::cout << std::format("{:<4}{}\n", "0.", "Выход");
}

void Console::printMenuViewAll() const {
    for (std::size_t i = 0; i < viewAllMenuItems.size(); ++i) {
        std::cout << i + 1 << ". " << viewAllMenuItems[i] << '\n';
    }

    std::cout << "0. Назад\n";
}









/*



Запуск



*/

void Console::run() {
    bool running = true;

    while (running) {
        printMenu();

        try {
            switch (read_int("\nExit - в любом действии, чтобы вернуться в меню.\nВыбор: ", -1)) {
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
                    std::cout << "[*] Ошибка: Нет такого пункта меню.\n";
                    read_line("\nНажмите Enter, чтобы вернуться в меню...");
                    break;
            }
        } catch (InputCommand) {
            if (!std::cin) {
                running = false;
            }
        }
    }
}
