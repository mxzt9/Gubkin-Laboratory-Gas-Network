#include <iostream>
#include <sstream>
#include <vector>
#include <set>
#include <format>

#include "console.hpp"
#include "units.hpp"
#include "utils.hpp"


/*



Базовые методы работы с консолью



*/

void Console::logAction(const std::string& message) const {
    logger.addLine(getTimestamp(), message);
}

void Console::clearConsole() const {
    std::system("cls");
}

std::string Console::readLine(const std::string& prompt, std::string defaultParam) const {
    std::string value {};
    std::string fullPrompt = prompt;

    // Обновление промпта при наличии значения по умолчанию
    if (!defaultParam.empty()) {
        fullPrompt += " [Значение по умолчанию: " + defaultParam + "]: ";
    } else {
        fullPrompt += " "; 
    }

    std::cout << fullPrompt;

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

std::string Console::readValidName(const std::string& prompt, std::string defaultParam) const {
    while (true) {
        const std::string value = readLine(prompt, defaultParam);

        if (isValidName(value)) {
            return value;
        }

        std::cerr << "[*] Ошибка: название не должно быть пустым и содержать управляющие символы.\n";
    }
}

int Console::readInt(const std::string& prompt, int defaultParam, bool isPositive) const {
    std::string defaultStr = (defaultParam != -1) ? std::to_string(defaultParam) : "";

    while (true) {
        const std::string input = readLine(prompt, defaultStr);

        if (input.empty()) {
            std::cerr << "[*] Ошибка: Некорректный ввод\n";
            continue;
        }

        // Преобразование строки в int
        try {
            int value = std::stoi(input);

            if (value <= 0 && isPositive) {
                std::cerr << "[*] Ошибка: Введите число больше 0.\n";
                continue;
            }

            return value;
        } 
        catch (const std::invalid_argument&) {
            std::cerr << "[*] Ошибка: Строка не содержит число.\n";
        } 
        catch (const std::out_of_range&) {
            std::cerr << "[*] Ошибка: Число слишком большое для int.\n";
        }
    }
}

bool Console::readBool(const std::string& prompt, bool defaultParam) const {
    std::string defaultStr = defaultParam ? "y" : "n";
    std::string formattedPrompt = prompt + " (y/n)";

    while (true) {
        const std::string value = readLine(formattedPrompt, defaultStr);

        if (value == "y" || value == "Y") { return true; }
        if (value == "n" || value == "N") { return false; }

        std::cerr << "[*] Ошибка: введите y или n.\n";
    }
}

StationType Console::readStationType(StationType defaultParam) const {
    std::string prompt = "Класс станции (0 - Light, 1 - Medium, 2 - Heavy):";
    int defaultInt = static_cast<int>(defaultParam);

    while (true) {
        const int type = readInt(prompt, defaultInt);

        if (type >= 0 && type <= 2) { 
            return static_cast<StationType>(type); 
        }

        std::cerr << "[*] Ошибка: Введите 0, 1 или 2.\n";
    }
}

std::vector<int> Console::readMultipleChoice(const std::string& prompt) const {
    while (true) {
        const std::string input = readLine(prompt);

        if (input.empty()) {
            std::cerr << "[*] Ошибка: Некорректный ввод.\n";
            continue;
        }

        std::set<int> unique_ids;
        std::stringstream stringStream(input);
        std::string token;
        bool hasError = input.back() == ',';

        while (std::getline(stringStream, token, ',')) {
            if (token.empty()) {
                hasError = true;
                break;
            }

            try {
                unique_ids.insert(std::stoi(token));
            }
            catch (...) {
                hasError = true;
                break;
            }
        }

        if (!hasError && !unique_ids.empty()) {
            return std::vector<int>(unique_ids.begin(), unique_ids.end());
        }

        std::cerr << "[*] Ошибка: Некорректный ввод.\n";
    }
}

std::vector<char> Console::readActiveSearch(const std::string& prompt) const {
    while (true) {
        const std::string input = readLine(prompt);

        // Проверка на размер строки
        if (input.size() < 2) {
            std::cerr << "[*] Ошибка: Некорректный ввод\n";
            continue;
        }

        // Проверка наличия знаков сравнения
        if (input[0] != '>' && input[0] != '<' && input[0] != '=') {
            std::cerr << "[*] Ошибка: Условие должно начинаться с >, < или =\n";
            continue;
        }
        // Проверка на знак процента в конце строки
        const bool isPercent = input.back() == '%';
        // Индекс последней цифры
        const std::size_t numberEnd = isPercent ? input.size() - 1 : input.size();

        // Проверка на некорректный ввод вида (>%)
        if (numberEnd == 1) {
            std::cerr << "[*] Ошибка: После знака сравнения должно быть число\n";
            continue;
        }

        int value {};
        try {
            value = std::stoi(input.substr(1, numberEnd - 1));
        } 
        catch (const std::invalid_argument&) {
            std::cerr << "[*] Ошибка: Некорректно введено число.\n";
            continue;
        }
        catch (const std::out_of_range&) {
            std::cerr << "[*] Ошибка: Число слишком большое.\n";
            continue;
        }

        if (value < 0 || (isPercent && value > 100)) {
            std::cerr << "[*] Ошибка: Число должно быть неотрицательным, процент - от 0 до 100\n";
            continue;
        }

        return std::vector<char>(input.begin(), input.end());
    }
}

/*



Хэндлеры



*/

// Добавление 
void Console::handleAddPipe() {
    const int defaultNameNum = network.currentPipeId;
    const std::string defaultName = "Pipe_" + std::to_string(defaultNameNum);

    const int diameter = readInt("Диаметр (мм): ", 500, true);
    const int length = readInt("Длина (км): ", 100, true);
    const bool isRepair = readBool("В ремонте?", false);
    const std::string name = readValidName("Название: ", defaultName);
    const int num = readInt("Сколько труб добавить: ", 1, true);

    int added = 0;

    for (int i = 0; i < num; ++i) {
        const int id = network.currentPipeId;
        const std::string actualName = name == defaultName ? "Pipe_" + std::to_string(id) : name;
        if (!network.addPipe(diameter, length, actualName, isRepair)) {
            logAction("Ошибка добавления трубы: ID=" + std::to_string(id));
            std::cerr << "[!] Не удалось добавить трубу.\n";
            break;
        }

        ++added;

        logAction("Добавлена труба: ID=" + std::to_string(id) + "; имя=" + actualName
            + "; диаметр=" + std::to_string(diameter) + "; длина=" + std::to_string(length)
            + "; ремонт=" + (isRepair ? "да" : "нет"));
    }
    std::cout << "Добавлено труб: " << added << " из " << num << "\n";

    readLine("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleAddCS() {
    const int defaultNameNum = network.currentCStationId;
    const std::string defaultName = "CStation_" + std::to_string(defaultNameNum);

    const int numWorkers = readInt("Количество цехов: ", 10, true);
    int numActiveWorkers = readInt("Количество цехов в работе: ", numWorkers);

    while (numActiveWorkers < 0 || numActiveWorkers > numWorkers) {
        std::cout << "[*] Ошибка: Количество активных цехов должно быть от 0 до общего числа цехов.\n";
        numActiveWorkers = readInt("Количество цехов в работе: ", numWorkers);
    }

    const std::string name = readValidName("Название: ", defaultName);
    const StationType type = readStationType(StationType::Light);
    const int num = readInt("Сколько КС добавить: ", 1, true);

    int added = 0;
    for (int i = 0; i < num; ++i) {
        const int id = network.currentCStationId;

        const std::string actualName = name == defaultName ? "CStation_" + std::to_string(id) : name;

        if (!network.addCStation(numWorkers, numActiveWorkers, actualName, type)) {
            logAction("Ошибка добавления КС: ID=" + std::to_string(id));
            std::cerr << "[!] Не удалось добавить КС.\n";
            break;
        }

        ++added;

        logAction("Добавлена КС: ID=" + std::to_string(id) + "; имя=" + actualName
            + "; цехов=" + std::to_string(numWorkers) + "; активных=" + std::to_string(numActiveWorkers)
            + "; класс=" + std::to_string(static_cast<int>(type)));
    }
    std::cout << "Добавлено КС: " << added << " из " << num << "\n";
    readLine("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleViewAll() {
    logAction("Просмотрены все объекты");

    printPipeTableHeader();

    for (const auto& [pipe_id, pipe] : network.getPipeMap()) {
        printPipe(pipe_id, pipe);
    }

    printCStationTableHeader();

    for (const auto& [CStation_id, CStation] : network.getCStationMap()) {
        printCS(CStation_id, CStation);
    }

    std::cout << "\n\n";


    printMenuViewAll();
    switch (readInt("Выбор: ")) {
        case 1: {
            clearConsole();
            handleSearchPipesByName();
            readLine("\nНажмите Enter, чтобы продолжить...");
            break;
        }
        case 2: {
            clearConsole();
            handleSearchPipesByRepair();
            readLine("\nНажмите Enter, чтобы продолжить...");
            break;
        }
        case 3: {
            clearConsole();
            handleSearchCStationsByName();
            readLine("\nНажмите Enter, чтобы продолжить...");
            break;
        }
        case 4: {
            clearConsole();
            handleSearchCStationsByActive();
            readLine("\nНажмите Enter, чтобы продолжить...");
            break;
        }
        case 0:
            return;
        default: {
            logAction("Выбран несуществующий пункт меню просмотра");
            std::cout << "Ошибка: Нет такого пункта меню.\n";
            readLine("\nНажмите Enter, чтобы продолжить...");
            break;
        }
    }
}


// Редактирование
void Console::handleEditPipe() {
    if (network.getPipeMap().empty()) {
        logAction("Редактирование труб невозможно: список пуст");
        readLine("Нет труб для редактирования. Нажмите Enter, чтобы вернуться в меню...");
        return;
    }

    const auto ids = readMultipleChoice("ID труб через запятую: ");

    int edited = 0;

    for (int id : ids) {
        if (!network.getPipeMap().contains(id)) {
            std::cout << "Труба ID=" << id << " не найдена, пропущена.\n";

            logAction("Редактирование пропущено: труба ID=" + std::to_string(id) + " не найдена");
            continue;
        }

        const bool repair = readBool("Труба ID=" + std::to_string(id) + " в ремонте?", network.getPipeMap().at(id).getRepair());
        
        if (network.editPipe(id, repair)) {
            ++edited;

            const std::string message = "Труба ID=" + std::to_string(id) + ": статус ремонта установлен - " + (repair ? "в ремонте" : "не в ремонте");

            logAction(message);
            std::cout << message << '\n';
        }
    }
    std::cout << "Обработано труб: " << edited << " из " << ids.size() << '\n';
    readLine("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleEditCStation() {
    if (network.getCStationMap().empty()) {
        logAction("Редактирование КС невозможно: список пуст");
        readLine("Нет КС для редактирования. Нажмите Enter, чтобы вернуться в меню...");
        return;
    }

    const auto ids = readMultipleChoice("ID КС через запятую: ");

    int edited = 0;
    
    for (int id : ids) {
        if (!network.getCStationMap().contains(id)) {
            std::cout << "КС ID=" << id << " не найдена, пропущена.\n";
            logAction("Редактирование пропущено: КС ID=" + std::to_string(id) + " не найдена");
            continue;
        }

        const auto& station = network.getCStationMap().at(id);
        int active;

        do {
            active = readInt("КС ID=" + std::to_string(id) + ": активных цехов (от 0 до " + std::to_string(station.getNumWorkers()) + "):", station.getNumActiveWorkers());
            
            if (active < 0 || active > station.getNumWorkers()) {
                std::cerr << "[!] Количество активных цехов вне допустимого диапазона.\n";
            }

        } while (active < 0 || active > station.getNumWorkers());
        
        if (network.editCStation(id, active)) {
            ++edited;

            const std::string message = "КС ID=" + std::to_string(id) + ": установлено активных цехов - " + std::to_string(active);
            
            logAction(message);
            std::cout << message << '\n';
        }
    }
    std::cout << "Обработано КС: " << edited << " из " << ids.size() << '\n';
    readLine("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleDeletePipe() {
    if (network.getPipeMap().empty()) {
        logAction("Удаление труб невозможно: список пуст");
        readLine("Нет труб для удаления. Нажмите Enter, чтобы вернуться в меню...");
        return;
    }

    const auto ids = readMultipleChoice("ID труб для удаления через запятую: ");

    for (int id : ids) {
        const bool removed = network.deletePipe(id);
        
        const std::string message = "Труба ID=" + std::to_string(id) + (removed ? " удалена" : " не найдена, удаление пропущено");

        logAction(message);
        std::cout << message << '\n';
    }
    readLine("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleDeleteCS() {
    if (network.getCStationMap().empty()) {
        logAction("Удаление КС невозможно: список пуст");
        readLine("Нет КС для удаления. Нажмите Enter, чтобы вернуться в меню...");
        return;
    }

    const auto ids = readMultipleChoice("ID КС для удаления через запятую: ");

    for (int id : ids) {
        const bool removed = network.deleteCStation(id);
        const std::string message = "КС ID=" + std::to_string(id) + (removed ? " удалена; связанные трубы отсоединены" : " не найдена, удаление пропущено");

        logAction(message);
        std::cout << message << '\n';
    }
    readLine("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleSave() {
    clearConsole();

    while (true) {
        const std::string path = readLine("Путь к файлу сети:", "data/network_" + getTimestamp() + ".txt");

        if (network.saveToFile(path)) {
            logAction("Сеть сохранена: файл=" + path);
            std::cout << "Трубы и КС сохранены в " << path << '\n';
            break;
        }

        logAction("Ошибка сохранения сети: файл=" + path);
        std::cerr << "[!] Не удалось сохранить сеть. Проверьте путь и доступ к файлу.\n";
    }
    readLine("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleLoad() {
    clearConsole();

    while (true) {
        const std::string path = readLine("Путь к файлу сети:");

        if (network.loadFromFile(path)) {
            logAction("Сеть загружена: файл=" + path);
            std::cout << "Текущая сеть заменена данными из " << path << '\n';
            break;
        }
        
        logAction("Ошибка загрузки сети: файл=" + path);
        std::cerr << "[!] Проверьте путь и формат файла. Текущая сеть не изменена.\n";
    }
    readLine("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleConnectPipe() {
    clearConsole();

    int CStationIdFrom = readInt("ID КС начала трубы: ");
    int CStationIdTo = readInt("ID КС конца трубы: ");

    const int targetDiameter = readInt("Диаметр трубы: ", -1, true);

    // Проверка на наличие таких Id
    if (!network.getCStationMap().contains(CStationIdFrom) || !network.getCStationMap().contains(CStationIdTo)) {
        logAction("Ошибка присоединения трубы: одна или обе КС не найдены; начало=" + std::to_string(CStationIdFrom) + "; конец=" + std::to_string(CStationIdTo));
        std::cerr << "[!] Ошибка: одна или обе компрессорные станции не найдены\n";
        
        readLine("\nНажмите Enter, чтобы вернуться в меню...");
        return;
    } 

    // Проверка на то, что Id КС не совпадают или равны -1
    if (CStationIdFrom == CStationIdTo || CStationIdFrom == -1 || CStationIdTo == -1) {
        logAction("Ошибка присоединения трубы: некорректная пара КС; начало=" + std::to_string(CStationIdFrom) + "; конец=" + std::to_string(CStationIdTo));
        std::cerr << "[*] Ошибка: начало и конец трубы должны быть разными КС\n";
        
        readLine("\nНажмите Enter, чтобы вернуться в меню...");
        return;
    }

    // Поиск свободной трубы нужного диаметра
    for (const auto& [pipeId, pipe] : network.getPipeMap()) {
        if (pipe.getDiameter() == targetDiameter && !(pipe.getCStationFromId() != -1 && pipe.getCStationToId() != -1)) {
            const int currentPipeId = pipeId;

            // Проверка на ошибку добавления
            if (!network.connectPipe(currentPipeId, CStationIdFrom, CStationIdTo)) {
                logAction("Ошибка присоединения существующей трубы: ID=" + std::to_string(currentPipeId));
                std::cerr << "[*] Ошибка: выбранную трубу не удалось присоединить\n";

                break;
            };

            logAction("Труба присоединена: ID=" + std::to_string(currentPipeId) + "; КС " + std::to_string(CStationIdFrom) + " -> " + std::to_string(CStationIdTo));
            std::cout << "Труба ID=" << currentPipeId << " соединяет КС " << CStationIdFrom << " -> КС " << CStationIdTo << "\n";

            readLine("\nНажмите Enter, чтобы вернуться в меню...");
            return; 
        }
    }

    
    bool choice = readBool("[!] Свободная труба нужного диаметра не найдена. Создать и присоединить?: ", false);
    
    if (!choice) {
        logAction("Создание трубы для присоединения отменено: диаметр=" + std::to_string(targetDiameter));
        readLine("\nНажмите Enter, чтобы вернуться в меню...");

        return;
    }

    const int defaultNameNum = network.currentPipeId;
    const std::string defaultName = "Pipe_" + std::to_string(defaultNameNum);

    const int length = readInt("Длина (км): ", 100, true);
    const std::string name = readValidName("Название: ", defaultName);

    const int newPipeId = network.getNextPipeId();

    if (!network.addPipe(targetDiameter, length, name, false, newPipeId) || !network.connectPipe(newPipeId, CStationIdFrom, CStationIdTo)) {
        logAction("Ошибка создания и присоединения трубы: ID=" + std::to_string(newPipeId));
        std::cerr << "[*] Ошибка: не удалось создать и присоединить трубу\n";

        readLine("\nНажмите Enter, чтобы вернуться в меню...");
        return;
    }
    
    logAction("Труба создана и присоединена: ID=" + std::to_string(newPipeId) + "; КС " + std::to_string(CStationIdFrom) + " -> " + std::to_string(CStationIdTo));
    std::cout << "Создана труба ID=" << newPipeId << ", соединяющая КС " << CStationIdFrom << " -> КС " << CStationIdTo << "\n";
    
    readLine("\nНажмите Enter, чтобы вернуться в меню...");
}

/*



Хэндлеры поиска



*/

void Console::handleSearchPipesByName() const {
    const std::string name = readLine("\nВведите начало названия для поиска: ");
    const std::vector<int> pipeList = network.searchPipesByName(name);
    logAction("Поиск труб по имени: запрос=" + name
        + "; найдено=" + std::to_string(pipeList.size()));

        
    if (pipeList.empty()) {
        std::cout << "[!] Трубы с таким именем не найдены.\n";
        return;
    }

    printPipeTableHeader();

    for (const int id : pipeList) {
        printPipe(id, network.getPipeMap().at(id));
    }
}

void Console::handleSearchPipesByRepair() const {
    const bool repairStatus = readBool("В ремонте?", false);
    const std::vector<int> pipeList = network.searchPipesByRepair(repairStatus);
    logAction("Поиск труб по ремонту: ремонт=" + std::string(repairStatus ? "да" : "нет")
        + "; найдено=" + std::to_string(pipeList.size()));

        
    if (pipeList.empty()) {
        std::cout << "[!] Трубы с указанным статусом не найдены.\n";
        return;
    }

    printPipeTableHeader();

    for (const int id : pipeList) {
        printPipe(id, network.getPipeMap().at(id));
    }
};

void Console::handleSearchCStationsByName() const {
    const std::string name = readLine("\nВведите начало названия для поиска: ");
    const std::vector<int> CStationList = network.searchCStationsByName(name);
    logAction("Поиск КС по имени: запрос=" + name
        + "; найдено=" + std::to_string(CStationList.size()));

        
    if (CStationList.empty()) {
        std::cout << "[!] КС с таким именем не найдены.\n";
        return;
    }

    printCStationTableHeader();

    for (const int id : CStationList) {
        printCS(id, network.getCStationMap().at(id));
    }

}

void Console::handleSearchCStationsByActive() const {
    const std::vector<char> condition = readActiveSearch(
        "Введите условие поиска (>50%, <5, =10): "
    );

    const std::vector<int> CStationList = network.searchCStationsByActive(condition);

    if (CStationList.empty()) {
        std::cout << "[!] КС, подходящие под условие, не найдены.\n";
        return;
    }
    
    logAction("Поиск КС по активным цехам: условие="
        + std::string(condition.begin(), condition.end())
        + "; найдено=" + std::to_string(CStationList.size()));

    printCStationTableHeader();


    for (const int id : CStationList) {
        printCS(id, network.getCStationMap().at(id));
    }
}

void Console::handleTopologicalSort() {
    std::vector<int> result;

    if (!network.topologicalSort(result)) {
        std::cerr << "[*] Ошибка: В графе присутствует петля - сортировка невозможна";
        readLine("\nНажмите Enter, чтобы вернуться в меню...");
        return;
    }

    std::cout << "Топологически отсортированный массив:\n";
    for (const auto& id : result) {
        std::cout << id << " ";
    }
    readLine("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleFindShortestPath() {
    std::vector<Edge> result;

    int startId = readInt("Введите ID стартовой КС: ");
    int finishId = readInt("Введите ID конечной КС: ");

    if (!network.getCStationMap().contains(startId) || !network.getCStationMap().contains(finishId)) {
        std::cerr << "[!] Ошибка: одна или обе компрессорные станции не найдены\n";
        readLine("\nНажмите Enter, чтобы вернуться в меню...");
        return;
    } else if (startId == finishId) {
        std::cerr << "[!] Ошибка: ID КС совпадают\n";
        readLine("\nНажмите Enter, чтобы вернуться в меню...");
        return;
    }


    if (!network.findShortestPath(startId, finishId, result)) {
        std::cerr << "[*] Ошибка: не удалось построить кратчайший путь\n";
        readLine("\nНажмите Enter, чтобы вернуться в меню...");
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
        std::cout << std::format("КС {} -> КС {} | Длина: {}\n",
            result[i - 1].id,
            result[i].id,
            result[i].distance
        );

        totalDistance += result[i].distance;
    }

    std::cout << "\nОбщая длина пути: " << totalDistance << '\n';
    readLine("\nНажмите Enter, чтобы вернуться в меню...");
}

/*



Принтеры



*/

void Console::printMenu() const {
    clearConsole();

    for (std::size_t i = 0; i < mainMenuItems.size(); ++i) {
        std::cout << i + 1 << ". " << mainMenuItems[i] << '\n';
    }

    std::cout << "0. Выход\n";
}

void Console::printMenuViewAll() const {
    for (std::size_t i = 0; i < viewAllMenuItems.size(); ++i) {
        std::cout << i + 1 << ". " << viewAllMenuItems[i] << '\n';
    }

    std::cout << "0. Выход\n";
};

void Console::printPipeTableHeader() const {
    std::cout << "\n                                    [Трубы]                                       ";
    std::cout << "\n───────┬────────────┬──────────┬──────────┬──────────┬──────────────┬───────────\n";
    std::cout << std::format("{:<6} │ {:<10} │ {:<8} │ {:<8} │ {:<8} │ {:<12} │ {}\n",
        "Id", "Diameter", "Length", "Repair", "Name", "ID CS From", "ID CS To");
    std::cout <<   "───────┼────────────┼──────────┼──────────┼──────────┼──────────────┼───────────\n";
    
}

void Console::printCStationTableHeader() const {
    std::cout << "\n                            [Компрессорные станции]                               ";
    std::cout << "\n───────┬────────────┬──────────┬──────────┬─────────────────────────────────────\n";
    std::cout << std::format("{:<6} │ {:<10} │ {:<8} │ {:<8} │ {}\n",
        "Id", "Workshops", "Active", "Type", "Name");
    std::cout << "───────┼────────────┼──────────┼──────────┼─────────────────────────────────────\n";
    
}

void Console::printPipe(int id, const Pipe& pipe) const {
    std::cout << std::format("{:<6} │ {:<10} │ {:<8} │ {:<8} │ {:<8} │ {:<12} │ {}\n",
        id,
        pipe.getDiameter(),
        pipe.getLength(),
        pipe.getRepair() ? "yes" : "no",
        pipe.getName(),
        pipe.getCStationFromId() == -1 ? "None" : std::to_string(pipe.getCStationFromId()),
        pipe.getCStationToId() == -1 ? "None" : std::to_string(pipe.getCStationToId()));
}

void Console::printCS(int id, const CompressorStation& CStation) const {
    std::string type;

    if (CStation.getStationType() == StationType::Light) { type = "Light"; }
    else if (CStation.getStationType() == StationType::Medium) { type = "Medium"; }
    else { type = "Heavy"; }

    std::cout << std::format("{:<6} │ {:<10} │ {:<8} │ {:<8} │ {}\n",
        id,
        CStation.getNumWorkers(),
        CStation.getNumActiveWorkers(),
        type,
        CStation.getName());
}

/*



Запуск



*/

void Console::run() {
    bool running = true;
    logAction("Программа запущена");

    while (running) {
        printMenu();

        try {
            switch (readInt("\nExit - в любом действии, чтобы вернуться в меню.\nВыбор: ", -1)) {
                case 1:  clearConsole(); handleAddPipe(); break;
                case 2:  clearConsole(); handleAddCS(); break;
                case 3:  clearConsole(); handleViewAll(); break;
                case 4:  clearConsole(); handleEditPipe(); break;
                case 5:  clearConsole(); handleEditCStation(); break;
                case 6:  clearConsole(); handleDeletePipe(); break;
                case 7:  clearConsole(); handleDeleteCS(); break;
                case 8:  clearConsole(); handleSave(); break;
                case 9:  clearConsole(); handleLoad(); break;
                case 10: clearConsole(); handleConnectPipe(); break;
                case 11: clearConsole(); handleTopologicalSort(); break;
                case 12: clearConsole(); handleFindShortestPath(); break;
                case 0:
                    logAction("Выход из программы");
                    running = false;
                    break;
                default:
                    logAction("Выбран несуществующий пункт главного меню");
                    std::cout << "Ошибка: Нет такого пункта меню.\n";
                    readLine("\nНажмите Enter, чтобы вернуться в меню...");
                    break;
            }
        } catch (InputCommand) {
            if (!std::cin) {
                logAction("Выход из программы: поток ввода закрыт");
                running = false;
            } else {
                logAction("Возврат в меню по команде Exit; выполненные изменения сохранены");
            }
        }
    }
}
