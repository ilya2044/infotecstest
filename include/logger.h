#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>
#include <chrono>

enum class LogLevel
{
    LOW,
    MEDIUM,
    HIGH
};

std::string logLevelToString(LogLevel level);
LogLevel stringToLogLevel(const std::string& str);

class Logger
{
public:
    Logger(const std::string& filename, LogLevel defaultLevel = LogLevel::MEDIUM);
    ~Logger();
    void log(const std::string& message, LogLevel level);
    void setDefaultLevel(LogLevel level);
    LogLevel getDefaultLevel() const;

private:
    std::string filename_;
    LogLevel defaultLevel_;
    mutable std::mutex mutex_;
    std::ofstream logFile_;
    std::string getCurrentTimestamp() const;
    std::string formatLogEntry(const std::string& message, LogLevel level) const;
    void openLogFile();
};

#endif
