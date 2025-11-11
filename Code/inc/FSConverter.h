#pragma once

#include <filesystem>
#include <string>
#ifdef _WIN32
#include <windows.h>
#define PATH_MAX MAX_PATH
#else
#include <limits.h>
#include <unistd.h>
#endif
#include <stdexcept>

class FSConverter
{
public:
	FSConverter(std::string ResourceDir = "");

	std::string getResourcePath(const std::string& resourceName);

private:
	std::string findResourcesFolder();
	std::string executablePath();
	std::string resourceDir;
};