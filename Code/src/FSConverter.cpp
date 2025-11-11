#include "FSConverter.h"
#include <iostream>
#include <limits.h>
#include <stdexcept>

FSConverter::FSConverter(std::string resourceDir)
{
	this->resourceDir = resourceDir;
	if (this->resourceDir.empty())
	{
		this->resourceDir = findResourcesFolder();
	}

	if (this->resourceDir.empty())
	{
		std::cerr << "Error: Could not locate /Resources folder!" << std::endl;
		throw std::runtime_error("Resources folder not found.");
	}
	else
	{
		// Resource folder found
	}
}

std::string FSConverter::findResourcesFolder()
{
	std::filesystem::path execPath = executablePath();

	// Try to find the Resources folder in a few places relative to the executable
	std::filesystem::path potentialPaths[] = {
		execPath / "../Resources",			  // Resources in the parent directory of the executable
		execPath / "Resources",				  // Resources in the same directory as the executable
		execPath / "../../Resources",		  // Resources two levels up (in case of a bin/debug layout)
		execPath / "../../../Resources",	  // Resources two levels up (in case of a bin/debug layout)
		execPath / "../../../../Resources",	  // Resources two levels up (in case of a bin/debug layout)
		execPath / "../../../../../Resources" // Resources two levels up (in case of a bin/debug layout)
	};

	for (const auto& path : potentialPaths)
	{
		if (std::filesystem::exists(path))
		{
			return path.string();
		}
	}

	return ""; // Resources folder not found
}

std::string FSConverter::getResourcePath(const std::string& resourceName)
{
	std::filesystem::path fullPath = std::filesystem::path(resourceDir) / resourceName;

	if (!std::filesystem::exists(fullPath))
	{
		return "";
	}

	return fullPath.string();
}

std::string FSConverter::executablePath()
{
	char path[PATH_MAX] = {0};
#ifdef _WIN32
	DWORD count = GetModuleFileNameA(NULL, path, PATH_MAX);
	if (count == 0 || count == PATH_MAX)
	{
		return "";
	}
#else
	ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
	if (count == -1)
	{
		return "";
	}
#endif
	std::string execPath(path, count);
	return std::filesystem::path(path).parent_path().string();
}