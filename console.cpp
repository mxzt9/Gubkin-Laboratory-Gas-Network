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

    std::cout << ": ";

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

std::string Console::read_valid_name(const std::string& prompt, std::string defaultParam) const {
    while (true) {
        const std::string value = read_line(prompt, defaultParam);

        // Проверка названия, при ошибке повторяем ввод
        if (isValidName(value)) {
            return value;
        }

        std::cerr << "[*] Ошибка: Название не должно быть пустым и содержать управляющие символы.\n";
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
        std::cout << prompt;

        if (!range.empty()) {
            std::cout << " (" << range << ")";
        }

        // Если есть значение по умолчанию - выводим
        if (defaultParam != -1) {
            std::cout << " [Значение по умолчанию: " << defaultParam << "]";
        }

        std::cout << ": ";

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

            std::cout << "[*] Ошибка: Введите целое число.\n";
            continue;
        }

        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

        // Проверка числа на допустимый диапазон
        if (value < min || value > max) {
            std::cerr << "[*] Ошибка: Введите целое число " << range << ".\n";
            continue;
        }

        return value;
    }
}

bool Console::read_bool(const std::string& prompt, bool defaultParam) const {
    // Подготовка подсказки и значения по умолчанию
    const std::string defaultStr = defaultParam ? "y" : "n";
    const std::string formattedPrompt = prompt + " (y/n)";

    while (true) {
        const std::string value = read_line(formattedPrompt, defaultStr);

        if (value == "y" || value == "Y") { return true; }
        if (value == "n" || value == "N") { return false; }

        std::cerr << "[*] Ошибка: Введите y или n.\n";
    }
}

StationType Console::read_station_type(StationType defaultParam) const {
    // Чтение класса станции как числа и преобразование в StationType
    int defaultInt = static_cast<int>(defaultParam);
    return static_cast<StationType>(read_int("Класс станции (0 - Light, 1 - Medium, 2 - Heavy)", defaultInt, 0, 2));
}

std::vector<int> Console::read_multiple_int(const std::string& prompt) const {
    while (true) {
        const std::string value = read_line(prompt);

        std::stringstream stringStream(value);

        std::set<int> ids;
        int id;

        // Чтение ID через пробел, set убирает повторяющиеся значения
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

        std::cout << "[*] Ошибка: Введите условие вида >50%, <5 или =10.\n";
    }
}

/*
Хэндлеры
*/

// Добавление
void Console::handleAddPipe() {
    // Генерация названия трубы по умолчанию
    const int defaultNameNum = network.getNextPipeId();
    const std::string defaultName = "Pipe_" + std::to_string(defaultNameNum);

    // Чтение значений из консоли
    const int diameter = read_int("Диаметр (мм)", 500, 1);
    const int length = read_int("Длина (км)", 100, 1);
    const bool repair = read_bool("В ремонте?", false);
    const std::string name = read_valid_name("Название", defaultName);
    const int num = read_int("Сколько труб добавить", 1, 1);

    // Добавление труб
    int added = 0;

    for (int i = 0; i < num; ++i) {
        if (!network.addPipe(diameter, length, name, repair)) {
            std::cerr << "[*] Ошибка: Не удалось добавить трубу.\n";
            break;
        }

        ++added;
    }

    std::cout << "Добавлено труб: " << added << " из " << num << "\n";
    read_line("\nНажмите Enter, чтобы вернуться в меню");
}

