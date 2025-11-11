#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <iostream>
#include <string>

class Logger
{
public:
    static Logger& getInstance()
    {
        static Logger instance("session_log.txt");
        return instance;
    }

    void logOutput(const std::string& message);
    void logOutput(const char* message);
    void logInput(const std::string& input);
    void logError(const std::string& message);
    void logError(const char* message);

private:
    Logger(const std::string& filename);
    ~Logger();
    Logger(const Logger& other);
    Logger& operator=(const Logger& other);
    Logger(Logger&& other) noexcept;
    Logger& operator=(Logger&& other) noexcept;

    std::ofstream logFile;
};

#endif // LOGGER_H