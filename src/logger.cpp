#include "logger.h"
#include <iomanip>
#include <sstream>
#include <iostream>

std::string logLevelToString(LogLevel level)
{
    switch (level)
    {
        case LogLevel::LOW:
            return "LOW";
        case LogLevel::MEDIUM:
            return "MEDIUM";
        case LogLevel::HIGH:
            return "HIGH";
        default:
            return "UNKNOWN";
    }
}

LogLevel stringToLogLevel(const std::string& str)
{
    if (str == "LOW")
        return LogLevel::LOW;
    else if (str == "MEDIUM")
        return LogLevel::MEDIUM;
    else if (str == "HIGH")
        return LogLevel::HIGH;
    else if (str == "NOTICE")
        return LogLevel::LOW;
    else if (str == "IMPORTANT")
        return LogLevel::MEDIUM;
    else if (str == "PRIORITY")
        return LogLevel::HIGH;
    else if (str == "INFO")
        return LogLevel::LOW;
    else if (str == "WARNING")
        return LogLevel::MEDIUM;
    else if (str == "ERROR")
        return LogLevel::HIGH;
    else
        return LogLevel::MEDIUM;
}

Logger::Logger(const std::string& filename, LogLevel defaultLevel)
    : filename_(filename), defaultLevel_(defaultLevel)
{
    openLogFile();
}

Logger::~Logger()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (logFile_.is_open())
    {
        logFile_.close();
    }
}

void Logger::openLogFile()
{
    std::lock_guard<std::mutex> lock(mutex_);
    logFile_.open(filename_, std::ios::app);
    if (!logFile_.is_open())
    {
        std::cerr << "Ошибка: не удалось открыть файл журнала: " << filename_ << std::endl;
        throw std::runtime_error("Не удалось открыть файл журнала");
    }
}

void Logger::log(const std::string& message, LogLevel level)
{
    if (static_cast<int>(level) < static_cast<int>(defaultLevel_))
    {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    if (!logFile_.is_open())
    {
        std::cerr << "Ошибка: файл журнала не открыт" << std::endl;
        return;
    }

    std::string entry = formatLogEntry(message, level);
    logFile_ << entry << std::endl;

    if (!logFile_.good())
    {
        std::cerr << "Ошибка: запись не удалась" << std::endl;
    }
    else
    {
        logFile_.flush();
    }
}

void Logger::setDefaultLevel(LogLevel level)
{
    std::lock_guard<std::mutex> lock(mutex_);
    defaultLevel_ = level;
}

LogLevel Logger::getDefaultLevel() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return defaultLevel_;
}

std::string Logger::getCurrentTimestamp() const
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();

    return ss.str();
}

std::string Logger::formatLogEntry(const std::string& message, LogLevel level) const
{
    std::stringstream ss;
    ss << "[" << getCurrentTimestamp() << "] "
       << "[" << logLevelToString(level) << "] "
       << message;
    return ss.str();
}