void Console::handleAddCStation() {
    // Генерация названия КС по умолчанию
    const int defaultNameNum = network.getNextCStationId();
    const std::string defaultName = "CStation_" + std::to_string(defaultNameNum);

    // Чтение значений из консоли
    const int numWorkshops = read_int("Количество цехов", 10, 1);
    const int numActiveWorkshops = read_int("Количество цехов в работе", numWorkshops, 0, numWorkshops);

    const std::string name = read_valid_name("Название", defaultName);
    const StationType type = read_station_type(StationType::Light);
    const int num = read_int("Сколько КС добавить", 1, 1);

    // Добавление КС
    int added = 0;

    for (int i = 0; i < num; ++i) {
        if (!network.addCStation(numWorkshops, numActiveWorkshops, name, type)) {
            std::cerr << "[*] Ошибка: Не удалось добавить КС.\n";
            break;
        }

        ++added;
    }

    std::cout << "Добавлено КС: " << added << " из " << num << "\n";
    read_line("\nНажмите Enter, чтобы вернуться в меню");
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
            read_line("\nНажмите Enter, чтобы продолжить");
            break;
        }
        case 2: {
            clearConsole();
            handleSearchPipesByRepair();
            read_line("\nНажмите Enter, чтобы продолжить");
            break;
        }
        case 3: {
            clearConsole();
            handleSearchCStationsByName();
            read_line("\nНажмите Enter, чтобы продолжить");
            break;
        }
        case 4: {
            clearConsole();
            handleSearchCStationsByActive();
            read_line("\nНажмите Enter, чтобы продолжить");
            break;
        }
        case 0:
            return;
        default: {
            std::cout << "[*] Ошибка: Нет такого пункта меню.\n";
            read_line("\nНажмите Enter, чтобы продолжить");
            break;
        }
    }
}

// Редактирование
void Console::handleEditPipe() {
    // Проверка на наличие труб
    if (network.getPipeMap().empty()) {
        read_line("[!] Внимание: Нет труб для редактирования.\nНажмите Enter, чтобы вернуться в меню");
        return;
    }

    const auto ids = read_multiple_int("ID труб через пробел");

    // Редактирование
    int edited = 0;

    for (int id : ids) {
        if (!network.getPipeMap().contains(id)) {
            std::cout << "[!] Внимание: Труба ID=" << id << " не найдена, она была пропущена.\n";
            continue;
        }

        // Чтение нового значения ремонта
        const bool repair = read_bool("Труба ID=" + std::to_string(id) + " в ремонте?", network.getPipeMap().at(id).getRepair());

        // Если редактирование успешно, выводим сообщение об этом
        if (network.editPipe(id, repair)) {
            ++edited;

            std::cout << "Труба ID=" << id << ": статус ремонта установлен - " << (repair ? "в ремонте" : "не в ремонте") << '\n';
        }
    }

    std::cout << "Обработано труб: " << edited << " из " << ids.size() << '\n';
    read_line("\nНажмите Enter, чтобы вернуться в меню");
}

void Console::handleEditCStation() {
    // Проверка на наличие КС
    if (network.getCStationMap().empty()) {
        read_line("[!] Внимание: Нет КС для редактирования.\nНажмите Enter, чтобы вернуться в меню");
        return;
    }

    const auto ids = read_multiple_int("ID КС через пробел");

    // Редактирование
    int edited = 0;

    for (int id : ids) {
        if (!network.getCStationMap().contains(id)) {
            std::cout << "[!] Внимание: КС ID=" << id << " не найдена, она была пропущена.\n";
            continue;
        }

        // Чтение нового числа активных цехов
        const auto& station = network.getCStationMap().at(id);
        const int active = read_int("КС ID=" + std::to_string(id) + " активных цехов", station.getNumActiveWorkshops(), 0, station.getNumWorkshops());

        // Если редактирование успешно, выводим сообщение об этом
        if (network.editCStation(id, active)) {
            ++edited;

            std::cout << "КС ID=" << id << ": установлено активных цехов - " << active << '\n';
        }
    }

    std::cout << "Обработано КС: " << edited << " из " << ids.size() << '\n';
    read_line("\nНажмите Enter, чтобы вернуться в меню");
}

void Console::handleDeletePipe() {
    // Проверка на наличие труб
    if (network.getPipeMap().empty()) {
        read_line("[!] Внимание: Нет труб для удаления.\nНажмите Enter, чтобы вернуться в меню");
        return;
    }

    // Чтение ID труб для удаления
    const auto ids = read_multiple_int("ID труб для удаления через пробел");

    // Удаление выбранных труб и вывод результата
    for (int id : ids) {
        const bool removed = network.deletePipe(id);

        std::cout << (removed ? "" : "[!] Внимание: ") << "Труба ID=" << id << (removed ? " удалена" : " не найдена, удаление пропущено") << '\n';
    }

    read_line("\nНажмите Enter, чтобы вернуться в меню");
}

