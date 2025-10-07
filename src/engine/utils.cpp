#include <filesystem>

#include "include/utils.h"

#ifdef _WIN32
#include <array>
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace Snake
{
	std::filesystem::path Utils::getExecutablePath()
	{
#ifdef _WIN32
		std::array<char, MAX_PATH> buffer{};
		DWORD length = GetModuleFileNameA(NULL, buffer.data(), (DWORD)buffer.size());

		if (length == 0 || length == buffer.size())
			throw std::runtime_error("GetModuleFileName failed");

		return std::filesystem::canonical(std::filesystem::path(buffer.data()));
#else
		return std::filesystem::canonical("/proc/self/exe");
#endif
	}
};