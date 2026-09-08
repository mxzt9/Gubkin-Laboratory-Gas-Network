#include <iostream>
#include <fstream>
#include <filesystem>

#include "log.hpp"
#include "utils.hpp"

// При инициализации
Logger::Logger() : logOutputPath("data/log/log_" + getTimestamp() + ".txt") {
    // При инициализации экземпляра вызывает метод открытия потоков записи
    open();
}

Logger::~Logger() {

    // При уничтожении экземпляра записываем всё, что было в очереди
    if (!logQueue.empty()) {
        saveToFile();
    }
    
    // Вызов метода уничтожения
    close();
}

bool Logger::open() {

    // Инициализация и проверка на успешное открытие потока
    if (!fileStream.is_open()) {
        std::filesystem::create_directories("data/log");

        // std::ios::app -> добавление строчки без перезаписи остального текста
        fileStream.clear();
        fileStream.open(logOutputPath, std::ios::app | std::ios::binary);
    }

    // Если предудыщая попытка инициализации потока не удалась ->
    // Ошибка об открытии
    if (!fileStream.is_open()) {
        std::cerr << "[*] Logger: ошибка при открытии " << logOutputPath << "\n";
        return false;
    }

    return true;
}

 

void Logger::close() {
    if (fileStream.is_open()) {
        fileStream.close();
    }
}

void Logger::addLine(const std::string& timestamp, const std::string& logText) {
    logQueue.push("[" + timestamp + "] " + logText);
    if (!saveToFile()) {
        std::cerr << "[!] Логгер: запись не сохранена в " << logOutputPath << "\n";
    }
}

bool Logger::saveToFile() {
    
    // Проверка на успешно открытый поток
    if (!fileStream.is_open() && !open()) {
        return false;
    }

    // Опустошаем очередь, записываем лог в файл
    while (!logQueue.empty()) {
        fileStream << logQueue.front() << "\n";
        fileStream.flush();
        // Проверка на наличие состояния ошибки в потоке
        if (!fileStream) {
            return false;
        }
        // Удаление записи
        logQueue.pop();
    }

    // Принудительная запись буфера в файл
    fileStream.flush();
    return static_cast<bool>(fileStream);
}