void Console::handleDeleteCStation() {
    // Проверка на наличие КС
    if (network.getCStationMap().empty()) {
        read_line("[!] Внимание: Нет КС для удаления.\nНажмите Enter, чтобы вернуться в меню");
        return;
    }

    // Чтение ID КС для удаления
    const auto ids = read_multiple_int("ID КС для удаления через пробел");

    // Удаление выбранных КС и отсоединение связанных труб
    for (int id : ids) {
        const bool removed = network.deleteCStation(id);
        std::cout << (removed ? "" : "[!] Внимание: ") << "КС ID=" << id << (removed ? " удалена; связанные трубы отсоединены" : " не найдена, удаление пропущено") << '\n';
    }

    read_line("\nНажмите Enter, чтобы вернуться в меню");
}

void Console::handleSave() {
    clearConsole();

    while (true) {
        // Чтение пути с именем файла по умолчанию
        const std::string path = read_line("Путь к файлу сети", "data/network_" + getTimestamp() + ".txt");

        // Сохранение сети, при ошибке повторяем ввод пути
        if (network.saveToFile(path)) {
            std::cout << "Трубы и КС сохранены в " << path << '\n';
            break;
        }

        std::cerr << "[*] Ошибка: Не удалось сохранить сеть. Проверьте путь и доступ к файлу.\n";
    }

    read_line("\nНажмите Enter, чтобы вернуться в меню");
}

void Console::handleLoad() {
    clearConsole();

    while (true) {
        // Чтение пути к файлу сети
        const std::string path = read_line("Путь к файлу сети");

        // Замена текущей сети данными из файла при успешной загрузке
        if (network.loadFromFile(path)) {
            std::cout << "Текущая сеть заменена данными из " << path << '\n';
            break;
        }

        std::cerr << "[*] Ошибка: Проверьте путь и формат файла. Текущая сеть не изменена.\n";
    }

    read_line("\nНажмите Enter, чтобы вернуться в меню");
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
        if (!read_bool("[!] Внимание: Свободная труба нужного диаметра не найдена. Создать и присоединить?", false)) {
            read_line("\nНажмите Enter, чтобы вернуться в меню");
            return;
        }

        const std::string defaultName = "Pipe_" + std::to_string(network.getNextPipeId());
        const int length = read_int("Длина (км)", 100, 1);
        const std::string name = read_valid_name("Название", defaultName);

        if (!network.addPipe(targetDiameter, length, name, false)) {
            std::cerr << "[*] Ошибка: Не удалось создать трубу.\n";
            read_line("\nНажмите Enter, чтобы вернуться в меню");
            return;
        }

        // После создания повторяем поиск по диаметру.
    }

    // Чтение ID станций начала и конца трубы
    const int CStationIdFrom = read_int("ID КС начала трубы");
    const int CStationIdTo = read_int("ID КС конца трубы");

    // Проверка на наличие таких Id
    if (!network.getCStationMap().contains(CStationIdFrom) || !network.getCStationMap().contains(CStationIdTo)) {
        std::cerr << "[*] Ошибка: Одна или обе компрессорные станции не найдены.\n";

        read_line("\nНажмите Enter, чтобы вернуться в меню");
        return;
    }

    // Проверка на то, что Id КС не совпадают или равны -1
    if (CStationIdFrom == CStationIdTo || CStationIdFrom == -1 || CStationIdTo == -1) {
        std::cerr << "[*] Ошибка: Начало и конец трубы должны быть разными КС.\n";

        read_line("\nНажмите Enter, чтобы вернуться в меню");
        return;
    }

    // Присоединение трубы к выбранным КС
    if (!network.connectPipe(selectedPipeId, CStationIdFrom, CStationIdTo)) {
        std::cerr << "[*] Ошибка: Выбранную трубу не удалось присоединить.\n";
        read_line("\nНажмите Enter, чтобы вернуться в меню");
        return;
    }

    std::cout << "Труба ID=" << selectedPipeId << " соединяет КС " << CStationIdFrom << " -> КС " << CStationIdTo << "\n";
    read_line("\nНажмите Enter, чтобы вернуться в меню");
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
        std::cout << "[!] Внимание: Трубы с таким именем не найдены.\n";
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
        std::cout << "[!] Внимание: Трубы с указанным статусом не найдены.\n";
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
        std::cout << "[!] Внимание: КС с таким именем не найдены.\n";
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
        std::cout << "[!] Внимание: КС, подходящие под условие, не найдены.\n";
        return;
    }

    // Вывод найденных КС в таблице
    CompressorStation::printCStationTableHeader();

    for (const int id : CStationList) {
        network.getCStationMap().at(id).printCStation();
    }
}

