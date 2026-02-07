#include "../include/logger.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <filesystem>

namespace fs = std::filesystem;

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return "";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool fileExists(const std::string& filename) {
    return fs::exists(filename);
}

void testLogFileCreation() {
    std::cout << "Тест 1: создание лошфайла ";
    fs::remove("test1.log");
    
    Logger logger("test1.log");
    logger.log("Тестовое сообщение", LogLevel::LOW);
    
    assert(fileExists("test1.log"));
    std::cout << "OK\n";
}

void testLogLowLevel() {
    std::cout << "Тест 2: LOW level ";
    fs::remove("test2.log");
    
    Logger logger("test2.log");
    logger.setDefaultLevel(LogLevel::LOW);
    logger.log("LOW сообщение", LogLevel::LOW);
    
    std::string content = readFile("test2.log");
    assert(content.find("LOW сообщение") != std::string::npos);
    assert(content.find("[LOW]") != std::string::npos);
    std::cout << "OK\n";
}

void testLogMediumLevel() {
    std::cout << "Тест 3: MEDIUM level ";
    fs::remove("test3.log");
    
    Logger logger("test3.log");
    logger.setDefaultLevel(LogLevel::MEDIUM);
    logger.log("MEDIUM сообщение", LogLevel::MEDIUM);
    
    std::string content = readFile("test3.log");
    assert(content.find("MEDIUM сообщение") != std::string::npos);
    assert(content.find("[MEDIUM]") != std::string::npos);
    std::cout << "OK\n";
}

void testLogHighLevel() {
    std::cout << "Тест 4: HIGH level ";
    fs::remove("test4.log");
    
    Logger logger("test4.log");
    logger.setDefaultLevel(LogLevel::HIGH);
    logger.log("HIGH сообщение", LogLevel::HIGH);
    
    std::string content = readFile("test4.log");
    assert(content.find("HIGH сообщение") != std::string::npos);
    assert(content.find("[HIGH]") != std::string::npos);
    std::cout << "OK\n";
}

void testLevelFiltering() {
    std::cout << "Тест 5: фильтрация ";
    fs::remove("test5.log");
    
    Logger logger("test5.log");
    logger.setDefaultLevel(LogLevel::HIGH);
    logger.log("Это сообщение должно быть заблокировано", LogLevel::LOW);
    
    std::string content = readFile("test5.log");
    assert(content.find("Это сообщение должно быть заблокировано") == std::string::npos);
    std::cout << "OK\n";
}

int main() {
    std::cout << "\nТесты \n";
    
    try {
        testLogFileCreation();
        testLogLowLevel();
        testLogMediumLevel();
        testLogHighLevel();
        testLevelFiltering();
        
        fs::remove("test1.log");
        fs::remove("test2.log");
        fs::remove("test3.log");
        fs::remove("test4.log");
        fs::remove("test5.log");
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\nОшибка: " << e.what() << "\n";
        return 1;
    }
}
