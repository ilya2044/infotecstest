#include "logger.h"
#include <iostream>
#include <thread>
#include <queue>
#include <memory>
#include <condition_variable>

struct LogMessage
{
    std::string message;
    LogLevel level;
};

class LogQueue
{
public:
    LogQueue()
        : shutdown_(false)
    {
    }

    void enqueue(const LogMessage& msg)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(msg);
        condVar_.notify_one();
    }

    LogMessage dequeue()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (queue_.empty() && !shutdown_)
        {
            condVar_.wait(lock);
        }
        if (queue_.empty())
        {
            return LogMessage{"", LogLevel::MEDIUM};
        }
        LogMessage msg = queue_.front();
        queue_.pop();
        return msg;
    }

    bool isEmpty() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    void shutdown()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        shutdown_ = true;
        condVar_.notify_one();
    }

    bool isShutdown() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return shutdown_;
    }

private:
    std::queue<LogMessage> queue_;
    mutable std::mutex mutex_;
    std::condition_variable condVar_;
    bool shutdown_;
};

void loggingWorker(Logger& logger, LogQueue& logQueue)
{
    while (true)
    {
        LogMessage msg = logQueue.dequeue();
        if (logQueue.isShutdown() && logQueue.isEmpty())
        {
            break;
        }
        if (!msg.message.empty())
        {
            logger.log(msg.message, msg.level);
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Использование: " << argv[0] << " <logfile> [LEVEL]\n";
        return 1;
    }

    std::string logFilename = argv[1];
    LogLevel defaultLevel = LogLevel::MEDIUM;

    if (argc >= 3)
    {
        defaultLevel = stringToLogLevel(argv[2]);
    }

    try
    {
        Logger logger(logFilename, defaultLevel);

        std::cout << "Логгер инициализирован\n"
                  << "Файл журнала: " << logFilename << "\n"
                  << "Уровень по умолчанию: " << logLevelToString(defaultLevel) << "\n\n";

        LogQueue logQueue;

        std::thread worker(loggingWorker, std::ref(logger), std::ref(logQueue));

        std::cout << "Введите сообщение для логирования (формат: 'сообщение [LEVEL]')\n"
                  << "Доступные уровни: LOW, MEDIUM, HIGH\n"
                  << "Для выхода введите 'exit'\n"
              << "Для изменения уровня введите 'level LEVEL'\n\n";

        std::string line;
        while (true)
        {
            std::cout << "> ";
            std::getline(std::cin, line);

            if (line.empty())
                continue;

            if (line == "exit")
            {
                break;
            }

            if (line.substr(0, 6) == "level ")
            {
                std::string newLevel = line.substr(6);
                LogLevel level = stringToLogLevel(newLevel);
                logger.setDefaultLevel(level);
                std::cout << "Уровень изменен на: " << logLevelToString(level) << "\n";
                continue;
            }

            LogMessage logMsg;
            logMsg.level = LogLevel::MEDIUM;

            size_t lastSpace = line.rfind(' ');
            std::string lastWord = (lastSpace != std::string::npos)
                                       ? line.substr(lastSpace + 1)
                                       : "";

            if (lastWord == "LOW" || lastWord == "MEDIUM" || lastWord == "HIGH")
            {
                logMsg.message = line.substr(0, lastSpace);
                logMsg.level = stringToLogLevel(lastWord);
            }
            else
            {
                logMsg.message = line;
                logMsg.level = logger.getDefaultLevel();
            }

            logQueue.enqueue(logMsg);
            std::cout << "Сообщение добавлено\n";
        }

        logQueue.shutdown();
        worker.join();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