void Console::handleTopologicalSort() {
    std::vector<int> result;

    // Сортировка КС, при наличии цикла выводим ошибку
    if (!network.topologicalSort(result)) {
        std::cerr << "[*] Ошибка: В графе присутствует цикл - сортировка невозможна.\n";
        read_line("\nНажмите Enter, чтобы вернуться в меню");
        return;
    }

    // Вывод КС в полученном порядке
    std::cout << "Топологически отсортированный массив:\n";
    for (const auto& id : result) {
        std::cout << id << " ";
    }

    read_line("\nНажмите Enter, чтобы вернуться в меню");
}

void Console::handleFindShortestPath() {
    std::vector<Edge> result;

    // Чтение ID начальной и конечной КС
    int startId = read_int("Введите ID стартовой КС");
    int finishId = read_int("Введите ID конечной КС");

    // Проверка на наличие КС и разные ID
    if (!network.getCStationMap().contains(startId) || !network.getCStationMap().contains(finishId)) {
        std::cerr << "[*] Ошибка: Одна или обе компрессорные станции не найдены.\n";
        read_line("\nНажмите Enter, чтобы вернуться в меню");
        return;
    } else if (startId == finishId) {
        std::cerr << "[*] Ошибка: ID КС совпадают.\n";
        read_line("\nНажмите Enter, чтобы вернуться в меню");
        return;
    }

    // Поиск кратчайшего пути между выбранными КС
    if (!network.findShortestPath(startId, finishId, result)) {
        std::cerr << "[*] Ошибка: Не удалось построить кратчайший путь.\n";
        read_line("\nНажмите Enter, чтобы вернуться в меню");
        return;
    }

    int totalDistance = 0;

    // Вывод последовательности КС в маршруте
    std::cout << "[Маршрут]\n";
    for (size_t i = 0; i != result.size(); ++i) {
        std::cout << result[i].id;

        if (i + 1 < result.size()) {
            std::cout << " -> ";
        }
    }

    // Вывод участков маршрута и подсчёт общей длины
    std::cout << "\n\n[Участки маршрута]\n";
    for (size_t i = 1; i != result.size(); ++i) {
        std::cout << std::format("КС {} -> КС {:<4} |   Длина: {} км\n", result[i - 1].id, result[i].id, result[i].distance);

        totalDistance += result[i].distance;
    }

    std::cout << "\nОбщая длина пути: " << totalDistance << " км\n";
    read_line("\nНажмите Enter, чтобы вернуться в меню");
}

/*
Принтеры
*/

void Console::printMenu() const {
    clearConsole();

    // Вывод пунктов главного меню
    for (std::size_t i = 0; i < mainMenuItems.size(); ++i) {
        std::cout << std::format("{:<4}{}\n", std::format("{}.", i + 1), mainMenuItems[i]);
    }

    std::cout << std::format("{:<4}{}\n", "0.", "Выход");
}

void Console::printMenuViewAll() const {
    // Вывод фильтров поиска
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
                    std::cout << "[*] Ошибка: Нет такого пункта меню.\n";
                    read_line("\nНажмите Enter, чтобы вернуться в меню");
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
