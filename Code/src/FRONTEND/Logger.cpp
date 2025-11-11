#include "Logger.h"
#include <stdexcept>

Logger::Logger(const std::string& filename)
{
	try
	{
		logFile.open(filename, std::ios::out | std::ios::app);
		if (!logFile.is_open())
		{
			throw std::runtime_error("Could not open log file");
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Logger initialization error: " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown error during Logger initialization" << std::endl;
	}
}

Logger::~Logger()
{
	try
	{
		if (logFile.is_open())
		{
			logFile.close();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Logger destruction error: " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown error during Logger destruction" << std::endl;
	}
}

// Copy versions (Private - no copying allowed for singleton)
Logger::Logger(const Logger& other)
{
    // No copy of file stream, singletons shouldnt be copied
}

Logger& Logger::operator=(const Logger& other)
{
    // Self-assignment guard, do nothing
    if (this != &other) { /* nothing */ }
    return *this;
}

// Move versions (also private, no moving allowed for singleton)
Logger::Logger(Logger&& other) noexcept { /* do nothing */ }
Logger& Logger::operator=(Logger&& other) noexcept { return *this; }



void Logger::logOutput(const std::string& message)
{
	std::cout << message;
	if (logFile.is_open())
	{
		logFile << message;
	}
}

void Logger::logOutput(const char* message)
{
	std::cout << message;
	if (logFile.is_open())
	{
		logFile << message;
	}
}

void Logger::logInput(const std::string& input)
{
	std::string prompt = "> ";
	if (logFile.is_open())
	{
		logFile << prompt << input << std::endl;
	}
}

void Logger::logError(const std::string& errorMessage)
{
	std::cerr << "ERROR: " << errorMessage << std::endl;
	if (logFile.is_open())
	{
		logFile << "ERROR: " << errorMessage << std::endl;
	}
}

void Logger::logError(const char* errorMessage)
{
	std::cerr << "ERROR: " << errorMessage << std::endl;
	if (logFile.is_open())
	{
		logFile << "ERROR: " << errorMessage << std::endl;
	}
}