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

        std::cerr << "[*] Ошибка: название не должно быть пустым и содержать запятые или управляющие символы.\n";
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
        bool hasError = false;

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
            std::cout << "Некорректный ввод\n";
            continue;
        }

        // Проверка наличия знаков сравнения
        if (input[0] != '>' && input[0] != '<' && input[0] != '=') {
            std::cout << "Условие должно начинаться с >, < или =\n";
            continue;
        }
        // Проверка на знак процента в конце строки
        const bool isPercent = input.back() == '%';
        // Индекс последней цифры
        const size_t numberEnd = isPercent ? input.size() - 1 : input.size();

        // Проверка на некорректный ввод вида (>%)
        if (numberEnd == 1) {
            std::cout << "После знака сравнения должно быть число\n";
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

        if (isPercent && value > 100) {
            std::cout << "[*] Ошибка: Процент должен быть от 0 до 100\n";
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
    clearConsole();

    const int defaultNameNum = network.getCurrentPipeId();
    const std::string defaultName = "Pipe_" + std::to_string(defaultNameNum);

    const int diameter = readInt("Диаметр (мм): ", 500, true);
    const int length = readInt("Длина (км): ", 100, true);
    const bool isRepair = readBool("В ремонте?", false);
    const std::string name = readValidName("Название: ", defaultName);
    const int num = readInt("Сколько труб добавить: ", 1, true);

    for (int i = 0; i < num; ++i) {
        if (name == defaultName) {
            network.addPipe(diameter, length, "Pipe_" + std::to_string(network.getCurrentPipeId()), isRepair);
        } else {
            network.addPipe(diameter, length, name, isRepair);
        }
    }

    if (num == 1) {
        std::cout << "\n\n[Труба добавлена]\n";
    }
    else {
        std::cout << "\n\n[Трубы добавлены]\n";
    }

    readLine("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleAddCS() {
    clearConsole();

    const int defaultNameNum = network.getCurrentCStationId();
    const std::string defaultName = "CStation_" + std::to_string(defaultNameNum);

    const int numWorkers = readInt("Количество цехов: ", 10, true);
    int numActiveWorkers = readInt("Количество цехов в работе: ", numWorkers);

    while (numActiveWorkers > numWorkers) {
        std::cout << "[*] Ошибка: Количество активных цехов не может превышать общее.\n";
        numActiveWorkers = readInt("Количество цехов в работе: ", numWorkers);
    }

    const std::string name = readValidName("Название: ", defaultName);
    const StationType type = readStationType(StationType::Light);
    const int num = readInt("Сколько КС добавить: ", 1, true);

    for (int i = 0; i < num; ++i) {
        if (name == defaultName) {
            network.addCStation(numWorkers, numActiveWorkers, "CStation_" + std::to_string(network.getCurrentCStationId()), type);
        } else {
            network.addCStation(numWorkers, numActiveWorkers, name, type);
        }
    }

    if (num == 1) {
        std::cout << "\n\n[КС добавлена]\n";
    }
    else {
        std::cout << "\n\n[КС добавлены]\n";
    }

    readLine("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleViewAll() {
    clearConsole();

    std::cout << "\n────────────────────────────────────[Трубы]─────────────────────────────────────\n";
    printPipeTableHeader();

    for (const auto& pipe : network.getPipeArray()) {
        printPipe(pipe);
    }

    std::cout << "\n\n────────────────────────────[Компрессорные станции]─────────────────────────────\n";
    printCSTableHeader();

    for (const auto& station : network.getCStationArray()) {
        printCS(station);
    }

    std::cout << "\n\n";



    printMenuViewAll();

    switch (readInt("Выбор: ")) {
        case 1: {
            handleSearchPipesByName();
            readLine("\nНажмите Enter, чтобы продолжить...");
            break;
        }
        case 2: {
            handleSearchPipesByRepair();
            readLine("\nНажмите Enter, чтобы продолжить...");
            break;
        }
        case 3: {
            handleSearchCStationsByName();
            readLine("\nНажмите Enter, чтобы продолжить...");
            break;
        }
        case 4: {
            handleSearchCStationsByActive();
            readLine("\nНажмите Enter, чтобы продолжить...");
            break;
        }
        case 0: return;
        default: {
            std::cout << "Ошибка: Нет такого пункта меню.\n";
            readLine("\nНажмите Enter, чтобы продолжить...");
            break;
        }
    }
}


// Редактирование
void Console::handleEditPipe() {
    clearConsole();

    // Проверка на наличие труб
    if (network.getPipeArrayLen() == 0) {
        readLine("[!] Нет труб для редактирования.\nНажмите Enter, чтобы вернуться в меню...");
        return;
    }

    // Чтение выбора труб для редактирования
    const std::vector<int> ids = readMultipleChoice("ID труб/трубы (В форматах 1/1,2,3): ");

    // Проверка на наличие труб с такими Id
    for (const int id : ids) {
        if (!network.isPipeInArrayById(id)) {
            std::cout << "[*] Ошибка: Нет трубы с ID=" << id << "\n";
            readLine("\nНажмите Enter, чтобы вернуться в меню...");
            return;
        }
    }

    // Чтение прошлых и новых значений Трубы с первым Id
    const Pipe& oldPipe = network.getPipeById(ids[0]);

    const int diameter = readInt("Новый диаметр (мм): ", oldPipe.getDiameter(), true);
    const int length = readInt("Новая длина (км): ", oldPipe.getLength(), true);
    const bool isRepair = readBool("В ремонте?", oldPipe.getRepair());

    // Чтение новых имён
    std::vector<std::string> names;

    for (const int id : ids) {
        const Pipe& pipe = network.getPipeById(id);
        names.push_back(readValidName("Название трубы ID=" + std::to_string(id) + ": ", pipe.getName()));
    }

    // Редактирование
    for (size_t i = 0; i != ids.size(); ++i) {
        network.editPipe(ids[i], diameter, length, names[i], isRepair);
    }

    std::cout << "\n[Трубы отредактированы]\n";
    readLine("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleEditCStation() {
    clearConsole();

    // Проверка на наличие КС
    if (network.getCStationArrayLen() == 0) {
        readLine("[!] Нет КС для редактирования.\nНажмите Enter, чтобы вернуться в меню...");
        return;
    }

    // Проверка на наличие КС с такими Id
    const std::vector<int> ids = readMultipleChoice("ID КС (В форматах 1/1,2,3): ");

    for (const int id : ids) {
        if (!network.isCStationInArrayById(id)) {
            std::cout << "Ошибка: Нет КС с ID=" << id << "\n";
            readLine("\nНажмите Enter, чтобы вернуться в меню...");
            return;
        }
    }

    // Чтение прошлых и новых значений КС с первым Id
    const CompressorStation& oldCStation = network.getCStationById(ids[0]);

    const int numWorkers = readInt("Количество цехов: ", oldCStation.getNumWorkers(), true);
    int numActiveWorkers = readInt("Количество цехов в работе: ", oldCStation.getNumActiveWorkers());

    while (numActiveWorkers > numWorkers) {
        std::cout << "Количество активных цехов не может превышать общее.\n";
        numActiveWorkers = readInt("Количество цехов в работе: ", numWorkers);
    }
    
    const StationType type = readStationType(oldCStation.getStationType());

    // Чтение новых имён
    std::vector<std::string> names;

    for (const int id : ids) {
        const CompressorStation& station = network.getCStationById(id);
        names.push_back(readValidName("Название КС ID=" + std::to_string(id) + ": ", station.getName()));
    }

    // Редактирование
    for (size_t i = 0; i != ids.size(); ++i) {
        network.editCStation(ids[i], numWorkers, numActiveWorkers, names[i], type);
    }

    std::cout << "\n[КС отредактированы]\n";
    readLine("\nНажмите Enter, чтобы вернуться в меню...");
}


// Удаление 
void Console::handleDeletePipe() {
    clearConsole();

    if (network.getPipeArrayLen() == 0) {
        readLine("Нет труб для удаления.\nНажмите Enter, чтобы вернуться в меню...");
        return;
    }

    const std::vector<int> ids = readMultipleChoice("ID труб/трубы для удаления (В форматах 1/1,2,3): ");

    for (const auto& id : ids) {
        if (!network.isPipeInArrayById(id)) {
            std::cout << "Ошибка: Нет трубы с ID=" << id << "\n";
            continue;
        }

        network.deletePipe(id);
        std::cout << "Труба с ID=" << id << " удалена\n";

    }

    readLine("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleDeleteCS() {
    clearConsole();

    if (network.getCStationArrayLen() == 0) {
        readLine("Нет КС для удаления.\nНажмите Enter, чтобы вернуться в меню...");
        return;
    }

    const std::vector<int> ids = readMultipleChoice("ID КС для удаления (В форматах 1/1,2,3): ");

    for (const auto& id : ids) {
        if (!network.isCStationInArrayById(id)) {
            std::cout << "Ошибка: Нет КС с ID=" << id << "\n";
            continue;
        }
        network.deleteCStation(id);
        std::cout << "КС с ID=" << id << " удалена\n";
    }

    readLine("\nНажмите Enter, чтобы вернуться в меню...");
}


void Console::handleSave() {
    clearConsole();

    while (true) {
        const std::string timestamp = getTimestamp();

        const std::string dirPath = readLine("Путь к папке сохранения:", "data");

        const std::string pipeFileName = readLine("Имя файла труб:", "pipes_" + timestamp + ".csv");

        const std::string cStationFileName = readLine("Имя файла КС:", "cstations_" + timestamp + ".csv");

        if (network.saveToFile(dirPath, pipeFileName, cStationFileName)) {
            std::cout << "Сохранено:\n" << dirPath << "/" << pipeFileName << '\n' << dirPath << "/" << cStationFileName << '\n';
            break;
        }

        std::cerr << "[*] Ошибка сохранения\n";
    }

    readLine("\nНажмите Enter, чтобы вернуться в меню...");
}

void Console::handleLoad() {
    clearConsole();

    while (true) {
        const std::string pipePath = readLine("Полный путь к файлу труб: ");

        const std::string cStationPath = readLine("Полный путь к файлу КС: ");

        if (network.loadFromFile(pipePath, cStationPath)) {
            std::cout << "Загружено\n";
            break;
        } else {
            std::cerr << "[*] Ошибка загрузки: проверьте пути и формат файлов\n";
        }
    }

    readLine("\nНажмите Enter, чтобы вернуться в меню...");
}

/*



Хэндлеры поиска



*/

void Console::handleSearchPipesByName() const {
    clearConsole();

    const std::string name = readLine("\nВведите имя для поиска: ");
    const std::vector<const Pipe*> pipeList = network.searchPipesByName(name);

    std::cout << "\n[Трубы]\n";
    printPipeTableHeader();

    if (pipeList.empty()) {
        std::cout << "[!] Трубы с таким именем не найдены.\n";
        return;
    }

    for (const Pipe* pipe : pipeList) {
        printPipe(*pipe);
    }
}

void Console::handleSearchPipesByRepair() const {
    clearConsole();

    const bool repairStatus = readBool("В ремонте?", false);
    const std::vector<const Pipe*> pipeList = network.searchPipesByRepair(repairStatus);

    std::cout << "\n[Трубы]\n";
    printPipeTableHeader();

    if (pipeList.empty()) {
        std::cout << "[!] Трубы с статусом не найдены.\n";
        return;
    }

    for (const Pipe* pipe : pipeList) {
        printPipe(*pipe);
    }
};

void Console::handleSearchCStationsByName() const {
    clearConsole();

    const std::string name = readLine("\nВведите имя для поиска: ");
    const std::vector<const CompressorStation*> CStationList = network.searchCStationsByName(name);

    std::cout << "\n[Компрессорные станции]\n";
    printCSTableHeader();

    if (CStationList.empty()) {
        std::cout << "[!] КС с таким именем не найдены.\n";
        return;
    }

    for (const CompressorStation* CStation : CStationList) {
        printCS(*CStation);
    }
}

void Console::handleSearchCStationsByActive() const {
    clearConsole();

    const std::vector<char> condition = readActiveSearch(
        "Введите условие поиска (>50%, <5, =10): "
    );

    const std::vector<const CompressorStation*> CStationList = network.searchCStationsByActive(condition);

    std::cout << "\n[Компрессорные станции]\n";
    printCSTableHeader();

    if (CStationList.empty()) {
        std::cout << "[!] КС, подходящие под условие, не найдены.\n";
        return;
    }

    for (const CompressorStation* CStation : CStationList) {
        printCS(*CStation);
    }
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
    std::cout << std::format("{:<6} | {:<10} | {:<8} | {:<8} | {:<8} | {:<12} | {}\n",
        "Id", "Diameter", "Length", "Repair", "Name", "ID CS From", "ID CS To");
    std::cout << "-------+------------+----------+----------+----------+--------------+-----------\n";
}

void Console::printCSTableHeader() const {
    std::cout << std::format("{:<6} | {:<10} | {:<8} | {:<8} | {}\n",
        "Id", "Workshops", "Active", "Type", "Name");
    std::cout << "-------+------------+----------+----------+-------------------------------------\n";
}

void Console::printPipe(const Pipe& pipe) const {
    std::cout << std::format("{:<6} | {:<10} | {:<8} | {:<8} | {:<8} | {:<12} | {}\n",
        pipe.getId(),
        pipe.getDiameter(),
        pipe.getLength(),
        pipe.getRepair() ? "yes" : "no",
        pipe.getName(),
        pipe.getCStationFromId() == -1 ? "None" : std::to_string(pipe.getCStationFromId()),
        pipe.getCStationToId() == -1 ? "None" : std::to_string(pipe.getCStationToId()));
}

void Console::printCS(const CompressorStation& station) const {
    std::string type;

    if (station.getStationType() == StationType::Light) { type = "Light"; }
    else if (station.getStationType() == StationType::Medium) { type = "Medium"; }
    else { type = "Heavy"; }

    std::cout << std::format("{:<6} | {:<10} | {:<8} | {:<8} | {}\n",
        station.getId(),
        station.getNumWorkers(),
        station.getNumActiveWorkers(),
        type,
        station.getName());
}

/*



Запуск



*/

void Console::run() {
    bool running = true;

    while (running) {
        printMenu();

        try {
            switch (readInt("\nExit - в любом действии, чтобы вернуться в меню.\nВыбор: ", -1)) {
                case 1: handleAddPipe(); break;
                case 2: handleAddCS(); break;
                case 3: handleViewAll(); break;
                case 4: handleEditPipe(); break;
                case 5: handleEditCStation(); break;
                case 6: handleDeletePipe(); break;
                case 7: handleDeleteCS(); break;
                case 8: handleSave(); break;
                case 9: handleLoad(); break;
                case 0: running = false; break;
                default:
                    std::cout << "Ошибка: Нет такого пункта меню.\n";
                    readLine("\nНажмите Enter, чтобы вернуться в меню...");
                    break;
            }
        } catch (InputCommand) {
        }
    }
}